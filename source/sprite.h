#pragma once
#include "texture.h"
#include "utils.h"

typedef struct Sprite
{
    TextureId _texture;
    GelatoTransform _transform;
    float _uv_scale[2];
    float _uv_offset[2];
} Sprite;

Sprite create_sprite(TextureId texture);