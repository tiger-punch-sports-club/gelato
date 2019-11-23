#pragma once
#include "texture.h"
#include "utils.h"

typedef struct Sprite
{
    TextureId _texture;
    Transform _transform;
} Sprite;

Sprite create_sprite(TextureId texture);