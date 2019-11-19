#pragma once
#include "platform.h"

typedef struct TextureFormat
{
    uint64 _gl_enum;
} TextureFormat;

typedef struct TextureInternalFormat
{
    uint64 _gl_enum;
} TextureInternalFormat;

typedef struct TextureType
{
    uint64 _gl_enum;
} TextureType;

typedef struct TextureDescription
{
    TextureFormat _format;
    TextureInternalFormat _internal_format;
    TextureType _type;
    // uint32 wrap_s = GL_REPEAT, 
    // uint32 wrap_t = GL_REPEAT, 
    // uint32 min_filter = GL_LINEAR_MIPMAP_LINEAR, 
    // uint32 mag_filter = GL_LINEAR
} TextureDescription;

typedef struct TextureId
{
    uint64 _id;
} TextureId;

extern const struct TextureFormats {
    TextureFormat _rgb;
    TextureFormat _rgba;
} TextureFormats;

extern const struct TextureInternalFormats {
    TextureInternalFormat _rgb8;
    TextureInternalFormat _rgba8;
} TextureInternalFormats;

extern const struct TextureTypes {
    TextureType _float;
    TextureType _unsigned_byte;
} TextureTypes;

TextureId create_texture(TextureDescription texture_description, void* data);
void destroy_texture(TextureId texture);