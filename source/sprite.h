#pragma once
#include "texture.h"
#include "utils.h"

typedef struct GelatoSprite
{
    GelatoTextureId _texture;
    GelatoTransform _transform;
    float _uv_scale[2];
    float _uv_offset[2];
    float _color[4];
} GelatoSprite;

GelatoSprite gelato_create_sprite(GelatoTextureId texture);