#include "utils.h"
#include <string.h> 

/*  matrix: column-major, right-hand-side  */
/* 00 => 0 | 10 => 4 | 20 => 8  | 30 => 12 */ 
/* 01 => 1 | 11 => 5 | 21 => 9  | 31 => 13 */ 
/* 02 => 2 | 12 => 6 | 22 => 10 | 32 => 14 */ 
/* 03 => 3 | 13 => 7 | 23 => 11 | 33 => 15 */ 

void mul_matrix(float* left, float* right, float* dest)
{
    dest[0] = left[0] * right[0] + left[4] * right[1] + left[8] * right[2] + left[12] * right[3];
    dest[1] = left[1] * right[0] + left[5] * right[1] + left[9] * right[2] + left[13] * right[3];
    dest[2] = left[2] * right[0] + left[6] * right[1] + left[10] * right[2] + left[14] * right[3];
    dest[3] = left[3] * right[0] + left[7] * right[1] + left[11] * right[2] + left[15] * right[3];
 
    dest[4] = left[0] * right[4] + left[4] * right[5] + left[8] * right[6] + left[12] * right[7];
    dest[5] = left[1] * right[4] + left[5] * right[5] + left[9] * right[6] + left[13] * right[7];
    dest[6] = left[2] * right[4] + left[6] * right[5] + left[10] * right[6] + left[14] * right[7];
    dest[7] = left[3] * right[4] + left[7] * right[5] + left[11] * right[6] + left[15] * right[7];
 
    dest[8] = left[0] * right[8] + left[4] * right[9] + left[8] * right[10] + left[12] * right[11];
    dest[9] = left[1] * right[8] + left[5] * right[9] + left[9] * right[10] + left[13] * right[11];
    dest[10] = left[2] * right[8] + left[6] * right[9] + left[10] * right[10] + left[14] * right[11];
    dest[11] = left[3] * right[8] + left[7] * right[9] + left[11] * right[10] + left[15] * right[11];
 
    dest[12] = left[0] * right[12] + left[4] * right[13] + left[8] * right[14] + left[12] * right[15];
    dest[13] = left[1] * right[12] + left[5] * right[13] + left[9] * right[14] + left[13] * right[15];
    dest[14] = left[2] * right[12] + left[6] * right[13] + left[10] * right[14] + left[14] * right[15];
    dest[15] = left[3] * right[12] + left[7] * right[13] + left[11] * right[14] + left[15] * right[15];
}

void make_scale_matrix(float x, float y, float z, float* dest)
{
    dest[0] = x;
	dest[5] = y;
	dest[10] = z;
}

void make_identity_matrix(float* dest)
{
    memset(dest, 0, sizeof(float) * 16);
    dest[0] = dest[5] = dest[10] = dest[15] = 1.0f;
}

void make_projection_matrix(float* dest)
{
    make_identity_matrix(dest);

    float left = -1.0f;
    float right = 1.0f;
    float top = 1.0f;
    float bottom = -1.0f;
    float near_plane = -1.0f;
    float far_plane = 1.0f;

	float delta_x = right - left;
	float delta_y = top - bottom;
	float delta_z = far_plane - near_plane;

	dest[0] = 2.0f / delta_x;
    dest[12] = -(right + left) / delta_x;
	dest[5] = 2.0f / delta_y;
    dest[13] = -(top + bottom) / delta_y;
	dest[10] = -2.0f / delta_z;
    dest[14] = -(far_plane + near_plane) / delta_z;
}