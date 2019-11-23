#include "renderer.h"
#include <assert.h>
#include <stdio.h>
#include <GL/glew.h>

const struct
{
    float _vertices[16];
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
        -1.0f, -1.0f,
        1.0f, -1.0f,
        1.0f, 1.0f,
        -1.0f, 1.0f,

        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    },

    ._indices =
    {
        0, 1, 2,
        2, 3, 0
    },

    ._vertex_stride = 4,
    ._vertex_stride_bytes = 4 * sizeof(float),
    ._face_count = 2,
    ._vertex_count = 4,
    ._index_count = 6
};

struct {
    uint32 _vertex_array;
    uint32 _vertex_buffer;
    uint32 _index_buffer;
} QUAD;

void check_shader_error(uint32 shader);
uint32 compile_shader(uint32 shader_type, const char* source);
void init_shaders(Renderer* renderer);
void destroy_shader(ShaderId* shader);
void destroy_shaders(Renderer* renderer);
void init_render_target(Renderer* renderer);
void set_gl_state_pre_render(Renderer* renderer);
void set_gl_state_post_render();
void init_quad(Renderer* renderer);
void destroy_quad(Renderer* renderer);
uint32 create_buffer(void* data, uint32 stride_in_bytes, uint32 amount, GLenum buffer_type, GLenum buffer_type_usage_type);

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

void init_shaders(Renderer* renderer)
{
    ShaderId shader_program = { glCreateProgram() };
    uint32 vertex_shader = compile_shader(GL_VERTEX_SHADER, "\n#version 150\nuniform mat4 ModelMatrix;\nuniform mat4 ViewProjectionMatrix;\nuniform vec2 UvOffset;\nuniform vec2 UvScale;\nin vec3 VertexPosition;\nin vec2 VertexUV;\nout vec2 outVertexUV;\nvoid main() {\noutVertexUV = (VertexUV * UvScale) + UvOffset;\ngl_Position = (ViewProjectionMatrix * ModelMatrix) * vec4(VertexPosition, 1);\n}\n");
    uint32 fragment_shader = compile_shader(GL_FRAGMENT_SHADER, "#version 150\nuniform sampler2D SpriteTexture;\nin vec2 outVertexUV;\nout vec4 outColor;\nvoid main() {\noutColor = texture(SpriteTexture, outVertexUV);\n}\n");

    GL_CHECK(glAttachShader(shader_program._id, vertex_shader));
    GL_CHECK(glAttachShader(shader_program._id, fragment_shader));

    GL_CHECK(glLinkProgram(shader_program._id));
    GL_CHECK(glUseProgram(shader_program._id));

    GL_CHECK(glDeleteShader(vertex_shader));
    GL_CHECK(glDeleteShader(fragment_shader));

    GL_CHECK(glUseProgram(0));

    renderer->_sprite_shader = (SimpleSpriteShader)
    {
        ._shader = shader_program,
        ._model_matrix_location = glGetUniformLocation(shader_program._id, "ModelMatrix"),
        ._view_projection_matrix_location = glGetUniformLocation(shader_program._id, "ViewProjectionMatrix"),
        ._uv_offset_location = glGetUniformLocation(shader_program._id, "UvOffset"),
        ._uv_scale_location = glGetUniformLocation(shader_program._id, "UvScale"),
        ._sprite_texture_location = glGetUniformLocation(shader_program._id, "SpriteTexture")
    };
}

void destroy_shader(ShaderId* shader)
{
    if (shader && shader->_id)
    {
        GL_CHECK(glDeleteProgram(shader->_id));
        shader->_id = 0;
    }
}

void destroy_shaders(Renderer* renderer)
{
    destroy_shader(&renderer->_sprite_shader._shader);
    destroy_shader(&renderer->_to_screen_shader._shader);
}

uint32 compile_shader(uint32 shader_type, const char* source)
{
    uint32 shader_id = GL_CHECK(glCreateShader(shader_type));

    GL_CHECK(glShaderSource(shader_id, 1, &source, NULL));
    GL_CHECK(glCompileShader(shader_id));

    check_shader_error(shader_id);
    return shader_id;
}

void init_render_target(Renderer* renderer)
{
    TextureDescription texture_desc =
    {
        ._width = renderer->_render_width,
        ._height = renderer->_render_height,
        ._format = TextureFormats._rgba,
        ._internal_format = TextureInternalFormats._rgba8,
        ._type = TextureTypes._float,
        ._wrap_s = TextureWraps._clamp_to_edge,
        ._wrap_t = TextureWraps._clamp_to_edge,
        ._min_filter = TextureMinFilters._linear,
        ._mag_filter = TextureMagFilters._linear
    };

    renderer->_render_target = create_texture(texture_desc, NULL);
}

void set_gl_state_pre_render(Renderer* renderer)
{
    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glEnable(GL_CULL_FACE));
    GL_CHECK(glEnable(GL_BLEND));
    GL_CHECK(glEnable(GL_SCISSOR_TEST));
    GL_CHECK(glDepthFunc(GL_LEQUAL));
    GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    GL_CHECK(glViewport(0, 0, renderer->_window_width, renderer->_window_height));
    GL_CHECK(glScissor(0, 0, renderer->_window_width, renderer->_window_height));
    GL_CHECK(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    GLint x = renderer->_window_width / 2 - renderer->_render_width / 2;
    GLint y = renderer->_window_height / 2 - renderer->_render_height / 2;
    GL_CHECK(glViewport(x, y, renderer->_render_width, renderer->_render_height));
    GL_CHECK(glScissor(x, y, renderer->_render_width, renderer->_render_height));
    GL_CHECK(glClearColor(1.0f, 1.0f, 1.0f, 1.0f));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void set_gl_state_post_render()
{
    GL_CHECK(glDisable(GL_DEPTH_TEST));
    GL_CHECK(glDisable(GL_CULL_FACE));
    GL_CHECK(glDisable(GL_BLEND));
    GL_CHECK(glDisable(GL_SCISSOR_TEST));
}

void init_quad(Renderer* renderer)
{
    GL_CHECK(glGenVertexArrays(1, &QUAD._vertex_array));
    GL_CHECK(glBindVertexArray(QUAD._vertex_array));
    
    uint32* vertices = (uint32*) (&QUAD_DATA._vertices);
    QUAD._vertex_buffer = create_buffer(vertices, QUAD_DATA._vertex_stride_bytes, QUAD_DATA._vertex_count, GL_ARRAY_BUFFER, GL_STATIC_DRAW);

    uint32* indices = (uint32*) (&QUAD_DATA._indices);
    QUAD._index_buffer = create_buffer(indices, sizeof(uint32), QUAD_DATA._index_count, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
    
    GL_CHECK(glBindVertexArray(0));
}

void destroy_quad(Renderer* renderer)
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

/********************************************************
********************* PUBLIC API ************************
********************************************************/

Renderer create_renderer(RendererDescription renderer_description)
{
    Renderer renderer =
    {
        ._window_width = renderer_description._window_width,
        ._window_height = renderer_description._window_height,
        ._render_width = renderer_description._render_width,
        ._render_height = renderer_description._render_height,
        ._virtual_target_width = renderer_description._render_width,
        ._virtual_target_height = renderer_description._render_height
    };

    return renderer;
}

void initialize_renderer(Renderer* renderer)
{
    glewExperimental = GL_TRUE;
    assert(glewInit() == GLEW_OK);

    init_shaders(renderer);
    init_render_target(renderer);
    init_quad(renderer);
}

void deinitialize_renderer(Renderer* renderer)
{
    destroy_quad(renderer);
    destroy_texture(renderer->_render_target);
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

    *pixel_scale_x = width / virtual_width;     // win_width / virtual_width;
    *pixel_scale_y = height / virtual_height;   // win_height / virtual_height;
}

void renderer_resize(Renderer* renderer, uint32 window_width, uint32 window_height)
{
    renderer->_window_width = window_width;
    renderer->_window_height = window_height;

    fit_to_virtual_resolution(renderer->_window_width, renderer->_window_height, renderer->_virtual_target_width, renderer->_virtual_target_height, &renderer->_render_width, &renderer->_render_height, &renderer->_pixel_scale_x, &renderer->_pixel_scale_y);
}

void render(Renderer* renderer, Sprite* sprites, uint64 sprites_count)
{
    set_gl_state_pre_render(renderer);

    // todo:
    // 	--> copy quad data 
	//	--> send data to gpu
	// render sprite

    set_gl_state_post_render();
}

Sprite create_sprite(TextureId texture)
{
    Sprite sprite =
    {
        ._texture = texture,
        ._transform =
        {
            ._position[0] = 0.0f,
            ._position[1] = 0.0f,
            ._position[2] = 0.0f,

            ._scale[0] = 1.0f,
            ._scale[1] = 1.0f,

            ._rotation = 0.0f,

            ._uv_scale[0] = 1.0f,
            ._uv_scale[1] = 1.0f,

            ._uv_offset[0] = 0.0f,
            ._uv_offset[1] = 0.0f,
        }
    };

    return sprite;
}

