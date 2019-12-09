#pragma once
#include "texture.h"
#include "utils.h"

typedef struct GelatoSprite
{
    GelatoTextureId _texture;
    GelatoTransform _transform;
    float _uv_scale[2];
    float _uv_start[2];
    float _uv_end[2];
    float _color[4];
} GelatoSprite;

typedef struct GelatoSpriteSheetDescription
{
    uint32 _width;
    uint32 _height;
    uint32 _tile_size_x;
    uint32 _tile_size_y;
} GelatoSpriteSheetDescription;

typedef struct GelatoSpriteSheetInfo
{
    GelatoTextureId _texture_id;
    
    uint32 _tiles_count_u;
    uint32 _tiles_count_v;

    float _u_step;
    float _v_step;
} GelatoSpriteSheetInfo;

GelatoSprite gelato_create_sprite(GelatoTextureId texture);
GelatoSpriteSheetInfo gelato_create_sprite_sheet(GelatoSpriteSheetDescription description, GelatoTextureId texture);
void gelato_change_sprite_tile(GelatoSprite* sprite, uint32 u_index, uint32 v_index, GelatoSpriteSheetInfo info);