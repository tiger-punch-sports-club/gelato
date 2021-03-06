#include "platform.h"
#include "texture.h"
#include <GL/glew.h>

const struct GelatoTextureFormats GelatoTextureFormats =
{
    ._rgb = { GL_RGB },
    ._rgba = { GL_RGBA },
	._depth_component = { GL_DEPTH_COMPONENT }
};

const struct GelatoTextureInternalFormats GelatoTextureInternalFormats =
{
    ._rgb8 = { GL_RGB8 },
    ._rgba8 = { GL_RGBA8 },
	._depth_component_32f = { GL_DEPTH_COMPONENT32F }
};

const struct GelatoTextureTypes GelatoTextureTypes =
{
    ._float = { GL_FLOAT },
    ._unsigned_byte = { GL_UNSIGNED_BYTE }
};

const struct GelatoTextureWraps GelatoTextureWraps =
{
   ._clamp_to_border = { GL_CLAMP_TO_BORDER },
   ._clamp_to_edge = { GL_CLAMP_TO_EDGE },
   ._repeat = { GL_REPEAT }
};

const struct GelatoTextureMinFilters GelatoTextureMinFilters =
{
   ._nearest = { GL_NEAREST },
   ._linear = { GL_LINEAR },
   ._nearest_mipmap_nearest = { GL_NEAREST_MIPMAP_NEAREST },
   ._linear_mipmap_nearest = { GL_LINEAR_MIPMAP_NEAREST },
   ._nearest_mipmap_linear = { GL_NEAREST_MIPMAP_LINEAR },
   ._linear_mipmap_linear = { GL_LINEAR_MIPMAP_LINEAR }
};

const struct GelatoTextureMagFilters GelatoTextureMagFilters =
{
    ._nearest = { GL_NEAREST },
    ._linear = { GL_LINEAR }
};

GelatoTextureId gelato_create_texture(GelatoTextureDescription texture_description, void* data)
{
    GLuint texture_id;

    GL_CHECK(glGenTextures(1, &texture_id));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, texture_id));

    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint) texture_description._min_filter._gl_enum));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint) texture_description._mag_filter._gl_enum));

    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint) texture_description._wrap_s._gl_enum));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint) texture_description._wrap_t._gl_enum));

    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, (GLint) texture_description._internal_format._gl_enum, (GLsizei) texture_description._width, (GLsizei) texture_description._height, 0, (GLenum) texture_description._format._gl_enum, (GLenum) texture_description._type._gl_enum, data));
	
	GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));

    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

    return (GelatoTextureId) { texture_id };
}

void gelato_destroy_texture(GelatoTextureId texture)
{
    GL_CHECK(glDeleteTextures(1, (GLuint*) &texture._id));
} 