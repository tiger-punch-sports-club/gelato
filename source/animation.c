#include "animation.h"
#include <stdlib.h>

GelatoFlipbookAnimation gelato_create_flipbook_animation(uint32 sprite_sheet_width, uint32 sprite_sheet_height, uint32 tile_count_x, uint32 tile_count_y)
{
    GelatoFlipbookAnimation animation = { 0 };
    return animation;
}

void gelato_destroy_flipbook_animation(GelatoFlipbookAnimation* flipbook_animation)
{
    free(flipbook_animation->_frame_uvs);
}

void gelato_progress_flipbook_animation(GelatoFlipbookAnimation* flipbook_animation)
{

}