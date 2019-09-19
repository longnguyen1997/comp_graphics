#include "curve.h"
#include "vertexrecorder.h"
#include <cassert>
using namespace std;

const float c_pi = 3.14159265358979323846f;

namespace
{
    // Approximately equal to.  We don't want to use == because of
    // precision issues with floating point.
    inline bool approx(const Vector3f &lhs, const Vector3f &rhs)
    {
        const float eps = 1e-8f;
        return (lhs - rhs).absSquared() < eps;
    }


}

const Matrix4f b_spline_basis = Matrix4f(
                                    1.0f, 4.0f, 1.0f, 0.0f,
                                    -3.0f, 0.0f, 3.0f, 0.0f,
                                    3.0f, -6.0f, 3.0f, 0.0f,
                                    -1.0f, 3.0f, -3.0f, 1.0f
                                    ).transposed() * (1.0 / 6.0);

const Matrix4f b_spline_inverse = b_spline_basis.inverse();

const Matrix4f bezier_basis = Matrix4f(
                                  1.0f, -3.0f, 3.0f, -1.0f,
                                  0.0f, 3.0f, -6.0f, 3.0f,
                                  0.0f, 0.0f, 3.0f, -3.0f,
                                  0.0f, 0.0f, 0.0f, 1.0f
                              );

const Matrix4f bezier_inverse = bezier_basis.inverse();

float v_i(const vector< Vector3f > &P, int i, float t)
{
    // P MUST BE OF SIZE 4!
    return (pow(1 - t, 3) * P[0][i] +
            (3 * t * pow(1 - t, 2)) * P[1][i] +
            (3 * pow(t, 2) * (1 - t)) * P[2][i] +
            pow(t, 3) * P[3][i]);
}

Vector3f v(const vector< Vector3f > &P, float t)
{
    return Vector3f(v_i(P, 0, t), v_i(P, 1, t), v_i(P, 2, t));
}

float t_i(const vector< Vector3f > &P, int i, float t)
{
    // P MUST BE OF SIZE 4!
    float q_prime = ((-3 * pow(1 - t, 2)) * P[0][i] +
                     (3 * pow(1 - t, 2) - 6 * t * (1 - t)) * P[1][i] +
                     (6 * t * (1 - t) - 3 * pow(t, 2)) * P[2][i] +
                     (3 * pow(t, 2)) * P[3][i]);
    return q_prime;
}

Vector3f t(const vector< Vector3f > &P, float t)
{
    return Vector3f(t_i(P, 0, t), t_i(P, 1, t), t_i(P, 2, t)).normalized();
}

// Required for the recursive update equation.
vector<Vector3f> b_vectors{Vector3f(
        243242 * 3.1415,
        2.3284 + 3.14159265,
        5 * 2.7123 - 0.17123 * c_pi
    ).normalized()};

void reset_b_vectors()
{
    b_vectors.clear();
    b_vectors.push_back(Vector3f(
                            243242 * 3.1415,
                            2.3284 + 3.14159265,
                            5 * 2.7123 - 0.17123 * c_pi
                        ).normalized());
}

bool is_drawing_Bspline = false;

Curve evalBezier(const vector< Vector3f > &P, unsigned steps)
{
    // Check
    if (P.size() < 4 || P.size() % 3 != 1)
    {
        cerr << "evalBezier must be called with 3n+1 control points." << endl;
        exit(0);
    }

    cout << "evalBezier called!" << endl;

    // TODO:
    // You should implement this function so that it returns a Curve
    // (e.g., a vector< CurvePoint >).  The variable "steps" tells you
    // the number of points to generate on each piece of the spline.
    // At least, that's how the sample solution is implemented and how
    // the SWP files are written.  But you are free to interpret this
    // variable however you want, so long as you can control the
    // "resolution" of the discretized spline curve with it.

    // Make sure that this function computes all the appropriate
    // Vector3fs for each CurvePoint: V,T,N,B.
    // [NBT] should be unit and orthogonal.

    // Also note that you may assume that all Bezier curves that you
    // receive have G1 continuity.  Otherwise, the TNB will not be
    // be defined at points where this does not hold.

    Curve bezier_curve;
    // Sanity check. If the previous curve drawn through
    // evalBezier was a B-spline, we reset b_vectors
    // so that any existing B_i doesn't interfere.
    if (!is_drawing_Bspline) reset_b_vectors();

    // Compute the cubic Bezier curve for each section of four points piecewise.
    int section = 0;
    while (section < P.size() - 3)
    {

        const vector<Vector3f> P_piece = vector<Vector3f>(
                                             P.begin() + section,
                                             P.begin() + section + 4
                                         );
        assert(P_piece.size() == 4);
        // Loop through the # steps required.
        for (unsigned i = 0; i <= steps; ++i)
        {
            if (i == steps && section < P.size() - 4)
            {
                cout << "Not last piece in Bezier curve, skipping this point." << endl;
                continue;
            }
            float increment = float(i) / steps;
            // 1) Calculate vertex (q(t)).
            Vector3f V = v(P_piece, increment);
            // 2) Calculate tangent.
            Vector3f T = t(P_piece, increment);
            // 3) Calculate normal. Arbitrary B_0.
            Vector3f B_i_minus_one = b_vectors.back();
            Vector3f N = Vector3f::cross(B_i_minus_one, T).normalized();
            // 4) Calculate binormal.
            Vector3f B = Vector3f::cross(T, N).normalized();
            // Generate CurvePoint from data.
            struct CurvePoint point = {V, T, N, B};
            b_vectors.push_back(B);
            bezier_curve.push_back(point);
        }
        section += 3;
    }

    cerr << "\t>>> evalBezier has been called with the input below." << endl;

    cerr << "\t\t>>> Control points (vector<Vector3f>): " << endl;
    for (int i = 0; i < (int)P.size(); ++i)
    {
        cerr << "\t\t\t>>> " << P[i] << endl;
    }

    cerr << "\t\t>>> Number of steps: " << steps << endl;

    return bezier_curve;
}

Curve evalBspline(const vector< Vector3f > &P, unsigned steps)
{
    // Check
    if (P.size() < 4)
    {
        cerr << "evalBspline must be called with 4 or more control points." << endl;
        exit(0);
    }

    // TODO:
    // It is suggested that you implement this function by changing
    // basis from B-spline to Bezier.  That way, you can just call
    // your evalBezier function.

    cerr << "\t>>> evalBSpline has been called with the following input:" << endl;

    cerr << "\t>>> Control points (type vector< Vector3f >): " << endl;
    for (int i = 0; i < (int)P.size(); ++i)
    {
        cerr << "\t>>> " << P[i] << endl;
    }

    Curve b_spline;
    is_drawing_Bspline = true;

    int piece = 0;
    // Deal with groups of 4 at a time.
    while (piece < P.size() - 3)
    {
        vector<Vector3f> G_vector = vector<Vector3f>(
                                        P.begin() + piece,
                                        P.begin() + piece + 4
                                    );
        assert(G_vector.size() == 4);
        Matrix4f G(
            Vector4f(G_vector[0], 0),
            Vector4f(G_vector[1], 0),
            Vector4f(G_vector[2], 0),
            Vector4f(G_vector[3], 0)
        );
        // Convert to the Bernstein basis for bezier curves.
        Matrix4f basis_change_P = G * b_spline_basis * bezier_inverse;
        vector<Vector3f> P_converted
        {
            basis_change_P.getCol(0).xyz(),
            basis_change_P.getCol(1).xyz(),
            basis_change_P.getCol(2).xyz(),
            basis_change_P.getCol(3).xyz()
        };
        Curve portion = evalBezier(P_converted, steps);
        // Uncomment to print debug.
        /*
        for (int i = 0; i < portion.size(); i++) {
            cout<<portion[i].V<<' ';
        }
        cout<<endl;
        */
        if (piece == P.size() - 4) for (CurvePoint p : portion) b_spline.push_back(p);
        else for (int i = 0; i < portion.size() - 1; i++) b_spline.push_back(portion[i]);
        piece += 1;
    }

    cerr << "\t>>> Steps (type steps): " << steps << endl;

    is_drawing_Bspline = false;
    reset_b_vectors();

    // Return an empty curve right now.
    return b_spline;
}

Curve evalCircle(float radius, unsigned steps)
{
    // This is a sample function on how to properly initialize a Curve
    // (which is a vector< CurvePoint >).

    // Preallocate a curve with steps+1 CurvePoints
    Curve R(steps + 1);

    // Fill it in counterclockwise
    for (unsigned i = 0; i <= steps; ++i)
    {
        // step from 0 to 2pi
        float t = 2.0f * c_pi * float(i) / steps;

        // Initialize position
        // We're pivoting counterclockwise around the y-axis
        R[i].V = radius * Vector3f(cos(t), sin(t), 0);

        // Tangent vector is first derivative
        R[i].T = Vector3f(-sin(t), cos(t), 0);

        // Normal vector is second derivative
        R[i].N = Vector3f(-cos(t), -sin(t), 0);

        // Finally, binormal is facing up.
        R[i].B = Vector3f(0, 0, 1);
    }

    return R;
}

void recordCurve(const Curve &curve, VertexRecorder *recorder)
{
    const Vector3f WHITE(1, 1, 1);
    for (int i = 0; i < (int)curve.size() - 1; ++i)
    {
        recorder->record_poscolor(curve[i].V, WHITE);
        recorder->record_poscolor(curve[i + 1].V, WHITE);
    }
}
void recordCurveFrames(const Curve &curve, VertexRecorder *recorder, float framesize)
{
    Matrix4f T;
    const Vector3f RED(1, 0, 0);
    const Vector3f GREEN(0, 1, 0);
    const Vector3f BLUE(0, 0, 1);

    const Vector4f ORGN(0, 0, 0, 1);
    const Vector4f AXISX(framesize, 0, 0, 1);
    const Vector4f AXISY(0, framesize, 0, 1);
    const Vector4f AXISZ(0, 0, framesize, 1);

    for (int i = 0; i < (int)curve.size(); ++i)
    {
        T.setCol(0, Vector4f(curve[i].N, 0));
        T.setCol(1, Vector4f(curve[i].B, 0));
        T.setCol(2, Vector4f(curve[i].T, 0));
        T.setCol(3, Vector4f(curve[i].V, 1));

        // Transform orthogonal frames into model space
        Vector4f MORGN  = T * ORGN;
        Vector4f MAXISX = T * AXISX;
        Vector4f MAXISY = T * AXISY;
        Vector4f MAXISZ = T * AXISZ;

        // Record in model space
        recorder->record_poscolor(MORGN.xyz(), RED);
        recorder->record_poscolor(MAXISX.xyz(), RED);

        recorder->record_poscolor(MORGN.xyz(), GREEN);
        recorder->record_poscolor(MAXISY.xyz(), GREEN);

        recorder->record_poscolor(MORGN.xyz(), BLUE);
        recorder->record_poscolor(MAXISZ.xyz(), BLUE);
    }
}

