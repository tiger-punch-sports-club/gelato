#pragma once
#include "platform.h"
#include "texture.h"

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

typedef struct Sprite
{
    TextureId _texture;
    Transform _transform;
} Sprite;

typedef struct RendererDescription
{
    uint32 _render_width;
    uint32 _render_height;

    uint32 _window_width;
    uint32 _window_height;
} RendererDescription;

typedef struct Renderer
{
    uint32 _window_width;
    uint32 _window_height;
    uint32 _virtual_target_width;
    uint32 _virtual_target_height;
    uint32 _render_width;
    uint32 _render_height;
    float _pixel_scale_x;
    float _pixel_scale_y;

    Shader _sprite_shader;
    Shader _to_screen_shader;

    TextureId _render_target;
} Renderer;

Renderer create_renderer(RendererDescription renderer_description);
void initialize_renderer(Renderer* renderer);
void deinitialize_renderer(Renderer* renderer);
void renderer_resize(Renderer* renderer, uint32 window_width, uint32 window_height);
void render(Renderer* renderer, Sprite* sprites, uint64 sprites_count);

Sprite create_sprite(TextureId texture);