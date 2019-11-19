#pragma once
#include "platform.h"

typedef struct Transform
{
    float _position[3];
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
    TextureId _texture;
    Transform _transform;
} Sprite;

typedef struct RendererDescription
{
    uint64 _render_width;
    uint64 _render_height;
    uint64 _window_width;
    uint64 _window_height;
} RendererDescription;

typedef struct Renderer
{
    uint64 _render_width;
    uint64 _render_height;

    Shader _sprite_shader;
    Shader _to_screen_shader;

    TextureId _render_target;
} Renderer;

Renderer create_renderer(RendererDescription renderer_description);
void initialize_renderer(Renderer* renderer);
void deinitialize_renderer(Renderer* renderer);
void resize(Renderer* renderer, uint64 window_width, uint64 window_height);

void render(Renderer* renderer, Sprite* sprites, uint64 sprites_count);

TextureId create_texture(TextureDescription texture_description, void* data);
void destroy_texture(TextureId texture);

Sprite create_sprite(TextureId texture);