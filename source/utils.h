#pragma once

typedef struct GelatoTransform
{
    float _position[3];
    float _scale[2];
    float _angle_degrees;
} GelatoTransform;

void gelato_mul_matrix(float* left, float* right, float* dest);
void gelato_mul_vec_matrix(float* vec, float* matrix, float* dest);
void gelato_make_scale_matrix(float x, float y, float z, float* dest);
void gelato_make_translation_matrix(float x, float y, float z, float* dest);
void gelato_make_rotation_matrix(float angle_in_degrees, float* dest);
void gelato_make_identity_matrix(float* dest);
void gelato_make_projection_matrix(float left, float right, float bottom, float top, float near_plane, float far_plane, float* dest);
void gelato_make_transformation(GelatoTransform* transform, float* dest);
void gelato_make_camera_transformation(GelatoTransform* transform, float* dest);