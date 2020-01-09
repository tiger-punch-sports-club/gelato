# Gelato89
Gelato89 is a c99-compliant, 2d only sprite renderer with order independent transparency.
Nothing more, nothing less.

API
=============================================================================================================
## Renderer Functions
    GelatoRenderer gelato_create_renderer(GelatoRendererDescription renderer_description);
    void gelato_initialize_renderer(GelatoRenderer* renderer);
    void gelato_deinitialize_renderer(GelatoRenderer* renderer);
    void gelato_renderer_resize(GelatoRenderer* renderer, uint32 window_width, uint32 window_height);
    void gelato_render(GelatoRenderer* renderer, GelatoTransform* camera_transform, GelatoSprite* sorted_sprites, uint64 sprites_count);

## Sprite Functions
    GelatoSprite gelato_create_sprite(GelatoTextureId texture);

## Texture Functions
    GelatoTextureId gelato_create_texture(GelatoTextureDescription texture_description, void* data);
    void gelato_destroy_texture(GelatoTextureId texture);

## Utils Functions
    void gelato_mul_matrix(float* left, float* right, float* dest);
    void gelato_mul_vec_matrix(float* vec, float* matrix, float* dest);
    void gelato_make_scale_matrix(float x, float y, float z, float* dest);
    void gelato_make_translation_matrix(float x, float y, float z, float* dest);
    void gelato_make_rotation_matrix(float angle_in_degrees, float* dest);
    void gelato_make_identity_matrix(float* dest);
    void gelato_make_projection_matrix(float left, float right, float bottom, float top, float near_plane, float far_plane, float* dest);
    void gelato_make_transformation(GelatoTransform* transform, float* dest);
    void gelato_make_camera_transformation(GelatoTransform* transform, float* dest);

Renderer
=============================================================================================================
## Renderer Functions
    GelatoRenderer gelato_create_renderer(GelatoRendererDescription renderer_description);
    void gelato_initialize_renderer(GelatoRenderer* renderer);
    void gelato_deinitialize_renderer(GelatoRenderer* renderer);
    void gelato_renderer_resize(GelatoRenderer* renderer, uint32 window_width, uint32 window_height);
    void gelato_render(GelatoRenderer* renderer, GelatoTransform* camera_transform, GelatoSprite* sorted_sprites, uint64 sprites_count);

## How to create a renderer
1. Create a GelatoRendererDescription
1. GelatoRendererDescription consists of
```c
    typedef struct GelatoRendererDescription
    {
        uint32 _render_width;
        uint32 _render_height;
        uint32 _window_width;
        uint32 _window_height;
        float _clear_color_letter_box[3];
        float _clear_color_render_target[3];
    } GelatoRendererDescription;
```
    2. window size is the actual size of the window
    2. render width is the desired size. for example 1920x1080. This is then used as the desired resolution.
    2. clear color letter box is the color for the black bars around the gameplay screen
    2. clear color render target is the clear color for the gameplay screen
1. call gelato_create_renderer() with the description. this will return a renderer handle
1. call gelato_initialize_renderer() with the returned renderer handle
1. in your update loop call gelato_render() when you are ready to render
1. call gelato_deinitialize_renderer when your app shuts down to clear memory

Sprite
=============================================================================================================
## Sprite Functions
    GelatoSprite gelato_create_sprite(GelatoTextureId texture);

```c
typedef struct GelatoSprite
{
    GelatoTextureId _texture;
    GelatoTransform _transform;
    float _uv_scale[2];
    float _uv_offset[2];
    float _color[4];
} GelatoSprite;
```

## How to create a sprite
1. [Create a texture](/texture/)
1. Pass GelatoTextureId to gelato_create_sprite(). This will return a functioning sprite object
    2. Adjust transform to your liking
    2. Rember that the scale is in pixels and always relative to the given desired render target size specified in the GelatoRendererDescription.

Texture
=============================================================================================================
## Texture Functions
    GelatoTextureId gelato_create_texture(GelatoTextureDescription texture_description, void* data);
    void gelato_destroy_texture(GelatoTextureId texture);

## GelatoTextureDescription
```c
typedef struct GelatoTextureDescription
{
    uint32 _width;
    uint32 _height;
    GelatoTextureFormat _format;
    GelatoTextureInternalFormat _internal_format;
    GelatoTextureType _type;
    GelatoTextureWrap _wrap_s;
    GelatoTextureWrap _wrap_t;
    GelatoTextureMinFilter _min_filter;
    GelatoTextureMagFilter _mag_filter;
} GelatoTextureDescription;
```

## Create a texture
1. Since Gelato does not come with texturing loading out of the box, you have to load the textures using your own methods.
1. After loading the texture, you need to provide a GelatoTextureDescription and a void* to the loaded texture data
	2. There are static structs with predefined settings for all fields inside the description
	2. GelatoTextureMinFilters, GelatoTextureMagFilters, GelatoTextureWraps, GelatoTextureTypes, GelatoTextureInternalFormats, GelatoTextureFormats

```cpp
uint32 cat_texture_width = 0;
uint32 cat_texture_height = 0;
GelatoTextureId cat_texture = load_texture("content/cat.png", cat_texture_width, cat_texture_height);

GelatoSprite the_main_sprite = gelato_create_sprite(cat_texture);
the_main_sprite._transform._scale[0] = cat_texture_width;
the_main_sprite._transform._scale[1] = cat_texture_height;
the_main_sprite._transform._position[0] = renderer._virtual_target_width / 2.0f;
the_main_sprite._transform._position[1] = renderer._virtual_target_height / 2.0f;
```

Transform
=============================================================================================================
## Transform
```c
    typedef struct GelatoTransform
    {
        float _position[3];
        float _scale[2];
        float _angle_degrees;
    } GelatoTransform;
```

Transform is in pixel scale. Positions and scale are in pixels and are always relative to the
given render target size specified in the GelatoRendererDescription at creation. So a sprite might look more zoomed in
at 1270 x 720 than it does in 1920 x 1080 render target size.
The angles in degrees specify the rotation around the z-axis.
