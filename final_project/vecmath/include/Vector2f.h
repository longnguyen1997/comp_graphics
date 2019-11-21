#ifndef VECTOR_2F_H
#define VECTOR_2F_H

#include <cmath>

class Vector3f;

class Vector2f
{
public:
    
    static const Vector2f ZERO;
	static const Vector2f UP;
	static const Vector2f RIGHT;

    explicit Vector2f( float f = 0.f );
    Vector2f( float x, float y );

	// copy constructors
    Vector2f( const Vector2f& rv );

	// assignment operators
	Vector2f& operator = ( const Vector2f& rv );

	// no destructor necessary

	// returns the ith element
    const float& operator [] ( int i ) const;
	float& operator [] ( int i );

    float& x();
	float& y();

	float x() const;
	float y() const;

    Vector2f xy() const;
	Vector2f yx() const;
	Vector2f xx() const;
	Vector2f yy() const;

	// returns ( -y, x )
    Vector2f normal() const;

    float abs() const;
    float absSquared() const;
    void normalize();
    Vector2f normalized() const;

    void negate();

	// ---- Utility ----
    operator const float* () const; // automatic type conversion for OpenGL 
    operator float* (); // automatic type conversion for OpenGL 
	void print() const;

	Vector2f& operator += ( const Vector2f& v );
	Vector2f& operator -= ( const Vector2f& v );
	Vector2f& operator *= ( float f );

    static float dot( const Vector2f& v0, const Vector2f& v1 );

	static Vector3f cross( const Vector2f& v0, const Vector2f& v1 );

	// returns v0 * ( 1 - alpha ) * v1 * alpha
	static Vector2f lerp( const Vector2f& v0, const Vector2f& v1, float alpha );

private:

	float m_elements[2];

};

// component-wise operators
Vector2f operator + ( const Vector2f& v0, const Vector2f& v1 );
Vector2f operator - ( const Vector2f& v0, const Vector2f& v1 );
Vector2f operator * ( const Vector2f& v0, const Vector2f& v1 );
Vector2f operator / ( const Vector2f& v0, const Vector2f& v1 );

// unary negation
Vector2f operator - ( const Vector2f& v );

// multiply and divide by scalar
Vector2f operator * ( float f, const Vector2f& v );
Vector2f operator * ( const Vector2f& v, float f );
Vector2f operator / ( const Vector2f& v, float f );

bool operator == ( const Vector2f& v0, const Vector2f& v1 );
bool operator != ( const Vector2f& v0, const Vector2f& v1 );

#endif // VECTOR_2F_H
