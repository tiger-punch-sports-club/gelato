#pragma once
#include "platform.h"

typedef struct Transform
{
    float _position[2];
    float _scale[2];
    float _rotation;
    float _uv_scale[2];
    float _uv_offset[2];
} Transform;

typedef struct ShaderId
{
    uint64 _id;
} ShaderId;

typedef struct Shader
{
    ShaderId _vertex_shader;
    ShaderId _fragment_shader;
} Shader;

typedef struct TextureDescription
{
    uint64 _id;
} TextureDescription;

typedef struct TextureId
{
    uint64 _id;
} TextureId;

typedef struct Sprite
{
    TextureId _texture_id;
    Transform _transform;
} Sprite;

typedef struct RendererDescription
{
    uint64 _id;
} RendererDescription;

typedef struct Renderer
{
    uint64 _id;
    Shader _sprite_shader;
} Renderer;

Renderer create_renderer(RendererDescription renderer_description);
void initialize_renderer(Renderer* renderer);
void deinitialize_renderer(Renderer* renderer);

void render(Renderer* renderer, Sprite* sprites, uint64 sprites_count);
TextureId create_texture(TextureDescription texture_description, void* data);