#include "texture.h"
#include <GL/glew.h>

const struct TextureFormats TextureFormats =
{
    ._rgb = { GL_RGB },
    ._rgba = { GL_RGBA },
};

const struct TextureInternalFormats TextureInternalFormats =
{
    ._rgb8 = { GL_RGB8 },
    ._rgba8 = { GL_RGBA8 },
};

const struct TextureTypes TextureTypes =
{
    ._float = { GL_FLOAT },
    ._unsigned_byte = { GL_UNSIGNED_BYTE }
};

TextureId create_texture(TextureDescription texture_description, void* data)
{
    TextureId texture_id = { 0 };

    // todo: based on texture description create texture

    return texture_id;
}

void destroy_texture(TextureId texture)
{
    glDeleteTextures(1, (GLuint*) &texture._id);
} 