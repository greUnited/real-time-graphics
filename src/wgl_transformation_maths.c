#include "wgl_transformation_maths.h"
// TODO: Set out the needed transformation functions
float
vec3f_magnitude(vec3f vec)
{
	return sqrtf(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
}

void
vec3f_normalize(vec3f vec)
{
	float vec_magnitude = vec3f_magnitude(vec);
	vec[0] = vec[0] / vec_magnitude;
	vec[1] = vec[1] / vec_magnitude;
	vec[2] = vec[2] / vec_magnitude;
}

// Affine transformations
void
m_translate(mat4f mat, vec3f vec)
{
	mat[3][0] = vec[0];
	mat[3][1] = vec[1];
	mat[3][2] = vec[2];
}

void
m_scale(mat4f mat, vec3f vec)
{
	mat[0][0] += vec[0];
	mat[1][1] += vec[1];
	mat[2][2] += vec[2];
}

void
m_set_scale(mat4f mat, vec3f vec)
{
	mat[0][0] = vec[0];
	mat[1][1] = vec[1];
	mat[2][2] = vec[2];
}

void
m_rotate_x(mat4f mat, float angle)
{
	mat[1][1] = cosf(angle); mat[1][2] = -sinf(angle);
	mat[2][1] = sinf(angle); mat[2][2] = cosf(angle);
}

void
m_rotate_y(mat4f mat, float angle)
{
	mat[0][0] = cosf(angle); mat[0][2] = -sinf(angle);
	mat[2][0] = sinf(angle); mat[2][2] = cosf(angle);
}

void
m_rotate_z(mat4f mat, float angle)
{
	mat[0][0] = cosf(angle); mat[0][1] = -sinf(angle);
	mat[1][0] = sinf(angle); mat[1][1] = cosf(angle);
}

void
m_rotate_axis(mat4f mat, vec3f axis, float angle)
{
	// TODO
}

void
m_view_ortho(mat4f mat, float view_left, float view_right, float view_bottom, float view_top, float plane_near, float plane_far)
{
	m_mat_zero(mat);

	float diff_right_left, diff_top_bottom, diff_far_near;

	diff_right_left = 1.0f / (view_right - view_left);
	diff_top_bottom = 1.0f / (view_top - view_bottom);
	diff_far_near = -1.0f / (plane_far - plane_near);

	mat[0][0] = 2.0f * diff_right_left;
	mat[1][1] = 2.0f * diff_top_bottom;
	mat[2][2] = -diff_far_near;
	mat[3][0] = -(view_right + view_left) * diff_right_left;
	mat[3][1] = -(view_top + view_bottom) * diff_top_bottom;
	mat[3][2] = plane_near * diff_far_near;
	mat[3][3] = 1.0f;
}

void
m_view_perspective(mat4f mat, float fov_y, float aspect_ratio, float frustum_front, float frustum_back)
{
	float tangent = tanf(fov_y / 2 * DEG_TO_RAD);
	float frustum_top = frustum_front * tangent;
	float frustum_right = frustum_top * aspect_ratio;

	mat[0][0] = frustum_front / frustum_right;
	mat[1][1] = frustum_front / frustum_top;
	mat[2][2] = -(frustum_back + frustum_front) / (frustum_back - frustum_front);
	mat[2][3] = -1.0f;
	mat[3][2] = -(2 * frustum_back * frustum_front) / (frustum_back - frustum_front);
	mat[3][3] = 0.0f;
}

void
m_mat_zero(mat4f mat)
{
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			mat[i][j] = 0.0f;
		}
	}
}

void
m_mat_identity(mat4f mat)
{
	// TODO
}
