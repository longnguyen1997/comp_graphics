#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "Vector2f.h"
#include "Vector3f.h"

//////////////////////////////////////////////////////////////////////////
// Public
//////////////////////////////////////////////////////////////////////////

// static
const Vector2f Vector2f::ZERO = Vector2f( 0, 0 );

// static
const Vector2f Vector2f::UP = Vector2f( 0, 1 );

// static
const Vector2f Vector2f::RIGHT = Vector2f( 1, 0 );

Vector2f::Vector2f( float f )
{
    m_elements[0] = f;
    m_elements[1] = f;
}

Vector2f::Vector2f( float x, float y )
{
    m_elements[0] = x;
    m_elements[1] = y;
}

Vector2f::Vector2f( const Vector2f& rv )
{
    m_elements[0] = rv[0];
    m_elements[1] = rv[1];
}

Vector2f& Vector2f::operator = ( const Vector2f& rv )
{
 	if( this != &rv )
	{
        m_elements[0] = rv[0];
        m_elements[1] = rv[1];
    }
    return *this;
}

const float& Vector2f::operator [] ( int i ) const
{
    return m_elements[i];
}

float& Vector2f::operator [] ( int i )
{
    return m_elements[i];
}

float& Vector2f::x()
{
    return m_elements[0];
}

float& Vector2f::y()
{
    return m_elements[1];
}

float Vector2f::x() const
{
    return m_elements[0];
}	

float Vector2f::y() const
{
    return m_elements[1];
}

Vector2f Vector2f::xy() const
{
    return *this;
}

Vector2f Vector2f::yx() const
{
    return Vector2f( m_elements[1], m_elements[0] );
}

Vector2f Vector2f::xx() const
{
    return Vector2f( m_elements[0], m_elements[0] );
}

Vector2f Vector2f::yy() const
{
    return Vector2f( m_elements[1], m_elements[1] );
}

Vector2f Vector2f::normal() const
{
    return Vector2f( -m_elements[1], m_elements[0] );
}

float Vector2f::abs() const
{
    return sqrt(absSquared());
}

float Vector2f::absSquared() const
{
    return m_elements[0] * m_elements[0] + m_elements[1] * m_elements[1];
}

void Vector2f::normalize()
{
    float norm = abs();
    m_elements[0] /= norm;
    m_elements[1] /= norm;
}

Vector2f Vector2f::normalized() const
{
    float norm = abs();
    return Vector2f( m_elements[0] / norm, m_elements[1] / norm );
}

void Vector2f::negate()
{
    m_elements[0] = -m_elements[0];
    m_elements[1] = -m_elements[1];
}

Vector2f::operator const float* () const
{
    return m_elements;
}

Vector2f::operator float* ()
{
    return m_elements;
}

void Vector2f::print() const
{
	printf( "< %.4f, %.4f >\n",
		m_elements[0], m_elements[1] );
}

Vector2f& Vector2f::operator += ( const Vector2f& v )
{
	m_elements[ 0 ] += v.m_elements[ 0 ];
	m_elements[ 1 ] += v.m_elements[ 1 ];
	return *this;
}

Vector2f& Vector2f::operator -= ( const Vector2f& v )
{
	m_elements[ 0 ] -= v.m_elements[ 0 ];
	m_elements[ 1 ] -= v.m_elements[ 1 ];
	return *this;
}

Vector2f& Vector2f::operator *= ( float f )
{
	m_elements[ 0 ] *= f;
	m_elements[ 1 ] *= f;
	return *this;
}

// static
float Vector2f::dot( const Vector2f& v0, const Vector2f& v1 )
{
    return v0[0] * v1[0] + v0[1] * v1[1];
}

// static
Vector3f Vector2f::cross( const Vector2f& v0, const Vector2f& v1 )
{
	return Vector3f
		(
			0,
			0,
			v0.x() * v1.y() - v0.y() * v1.x()
		);
}

// static
Vector2f Vector2f::lerp( const Vector2f& v0, const Vector2f& v1, float alpha )
{
	return alpha * ( v1 - v0 ) + v0;
}

//////////////////////////////////////////////////////////////////////////
// Operator overloading
//////////////////////////////////////////////////////////////////////////

Vector2f operator + ( const Vector2f& v0, const Vector2f& v1 )
{
    return Vector2f( v0.x() + v1.x(), v0.y() + v1.y() );
}

Vector2f operator - ( const Vector2f& v0, const Vector2f& v1 )
{
    return Vector2f( v0.x() - v1.x(), v0.y() - v1.y() );
}

Vector2f operator * ( const Vector2f& v0, const Vector2f& v1 )
{
    return Vector2f( v0.x() * v1.x(), v0.y() * v1.y() );
}

Vector2f operator / ( const Vector2f& v0, const Vector2f& v1 )
{
    return Vector2f( v0.x() * v1.x(), v0.y() * v1.y() );
}

Vector2f operator - ( const Vector2f& v )
{
    return Vector2f( -v.x(), -v.y() );
}

Vector2f operator * ( float f, const Vector2f& v )
{
    return Vector2f( f * v.x(), f * v.y() );
}

Vector2f operator * ( const Vector2f& v, float f )
{
    return Vector2f( f * v.x(), f * v.y() );
}

Vector2f operator / ( const Vector2f& v, float f )
{
    return Vector2f( v.x() / f, v.y() / f );
}

bool operator == ( const Vector2f& v0, const Vector2f& v1 )
{
    return( v0.x() == v1.x() && v0.y() == v1.y() );
}

bool operator != ( const Vector2f& v0, const Vector2f& v1 )
{
    return !( v0 == v1 );
}
