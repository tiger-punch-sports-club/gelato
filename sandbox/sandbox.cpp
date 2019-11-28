#include <stdio.h>
#include <vector>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "stb_image.h"

extern "C" 
{
	#include <renderer.h>
	#include <sprite.h>
}

GelatoRenderer* g_renderer;
void on_window_resized(GLFWwindow* window, int width, int height)
{
	glfwGetFramebufferSize(window, &width, &height);
	gelato_renderer_resize(g_renderer, width, height);
}

GelatoTextureId load_texture(const char* path)
{
	// load texture
	const char* pineapple_image_path = path;
	FILE* file = fopen(pineapple_image_path, "rb");
	int image_width, image_height, image_channels;
	unsigned char* data = stbi_load_from_file(file, &image_width, &image_height, &image_channels, 0);
	fclose(file);

	/* invert texture on y axis */
	for (int j = 0; j * 2 < image_height; ++j)
	{
		int index1 = j * image_width * image_channels;
		int index2 = (image_height - 1 - j) * image_width * image_channels;
		for (int i = image_width * image_channels; i > 0; --i)
		{
			unsigned char temp = data[index1];
			data[index1] = data[index2];
			data[index2] = temp;
			++index1;
			++index2;
		}
	}

	// create texture
	GelatoTextureId texture = gelato_create_texture(
		(GelatoTextureDescription) 
		{
		    ._width = (uint32)image_width,
		    ._height = (uint32)image_height,
		    ._format = GelatoTextureFormats._rgba,
			._internal_format = GelatoTextureInternalFormats._rgba8,
			._type = GelatoTextureTypes._unsigned_byte,
			._wrap_s = GelatoTextureWraps._clamp_to_edge,
			._wrap_t = GelatoTextureWraps._clamp_to_edge,
			._min_filter = GelatoTextureMinFilters._nearest,
    		._mag_filter = GelatoTextureMagFilters._nearest
		}, 
		(void*) data
	);

	stbi_image_free(data);

	return texture;
}

int main(void)
{
	if (glfwInit())
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RED_BITS, 8);
		glfwWindowHint(GLFW_GREEN_BITS, 8);
		glfwWindowHint(GLFW_BLUE_BITS, 8);
		glfwWindowHint(GLFW_ALPHA_BITS, 8);
		glfwWindowHint(GLFW_STENCIL_BITS, 8);
		glfwWindowHint(GLFW_DEPTH_BITS, 32);
		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	}

	GLFWwindow *window = glfwCreateWindow(640, 480, "Gelato89 - Sandbox", NULL, NULL);
  	glfwMakeContextCurrent(window);

	GelatoRendererDescription description = { 0 };
	description._window_width = 640;
	description._window_height = 480;
	description._render_width = 640;
	description._render_height = 480;
	description._clear_color_render_target[0] = 0.4f;
	description._clear_color_render_target[1] = 0.4f;
	description._clear_color_render_target[2] = 0.7f;

	GelatoRenderer renderer = gelato_create_renderer(description);
	gelato_initialize_renderer(&renderer);
	g_renderer = &renderer;
	glfwSetWindowSizeCallback(window, on_window_resized);

	GelatoTextureId texture = load_texture("content/pineapple.png");
	GelatoSprite pineapple = gelato_create_sprite(texture);
	pineapple._transform._scale[0] = 59.0f * 0.55f;
	pineapple._transform._scale[1] = 118.0f * 0.55f;
	pineapple._transform._position[0] = 320.0f;
	pineapple._transform._position[1] = 240.0f;
	pineapple._transform._position[2] = 5.f;


	GelatoTransform camera_transform = {};
	camera_transform._scale[0] = 1.0f;
	camera_transform._scale[1] = 1.0f;

	std::vector<GelatoSprite> sprites;

	GelatoTextureId flamingo_texture = load_texture("content/flamingo.png");
	for (int i = 0; i < 100; ++i)
	{
		GelatoSprite sprite = gelato_create_sprite(flamingo_texture);
		uint32 width = 152.0f * 0.33f;
		sprite._transform._scale[0] = width;

		uint32 height = 197.0f * 0.33f;
		sprite._transform._scale[1] = height;

		uint32 screen_width = description._render_width;
		float screen_width_step = (float) screen_width / width;

		sprite._transform._position[0] = screen_width_step * i;
		sprite._transform._position[1] = description._render_height / 2;

		sprites.push_back(sprite);
	}

	sprites.push_back(pineapple);
	GelatoSprite& pineapple_sprite = sprites[sprites.size() - 1];
	while (!glfwWindowShouldClose(window))
	{
		static float angle_speed = 80.0f;
		static float move_speed = 10.0f;

		pineapple_sprite._transform._angle_degrees += 0.016f * angle_speed;
		if(pineapple_sprite._transform._angle_degrees >= 360)
		{
			pineapple_sprite._transform._angle_degrees = 0;
		}

		if (glfwGetKey(window, GLFW_KEY_RIGHT))
		{
			pineapple_sprite._transform._position[0] += move_speed;
		}
		else if (glfwGetKey(window, GLFW_KEY_LEFT))
		{
			pineapple_sprite._transform._position[0] -= move_speed;
		}
			if (glfwGetKey(window, GLFW_KEY_UP))
		{
			pineapple_sprite._transform._position[1] += move_speed;
		}
		else if (glfwGetKey(window, GLFW_KEY_DOWN))
		{
			pineapple_sprite._transform._position[1] -= move_speed;
		}

		gelato_render(&renderer, &camera_transform, sprites.data(), sprites.size());
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
	
	gelato_deinitialize_renderer(&renderer);
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}