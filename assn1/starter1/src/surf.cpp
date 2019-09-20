#include "surf.h"
#include "vertexrecorder.h"
#include <cassert>
using namespace std;

namespace
{

    // We're only implenting swept surfaces where the profile curve is
    // flat on the xy-plane.  This is a check function.
    static bool checkFlat(const Curve &profile)
    {
        for (unsigned i = 0; i < profile.size(); i++)
            if (profile[i].V[2] != 0.0 ||
                    profile[i].T[2] != 0.0 ||
                    profile[i].N[2] != 0.0)
                return false;

        return true;
    }
}

// DEBUG HELPER
Surface quad()
{
    Surface ret;
    ret.VV.push_back(Vector3f(-1, -1, 0));
    ret.VV.push_back(Vector3f(+1, -1, 0));
    ret.VV.push_back(Vector3f(+1, +1, 0));
    ret.VV.push_back(Vector3f(-1, +1, 0));

    ret.VN.push_back(Vector3f(0, 0, 1));
    ret.VN.push_back(Vector3f(0, 0, 1));
    ret.VN.push_back(Vector3f(0, 0, 1));
    ret.VN.push_back(Vector3f(0, 0, 1));

    ret.VF.push_back(Tup3u(0, 1, 2));
    ret.VF.push_back(Tup3u(0, 2, 3));
    return ret;
}


const float c_pi = 3.14159265358979323846f;
const float two_pi = 2 * c_pi;

// See https://www.mathworks.com/help/phased/ref/roty.html.
Matrix3f get_vertex_rotation_matrix(float theta)
{
    // Rotating CCW around the y-axis.
    return Matrix3f(
               cos(theta), 0.0f, sin(theta),
               0.0f, 1.0f, 0.0f,
               -sin(theta), 0.0f, cos(theta)
           );
}

Matrix3f inverse_transpose_matrix(float theta)
{
    return get_vertex_rotation_matrix(theta).inverse().transposed();
}

Surface makeSurfRev(const Curve &profile, unsigned steps)
{

    if (!checkFlat(profile))
    {
        cerr << "surfRev profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    vector<Vector3f> VV;
    vector<Vector3f> VN;
    vector<Tup3u> VF;

    const int num_points = profile.size();
    cout << "Generating rotated duplicates on the curve profile." << endl;
    // Duplicate points for steps around the rotation.
    for (unsigned s = 0; s < steps; ++s)
    {
        const float t = two_pi * float(s) / steps;
        vector<Vector3f> rotated_Vs;
        vector<Vector3f> rotated_Ns;
        for (CurvePoint p : profile)
        {
            const Vector3f rotated_V = get_vertex_rotation_matrix(t) * p.V;
            const Vector3f rotated_N = (inverse_transpose_matrix(t) * p.N).normalized();
            rotated_Vs.push_back(rotated_V);
            // Not negating the normals gives inverse lighting. Negate them.
            rotated_Ns.push_back(-rotated_N);
        }
        if (VV.size() > 0)
        {
            // Link the previous profile copy to the newly generated one.
            for (int i = VV.size() - 1; i > VV.size() - num_points; --i)
            {
                // For each point in the previous profile copy, link it to
                // the other vertices in the triangle mesh.
                VF.push_back(Tup3u(i, i + num_points, i + num_points - 1));
                VF.push_back(Tup3u(i, i + num_points - 1, i - 1));
            }
        }
        for (Vector3f V : rotated_Vs) VV.push_back(V);
        for (Vector3f N : rotated_Ns) VN.push_back(N);
    }
    // Link the last copy to the initial profile curve.
    for (int i = 1; i < num_points; ++i)
    {
        VF.push_back(Tup3u(VV.size() - i, num_points - i, num_points - i - 1));
        VF.push_back(Tup3u(VV.size() - i, num_points - i - 1, VV.size() - i - 1));
    }
    // Sanity checks.
    assert(VN.size() == num_points * steps);
    assert(VV.size() == num_points * steps);
    return Surface{VV, VN, VF};
}

Matrix4f gencyl_transform_matrix(CurvePoint p)
{
    const Vector4f N(p.N, 0);
    const Vector4f B(p.B, 0);
    const Vector4f T(p.T, 0);
    const Vector4f V(p.V, 1);
    return Matrix4f(N, B, T, V);
}

Matrix3f gencyl_normal_transform_matrix(CurvePoint p)
{
    return gencyl_transform_matrix(p).getSubmatrix3x3(0, 0).inverse().transposed();
}

Surface makeGenCyl(const Curve &profile, const Curve &sweep )
{
    if (!checkFlat(profile))
    {
        cerr << "genCyl profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    vector<Vector3f> VV;
    vector<Vector3f> VN;
    vector<Tup3u> VF;

    const int num_points = profile.size();
    // Duplicate profile for each point along the sweep curve.
    for (CurvePoint sweep_point : sweep)
    {
        vector<Vector3f> swept_Vs;
        vector<Vector3f> swept_Ns;
        for (CurvePoint p : profile)
        {
            swept_Vs.push_back(
                (gencyl_transform_matrix(sweep_point) * Vector4f(p.V, 1)).xyz()
            );
            // Invert the normals.
            swept_Ns.push_back(
                -1 * (gencyl_normal_transform_matrix(sweep_point) * p.N)
            );

        }
        if (VV.size() > 0)
        {
            // Link the previous profile copy to the newly generated one.
            for (int i = VV.size() - 1; i > VV.size() - num_points; --i)
            {
                // For each point in the previous profile copy, link it to
                // the other vertices in the triangle mesh.
                VF.push_back(Tup3u(i, i + num_points, i + num_points - 1));
                VF.push_back(Tup3u(i, i + num_points - 1, i - 1));
            }
        }
        for (Vector3f V : swept_Vs) VV.push_back(V);
        for (Vector3f N : swept_Ns) VN.push_back(N);
    }
    // Link the last copy to the initial profile curve.
    for (int i = 1; i < num_points; ++i)
    {
        VF.push_back(Tup3u(VV.size() - i, num_points - i, num_points - i - 1));
        VF.push_back(Tup3u(VV.size() - i, num_points - i - 1, VV.size() - i - 1));
    }
    // Sanity checks.
    assert(VN.size() == num_points * sweep.size());
    assert(VV.size() == num_points * sweep.size());
    return Surface{VV, VN, VF};
}

void recordSurface(const Surface &surface, VertexRecorder *recorder)
{
    const Vector3f WIRECOLOR(0.4f, 0.4f, 0.4f);
    for (int i = 0; i < (int)surface.VF.size(); i++)
    {
        recorder->record(surface.VV[surface.VF[i][0]], surface.VN[surface.VF[i][0]], WIRECOLOR);
        recorder->record(surface.VV[surface.VF[i][1]], surface.VN[surface.VF[i][1]], WIRECOLOR);
        recorder->record(surface.VV[surface.VF[i][2]], surface.VN[surface.VF[i][2]], WIRECOLOR);
    }
}

void recordNormals(const Surface &surface, VertexRecorder *recorder, float len)
{
    const Vector3f NORMALCOLOR(0, 1, 1);
    for (int i = 0; i < (int)surface.VV.size(); i++)
    {
        recorder->record_poscolor(surface.VV[i], NORMALCOLOR);
        recorder->record_poscolor(surface.VV[i] + surface.VN[i] * len, NORMALCOLOR);
    }
}

void outputObjFile(ostream &out, const Surface &surface)
{

    for (int i = 0; i < (int)surface.VV.size(); i++)
        out << "v  "
            << surface.VV[i][0] << " "
            << surface.VV[i][1] << " "
            << surface.VV[i][2] << endl;

    for (int i = 0; i < (int)surface.VN.size(); i++)
        out << "vn "
            << surface.VN[i][0] << " "
            << surface.VN[i][1] << " "
            << surface.VN[i][2] << endl;

    out << "vt  0 0 0" << endl;

    for (int i = 0; i < (int)surface.VF.size(); i++)
    {
        out << "f  ";
        for (unsigned j = 0; j < 3; j++)
        {
            unsigned a = surface.VF[i][j] + 1;
            out << a << "/" << "1" << "/" << a << " ";
        }
        out << endl;
    }
}
