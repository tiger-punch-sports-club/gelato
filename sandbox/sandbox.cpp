#include <stdio.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "stb_image.h"

extern "C" 
{
	#include <renderer.h>
}

Renderer* g_renderer;
void on_window_resized(GLFWwindow* window, int width, int height)
{
	glfwGetFramebufferSize(window, &width, &height);
	renderer_resize(g_renderer, width, height);
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

	GLFWwindow *window = glfwCreateWindow(640, 480, "Gelato32 - Sandbox", NULL, NULL);
  	glfwMakeContextCurrent(window);

	RendererDescription description = { 0 };
	description._window_width = 640;
	description._window_height = 480;
	description._render_width = 640;
	description._render_height = 480;

	Renderer renderer = create_renderer(description);
	initialize_renderer(&renderer);
	g_renderer = &renderer;
	glfwSetWindowSizeCallback(window, on_window_resized);

	// load texture
	const char* pineapple_image_path = "content/pineapple.png";
	FILE* file = fopen(pineapple_image_path, "rb");
	int image_width, image_height, image_channels;
	unsigned char* data = stbi_load_from_file(file, &image_width, &image_height, &image_channels, 0);
	fclose(file);

	// create texture
	TextureId texture = create_texture((TextureDescription) {
		    ._width = (uint32)image_width,
		    ._height = (uint32)image_height,
		    ._format = TextureFormats._rgba,
			._internal_format = TextureInternalFormats._rgba8,
			._type = TextureTypes._unsigned_byte,
			._wrap_s = TextureWraps._clamp_to_edge,
			._wrap_t = TextureWraps._clamp_to_edge,
			._min_filter = TextureMinFilters._nearest,
    		._mag_filter = TextureMagFilters._nearest
	}, (void*) data);
	
	// create sprite
	Sprite sprite = create_sprite(texture);

	while (!glfwWindowShouldClose(window))
	{
		render(&renderer, &sprite, 1);
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
	
	deinitialize_renderer(&renderer);
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}