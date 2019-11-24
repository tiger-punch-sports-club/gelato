#pragma once
#include "platform.h"
#include "texture.h"
#include "sprite.h"

typedef struct ShaderId
{
    uint32 _id;
} ShaderId;

typedef struct RendererDescription
{
    uint32 _render_width;
    uint32 _render_height;
    uint32 _window_width;
    uint32 _window_height;
    float _clear_color_letter_box[3];
    float _clear_color_render_target[3];
} RendererDescription;

typedef struct SimpleSpriteShader
{
    ShaderId _shader;
    int32 _vertex_attribute_location;
    int32 _uv_attribute_location;
    int32 _model_matrix_location;
    int32 _view_projection_matrix_location;
    int32 _uv_offset_location;
    int32 _uv_scale_location;
    int32 _sprite_texture_location;
} SimpleSpriteShader;

typedef struct ToScreenShader
{
    ShaderId _shader;
} ToScreenShader;

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

    SimpleSpriteShader _sprite_shader;
    ToScreenShader _to_screen_shader;

    TextureId _render_target;

    float _projection_matrix[16];
    float _clear_color_letter_box[3];
    float _clear_color_render_target[3];
} Renderer;

Renderer create_renderer(RendererDescription renderer_description);
void initialize_renderer(Renderer* renderer);
void deinitialize_renderer(Renderer* renderer);
void renderer_resize(Renderer* renderer, uint32 window_width, uint32 window_height);
void render(Renderer* renderer, Transform* camera_transform, Sprite* sprites, uint64 sprites_count);