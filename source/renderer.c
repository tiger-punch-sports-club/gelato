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

typedef struct RendererState
{
    uint32 _bound_sprites;
    uint32 _bound_textures;
    uint32 _bound_batches;

    GelatoTextureId _bound_textures_list[CFG_MAX_BOUND_TEXTURES];
    float _vertex_data[CFG_MAX_SPRITES_PER_BATCH * CFG_FLOATS_PER_SPRITE];
} RendererState;

RendererState BATCH_RENDERER_STATE =
{
    ._bound_sprites = 0,
    ._bound_textures = 0,
    ._bound_batches = 0,
    ._bound_textures_list = { 0 }
};

uint8 BAYER_FILTER[64] = 
{
	0, 128, 38, 160, 16, 136, 45, 168,		// 0
	191, 66, 224, 96, 200, 74, 233, 103,	// 1
	51, 176, 23, 142, 58, 184, 30, 152,		// 2
	241, 111, 208, 81, 249, 119, 216, 88,	// 3
	20, 139, 47, 170, 9, 131, 41, 163,		// 4
	204, 77, 237, 107, 196, 70, 229, 100,	// 5
	63, 188, 33, 155, 54, 179, 26, 146,		// 6
	254, 123, 220, 92, 244, 115, 212, 85	// 7
};

// --------------------
// helper functions
// --------------------
void check_shader_error(uint32 shader);
uint32 compile_shader(GLenum shader_type, const char* source);
void init_shaders(GelatoRenderer* renderer);
void destroy_shader(GelatoShaderId* shader);
void destroy_shaders(GelatoRenderer* renderer);
void init_framebuffer(GelatoRenderer* renderer);
void destroy_framebuffer(GelatoRenderer* renderer);
void init_render_targets(GelatoRenderer* renderer);
void resize_render_targets(GelatoRenderer* renderer, uint32 width, uint32 height);
void destroy_render_targets(GelatoRenderer* renderer);
void set_gl_state_pre_render(GelatoRenderer* renderer);
void set_gl_state_post_render();
void init_quad(GelatoRenderer* renderer);
void destroy_quad(GelatoRenderer* renderer);
uint32 create_buffer(void* data, uint32 stride_in_bytes, uint32 element_count, GLenum buffer_type, GLenum buffer_type_usage_type);
void make_projection_matrix(GelatoRenderer* renderer);
void init_bayer_filter(GelatoRenderer* renderer);

// --------------------
// batch rendering
// --------------------
void reset_tracking();
void begin_render(GelatoRenderer* renderer);
void end_render(GelatoRenderer* renderer);
void render_sprites(GelatoRenderer* renderer, GelatoSprite* sorted_sprites, uint32 sprites_count);
void submit(GelatoRenderer* renderer, GelatoSprite* sprite, GelatoTransform* transform);
bool texture_list_contains(GelatoTextureId texture, uint32* index);

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
	{	
		// GelatoSimpleSpriteShader
		GelatoShaderId shader_program = { glCreateProgram() };
		uint32 vertex_shader = compile_shader(GL_VERTEX_SHADER, "#version 150\nuniform mat4 ViewProjectionMatrix;\nin vec3 VertexPosition;\nin vec2 VertexUV;\nin vec4 VertexColor;\nin float VertexTextureIndex;\nout vec2 outVertexUV;\nout vec4 outVertexColor;\nout float outVertexTextureIndex;\nvoid main() {\noutVertexUV = VertexUV;\ngl_Position = ViewProjectionMatrix * vec4(VertexPosition, 1);\noutVertexColor = VertexColor;\noutVertexTextureIndex = VertexTextureIndex;\n}\n");
		uint32 fragment_shader = compile_shader(GL_FRAGMENT_SHADER, "#version 150\nuniform sampler2D TexturePool[16];\nin vec2 outVertexUV;\nin vec4 outVertexColor;\nin float outVertexTextureIndex;\nout vec4 outColor;\nvoid main() {\nvec4 color = texture2D(TexturePool[int(floor(outVertexTextureIndex))], outVertexUV);\nif (color.a <= 0.0f) {\ndiscard;\n}\n outColor = color * outVertexColor;\n}");

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
			._sprite_texture_location = glGetUniformLocation(shader_program._id, "SpriteTexture")
		};

		char buffer[1024];
		for (uint32 i = 0; i < CFG_MAX_BOUND_TEXTURES; i++)
		{
			sprintf_s(buffer, sizeof(buffer), "TexturePool[%d]", i);
			renderer->_sprite_shader._texture_pool_location[i] = glGetUniformLocation(shader_program._id, (const GLchar*)&buffer);
		}
	}

	{	
		// DitheringShader
		GelatoShaderId shader_program = { glCreateProgram() };
		uint32 vertex_shader = compile_shader(GL_VERTEX_SHADER, "#version 150\nuniform mat4 ViewProjectionMatrix;\nin vec3 VertexPosition;\nin vec2 VertexUV;\nout vec2 outVertexUV;\nvoid main() {\noutVertexUV = VertexUV;\ngl_Position = ViewProjectionMatrix * vec4(VertexPosition, 1);\n}");
		uint32 fragment_shader = compile_shader(GL_FRAGMENT_SHADER, "#version 150\nuniform sampler2D ColorTexture;\nuniform sampler2D DitheringTexture;\nuniform vec2 TextureSize;\nin vec2 outVertexUV;\nin vec4 outVertexColor;\nin float outVertexTextureIndex;\nout vec4 outColor;\n// Number of colors. 32 (5 bits) per channel\nconst vec3 _Colors = vec3(32.0);\nfloat channelError(float col, float colMin, float colMax)\n{\n    float range = abs(colMin - colMax);\n    float aRange = abs(col - colMin);\n    return aRange / range;\n}\nfloat ditheredChannel(float error, vec2 ditherBlockUV)\n{\n    float pattern = texture2D(DitheringTexture, ditherBlockUV).r;\n    return step(pattern, error);\n}\nfloat mix_f(float a, float b, float amt)\n{\n    return ((1.0 - amt) * a) + (b * amt);\n}\n/// YUV/RGB color space calculations\nvec3 RGBtoYUV(vec3 rgb) {\n    vec3 yuv;\n    yuv.r = rgb.r * 0.2126 + 0.7152 * rgb.g + 0.0722 * rgb.b;\n    yuv.g = (rgb.b - yuv.r) / 1.8556;\n    yuv.b = (rgb.r - yuv.r) / 1.5748;\n    // Adjust to work on GPU\n    yuv.gb += vec2(0.5, 0.5);\n    return yuv;\n}\nvec3 YUVtoRGB(vec3 yuv) {\n    yuv.gb -= 0.5;\n    return vec3(\n        yuv.r * 1.0 + yuv.g * 0.0 + yuv.b * 1.5748,\n        yuv.r * 1.0 + yuv.g * -0.187324 + yuv.b * -0.468124,\n        yuv.r * 1.0 + yuv.g * 1.8556 + yuv.b * 0.0);\n}\nvec3 ditherColor(vec3 col, vec2 uv, float xres, float yres) {\n    vec3 yuv = RGBtoYUV(col);\n    vec3 col1 = floor(yuv * _Colors) / _Colors;\n    vec3 col2 = ceil(yuv * _Colors) / _Colors;\n    \n    // Calculate dither texture UV based on the input texture\n    vec2 ditherBlockUV = uv * vec2(xres / 8.0, yres / 8.0);\n       yuv.x = mix_f(col1.x, col2.x, ditheredChannel(channelError(yuv.x, col1.x, col2.x), ditherBlockUV));\n    yuv.y = mix_f(col1.y, col2.y, ditheredChannel(channelError(yuv.y, col1.y, col2.y), ditherBlockUV));\n    yuv.z = mix_f(col1.z, col2.z, ditheredChannel(channelError(yuv.z, col1.z, col2.z), ditherBlockUV));\n        return(YUVtoRGB(yuv));\n}\nvoid main()\n{\n    vec4 color = texture2D(ColorTexture, outVertexUV);\n    outColor = vec4(ditherColor(color.rgb, outVertexUV, TextureSize.x, TextureSize.y), color.a);\n}\n");

		GL_CHECK(glAttachShader(shader_program._id, vertex_shader));
		GL_CHECK(glAttachShader(shader_program._id, fragment_shader));

		GL_CHECK(glLinkProgram(shader_program._id));
		GL_CHECK(glUseProgram(shader_program._id));

		GL_CHECK(glDeleteShader(vertex_shader));
		GL_CHECK(glDeleteShader(fragment_shader));

		GL_CHECK(glUseProgram(0));

		renderer->_dithering_shader = (GelatoDitheringShader) 
		{
			._shader = shader_program,
			._vertex_attribute_location = glGetAttribLocation(shader_program._id, "VertexPosition"),
			._uv_attribute_location = glGetAttribLocation(shader_program._id, "VertexUV"),
			._view_projection_matrix_location = glGetUniformLocation(shader_program._id, "ViewProjectionMatrix"),
			._color_texture_location = glGetUniformLocation(shader_program._id, "ColorTexture"),
			._dithering_texture_location = glGetUniformLocation(shader_program._id, "DitheringTexture"),
			._texture_size_location = glGetUniformLocation(shader_program._id, "TextureSize")
		};
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

void init_framebuffer(GelatoRenderer* renderer)
{
	GL_CHECK(glGenFramebuffers(1, &renderer->_framebuffer_id._id));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, renderer->_framebuffer_id._id));
	GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderer->_color_render_target._id, 0));
	GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderer->_depth_render_target._id, 0));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void destroy_framebuffer(GelatoRenderer* renderer)
{
	GL_CHECK(glDeleteFramebuffers(1, &renderer->_framebuffer_id._id));
}

void init_render_targets(GelatoRenderer* renderer)
{
	GelatoTextureDescription color_render_target_description =
	{
		._width = renderer->_render_width,
		._height = renderer->_render_height,
		._format = GelatoTextureFormats._rgba,
		._internal_format = GelatoTextureInternalFormats._rgba8,
		._type = GelatoTextureTypes._float,
		._wrap_s = GelatoTextureWraps._clamp_to_edge,
		._wrap_t = GelatoTextureWraps._clamp_to_edge,
		._min_filter = GelatoTextureMinFilters._linear,
		._mag_filter = GelatoTextureMagFilters._linear
	};

	renderer->_color_render_target = gelato_create_texture(color_render_target_description, (void*)NULL);
	renderer->_post_processing_color_render_target = gelato_create_texture(color_render_target_description, (void*)NULL);

	GelatoTextureDescription depth_render_target_description =
	{
		._width = renderer->_render_width,
		._height = renderer->_render_height,
		._format = GelatoTextureFormats._depth_component ,
		._internal_format = GelatoTextureInternalFormats._depth_component_32f,
		._type = GelatoTextureTypes._float,
		._wrap_s = GelatoTextureWraps._clamp_to_edge,
		._wrap_t = GelatoTextureWraps._clamp_to_edge,
		._min_filter = GelatoTextureMinFilters._linear,
		._mag_filter = GelatoTextureMagFilters._linear
	};

	renderer->_depth_render_target = gelato_create_texture(depth_render_target_description, (void*)NULL);
}

void resize_render_targets(GelatoRenderer* renderer, uint32 width, uint32 height)
{
	destroy_render_targets(renderer);
	init_render_targets(renderer);
}

void destroy_render_targets(GelatoRenderer* renderer)
{
	gelato_destroy_texture(renderer->_color_render_target);
	gelato_destroy_texture(renderer->_depth_render_target);
	gelato_destroy_texture(renderer->_post_processing_color_render_target);
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
    GL_CHECK(glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE));
    GL_CHECK(glEnable(GL_SAMPLE_ALPHA_TO_ONE));
	
	// Clear entire window
    GL_CHECK(glViewport(0, 0, renderer->_window_width, renderer->_window_height));
    GL_CHECK(glScissor(0, 0, renderer->_window_width, renderer->_window_height));
    GL_CHECK(glClearColor(renderer->_clear_color_letter_box[0], renderer->_clear_color_letter_box[1], renderer->_clear_color_letter_box[2], 1.0f));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	// Attach framebuffer and clear it
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderer->_framebuffer_id._id);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	GL_CHECK(glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderer->_color_render_target._id, 0));
	GL_CHECK(glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderer->_depth_render_target._id, 0));
	GL_CHECK(glViewport(0, 0, renderer->_render_width, renderer->_render_height));
	GL_CHECK(glScissor(0, 0, renderer->_render_width, renderer->_render_height));
	GL_CHECK(glClearColor(renderer->_clear_color_render_target[0], renderer->_clear_color_render_target[1], renderer->_clear_color_render_target[2], 1.0f));
	GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void set_gl_state_post_render()
{
    GL_CHECK(glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE));
    GL_CHECK(glDisable(GL_SAMPLE_ALPHA_TO_ONE));
    GL_CHECK(glDisable(GL_DEPTH_TEST));
    GL_CHECK(glDisable(GL_CULL_FACE));
    GL_CHECK(glDisable(GL_BLEND));
    GL_CHECK(glDisable(GL_SCISSOR_TEST));
}

void init_quad(GelatoRenderer* renderer)
{
    GL_CHECK(glGenVertexArrays(1, &QUAD._vertex_array));
    GL_CHECK(glBindVertexArray(QUAD._vertex_array));

    QUAD._vertex_buffer = create_buffer(&BATCH_RENDERER_STATE._vertex_data[0], QUAD_DATA._vertex_stride_bytes, CFG_VERTEX_COUNT_PER_SPRITE * CFG_MAX_SPRITES_PER_BATCH, GL_ARRAY_BUFFER, GL_STREAM_DRAW);

    uint32 indices[6 * CFG_MAX_SPRITES_PER_BATCH];
    uint32 offset = 0;
    for(uint32 i = 0; i < 6 * CFG_MAX_SPRITES_PER_BATCH;)
    {
        indices[i++] = offset + 0;
        indices[i++] = offset + 1;
        indices[i++] = offset + 2;

        indices[i++] = offset + 2;
        indices[i++] = offset + 3;
        indices[i++] = offset + 0;

        offset += 4;
    }

    QUAD._index_buffer = create_buffer(&indices, sizeof(uint32), 6 * CFG_MAX_SPRITES_PER_BATCH, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);

    GL_CHECK(glBindVertexArray(0));
}

void destroy_quad(GelatoRenderer* renderer)
{
    GL_CHECK(glDeleteVertexArrays(1, &QUAD._vertex_array));
    GL_CHECK(glDeleteBuffers(1, &QUAD._vertex_buffer));
    GL_CHECK(glDeleteBuffers(1, &QUAD._index_buffer));
}

uint32 create_buffer(void* data, uint32 stride_in_bytes, uint32 element_count, GLenum buffer_type, GLenum buffer_type_usage_type)
{
    uint32 handle;

    GL_CHECK(glGenBuffers(1, &handle));
    GL_CHECK(glBindBuffer(buffer_type, handle));
    GL_CHECK(glBufferData(buffer_type, stride_in_bytes * element_count, data, buffer_type_usage_type));
    GL_CHECK(glBindBuffer(buffer_type, 0));

    return handle;
}

void make_projection_matrix(GelatoRenderer* renderer)
{
	float left = 0.0f;
	float right = (float)renderer->_window_width;
	float top = (float)renderer->_window_height;
	float bottom = 0.0f;
	float near_plane = -10.0f;
	float far_plane = 10.0f;

	gelato_make_projection_matrix(left, right, bottom, top, near_plane, far_plane, &renderer->_projection_matrix[0]);
}

void init_bayer_filter(GelatoRenderer* renderer)
{
	renderer->_bayer_filter_texture = gelato_create_texture(((GelatoTextureDescription){
		._width = 8,
		._height = 8,
		._format = GelatoTextureFormats._rgba,
		._internal_format = GelatoTextureInternalFormats._rgba8,
		._type = GelatoTextureTypes._unsigned_byte,
		._wrap_s = GelatoTextureWraps._repeat,
		._wrap_t = GelatoTextureWraps._repeat,
		._min_filter = GelatoTextureMinFilters._nearest,
		._mag_filter = GelatoTextureMagFilters._nearest
	}), &BAYER_FILTER[0]);
}

void reset_tracking()
{
    for (uint32 i = 0; i < BATCH_RENDERER_STATE._bound_textures; ++i)
    {
        BATCH_RENDERER_STATE._bound_textures_list[i] = (GelatoTextureId) { 0 };
    }

    BATCH_RENDERER_STATE._bound_sprites = 0;
    BATCH_RENDERER_STATE._bound_textures = 0;
    BATCH_RENDERER_STATE._bound_batches = 0;
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
	{
		// unbind simple shader
		GL_CHECK(glDisableVertexAttribArray(renderer->_sprite_shader._texture_index_attribute_location));
		GL_CHECK(glDisableVertexAttribArray(renderer->_sprite_shader._color_attribute_location));
		GL_CHECK(glDisableVertexAttribArray(renderer->_sprite_shader._uv_attribute_location));
		GL_CHECK(glDisableVertexAttribArray(renderer->_sprite_shader._vertex_attribute_location));
	}

    {
		// perform post processing here
		GL_CHECK(glUseProgram(renderer->_dithering_shader._shader._id));

		GL_CHECK(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
		GL_CHECK(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderer->_framebuffer_id._id));
		GL_CHECK(glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderer->_post_processing_color_render_target._id, 0));
		GL_CHECK(glClearColor(0.0f, 0.0f, 1.0f, 1.0f));
		GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));

		GL_CHECK(glActiveTexture(GL_TEXTURE0));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, renderer->_color_render_target._id));
		GL_CHECK(glUniform1i(renderer->_dithering_shader._color_texture_location, 0));

		GL_CHECK(glActiveTexture(GL_TEXTURE1));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, renderer->_bayer_filter_texture._id));
		GL_CHECK(glUniform1i(renderer->_dithering_shader._dithering_texture_location, 1));

		float ortho_matrix[16];
		gelato_make_projection_matrix(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f, &ortho_matrix[0]);
		GL_CHECK(glUniformMatrix4fv(renderer->_dithering_shader._view_projection_matrix_location, 1, GL_FALSE, &ortho_matrix[0]));
		
		float texture_size[2] = { renderer->_render_width, renderer->_render_height };
		GL_CHECK(glUniform2fv(renderer->_dithering_shader._texture_size_location, 1, &texture_size[0]));

		{
			memcpy(&BATCH_RENDERER_STATE._vertex_data[0], &QUAD_DATA._vertices[0], QUAD_DATA._vertex_stride_bytes * QUAD_DATA._vertex_count);
			GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, 0, CFG_SPRITE_SIZE_BYTES, &BATCH_RENDERER_STATE._vertex_data));

			GL_CHECK(glEnableVertexAttribArray(renderer->_dithering_shader._vertex_attribute_location));
			GL_CHECK(glVertexAttribPointer(renderer->_dithering_shader._vertex_attribute_location, 3, GL_FLOAT, GL_FALSE, QUAD_DATA._vertex_stride_bytes, NULL));

			GL_CHECK(glEnableVertexAttribArray(renderer->_dithering_shader._uv_attribute_location));
			GL_CHECK(glVertexAttribPointer(renderer->_dithering_shader._uv_attribute_location, 2, GL_FLOAT, GL_FALSE, QUAD_DATA._vertex_stride_bytes, (GLvoid*)(3 * sizeof(float))));

			GL_CHECK(glDisable(GL_DEPTH_TEST));
			GL_CHECK(glDisable(GL_CULL_FACE));

			GL_CHECK(glDrawElements(GL_TRIANGLES, QUAD_DATA._index_count, GL_UNSIGNED_INT, 0));

			GL_CHECK(glEnable(GL_DEPTH_TEST));
			GL_CHECK(glEnable(GL_CULL_FACE));

			GL_CHECK(glDisableVertexAttribArray(renderer->_dithering_shader._vertex_attribute_location));
			GL_CHECK(glDisableVertexAttribArray(renderer->_dithering_shader._uv_attribute_location));
		}
    }

    {
		// blit framebuffer to screen
		GL_CHECK(glUseProgram(0));
		GL_CHECK(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
		GL_CHECK(glBindFramebuffer(GL_READ_FRAMEBUFFER, renderer->_framebuffer_id._id));
		GL_CHECK(glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderer->_post_processing_color_render_target._id, 0));

		GLint x = renderer->_window_width / 2 - renderer->_render_width / 2;
		GLint y = renderer->_window_height / 2 - renderer->_render_height / 2;
		GL_CHECK(glViewport(x, y, renderer->_render_width, renderer->_render_height));
		GL_CHECK(glScissor(x, y, renderer->_render_width, renderer->_render_height));

		GL_CHECK(glDrawBuffer(GL_BACK));
		GL_CHECK(glBlitFramebuffer(0, 0, renderer->_render_width, renderer->_render_height, x, y, x + renderer->_render_width, y + renderer->_render_height, GL_COLOR_BUFFER_BIT, GL_NEAREST));
		GL_CHECK(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
    }

	{	// Unbind vertex arrays
		GL_CHECK(glBindVertexArray(0));
		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
		GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}
	
}

void render_sprites(GelatoRenderer* renderer, GelatoSprite* sorted_sprites, uint32 sprites_count)
{
    uint32 sprites_left = sprites_count;
    for(uint64 i = 0; i < sprites_count;)
    {
        GelatoSprite* sprite = &sorted_sprites[i];
        GelatoTransform* transform = &sprite->_transform;

        bool batch_has_room = BATCH_RENDERER_STATE._bound_sprites < CFG_MAX_SPRITES_PER_BATCH;
        bool flush = false;

        if (batch_has_room)
        {
            bool texture_list_has_room = BATCH_RENDERER_STATE._bound_textures < CFG_MAX_BOUND_TEXTURES;
            uint32 texture_index = 0;
            bool texture_exists_in_batch = texture_list_contains(sprite->_texture, &texture_index);

            if (texture_exists_in_batch)
            {
                submit(renderer, sprite, transform);
                sprites_left--;
                i++;
            }
            else if (texture_list_has_room)
            {
                BATCH_RENDERER_STATE._bound_textures_list[BATCH_RENDERER_STATE._bound_textures] = sprite->_texture;
                BATCH_RENDERER_STATE._bound_textures++;
                submit(renderer, sprite, transform);
                sprites_left--;
                i++;
            }
            else
            {
                flush = true;
            }
        } 
        else 
        {
            flush = true;
        }

        flush = flush || (sprites_left <= 0);

        if (flush)
        {
            // copy data
            GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, 0, CFG_SPRITE_SIZE_BYTES * BATCH_RENDERER_STATE._bound_sprites, &BATCH_RENDERER_STATE._vertex_data));

            // bind textures
            for (uint32 j = 0, l = BATCH_RENDERER_STATE._bound_textures; j < l; ++j)
            {
                GL_CHECK(glActiveTexture(GL_TEXTURE0 + j));
                GL_CHECK(glBindTexture(GL_TEXTURE_2D, BATCH_RENDERER_STATE._bound_textures_list[j]._id));
                GL_CHECK(glUniform1i(renderer->_sprite_shader._texture_pool_location[j], j));
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
    uint64 current_offset = BATCH_RENDERER_STATE._bound_sprites * QUAD_DATA._vertex_stride * QUAD_DATA._vertex_count;

    float* vertex_data = &BATCH_RENDERER_STATE._vertex_data[current_offset];

    // create sprite transformation
    float model_matrix[16];
    gelato_make_transformation(transform, &model_matrix[0]);

    // write position
    const float* quad_pos = &QUAD_DATA._vertices[0];
    float* vertex_position_0 = (float*) quad_pos;
    gelato_mul_vec_matrix(vertex_position_0, &model_matrix[0], &vertex_data[0]);

    float* vertex_position_1 = vertex_position_0 + QUAD_DATA._vertex_stride;
    gelato_mul_vec_matrix(vertex_position_1, &model_matrix[0], vertex_data + QUAD_DATA._vertex_stride * 1);

    float* vertex_position_2 = vertex_position_1 + QUAD_DATA._vertex_stride;
    gelato_mul_vec_matrix(vertex_position_2, &model_matrix[0], vertex_data + QUAD_DATA._vertex_stride * 2);

    float* vertex_position_3 = vertex_position_2 + QUAD_DATA._vertex_stride;
    gelato_mul_vec_matrix(vertex_position_3, &model_matrix[0], vertex_data + QUAD_DATA._vertex_stride * 3);

    // write uv
    float u_scale = sprite->_uv_scale[0];
    float v_scale = sprite->_uv_scale[1];

    float u_start = sprite->_uv_start[0] * u_scale;
    float v_start = sprite->_uv_start[1] * v_scale;

    float u_end = sprite->_uv_end[0] * u_scale;
    float v_end = sprite->_uv_end[1] * v_scale;

    uint32 uv_size = sizeof(float) * 2;
    float* uvs = &vertex_data[3];
    const float* quad_uvs = &QUAD_DATA._vertices[3];

    float* uv_0 = &uvs[0];
    memcpy(uv_0, quad_uvs, uv_size);
    uv_0[0] = u_start;
    uv_0[1] = v_start;

    float* uv_1 = uv_0 + QUAD_DATA._vertex_stride;
    memcpy(uv_1, quad_uvs + QUAD_DATA._vertex_stride, uv_size);
    uv_1[0] = u_end;
    uv_1[1] = v_start;

    float* uv_2 = uv_1 + QUAD_DATA._vertex_stride;
    memcpy(uv_2, quad_uvs + QUAD_DATA._vertex_stride * 2, uv_size);
    uv_2[0] = u_end;
    uv_2[1] = v_end;

    float* uv_3 = uv_2 + QUAD_DATA._vertex_stride;
    memcpy(uv_3, quad_uvs + QUAD_DATA._vertex_stride * 3, uv_size);
    uv_3[0] = u_start;
    uv_3[1] = v_end;

    // write colors
    uint32 color_size = 4 * sizeof(float);
    float* color_0 = &vertex_data[5];
    memcpy(color_0, &sprite->_color[0], color_size);

    float* color_1 = color_0 + QUAD_DATA._vertex_stride;
    memcpy(color_1, &sprite->_color[0], color_size);

    float* color_2 = color_1 + QUAD_DATA._vertex_stride;
    memcpy(color_2, &sprite->_color[0], color_size);

    float* color_3 = color_2 + QUAD_DATA._vertex_stride;
    memcpy(color_3, &sprite->_color[0], color_size);

    // write texture index
    uint32 texture_index = 0;
    texture_list_contains(sprite->_texture, &texture_index);

    float* texture_index_0 = &vertex_data[9];
    *texture_index_0 = (float)(texture_index + 0.25f);

    float* texture_index_1 = texture_index_0 + QUAD_DATA._vertex_stride;
    *texture_index_1 = (float)(texture_index + 0.25f);

    float* texture_index_2 = texture_index_1 + QUAD_DATA._vertex_stride;
    *texture_index_2 = (float)(texture_index + 0.25f);

    float* texture_index_3 = texture_index_2 + QUAD_DATA._vertex_stride;
    *texture_index_3 = (float)(texture_index + 0.25f);

    BATCH_RENDERER_STATE._bound_sprites++;
}

bool texture_list_contains(GelatoTextureId texture, uint32* index)
{
    for (uint32 i = 0; i < BATCH_RENDERER_STATE._bound_textures; i++)
    {
        if (BATCH_RENDERER_STATE._bound_textures_list[i]._id == texture._id)
        {
            *index = i;
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
		init_render_targets(renderer);
		init_framebuffer(renderer);
		init_bayer_filter(renderer);
        gelato_renderer_resize(renderer, renderer->_window_width, renderer->_window_height);
    }
    else
    {
        assert(0);
    }
}

void gelato_deinitialize_renderer(GelatoRenderer* renderer)
{
	destroy_framebuffer(renderer);
	destroy_render_targets(renderer);
	gelato_destroy_texture(renderer->_bayer_filter_texture);
    destroy_shaders(renderer);
    destroy_quad(renderer);
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
	resize_render_targets(renderer, renderer->_render_width, renderer->_render_height);

	make_projection_matrix(renderer);
}

void gelato_render(GelatoRenderer* renderer, GelatoTransform* camera_transform, GelatoSprite* sorted_sprites, uint32 sprites_count)
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
