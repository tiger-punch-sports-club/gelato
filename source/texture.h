#pragma once
#include "platform.h"

typedef struct GelatoTextureFormat
{
    uint32 _gl_enum;
} GelatoTextureFormat;

typedef struct GelatoTextureInternalFormat
{
    uint32 _gl_enum;
} GelatoTextureInternalFormat;

typedef struct GelatoTextureType
{
    uint32 _gl_enum;
} GelatoTextureType;

typedef struct GelatoTextureWrap
{
    uint32 _gl_enum;
} GelatoTextureWrap;

typedef struct GelatoTextureMinFilter
{
    uint32 _gl_enum;
} GelatoTextureMinFilter;

typedef struct GelatoTextureMagFilter
{
    uint32 _gl_enum;
} GelatoTextureMagFilter;

typedef struct GelatoTextureTarget
{
    uint32 _gl_enum;
} GelatoTextureTarget;

typedef struct GelatoTextureDescription
{
    uint32 _width;
    uint32 _height;
    GelatoTextureFormat _format;
    GelatoTextureInternalFormat _internal_format;
    GelatoTextureType _type;
    GelatoTextureWrap _wrap_s;
    GelatoTextureWrap _wrap_t;
    GelatoTextureMinFilter _min_filter;
    GelatoTextureMagFilter _mag_filter;
} GelatoTextureDescription;

typedef struct GelatoTextureId
{
    uint64 _id;
    //todo: maybe put: uint32 _width; uint32 _height;
} GelatoTextureId;

extern const struct GelatoTextureFormats
{
    GelatoTextureFormat _rgb;
    GelatoTextureFormat _rgba;
} GelatoTextureFormats;

extern const struct GelatoTextureInternalFormats
{
    GelatoTextureInternalFormat _rgb8;
    GelatoTextureInternalFormat _rgba8;
} GelatoTextureInternalFormats;

extern const struct GelatoTextureTypes
{
    GelatoTextureType _float;
    GelatoTextureType _unsigned_byte;
} GelatoTextureTypes;

extern const struct GelatoTextureWraps
{
    GelatoTextureWrap _clamp_to_border;
    GelatoTextureWrap _clamp_to_edge;
    GelatoTextureWrap _repeat;
} GelatoTextureWraps;

extern const struct GelatoTextureMinFilters
{
    GelatoTextureMinFilter _nearest;
    GelatoTextureMinFilter _linear;
    GelatoTextureMinFilter _nearest_mipmap_nearest;
    GelatoTextureMinFilter _linear_mipmap_nearest;
    GelatoTextureMinFilter _nearest_mipmap_linear;
    GelatoTextureMinFilter _linear_mipmap_linear;
} GelatoTextureMinFilters;

extern const struct GelatoTextureMagFilters
{
    GelatoTextureMagFilter _nearest;
    GelatoTextureMagFilter _linear;
} GelatoTextureMagFilters;

GelatoTextureId gelato_create_texture(GelatoTextureDescription texture_description, void* data);
void gelato_destroy_texture(GelatoTextureId texture);