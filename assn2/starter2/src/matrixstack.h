#ifndef MATRIX_STACK_H
#define MATRIX_STACK_H

#include <vector>
#include <vecmath.h>

class MatrixStack
{
public:
	MatrixStack();
	void clear();
	Matrix4f top();
	void push( const Matrix4f& m );
	void pop();

private:
	std::vector< Matrix4f > m_matrices;
};

#endif // MATRIX_STACK_H
