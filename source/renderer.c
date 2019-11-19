#include "renderer.h"
#include <assert.h>
#include <GL/glew.h>

void init_shaders(Renderer* renderer)
{
    
}

void destroy_shaders(Renderer* renderer)
{
    if (renderer->_sprite_shader._vertex_shader._id)
    {
        glDeleteShader(renderer->_sprite_shader._vertex_shader._id);
        renderer->_sprite_shader._vertex_shader._id = 0;
    }

    if (renderer->_sprite_shader._fragment_shader._id)
    {
        glDeleteShader(renderer->_sprite_shader._fragment_shader._id);    
        renderer->_sprite_shader._fragment_shader._id = 0;
    }
}

void init_render_target(Renderer* renderer)
{

}

void set_gl_state_pre_render()
{
    // set culling & stuff
}

void set_gl_state_post_render()
{
    // unset culling & stuff
}

/********************************************************
********************* PUBLIC API ************************
********************************************************/

Renderer create_renderer(RendererDescription renderer_description)
{
    Renderer renderer =
    {
        ._render_width = renderer_description._render_width,
        ._render_height = renderer_description._render_height
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

void resize(Renderer* renderer, uint64 window_width, uint64 window_height)
{
    // todo: resize based on aspect ratio
}

void render(Renderer* renderer, Sprite* sprites, uint64 sprites_count)
{
    set_gl_state_pre_render();
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    set_gl_state_post_render();
}

TextureId create_texture(TextureDescription texture_description, void* data)
{
    TextureId texture_id = { 0 };
    return texture_id;
}

void destroy_texture(TextureId texture)
{
    glDeleteTextures(1, (GLuint*) &texture._id);
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