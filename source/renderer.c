#include "renderer.h"
#include "sprite.h"
#include <assert.h>
#include <stdio.h>
#include <GL/glew.h>
#include <stdbool.h>
#include <string.h>

// ----------------------
// Sprite quad data
// ----------------------
const struct
{
    float _vertices[40];
    uint32 _indices[6];
    uint32 _vertex_stride;
    uint32 _vertex_stride_bytes;
    uint32 _face_count;
    uint32 _vertex_count;
    uint32 _index_count;
} QUAD_DATA =
{
    ._vertices =
    {
        -1.0f, -1.0f, 0.0f,         // Position 0
        0.0f, 0.0f,                 // UV 0
        1.0f, 1.0f, 1.0f, 1.0f,     // Color 0
        0.0f,                       // Texture Index 0

        1.0f, -1.0f, 0.0f,          // Position 1
        1.0f, 0.0f,                 // UV 1
        1.0f, 1.0f, 1.0f, 1.0f,     // Color 1
        0.0f,                       // Texture Index 1

        1.0f, 1.0f, 0.0f,           // Position 2
        1.0f, 1.0f,                 // UV 2
        1.0f, 1.0f, 1.0f, 1.0f,     // Color 2
        0.0f,                       // Texture Index 2

        -1.0f, 1.0f, 0.0f,          // Position 3
        0.0f, 1.0f,                 // UV 3
        1.0f, 1.0f, 1.0f, 1.0f,     // Color 3
        0.0f                        // Texture Index 3
    },

    ._indices =
    {
        0, 1, 2,
        2, 3, 0
    },

    ._vertex_stride = 10,
    ._vertex_stride_bytes = 10 * sizeof(float),
    ._face_count = 2,
    ._vertex_count = 4,
    ._index_count = 6
};

struct
{
    uint32 _vertex_array;
    uint32 _vertex_buffer;
    uint32 _index_buffer;
} QUAD;

typedef struct Node
{
    uint32 _data;
    uint32 _next;
} Node;

struct 
{
    uint32 _bound_indices;
    uint32 _bound_sprites;
    uint32 _bound_textures;
    uint32 _bound_batches;
    uint32 _bound_offset;

    GelatoTextureId _bound_textures_list[CFG_MAX_BOUND_TEXTURES];
    float _vertex_data[CFG_MAX_SPRITES_PER_BATCH * CFG_FLOATS_PER_SPRITE];
} BATCH_RENDERER_STATE =
{
    ._bound_indices = 0,
    ._bound_sprites = 0,
    ._bound_textures = 0,
    ._bound_batches = 0,
    ._bound_offset = 0,
    ._bound_textures_list = { 0 }
} ;


void check_shader_error(uint32 shader);
uint32 compile_shader(GLenum shader_type, const char* source);
void init_shaders(GelatoRenderer* renderer);
void destroy_shader(GelatoShaderId* shader);
void destroy_shaders(GelatoRenderer* renderer);
void init_render_target(GelatoRenderer* renderer);
void set_gl_state_pre_render(GelatoRenderer* renderer);
void set_gl_state_post_render();
void init_quad(GelatoRenderer* renderer);
void destroy_quad(GelatoRenderer* renderer);
uint32 create_buffer(void* data, uint32 stride_in_bytes, uint32 amount, GLenum buffer_type, GLenum buffer_type_usage_type);

void reset_tracking();
void begin_render(GelatoRenderer* renderer);
void end_render(GelatoRenderer* renderer);
void render_sprites(GelatoRenderer* renderer, GelatoSprite* sorted_sprites, uint32 sprites_count);
void submit(GelatoRenderer* renderer, GelatoSprite* sprite, GelatoTransform* transform);
bool texture_list_contains(GelatoTextureId texture);

void check_shader_error(uint32 shader)
{
    char error[1024];
    GL_CHECK(glGetShaderInfoLog(shader, 1024, NULL, error));

    GLint success = 0;
    GL_CHECK(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));

    if (success == 0)
    {
        printf("Shader error log for: %d\n%s\n", shader, error);
    }

    assert(success != 0);
}

void init_shaders(GelatoRenderer* renderer)
{
    GelatoShaderId shader_program = { glCreateProgram() };
    uint32 vertex_shader = compile_shader(GL_VERTEX_SHADER, "#version 150\nuniform mat4 ViewProjectionMatrix;\nin vec3 VertexPosition;\nin vec2 VertexUV;\nin vec4 VertexColor;\nin float VertexTextureIndex;\nout vec2 outVertexUV;\nout vec4 outVertexColor;\nout float outVertexTextureIndex;\nvoid main() {\noutVertexUV = VertexUV;\ngl_Position = ViewProjectionMatrix * vec4(VertexPosition, 1);\noutVertexColor = VertexColor;\noutVertexTextureIndex = VertexTextureIndex;\n}\n");
    uint32 fragment_shader = compile_shader(GL_FRAGMENT_SHADER, "#version 150\nuniform sampler2D TexturePool[16];\nin vec2 outVertexUV;\nin vec4 outVertexColor;\nin float outVertexTextureIndex;\nout vec4 outColor;\nvoid main() {\noutColor = texture2D(TexturePool[int(floor(outVertexTextureIndex))], outVertexUV) * outVertexColor;\n}\n");

    GL_CHECK(glAttachShader(shader_program._id, vertex_shader));
    GL_CHECK(glAttachShader(shader_program._id, fragment_shader));

    GL_CHECK(glLinkProgram(shader_program._id));
    GL_CHECK(glUseProgram(shader_program._id));

    GL_CHECK(glDeleteShader(vertex_shader));
    GL_CHECK(glDeleteShader(fragment_shader));

    GL_CHECK(glUseProgram(0));

    renderer->_sprite_shader = (GelatoSimpleSpriteShader)
    {
        ._shader = shader_program,
        ._vertex_attribute_location = glGetAttribLocation(shader_program._id, "VertexPosition"),
        ._uv_attribute_location = glGetAttribLocation(shader_program._id, "VertexUV"),
        ._color_attribute_location = glGetAttribLocation(shader_program._id, "VertexColor"),
        ._texture_index_attribute_location = glGetAttribLocation(shader_program._id, "VertexTextureIndex"),
        ._model_matrix_location = glGetUniformLocation(shader_program._id, "ModelMatrix"),
        ._view_projection_matrix_location = glGetUniformLocation(shader_program._id, "ViewProjectionMatrix"),
        ._uv_offset_location = glGetUniformLocation(shader_program._id, "UvOffset"),
        ._uv_scale_location = glGetUniformLocation(shader_program._id, "UvScale"),
        ._sprite_texture_location = glGetUniformLocation(shader_program._id, "SpriteTexture"),
    };

    char buffer[1024];

    for (uint32 i = 0; i < CFG_MAX_BOUND_TEXTURES; i++)
    {
        sprintf(buffer, "TexturePool[%d]", i);
        renderer->_sprite_shader._texture_pool_location[i] = glGetUniformLocation(shader_program._id, (const GLchar*) &buffer);
    }
}

void destroy_shader(GelatoShaderId* shader)
{
    if (shader && shader->_id)
    {
        GL_CHECK(glDeleteProgram(shader->_id));
        shader->_id = 0;
    }
}

void destroy_shaders(GelatoRenderer* renderer)
{
    destroy_shader(&renderer->_sprite_shader._shader);
}

uint32 compile_shader(GLenum shader_type, const char* source)
{
    uint32 shader_id = GL_CHECK(glCreateShader(shader_type));

    GL_CHECK(glShaderSource(shader_id, 1, &source, NULL));
    GL_CHECK(glCompileShader(shader_id));

    check_shader_error(shader_id);
    return shader_id;
}

void set_gl_state_pre_render(GelatoRenderer* renderer)
{
    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glEnable(GL_CULL_FACE));
    GL_CHECK(glEnable(GL_BLEND));
    GL_CHECK(glEnable(GL_SCISSOR_TEST));
    GL_CHECK(glDepthFunc(GL_LEQUAL));
    GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GL_CHECK(glCullFace(GL_BACK));
    GL_CHECK(glFrontFace(GL_CCW));

    GL_CHECK(glViewport(0, 0, renderer->_window_width, renderer->_window_height));
    GL_CHECK(glScissor(0, 0, renderer->_window_width, renderer->_window_height));
    GL_CHECK(glClearColor(renderer->_clear_color_letter_box[0], renderer->_clear_color_letter_box[1], renderer->_clear_color_letter_box[2], 1.0f));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    GLint x = renderer->_window_width / 2 - renderer->_render_width / 2;
    GLint y = renderer->_window_height / 2 - renderer->_render_height / 2;
    GL_CHECK(glViewport(x, y, renderer->_render_width, renderer->_render_height));
    GL_CHECK(glScissor(x, y, renderer->_render_width, renderer->_render_height));
    GL_CHECK(glClearColor(renderer->_clear_color_render_target[0], renderer->_clear_color_render_target[1], renderer->_clear_color_render_target[2], 1.0f));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void set_gl_state_post_render()
{
    GL_CHECK(glDisable(GL_DEPTH_TEST));
    GL_CHECK(glDisable(GL_CULL_FACE));
    GL_CHECK(glDisable(GL_BLEND));
    GL_CHECK(glDisable(GL_SCISSOR_TEST));
}

void init_quad(GelatoRenderer* renderer)
{
    GL_CHECK(glGenVertexArrays(1, &QUAD._vertex_array));
    GL_CHECK(glBindVertexArray(QUAD._vertex_array));
    
    uint32* vertices = (uint32*) (&QUAD_DATA._vertices);
    QUAD._vertex_buffer = create_buffer(vertices, QUAD_DATA._vertex_stride_bytes, QUAD_DATA._vertex_count, GL_ARRAY_BUFFER, GL_STREAM_DRAW);

    uint32* indices = (uint32*) (&QUAD_DATA._indices);
    QUAD._index_buffer = create_buffer(indices, sizeof(uint32), QUAD_DATA._index_count, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
    
    GL_CHECK(glBindVertexArray(0));
}

void destroy_quad(GelatoRenderer* renderer)
{
    GL_CHECK(glDeleteVertexArrays(1, &QUAD._vertex_array));
    GL_CHECK(glDeleteBuffers(1, &QUAD._vertex_buffer));
    GL_CHECK(glDeleteBuffers(1, &QUAD._index_buffer));
}

uint32 create_buffer(void* data, uint32 stride_in_bytes, uint32 amount, GLenum buffer_type, GLenum buffer_type_usage_type)
{
    uint32 handle;

    GL_CHECK(glGenBuffers(1, &handle));
    GL_CHECK(glBindBuffer(buffer_type, handle));
    GL_CHECK(glBufferData(buffer_type, stride_in_bytes * amount, data, buffer_type_usage_type));
    GL_CHECK(glBindBuffer(buffer_type, 0));

    return handle;
}

void reset_tracking()
{
    BATCH_RENDERER_STATE._bound_indices = 0;
    BATCH_RENDERER_STATE._bound_sprites = 0;
    BATCH_RENDERER_STATE._bound_textures = 0;
    BATCH_RENDERER_STATE._bound_batches = 0;
    BATCH_RENDERER_STATE._bound_offset = 0;
    BATCH_RENDERER_STATE._bound_sprites = 0;
}

void begin_render(GelatoRenderer* renderer)
{
    GL_CHECK(glBindVertexArray(QUAD._vertex_array));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, QUAD._vertex_buffer));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, QUAD._index_buffer));

    GL_CHECK(glUseProgram(renderer->_sprite_shader._shader._id));
    
    GL_CHECK(glEnableVertexAttribArray(renderer->_sprite_shader._vertex_attribute_location));
    GL_CHECK(glVertexAttribPointer(renderer->_sprite_shader._vertex_attribute_location, 3, GL_FLOAT, GL_FALSE, QUAD_DATA._vertex_stride_bytes, NULL));

    GL_CHECK(glEnableVertexAttribArray(renderer->_sprite_shader._uv_attribute_location));
    GL_CHECK(glVertexAttribPointer(renderer->_sprite_shader._uv_attribute_location, 2, GL_FLOAT, GL_FALSE, QUAD_DATA._vertex_stride_bytes, (GLvoid*) (3 * sizeof(float))));

    GL_CHECK(glEnableVertexAttribArray(renderer->_sprite_shader._color_attribute_location));
    GL_CHECK(glVertexAttribPointer(renderer->_sprite_shader._color_attribute_location, 4, GL_FLOAT, GL_FALSE, QUAD_DATA._vertex_stride_bytes, (GLvoid*) (5 * sizeof(float))));

    GL_CHECK(glEnableVertexAttribArray(renderer->_sprite_shader._texture_index_attribute_location));
    GL_CHECK(glVertexAttribPointer(renderer->_sprite_shader._texture_index_attribute_location, 1, GL_FLOAT, GL_FALSE, QUAD_DATA._vertex_stride_bytes, (GLvoid*) (9 * sizeof(float))));
}

void end_render(GelatoRenderer* renderer)
{
    GL_CHECK(glBindVertexArray(0));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    GL_CHECK(glDisableVertexAttribArray(renderer->_sprite_shader._texture_index_attribute_location));
    GL_CHECK(glDisableVertexAttribArray(renderer->_sprite_shader._color_attribute_location));
    GL_CHECK(glDisableVertexAttribArray(renderer->_sprite_shader._uv_attribute_location));
    GL_CHECK(glDisableVertexAttribArray(renderer->_sprite_shader._vertex_attribute_location));
    GL_CHECK(glUseProgram(0));
}

void render_sprites(GelatoRenderer* renderer, GelatoSprite* sorted_sprites, uint32 sprites_count)
{
    for(uint64 i = 0; i < sprites_count; ++i)
    {
        GelatoSprite* sprite = &sorted_sprites[i];
        GelatoTransform* transform = &sprite->_transform;
    
        bool batch_has_room = BATCH_RENDERER_STATE._bound_sprites < CFG_MAX_SPRITES_PER_BATCH;
        bool flush = !batch_has_room;

        if (batch_has_room)
        {
            bool texture_list_has_room = BATCH_RENDERER_STATE._bound_textures < CFG_MAX_BOUND_TEXTURES;
            bool texture_exists_in_batch = texture_list_contains(sprite->_texture);

            if (texture_exists_in_batch) 
            {
                submit(renderer, sprite, transform);
            } 
            else if (texture_list_has_room)
            {
                BATCH_RENDERER_STATE._bound_textures_list[i] = sprite->_texture;
                BATCH_RENDERER_STATE._bound_textures++;
                submit(renderer, sprite, transform);
            }
            else
            {
                flush = true;
            }
        }
        
        flush = flush || BATCH_RENDERER_STATE._bound_sprites == sprites_count;
        if (flush)
        {
            // copy data
            GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, 0, CFG_SPRITE_SIZE_BYTES * BATCH_RENDERER_STATE._bound_sprites, &BATCH_RENDERER_STATE._vertex_data));
            
            // bind textures
            for (uint32 i = 0, l = BATCH_RENDERER_STATE._bound_textures; i < l; ++i)
            {
                GL_CHECK(glActiveTexture(GL_TEXTURE0 + i));
                GL_CHECK(glBindTexture(GL_TEXTURE_2D, BATCH_RENDERER_STATE._bound_textures_list[i]._id));
                GL_CHECK(glUniform1i(renderer->_sprite_shader._texture_pool_location[i], i));
            }

            // draw call
            uint32 index_count = BATCH_RENDERER_STATE._bound_sprites * QUAD_DATA._index_count;
            GL_CHECK(glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0));
            
            reset_tracking();
        }
    }

}

void submit(GelatoRenderer* renderer, GelatoSprite* sprite, GelatoTransform* transform)
{
    uint32 current_offset = BATCH_RENDERER_STATE._bound_sprites * QUAD_DATA._vertex_stride * QUAD_DATA._vertex_count;

    // write position
    float model_matrix[16];
    gelato_make_transformation(transform, &model_matrix[0]);

    float* vertex_data = &BATCH_RENDERER_STATE._vertex_data[current_offset];
    memcpy(vertex_data, &QUAD_DATA._vertices, QUAD_DATA._vertex_stride_bytes * QUAD_DATA._vertex_count);

    float vertex_position_0[3] = { vertex_data[0], vertex_data[1], vertex_data[2] };
    gelato_mul_vec_matrix(&vertex_position_0[0], &model_matrix[0], &vertex_data[0]);
    
    float vertex_position_1[3] = { vertex_data[QUAD_DATA._vertex_stride * 1], vertex_data[QUAD_DATA._vertex_stride * 1 + 1], vertex_data[QUAD_DATA._vertex_stride * 1 + 2] };
    gelato_mul_vec_matrix(&vertex_position_1[0], &model_matrix[0], &vertex_data[QUAD_DATA._vertex_stride * 1]);
    
    float vertex_position_2[3] = { vertex_data[QUAD_DATA._vertex_stride * 2], vertex_data[QUAD_DATA._vertex_stride * 2 + 1], vertex_data[QUAD_DATA._vertex_stride * 2 + 2] };
    gelato_mul_vec_matrix(&vertex_position_2[0], &model_matrix[0], &vertex_data[QUAD_DATA._vertex_stride * 2]);
    
    float vertex_position_3[3] = { vertex_data[QUAD_DATA._vertex_stride * 3], vertex_data[QUAD_DATA._vertex_stride * 3 + 1], vertex_data[QUAD_DATA._vertex_stride * 3 + 2] };
    gelato_mul_vec_matrix(&vertex_position_3[0], &model_matrix[0], &vertex_data[QUAD_DATA._vertex_stride * 3]);

    // write colors
    float* color_data = &BATCH_RENDERER_STATE._vertex_data[current_offset + 5];
    float* color_0 = &color_data[0];
    memcpy(color_0, &sprite->_color[0], 4 * sizeof(float));
    
    float* color_1 = &color_data[QUAD_DATA._vertex_stride * 1];
    memcpy(color_1, &sprite->_color[0], 4 * sizeof(float));

    float* color_2 = &color_data[QUAD_DATA._vertex_stride * 2];
    memcpy(color_2, &sprite->_color[0], 4 * sizeof(float));

    float* color_3 = &color_data[QUAD_DATA._vertex_stride * 3];
    memcpy(color_3, &sprite->_color[0], 4 * sizeof(float));

    // todo: uv-scale, uv-offsets

    BATCH_RENDERER_STATE._bound_sprites++;
}

bool texture_list_contains(GelatoTextureId texture)
{
    for (uint32 i = 0; i < BATCH_RENDERER_STATE._bound_textures; i++)
    {
        if (BATCH_RENDERER_STATE._bound_textures_list[i]._id == texture._id)
        {
            return true;
        }
    }

    return false;
}

/********************************************************
********************* PUBLIC API ************************
********************************************************/

GelatoRenderer gelato_create_renderer(GelatoRendererDescription renderer_description)
{
    GelatoRenderer renderer =
    {
        ._window_width = renderer_description._window_width,
        ._window_height = renderer_description._window_height,
        ._render_width = renderer_description._render_width,
        ._render_height = renderer_description._render_height,
        ._virtual_target_width = renderer_description._render_width,
        ._virtual_target_height = renderer_description._render_height,
        ._clear_color_letter_box[0] = renderer_description._clear_color_letter_box[0],
        ._clear_color_letter_box[1] = renderer_description._clear_color_letter_box[1],
        ._clear_color_letter_box[2] = renderer_description._clear_color_letter_box[2],
        ._clear_color_render_target[0] = renderer_description._clear_color_render_target[0],
        ._clear_color_render_target[1] = renderer_description._clear_color_render_target[1],
        ._clear_color_render_target[2] = renderer_description._clear_color_render_target[2],
    };

    return renderer;
}

void gelato_initialize_renderer(GelatoRenderer* renderer)
{
    glewExperimental = GL_TRUE;

    if(glewInit() == GLEW_OK)
    {
        init_shaders(renderer);    
        init_quad(renderer);
        gelato_renderer_resize(renderer, renderer->_window_width, renderer->_window_height);
    }
    else
    {
        assert(0);
    }
}

void gelato_deinitialize_renderer(GelatoRenderer* renderer)
{
    destroy_quad(renderer);
    destroy_shaders(renderer);
}

void max_resolution_aspect_ratio(float target_resolution_width, float target_resolution_height, float current_resolution_width, float current_resolution_height, float* new_width, float* new_height)
{
    uint32 current_height = (uint32) current_resolution_height;

    float target_aspect_ratio = target_resolution_width / target_resolution_height;

    uint32 width = (uint32) current_resolution_width;
    uint32 height = (uint32)(width / target_aspect_ratio + 0.5f);

    if (height > current_height)
    {
        height = current_height;
        width = (uint32)(height * target_aspect_ratio + 0.5f);
    }

    *new_width = (float) width;
    *new_height = (float) height;
}

void fit_to_virtual_resolution(uint32 window_width, uint32 window_height, uint32 virtual_target_width, uint32 virtual_target_height, uint32* new_width, uint32* new_height, float* pixel_scale_x, float* pixel_scale_y)
{
    float virtual_width = (float) virtual_target_width;
    float virtual_height = (float) virtual_target_height;

    float win_width = (float) window_width;
    float win_height = (float) window_height;

    float width, height;

    max_resolution_aspect_ratio(virtual_width, virtual_height, win_width, win_height, &width, &height);

    *new_width = (uint32) width;
    *new_height = (uint32) height;

    *pixel_scale_x = win_width / virtual_width;
    *pixel_scale_y = win_height / virtual_height;
}

void gelato_renderer_resize(GelatoRenderer* renderer, uint32 window_width, uint32 window_height)
{
    renderer->_window_width = window_width;
    renderer->_window_height = window_height;

    fit_to_virtual_resolution(renderer->_window_width, renderer->_window_height, renderer->_virtual_target_width, renderer->_virtual_target_height, &renderer->_render_width, &renderer->_render_height, &renderer->_pixel_scale_x, &renderer->_pixel_scale_y);

    float left = 0;
    float right = (float) renderer->_window_width;
    float top = (float) renderer->_window_height;
    float bottom = 0;
    float near_plane = -10.0f;
    float far_plane = 10.0f;
    
    gelato_make_projection_matrix(left, right, bottom, top, near_plane, far_plane, &renderer->_projection_matrix[0]);
}

void gelato_render(GelatoRenderer* renderer, GelatoTransform* camera_transform, GelatoSprite* sorted_sprites, uint64 sprites_count)
{
    set_gl_state_pre_render(renderer);
    begin_render(renderer);

    // set projection
    float pixel_scale_matrix[16];
    gelato_make_identity_matrix(&pixel_scale_matrix[0]);
    gelato_make_scale_matrix(renderer->_pixel_scale_x, renderer->_pixel_scale_y, 1.0f, &pixel_scale_matrix[0]);

    float view_matrix[16];
    gelato_make_camera_transformation(camera_transform, &view_matrix[0]);

    float scaled_view_matrix[16];
    gelato_make_identity_matrix(&scaled_view_matrix[0]);
    gelato_mul_matrix(&pixel_scale_matrix[0], &view_matrix[0], &scaled_view_matrix[0]);

    float view_projection_matrix[16];
    gelato_mul_matrix(&renderer->_projection_matrix[0], &scaled_view_matrix[0], &view_projection_matrix[0]);
    GL_CHECK(glUniformMatrix4fv(renderer->_sprite_shader._view_projection_matrix_location, 1, GL_FALSE, &view_projection_matrix[0]));

    render_sprites(renderer, sorted_sprites, sprites_count);

    end_render(renderer);
    set_gl_state_post_render();
}