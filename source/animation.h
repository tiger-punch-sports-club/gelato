#pragma once
#include "platform.h"

typedef struct GelatoFlipbookAnimation
{
    uint32 _frame_count;
    uint32 _current_frame;
    float* _frame_uvs;
} GelatoFlipbookAnimation;

typedef struct GelatoFlipbookSpriteSheetDescription
{
    uint32 x;
} GelatoFlipbookSpriteSheetDescription;

typedef struct GelatoFlipbookAnimationDescription
{
    uint32 sprite_sheet_width;
    uint32 sprite_sheet_height;
    uint32 tile_count_x;
    uint32 tile_count_y;
} GelatoFlipbookAnimationDescription;

GelatoFlipbookAnimation gelato_create_flipbook_animation(GelatoFlipbookAnimationDescription* description);
void gelato_destroy_flipbook_animation(GelatoFlipbookAnimation* flipbook_animation);
void gelato_progress_flipbook_animation(GelatoFlipbookAnimation* flipbook_animation);