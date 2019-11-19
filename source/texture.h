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

typedef struct TextureWrap
{
    uint64 _gl_enum;
} TextureWrap;

typedef struct TextureMinFilter
{
    uint64 _gl_enum;
} TextureMinFilter;

typedef struct TextureMagFilter
{
    uint64 _gl_enum;
} TextureMagFilter;

typedef struct TextureTarget
{
    uint64 _gl_enum;
} TextureTarget;

typedef struct TextureDescription
{
    uint64 _width;
    uint64 _height;
    TextureFormat _format;
    TextureInternalFormat _internal_format;
    TextureType _type;
    TextureWrap _wrap_s;
    TextureWrap _wrap_t;
    TextureMinFilter _min_filter;
    TextureMagFilter _mag_filter;
} TextureDescription;

typedef struct TextureId
{
    uint64 _id;
} TextureId;

extern const struct TextureFormats
{
    TextureFormat _rgb;
    TextureFormat _rgba;
} TextureFormats;

extern const struct TextureInternalFormats
{
    TextureInternalFormat _rgb8;
    TextureInternalFormat _rgba8;
} TextureInternalFormats;

extern const struct TextureTypes
{
    TextureType _float;
    TextureType _unsigned_byte;
} TextureTypes;

extern const struct TextureWraps
{
    TextureWrap _clamp_to_border;
    TextureWrap _clamp_to_edge;
    TextureWrap _repeat;
} TextureWraps;

extern const struct TextureMinFilters
{
    TextureMinFilter _nearest;
    TextureMinFilter _linear;
    TextureMinFilter _nearest_mipmap_nearest;
    TextureMinFilter _linear_mipmap_nearest;
    TextureMinFilter _nearest_mipmap_linear;
    TextureMinFilter _linear_mipmap_linear;
} TextureMinFilters;

extern const struct TextureMagFilters
{
    TextureMagFilter _nearest;
    TextureMagFilter _linear;
} TextureMagFilters;

TextureId create_texture(TextureDescription texture_description, void* data);
void destroy_texture(TextureId texture);