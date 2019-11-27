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

            ._angle_degrees = 0.0f
        },
        ._uv_scale[0] = 1.0f,
        ._uv_scale[1] = 1.0f,

        ._uv_offset[0] = 0.0f,
        ._uv_offset[1] = 0.0f,

        ._color[0] = 1.0f,
        ._color[1] = 1.0f,
        ._color[2] = 1.0f,
        ._color[3] = 1.0f
    };

    return sprite;
}