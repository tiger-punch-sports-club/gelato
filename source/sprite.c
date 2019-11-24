#include "sprite.h"

Sprite create_sprite(TextureId texture)
{
    Sprite sprite =
    {
        ._texture = texture,
        ._transform =
        {
            ._position[0] = 0.0f,
            ._position[1] = 0.0f,
            ._position[2] = 0.0f,

            ._scale[0] = 1.0f,
            ._scale[1] = 1.0f,

            ._rotation = 0.0f
        },
        ._uv_scale[0] = 1.0f,
        ._uv_scale[1] = 1.0f,

        ._uv_offset[0] = 0.0f,
        ._uv_offset[1] = 0.0f,
    };

    return sprite;
}