#include "camera.h"
#include <iostream>
#include "gl.h"
using namespace std;

const float c_pi = 3.14159265358979323846f;

Camera::Camera()
{
    mStartRot = Matrix4f::identity();
    mCurrentRot = Matrix4f::identity();
}

void Camera::SetDimensions(int w, int h)
{
    mDimensions[0] = w;
    mDimensions[1] = h;
}

void Camera::SetPerspective(float fovy)
{
    mPerspective[0] = fovy;
}

void Camera::SetViewport(int x, int y, int w, int h)
{
    mViewport[0] = x;
    mViewport[1] = y;
    mViewport[2] = w;
    mViewport[3] = h;
    mPerspective[1] = float( w ) / h;
}

void Camera::SetCenter(const Vector3f& center)
{
    mStartCenter = mCurrentCenter = center;
}

void Camera::SetRotation(const Matrix4f& rotation)
{
    mStartRot = mCurrentRot = rotation;
}

void Camera::SetDistance(const float distance)
{
    mStartDistance = mCurrentDistance = distance;
}

void Camera::MouseClick(Button button, int x, int y)
{
    mStartClick[0] = x;
    mStartClick[1] = y;

    mButtonState = button;
    switch (button)
    {
    case LEFT:
        mCurrentRot = mStartRot;
        break;
    case MIDDLE:
        mCurrentCenter = mStartCenter;
        break;
    case RIGHT:
        mCurrentDistance = mStartDistance;
        break;        
    default:
        break;
    }
}

void Camera::MouseDrag(int x, int y)
{
    switch (mButtonState)
    {
    case LEFT:
        ArcBallRotation(x,y);
        break;
    case MIDDLE:
        PlaneTranslation(x,y);
        break;
    case RIGHT:
        DistanceZoom(x,y);
        break;
    default:
        break;
    }
}


void Camera::MouseRelease(int x, int y)
{
    mStartRot = mCurrentRot;
    mStartCenter = mCurrentCenter;
    mStartDistance = mCurrentDistance;
    
    mButtonState = NONE;
}


void Camera::ArcBallRotation(int x, int y)
{
    float sx, sy, sz, ex, ey, ez;
    float scale;
    float sl, el;
    float dotprod;
    
    // find vectors from center of window
    sx = mStartClick[0] - ( mDimensions[0] / 2.f );
    sy = mStartClick[1] - ( mDimensions[1] / 2.f );
    ex = x - ( mDimensions[0] / 2.f );
    ey = y - ( mDimensions[1] / 2.f );
    
    // invert y coordinates (raster versus device coordinates)
    sy = -sy;
    ey = -ey;
    
    // scale by inverse of size of window and magical sqrt2 factor
    if (mDimensions[0] > mDimensions[1]) {
        scale = (float) mDimensions[1];
    } else {
        scale = (float) mDimensions[0];
    }

    scale = 1.f / scale;
    
    sx *= scale;
    sy *= scale;
    ex *= scale;
    ey *= scale;

    // project points to unit circle
    sl = hypot(sx, sy);
    el = hypot(ex, ey);
    
    if (sl > 1.f) {
        sx /= sl;
        sy /= sl;
        sl = 1.0;
    }
    if (el > 1.f) {
        ex /= el;
        ey /= el;
        el = 1.f;
    }
    
    // project up to unit sphere - find Z coordinate
    sz = sqrt(1.0f - sl * sl);
    ez = sqrt(1.0f - el * el);
    
    // rotate (sx,sy,sz) into (ex,ey,ez)
    
    // compute angle from dot-product of unit vectors (and double it).
    // compute axis from cross product.
    dotprod = sx * ex + sy * ey + sz * ez;

    if( dotprod != 1 )
    {
        Vector3f axis( sy * ez - ey * sz, sz * ex - ez * sx, sx * ey - ex * sy );
        axis.normalize();
        
        float angle = 2.0f * acos( dotprod );

        mCurrentRot = Matrix4f::rotation( axis, angle );
        mCurrentRot = mCurrentRot * mStartRot;
    }
    else
    {
        mCurrentRot = mStartRot;
    }


}

void Camera::PlaneTranslation(int x, int y)
{
    // map window x,y into viewport x,y

    // start
    int sx = mStartClick[0] - mViewport[0];
    int sy = mStartClick[1] - mViewport[1];

    // current
    int cx = x - mViewport[0];
    int cy = y - mViewport[1];


    // compute "distance" of image plane (wrt projection matrix)
    float d = float(mViewport[3])/2.0f / tan(mPerspective[0]*c_pi / 180.0f / 2.0f);

    // compute up plane intersect of clickpoint (wrt fovy)
    float su = -sy + mViewport[3]/2.0f;
    float cu = -cy + mViewport[3]/2.0f;

    // compute right plane intersect of clickpoint (ASSUMED FOVY is 1)
    float sr = (sx - mViewport[2]/2.0f);
    float cr = (cx - mViewport[2]/2.0f);

    Vector2f move(cr-sr, cu-su);

    // this maps move
    move *= -mCurrentDistance/d;

    mCurrentCenter = mStartCenter +
        + move[0] * Vector3f(mCurrentRot(0,0),mCurrentRot(0,1),mCurrentRot(0,2))
        + move[1] * Vector3f(mCurrentRot(1,0),mCurrentRot(1,1),mCurrentRot(1,2));

}

void Camera::ApplyViewport() const
{
    glViewport(mViewport[0],mViewport[1],mViewport[2],mViewport[3]);
}

Matrix4f Camera::GetPerspective() const
{
	return Matrix4f::perspectiveProjection(mPerspective[0]*c_pi / 180.0f, mPerspective[1], 1.0, 1000.0);
}


Matrix4f Camera::GetViewMatrix() const
{
	Vector3f eye(0, 0, mCurrentDistance);
	Vector3f center(0, 0, 0);
	Vector3f up(0, 1, 0);
	Matrix4f ret = Matrix4f::lookAt(eye, center, up);
	return ret;

	/*
    // back up distance
    gluLookAt(0,0,mCurrentDistance,
              0,0,0,
              0.0, 1.0, 0.0);

    // rotate object
    glMultMatrixf(mCurrentRot);

    //translate object to center
    glTranslatef(-mCurrentCenter[0],-mCurrentCenter[1],-mCurrentCenter[2]);    
	*/
}
Matrix4f Camera::GetModelMatrix() const
{
	Matrix4f ret = mCurrentRot * Matrix4f::translation(mCurrentCenter);
	return ret;
}

void Camera::SetUniforms(uint32_t program) const
{
	int loc = glGetUniformLocation(program, "P");
	glUniformMatrix4fv(loc, 1, false, GetPerspective());

	loc = glGetUniformLocation(program, "V");
	glUniformMatrix4fv(loc, 1, false, GetViewMatrix());

	loc = glGetUniformLocation(program, "camPos");
	Vector3f eye(0, 0, mCurrentDistance);
	glUniform3fv(loc, 1, eye);

	Matrix4f M = GetModelMatrix();
	loc = glGetUniformLocation(program, "M");
	glUniformMatrix4fv(loc, 1, false, M);

	Matrix4f N = M.inverse().transposed();
	loc = glGetUniformLocation(program, "N");
	glUniformMatrix4fv(loc, 1, false, N);
}

void Camera::DistanceZoom(int x, int y)
{
    int sy = mStartClick[1] - mViewport[1];
    int cy = y - mViewport[1];

    float delta = float(cy-sy)/mViewport[3];

    // exponential zoom factor
    mCurrentDistance = mStartDistance * exp(delta);  
}

