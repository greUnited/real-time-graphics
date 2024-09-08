#include "wgl_transformation_maths.h"
// TODO: Set out the needed transformation functions

// Affine transformations
void
m_translate(mat4f mat, vec3f vec)
{
	mat[3][0] = vec[0];
	mat[3][1] = vec[1];
	mat[3][2] = vec[2];
}
