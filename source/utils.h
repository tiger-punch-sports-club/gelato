#pragma once

typedef struct Transform
{
    float _position[3];
    float _scale[2];
    float _rotation;
    float _uv_scale[2];
    float _uv_offset[2];
} Transform;

void mul_matrix(float* left, float* right, float* dest);
void make_scale_matrix(float x, float y, float z, float* dest);
void make_identity_matrix(float* dest);