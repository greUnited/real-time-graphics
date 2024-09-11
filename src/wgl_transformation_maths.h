#ifndef WGL_TRANSFORMATION_MATHS_H
#define WGL_TRANSFORMATION_MATHS_H

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define DEG_TO_RAD M_PI / 180.0

// 1D array or 2D array, try 2D for now
// [r]ow [c]olumn -- column major
typedef float vec3f[3];
typedef float vec4f[4];
typedef float mat4f[4][4];

// TODO: Set out the needed transformation functions

float
vec3f_magnitude(vec3f vec);

void
vec3f_normalize(vec3f vec);

// Affine transformations
void
m_translate(mat4f mat, vec3f vec);

void
m_scale(mat4f mat, vec3f vec);

void
m_set_scale(mat4f mat, vec3f vec);

void
m_rotate_x(mat4f mat, float angle);

void
m_rotate_y(mat4f mat, float angle);

void
m_rotate_x(mat4f mat, float angle);

void
m_rotate_y(mat4f mat, float angle);

void
m_rotate_z(mat4f mat, float angle);

void
m_rotate_axis(mat4f mat, vec3f axis, float angle);

void
m_view_ortho(mat4f mat, float view_left, float view_right, float view_bottom, float view_top, float plane_near, float plane_far);

void
m_view_perspective(mat4f mat, float fov_y, float aspect_ratio, float frustum_front, float frustum_back);

void
m_mat_zero(mat4f mat);

void
m_mat_identity(mat4f mat);

#endif
