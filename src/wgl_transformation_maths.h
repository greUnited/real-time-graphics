// 1D array or 2D array, try 2D for now
// [r]ow [c]olumn -- column major
typedef float vec3f[3];
typedef float vec4f[4];
typedef float mat4f[4][4];

// TODO: Set out the needed transformation functions

// Affine transformations
void
m_translate(mat4f mat, vec3f);
