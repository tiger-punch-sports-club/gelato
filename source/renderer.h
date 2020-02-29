#pragma once
#include "platform.h"
#include "texture.h"
#include "sprite.h"

// ----------------------
// Batch renderer config
// ----------------------
#define CFG_MAX_SPRITES_PER_BATCH  500
#define CFG_MAX_INDICES CFG_MAX_SPRITES_PER_BATCH * 6
#define CFG_VERTEX_COUNT_PER_SPRITE 4
#define CFG_FLOATS_PER_VERTEX 10
#define CFG_FLOATS_PER_SPRITE CFG_FLOATS_PER_VERTEX * CFG_VERTEX_COUNT_PER_SPRITE
#define CFG_VERTEX_SIZE_BYTES CFG_FLOATS_PER_VERTEX * sizeof(float)
#define CFG_SPRITE_SIZE_BYTES CFG_VERTEX_SIZE_BYTES * CFG_VERTEX_COUNT_PER_SPRITE
#define CFG_MAX_BOUND_TEXTURES 16

typedef struct GelatoShaderId
{
    uint32 _id;
} GelatoShaderId;

typedef struct GelatoFramebufferId
{
	uint32 _id;
} GelatoFramebufferId;

typedef struct GelatoRendererDescription
{
    uint32 _render_width;
    uint32 _render_height;
    uint32 _window_width;
    uint32 _window_height;
    float _clear_color_letter_box[3];
    float _clear_color_render_target[3];
} GelatoRendererDescription;

typedef struct GelatoSimpleSpriteShader
{
    GelatoShaderId _shader;
    int32 _vertex_attribute_location;
    int32 _uv_attribute_location;
    int32 _color_attribute_location;
    int32 _texture_index_attribute_location;
    int32 _model_matrix_location;
    int32 _view_projection_matrix_location;
    int32 _uv_offset_location;
    int32 _uv_scale_location;
    int32 _sprite_texture_location;
    int32 _texture_pool_location[CFG_MAX_BOUND_TEXTURES];
} GelatoSimpleSpriteShader;

typedef struct GelatoDitheringShader
{
	GelatoShaderId _shader;
	int32 _vertex_attribute_location;
	int32 _uv_attribute_location;
	int32 _view_projection_matrix_location;
	int32 _color_texture_location;
	int32 _dithering_texture_location;
	int32 _texture_size_location;
} GelatoDitheringShader;

typedef struct GelatoRenderer
{
    uint32 _window_width;
    uint32 _window_height;
    uint32 _virtual_target_width;
    uint32 _virtual_target_height;
    uint32 _render_width;
    uint32 _render_height;
    float _pixel_scale_x;
    float _pixel_scale_y;

    GelatoSimpleSpriteShader _sprite_shader;
	GelatoDitheringShader _dithering_shader;

	GelatoFramebufferId _framebuffer_id;
	GelatoTextureId _color_render_target;
	GelatoTextureId _depth_render_target;
	GelatoTextureId _post_processing_color_render_target;
	GelatoTextureId _bayer_filter_texture;

    float _projection_matrix[16];
    float _clear_color_letter_box[3];
    float _clear_color_render_target[3];
} GelatoRenderer;

GelatoRenderer gelato_create_renderer(GelatoRendererDescription renderer_description);
void gelato_initialize_renderer(GelatoRenderer* renderer);
void gelato_deinitialize_renderer(GelatoRenderer* renderer);
void gelato_renderer_resize(GelatoRenderer* renderer, uint32 window_width, uint32 window_height);
void gelato_render(GelatoRenderer* renderer, GelatoTransform* camera_transform, GelatoSprite* sorted_sprites, uint32 sprites_count);