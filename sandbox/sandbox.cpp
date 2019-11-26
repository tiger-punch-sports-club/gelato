#include <stdio.h>

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

	// load texture
	const char* pineapple_image_path = "content/pineapple.png";
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
	GelatoTextureId texture = create_texture(
		(TextureDescription) 
		{
		    ._width = (uint32)image_width,
		    ._height = (uint32)image_height,
		    ._format = TextureFormats._rgba,
			._internal_format = TextureInternalFormats._rgba8,
			._type = TextureTypes._unsigned_byte,
			._wrap_s = TextureWraps._clamp_to_edge,
			._wrap_t = TextureWraps._clamp_to_edge,
			._min_filter = TextureMinFilters._nearest,
    		._mag_filter = TextureMagFilters._nearest
		}, 
		(void*) data
	);
	
	// create sprite
	GelatoSprite sprite = create_sprite(texture);
	stbi_image_free(data);

	sprite._transform._scale[0] = 64.0f;
	sprite._transform._scale[1] = 64.0f;

	sprite._transform._position[0] = 320.0f;
	sprite._transform._position[1] = 240.0f;

	GelatoTransform camera_transform = {};
	camera_transform._scale[0] = 1.0f;
	camera_transform._scale[1] = 1.0f;

	camera_transform._position[0] = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		float angle_speed = 80.0f;
		sprite._transform._angle_degrees += 0.016f * angle_speed;
		if(sprite._transform._angle_degrees >= 360)
		{
			sprite._transform._angle_degrees = 0;
		}

		gelato_render(&renderer, &camera_transform, &sprite, 1);
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
	
	gelato_deinitialize_renderer(&renderer);
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}