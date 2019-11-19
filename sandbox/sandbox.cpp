#include <stdio.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

extern "C" 
{
	#include <renderer.h>
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
	description._window_height = 480;

	Renderer renderer = create_renderer(description);
	initialize_renderer(&renderer);

	while (!glfwWindowShouldClose(window))
	{
		render(&renderer, nullptr, 0);
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
	
	deinitialize_renderer(&renderer);
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}