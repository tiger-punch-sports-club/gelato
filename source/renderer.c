#include "renderer.h"
#include <assert.h>
#include <GL/glew.h>

Renderer create_renderer(RendererDescription renderer_description)
{
    Renderer renderer  = { 0 };
    return renderer;
}

void initialize_renderer(Renderer* renderer)
{
    glewExperimental = GL_TRUE;
    assert(glewInit() == GLEW_OK);
}

void deinitialize_renderer(Renderer* renderer)
{

}

void render(Renderer* renderer, Sprite* sprites, uint64 sprites_count)
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

TextureId create_texture(TextureDescription texture_description, void* data)
{
    TextureId texture_id = { 0 };
    return texture_id;
}