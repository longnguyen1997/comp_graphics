#ifndef MATRIX2F_H
#define MATRIX2F_H

#include <cstdio>

class Vector2f;

// 2x2 Matrix, stored in column major order (OpenGL style)
class Matrix2f
{
public:

    // Fill a 2x2 matrix with "fill", default to 0.
	Matrix2f( float fill = 0.f );
	Matrix2f( float m00, float m01,
		float m10, float m11 );

	// setColumns = true ==> sets the columns of the matrix to be [v0 v1]
	// otherwise, sets the rows
	Matrix2f( const Vector2f& v0, const Vector2f& v1, bool setColumns = true );

	Matrix2f( const Matrix2f& rm ); // copy constructor
	Matrix2f& operator = ( const Matrix2f& rm ); // assignment operator
	// no destructor necessary

	const float& operator () ( int i, int j ) const;
	float& operator () ( int i, int j );

	Vector2f getRow( int i ) const;
	void setRow( int i, const Vector2f& v );

	Vector2f getCol( int j ) const;
	void setCol( int j, const Vector2f& v );

	float determinant();
	Matrix2f inverse( bool* pbIsSingular = NULL, float epsilon = 0.f );

	void transpose();
	Matrix2f transposed() const;

	// ---- Utility ----
	operator float* (); // automatic type conversion for GL
	void print();

	static float determinant2x2( float m00, float m01,
		float m10, float m11 );

	static Matrix2f ones();
	static Matrix2f identity();
	static Matrix2f rotation( float degrees );

private:

	float m_elements[ 4 ];

};

// Scalar-Matrix multiplication
Matrix2f operator * ( float f, const Matrix2f& m );
Matrix2f operator * ( const Matrix2f& m, float f );

// Matrix-Vector multiplication
// 2x2 * 2x1 ==> 2x1
Vector2f operator * ( const Matrix2f& m, const Vector2f& v );

// Matrix-Matrix multiplication
Matrix2f operator * ( const Matrix2f& x, const Matrix2f& y );

#endif // MATRIX2F_H
