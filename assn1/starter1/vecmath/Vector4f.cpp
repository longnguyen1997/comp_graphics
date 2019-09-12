#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "Vector4f.h"
#include "Vector2f.h"
#include "Vector3f.h"

Vector4f::Vector4f( float f )
{
	m_elements[ 0 ] = f;
	m_elements[ 1 ] = f;
	m_elements[ 2 ] = f;
	m_elements[ 3 ] = f;
}

Vector4f::Vector4f( float fx, float fy, float fz, float fw )
{
	m_elements[0] = fx;
	m_elements[1] = fy;
	m_elements[2] = fz;
	m_elements[3] = fw;
}

Vector4f::Vector4f( float buffer[ 4 ] )
{
	m_elements[ 0 ] = buffer[ 0 ];
	m_elements[ 1 ] = buffer[ 1 ];
	m_elements[ 2 ] = buffer[ 2 ];
	m_elements[ 3 ] = buffer[ 3 ];
}

Vector4f::Vector4f( const Vector2f& xy, float z, float w )
{
	m_elements[0] = xy.x();
	m_elements[1] = xy.y();
	m_elements[2] = z;
	m_elements[3] = w;
}

Vector4f::Vector4f( float x, const Vector2f& yz, float w )
{
	m_elements[0] = x;
	m_elements[1] = yz.x();
	m_elements[2] = yz.y();
	m_elements[3] = w;
}

Vector4f::Vector4f( float x, float y, const Vector2f& zw )
{
	m_elements[0] = x;
	m_elements[1] = y;
	m_elements[2] = zw.x();
	m_elements[3] = zw.y();
}

Vector4f::Vector4f( const Vector2f& xy, const Vector2f& zw )
{
	m_elements[0] = xy.x();
	m_elements[1] = xy.y();
	m_elements[2] = zw.x();
	m_elements[3] = zw.y();
}

Vector4f::Vector4f( const Vector3f& xyz, float w )
{
	m_elements[0] = xyz.x();
	m_elements[1] = xyz.y();
	m_elements[2] = xyz.z();
	m_elements[3] = w;
}

Vector4f::Vector4f( float x, const Vector3f& yzw )
{
	m_elements[0] = x;
	m_elements[1] = yzw.x();
	m_elements[2] = yzw.y();
	m_elements[3] = yzw.z();
}

Vector4f::Vector4f( const Vector4f& rv )
{
	m_elements[0] = rv.m_elements[0];
	m_elements[1] = rv.m_elements[1];
	m_elements[2] = rv.m_elements[2];
	m_elements[3] = rv.m_elements[3];
}

Vector4f& Vector4f::operator = ( const Vector4f& rv )
{
	if( this != &rv )
	{
		m_elements[0] = rv.m_elements[0];
		m_elements[1] = rv.m_elements[1];
		m_elements[2] = rv.m_elements[2];
		m_elements[3] = rv.m_elements[3];
	}
	return *this;
}

const float& Vector4f::operator [] ( int i ) const
{
	return m_elements[ i ];
}

float& Vector4f::operator [] ( int i )
{
	return m_elements[ i ];
}

float& Vector4f::x()
{
	return m_elements[ 0 ];
}

float& Vector4f::y()
{
	return m_elements[ 1 ];
}

float& Vector4f::z()
{
	return m_elements[ 2 ];
}

float& Vector4f::w()
{
	return m_elements[ 3 ];
}

float Vector4f::x() const
{
	return m_elements[0];
}

float Vector4f::y() const
{
	return m_elements[1];
}

float Vector4f::z() const
{
	return m_elements[2];
}

float Vector4f::w() const
{
	return m_elements[3];
}

Vector2f Vector4f::xy() const
{
	return Vector2f( m_elements[0], m_elements[1] );
}

Vector2f Vector4f::yz() const
{
	return Vector2f( m_elements[1], m_elements[2] );
}

Vector2f Vector4f::zw() const
{
	return Vector2f( m_elements[2], m_elements[3] );
}

Vector2f Vector4f::wx() const
{
	return Vector2f( m_elements[3], m_elements[0] );
}

Vector3f Vector4f::xyz() const
{
	return Vector3f( m_elements[0], m_elements[1], m_elements[2] );
}

Vector3f Vector4f::yzw() const
{
	return Vector3f( m_elements[1], m_elements[2], m_elements[3] );
}

Vector3f Vector4f::zwx() const
{
	return Vector3f( m_elements[2], m_elements[3], m_elements[0] );
}

Vector3f Vector4f::wxy() const
{
	return Vector3f( m_elements[3], m_elements[0], m_elements[1] );
}

Vector3f Vector4f::xyw() const
{
	return Vector3f( m_elements[0], m_elements[1], m_elements[3] );
}

Vector3f Vector4f::yzx() const
{
	return Vector3f( m_elements[1], m_elements[2], m_elements[0] );
}

Vector3f Vector4f::zwy() const
{
	return Vector3f( m_elements[2], m_elements[3], m_elements[1] );
}

Vector3f Vector4f::wxz() const
{
	return Vector3f( m_elements[3], m_elements[0], m_elements[2] );
}

float Vector4f::abs() const
{
	return sqrt( m_elements[0] * m_elements[0] + m_elements[1] * m_elements[1] + m_elements[2] * m_elements[2] + m_elements[3] * m_elements[3] );
}

float Vector4f::absSquared() const
{
	return( m_elements[0] * m_elements[0] + m_elements[1] * m_elements[1] + m_elements[2] * m_elements[2] + m_elements[3] * m_elements[3] );
}

void Vector4f::normalize()
{
	float norm = sqrt( m_elements[0] * m_elements[0] + m_elements[1] * m_elements[1] + m_elements[2] * m_elements[2] + m_elements[3] * m_elements[3] );
	m_elements[0] = m_elements[0] / norm;
	m_elements[1] = m_elements[1] / norm;
	m_elements[2] = m_elements[2] / norm;
	m_elements[3] = m_elements[3] / norm;
}

Vector4f Vector4f::normalized() const
{
	float length = abs();
	return Vector4f
		(
			m_elements[0] / length,
			m_elements[1] / length,
			m_elements[2] / length,
			m_elements[3] / length
		);
}

void Vector4f::homogenize()
{
	if( m_elements[3] != 0 )
	{
		m_elements[0] /= m_elements[3];
		m_elements[1] /= m_elements[3];
		m_elements[2] /= m_elements[3];
		m_elements[3] = 1;
	}
}

Vector4f Vector4f::homogenized() const
{
	if( m_elements[3] != 0 )
	{
		return Vector4f
			(
				m_elements[0] / m_elements[3],
				m_elements[1] / m_elements[3],
				m_elements[2] / m_elements[3],
				1
			);
	}
	else
	{
		return Vector4f
			(
				m_elements[0],
				m_elements[1],
				m_elements[2],
				m_elements[3]
			);
	}
}

void Vector4f::negate()
{
	m_elements[0] = -m_elements[0];
	m_elements[1] = -m_elements[1];
	m_elements[2] = -m_elements[2];
	m_elements[3] = -m_elements[3];
}

Vector4f::operator const float* () const
{
	return m_elements;
}

Vector4f::operator float* ()
{
	return m_elements;
}

void Vector4f::print() const
{
	printf( "< %.4f, %.4f, %.4f, %.4f >\n",
		m_elements[0], m_elements[1], m_elements[2], m_elements[3] );
}

// static
float Vector4f::dot( const Vector4f& v0, const Vector4f& v1 )
{
	return v0.x() * v1.x() + v0.y() * v1.y() + v0.z() * v1.z() + v0.w() * v1.w();
}

// static
Vector4f Vector4f::lerp( const Vector4f& v0, const Vector4f& v1, float alpha )
{
	return alpha * ( v1 - v0 ) + v0;
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////

Vector4f operator + ( const Vector4f& v0, const Vector4f& v1 )
{
	return Vector4f( v0.x() + v1.x(), v0.y() + v1.y(), v0.z() + v1.z(), v0.w() + v1.w() );
}

Vector4f operator - ( const Vector4f& v0, const Vector4f& v1 )
{
	return Vector4f( v0.x() - v1.x(), v0.y() - v1.y(), v0.z() - v1.z(), v0.w() - v1.w() );
}

Vector4f operator * ( const Vector4f& v0, const Vector4f& v1 )
{
	return Vector4f( v0.x() * v1.x(), v0.y() * v1.y(), v0.z() * v1.z(), v0.w() * v1.w() );
}

Vector4f operator / ( const Vector4f& v0, const Vector4f& v1 )
{
	return Vector4f( v0.x() / v1.x(), v0.y() / v1.y(), v0.z() / v1.z(), v0.w() / v1.w() );
}

Vector4f operator - ( const Vector4f& v )
{
	return Vector4f( -v.x(), -v.y(), -v.z(), -v.w() );
}

Vector4f operator * ( float f, const Vector4f& v )
{
	return Vector4f( f * v.x(), f * v.y(), f * v.z(), f * v.w() );
}

Vector4f operator * ( const Vector4f& v, float f )
{
	return Vector4f( f * v.x(), f * v.y(), f * v.z(), f * v.w() );
}

Vector4f operator / ( const Vector4f& v, float f )
{
    return Vector4f( v[0] / f, v[1] / f, v[2] / f, v[3] / f );
}

bool operator == ( const Vector4f& v0, const Vector4f& v1 )
{
    return( v0.x() == v1.x() && v0.y() == v1.y() && v0.z() == v1.z() && v0.w() == v1.w() );
}

bool operator != ( const Vector4f& v0, const Vector4f& v1 )
{
    return !( v0 == v1 );
}
