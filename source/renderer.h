#pragma once
#include "platform.h"

typedef struct RendererDescription
{
    uint64 _id;
} RendererDescription;

typedef struct Renderer
{
    uint64 _id;
} Renderer;

typedef struct TextureDescription
{
    uint64 _id;
} TextureDescription;


typedef struct TextureId
{
    uint64 _id;
} TextureId;

typedef struct Sprite
{
    TextureId _texture_id;
} Sprite;


Renderer create_renderer(RendererDescription renderer_description);
void initialize_renderer(Renderer* renderer);
void deinitialize_renderer(Renderer* renderer);

void render(Renderer* renderer, Sprite* sprites, uint64 sprites_count);
TextureId create_texture(TextureDescription texture_description, void* data);

