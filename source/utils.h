#pragma once

typedef struct Transform
{
    float _position[3];
    float _scale[2];
    float _rotation;
} Transform;

void mul_matrix(float* left, float* right, float* dest);
void make_scale_matrix(float x, float y, float z, float* dest);
void make_translation_matrix(float x, float y, float z, float* dest);
void make_rotation_matrix(float angle_in_degrees, float* dest);
void make_identity_matrix(float* dest);
void make_projection_matrix(float left, float right, float bottom, float top, float near_plane, float far_plane, float* dest);
void make_transformation(Transform* transform, float* dest);
void make_camera_transformation(Transform* transform, float* dest);