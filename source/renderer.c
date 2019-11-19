#include "renderer.h"
#include <assert.h>
#include <GL/glew.h>

void init_shaders(Renderer* renderer)
{
    // for the moment, we can just load the shaders from text file and later on just embedd the text file here!
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
    // Texture2D(std::string name, uint32 format = GL_RGBA, uint32 internal_format = GL_RGBA8, uint32 type = GL_UNSIGNED_BYTE, uint32 wrap_s = GL_REPEAT, uint32 wrap_t = GL_REPEAT, uint32 min_filter = GL_LINEAR_MIPMAP_LINEAR, uint32 mag_filter = GL_LINEAR);

    // _color_render_target = std::make_shared<Graphics::Texture2D>("Color Render Target", GL_RGBA, GL_RGBA16F, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR);
    //     _color_render_target->initialize(width, height, _settings.is_msaa_enabled(), _settings.msaa_sample_count());
    //     int err = glGetError();
    //             if (err != 0)
    //             {
    //                 SHF_PRINTF("Error %d happened BEFORE initializing Texture2D (%s | %s) \n", err, _name.c_str(), _path.c_str());
    //             }

//         _width = width;
// 			_height = height;
// 			_aspect_ratio = static_cast<float>(_width) / static_cast<float>(_height);

// 			_texture_target_type = msaa_enabled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

// 			/* create an enable texture buffer */
// 			glGenTextures(1, &_texture_id);
// 			glBindTexture(_texture_target_type, _texture_id);

// 			/* bind data to buffer*/
// 			if (msaa_enabled)
// 			{
// 				glTexImage2DMultisample(_texture_target_type, msaa_samples, _internal_format, width, height, false);
// 			} 
// 			else
// 			{
// 				glTexImage2D(_texture_target_type, 0, _internal_format, width, height, 0, _format, _type, nullptr);
// 			}


// 			/* set texture parameters */
// 			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _min_filter);
// 			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _mag_filter);

// 			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _wrap_s);
// 			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _wrap_t);

// 			/* unbind texture */
// 			glBindTexture(_texture_target_type, 0);

// #if DEVELOPMENT == 1
// 			{
// 				int err = glGetError();
// 				if (err != 0)
// 				{
// 					SHF_PRINTF("Error %d happened while initializing Texture2D (%s | %s) \n", err, _name.c_str(), _path.c_str());
// 				}
// 			}
// #endif
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