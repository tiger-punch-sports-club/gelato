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

GelatoTextureId load_texture(const char* path, uint32& width, uint32& height)
{
	// load texture
	FILE* file = fopen(path, "rb");
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

	GelatoTextureDescription desc = {};
	desc._width = static_cast<uint32>(image_width);
	desc._height = static_cast<uint32>(image_height);
	desc._format = GelatoTextureFormats._rgba;
	desc._internal_format = GelatoTextureInternalFormats._rgba8;
	desc._type = GelatoTextureTypes._unsigned_byte;
	desc._wrap_s = GelatoTextureWraps._clamp_to_edge;
	desc._wrap_t = GelatoTextureWraps._clamp_to_edge;
	desc._min_filter = GelatoTextureMinFilters._nearest;
	desc._mag_filter = GelatoTextureMagFilters._nearest;

	GelatoTextureId texture = gelato_create_texture(desc,static_cast<void*>(data));

	stbi_image_free(data);

	width = static_cast<uint32>(image_width);
	height = static_cast<uint32>(image_height);
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

	GLFWwindow* window = glfwCreateWindow(640, 480, "Gelato89 - Sandbox", nullptr, nullptr);
  	glfwMakeContextCurrent(window);

	GelatoRendererDescription description = { 0 };
	description._window_width = 640;
	description._window_height = 480;
	description._render_width = 1280;
	description._render_height = 720;
	description._clear_color_render_target[0] = 0.4f;
	description._clear_color_render_target[1] = 0.4f;
	description._clear_color_render_target[2] = 0.9f;

	GelatoRenderer renderer = gelato_create_renderer(description);
	gelato_initialize_renderer(&renderer);
	g_renderer = &renderer;
	glfwSetWindowSizeCallback(window, on_window_resized);

	uint32 pineapple_texture_width = 0;
	uint32 pineapple_texture_height = 0;
	GelatoTextureId pineapple_texture = load_texture("content/pineapple.png", pineapple_texture_width, pineapple_texture_height);

	uint32 cat_texture_width = 0;
	uint32 cat_texture_height = 0;
	GelatoTextureId cat_texture = load_texture("content/cat.png", cat_texture_width, cat_texture_height);

	GelatoSprite the_main_sprite = gelato_create_sprite(cat_texture);
	the_main_sprite._transform._scale[0] = cat_texture_width * 0.35f;
	the_main_sprite._transform._scale[1] = cat_texture_height * 0.35f;
	the_main_sprite._transform._position[0] = renderer._virtual_target_width / 2.0f;
	the_main_sprite._transform._position[1] = renderer._virtual_target_height / 2.0f;
	the_main_sprite._transform._position[2] = 8.f;


	//> sprite sheet demo
	uint32 lean_cup_sheet_texture_width = 0;
	uint32 lean_cup_sheet_texture_height = 0;
	GelatoTextureId lean_cup_sheet = load_texture("content/lean_cup_sheet.png", lean_cup_sheet_texture_width, lean_cup_sheet_texture_height);

	GelatoSpriteSheetDescription sprite_sheet_desc = { 0 };
	sprite_sheet_desc._width = lean_cup_sheet_texture_width;
	sprite_sheet_desc._height = lean_cup_sheet_texture_height;
	sprite_sheet_desc._tile_size_x = 59;
	sprite_sheet_desc._tile_size_y = 88;

	GelatoSpriteSheetInfo sprite_sheet_info = gelato_create_sprite_sheet(sprite_sheet_desc, lean_cup_sheet);
	gelato_change_sprite_tile(&the_main_sprite, 0, 1, sprite_sheet_info);

	//> Sprite sheet demo end

	GelatoTransform camera_transform = {};
	camera_transform._scale[0] = 1.0f;
	camera_transform._scale[1] = 1.0f;

	std::vector<GelatoSprite> sprites;

	uint32 flamingo_texture_width = 0;
	uint32 flamingo_texture_height = 0;
	GelatoTextureId flamingo_texture = load_texture("content/flamingo.png", flamingo_texture_width, flamingo_texture_height);
	float width = flamingo_texture_width * 0.28f;
	float height = flamingo_texture_height * 0.28f;

	float x_offset = static_cast<float>(-width) / 2.0f;
	float y_offset = static_cast<float>(-height) / 2.0f;

	uint32 max_x = renderer._virtual_target_width / static_cast<uint32>(width) * 1.5f;
	uint32 count = 0;
	for (int i = 0; i < 666; ++i)
	{
		x_offset += width;

		bool second_sprite = i % 2 != 0;
		GelatoSprite sprite = gelato_create_sprite(second_sprite ? flamingo_texture : pineapple_texture);
		
		sprite._transform._scale[0] = second_sprite ? width : pineapple_texture_width * 0.75f * 0.55f;
		sprite._transform._scale[1] = second_sprite ? height : pineapple_texture_height * 0.75f * 0.55f;

		sprite._transform._position[0] = x_offset;
		sprite._transform._position[1] = y_offset;
		sprite._transform._position[2] = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 3.0f;

		sprites.push_back(sprite);

		count++;
		if (count == max_x)
		{
			count = 0;
			x_offset = static_cast<float>(-width) / 2.0f;
			y_offset += 60;
		}

		if (y_offset >= renderer._virtual_target_height + height)
		{
			printf("broke after %d flamingos\n", i);
			break;
		}
	}

	sprites.push_back(the_main_sprite);
	GelatoSprite& pineapple_sprite = sprites[sprites.size() - 1];
	while (!glfwWindowShouldClose(window))
	{
		static float angle_speed = 40.0f;
		static float move_speed = 10.0f;

		pineapple_sprite._transform._rotation[2] += 0.016f * angle_speed;
		if(pineapple_sprite._transform._rotation[2] >= 360)
		{
			pineapple_sprite._transform._rotation[2] = 0;
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

		for (int i = 0; i < sprites.size() / 2; ++i)
		{
			GelatoSprite& sprite = sprites.at(i * 2);
			sprite._transform._rotation[2] -= 0.016f * angle_speed / 8;
			if(sprite._transform._rotation[2] <= -360.0f)
			{
				sprite._transform._rotation[2] = 0.0f;
			}
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