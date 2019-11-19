#include "renderer.h"
#include <assert.h>
#include <stdio.h>
#include <GL/glew.h>

void init_shaders(Renderer* renderer)
{
    // for the moment, we can just load the shaders from text file and later on just embedd the text file here!
}

void destroy_shaders(Renderer* renderer)
{
    if (renderer->_sprite_shader._vertex_shader._id)
    {
        GL_CHECK(glDeleteShader(renderer->_sprite_shader._vertex_shader._id));
        renderer->_sprite_shader._vertex_shader._id = 0;
    }

    if (renderer->_sprite_shader._fragment_shader._id)
    {
        GL_CHECK(glDeleteShader(renderer->_sprite_shader._fragment_shader._id));
        renderer->_sprite_shader._fragment_shader._id = 0;
    }
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
    GL_CHECK(glEnable(GL_SCISSOR_TEST));
    GL_CHECK(glDepthFunc(GL_LEQUAL));
    GL_CHECK(glEnable(GL_BLEND));
    GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    GL_CHECK(glViewport(0, 0, renderer->_window_width, renderer->_window_height));
    GL_CHECK(glScissor(0, 0, renderer->_window_width, renderer->_window_height));
    GL_CHECK(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		
    GL_CHECK(glViewport(renderer->_window_width / 2 - renderer->_render_width / 2, renderer->_window_height / 2 - renderer->_render_height / 2, renderer->_render_width, renderer->_render_height));
    GL_CHECK(glScissor(renderer->_window_width / 2 - renderer->_render_width / 2, renderer->_window_height / 2 - renderer->_render_height / 2, renderer->_render_width, renderer->_render_height));
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
}

void deinitialize_renderer(Renderer* renderer)
{
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

void fit_to_virtual_resolution(uint32 window_width, uint32 window_height, uint32 virtual_target_width, uint32 virtual_target_height, uint32* new_width, uint32* new_height)
{
    float virtual_width = (float) virtual_target_width;
    float virtual_height = (float) virtual_target_height;

    float win_width = (float) window_width;
    float win_height = (float) window_height;

    float width, height;

    max_resolution_aspect_ratio(virtual_width, virtual_height, win_width, win_height, &width, &height);

    *new_width = (uint32) width;
    *new_height = (uint32) height;
}

void renderer_resize(Renderer* renderer, uint32 window_width, uint32 window_height)
{
    renderer->_window_width = window_width;
    renderer->_window_height = window_height;

    fit_to_virtual_resolution(renderer->_window_width, renderer->_window_height, renderer->_virtual_target_width, renderer->_virtual_target_height, &renderer->_render_width, &renderer->_render_height);
}

void render(Renderer* renderer, Sprite* sprites, uint64 sprites_count)
{
    set_gl_state_pre_render(renderer);
    
    // render

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