#include "sprite.h"

GelatoSprite gelato_create_sprite(GelatoTextureId texture)
{
    GelatoSprite sprite =
    {
        ._texture = texture,
        ._transform =
        {
            ._position[0] = 0.0f,
            ._position[1] = 0.0f,
            ._position[2] = 0.0f,

            ._scale[0] = 1.0f,
            ._scale[1] = 1.0f,

            ._rotation[0] = 0.0f,
            ._rotation[1] = 0.0f,
            ._rotation[2] = 0.0f
        },
        ._uv_scale[0] = 1.0f,
        ._uv_scale[1] = 1.0f,

        ._uv_start[0] = 0.0f,
        ._uv_start[1] = 0.0f,

        ._uv_end[0] = 1.0f,
        ._uv_end[1] = 1.0f,

        ._color[0] = 1.0f,
        ._color[1] = 1.0f,
        ._color[2] = 1.0f,
        ._color[3] = 1.0f
    };

    return sprite;
}

GelatoSpriteSheetInfo gelato_create_sprite_sheet(GelatoSpriteSheetDescription description, GelatoTextureId texture)
{
    GelatoSpriteSheetInfo info =
    {
        ._texture_id = texture,

        ._tiles_count_u = description._width / description._tile_size_x,
        ._tiles_count_v = description._height / description._tile_size_y,    
        
        ._u_step = 1.0f / (float) (description._width / description._tile_size_x),
        ._v_step = 1.0f / (float) (description._height / description._tile_size_y)
    };

    return info;
}

void gelato_change_sprite_tile(GelatoSprite* sprite, uint32 u_index, uint32 v_index, GelatoSpriteSheetInfo info)
{    
    sprite->_texture = info._texture_id;

    float u_step = info._u_step * u_index;
    float v_step = info._v_step * v_index;

    sprite->_uv_start[0] = u_step;
    sprite->_uv_start[1] = 1.0f - v_step - info._v_step;

    sprite->_uv_end[0] = sprite->_uv_start[0] + info._u_step;
    sprite->_uv_end[1] = sprite->_uv_start[1] + info._v_step;
}