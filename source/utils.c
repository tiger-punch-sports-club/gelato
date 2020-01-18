#include "utils.h"
#include <string.h> 
#include <math.h> 

/*  matrix: column-major, right-hand-side  */
/* 00 => 0 | 10 => 4 | 20 => 8  | 30 => 12 */ 
/* 01 => 1 | 11 => 5 | 21 => 9  | 31 => 13 */ 
/* 02 => 2 | 12 => 6 | 22 => 10 | 32 => 14 */ 
/* 03 => 3 | 13 => 7 | 23 => 11 | 33 => 15 */ 

#define PI 3.14159265358979323846
void gelato_mul_matrix(float* left, float* right, float* dest)
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

void gelato_mul_vec_matrix(float* vec, float* matrix, float* dest)
{
    dest[0] = matrix[0] * vec[0] + matrix[4] * vec[1] + matrix[8] * vec[2] + matrix[12];
    dest[1] = matrix[1] * vec[0] + matrix[5] * vec[1] + matrix[9] * vec[2] + matrix[13];
    dest[2] = matrix[2] * vec[0] + matrix[6] * vec[1] + matrix[10] * vec[2] + matrix[14];
}

void gelato_make_scale_matrix(float x, float y, float z, float* dest)
{
    dest[0] = x;
	dest[5] = y;
	dest[10] = z;
}

void gelato_make_translation_matrix(float x, float y, float z, float* dest)
{
    dest[12] = x;
    dest[13] = y;
    dest[14] = z;
}

void gelato_make_rotation_matrix(float x, float y, float z, float* dest)
{
    float x_r = (x / 180.0f) * PI;
    float y_r = (y / 180.0f) * PI;
    float z_r = (z / 180.0f) * PI;

    // To Quaternion
    float c1 = cosf(y_r * 0.5f);
    float s1 = sinf(y_r * 0.5f);
                                    
    float c2 = cosf(z_r * 0.5f);
    float s2 = sinf(z_r * 0.5f);
                                    
    float c3 = cosf(x_r * 0.5f);
    float s3 = sinf(x_r * 0.5f);

    float c1c2 = c1 * c2;
    float s1s2 = s1 * s2;

    float qw	= c1c2 * c3 - s1s2 * s3;
    float qx = c1c2 * s3 + s1s2 * c3;
    float qy = s1 * c2 * c3 + c1 * s2 * s3;
    float qz = c1 * s2 * c3 - s1 * c2 * s3;

    float forward[3] = { 2.0f * (qx * qz - qw * qy), 2.0f * (qy * qz + qw * qx), 1.0f - 2.0f * (qx * qx + qy * qy) };
    float up[3] = { 2.0f * (qx * qy + qw * qz), 1.0f - 2.0f * (qx * qx + qz * qz), 2.0f * (qy * qz - qw * qx) };
    float right[3] = { 1.0f - 2.0f * (qy * qy + qz * qz), 2.0f * (qx * qy - qw * qz), 2.0f * (qx * qz + qw * qy) };

    dest[0] = right[0];    dest[4] = right[1];    dest[8] = right[2];     dest[12] = 0.0f;
    dest[1] = up[0];       dest[5] = up[1];       dest[9] = up[2];        dest[13] = 0.0f;
    dest[2] = forward[0];  dest[6] = forward[1];  dest[10] = forward[2];  dest[14] = 0.0f;
    dest[3] = 0.0f;        dest[7] = 0.0f;        dest[11] = 0.0f;        dest[15] = 1.0f;
}

void gelato_make_identity_matrix(float* dest)
{
    memset(dest, 0, sizeof(float) * 16);
    dest[0] = dest[5] = dest[10] = dest[15] = 1.0f;
}

void gelato_make_projection_matrix(float left, float right, float bottom, float top, float near_plane, float far_plane, float* dest)
{
    gelato_make_identity_matrix(dest);

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

void gelato_make_transformation(GelatoTransform* transform, float* dest)
{
    gelato_make_identity_matrix(dest);

    float translation_matrix[16];
    gelato_make_identity_matrix(&translation_matrix[0]);
    gelato_make_translation_matrix(transform->_position[0], transform->_position[1], transform->_position[2], &translation_matrix[0]);

    float scale_matrix[16];
    gelato_make_identity_matrix(&scale_matrix[0]);
    gelato_make_scale_matrix(transform->_scale[0], transform->_scale[1], 1.0f, &scale_matrix[0]);

    float rotation_matrix[16];
    gelato_make_identity_matrix(&rotation_matrix[0]);
    gelato_make_rotation_matrix(transform->_rotation[0], transform->_rotation[1], transform->_rotation[2], &rotation_matrix[0]);

    float tr_matrix[16];
    gelato_make_identity_matrix(&tr_matrix[0]);
    gelato_mul_matrix(&translation_matrix[0], &rotation_matrix[0], &tr_matrix[0]);

    gelato_mul_matrix(&tr_matrix[0], &scale_matrix[0], dest);
}

void gelato_make_camera_transformation(GelatoTransform* transform, float* dest)
{
    float transformation[16];
    gelato_make_transformation(transform, &transformation[0]);

    gelato_make_identity_matrix(dest);

    dest[0] = transformation[0];
    dest[1] = transformation[4];
    dest[2] = transformation[8];
    dest[4] = transformation[1];
    dest[5] = transformation[5];
    dest[6] = transformation[9];
    dest[8] = transformation[2];
    dest[9] = transformation[6];
    dest[10] = transformation[10];
    
    float translation[3] = { 0 };
    gelato_mul_vec_matrix(&transform->_position[0], dest, &translation[0]);

    gelato_make_translation_matrix(-translation[0], -translation[1], -translation[2], dest);
}