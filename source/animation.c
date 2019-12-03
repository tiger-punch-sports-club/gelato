#include "animation.h"
#include <stdlib.h>

GelatoFlipbookAnimation gelato_create_flipbook_animation(GelatoFlipbookAnimationDescription* description)
{
    // todo: determine tile size, to do uv-stepping
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