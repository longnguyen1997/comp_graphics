#include "matrixstack.h"

MatrixStack::MatrixStack()
{
	// Initialize the matrix stack with the identity matrix.

}

void MatrixStack::clear()
{
	// Revert to just containing the identity matrix.

}

Matrix4f MatrixStack::top()
{
	// Return the top of the stack
	// return Matrix4f();

    return Matrix4f::identity();
}

void MatrixStack::push( const Matrix4f& m )
{
	// Push m onto the stack.
	// The new top should be "old * m", so that conceptually the new matrix
    // is applied first in right-to-left evaluation.
}

void MatrixStack::pop()
{
	// Remove the top element from the stack

}
