#include "platform.h"
#include <stdio.h>
#include <GL/glew.h>

void check_gl_error(const char* file_name, int line_number)
{
    int error = glGetError();
    if (error != 0)
    {
       printf("gl error: %d in %s at %d \n", error, file_name, line_number);
    }
    assert(error == 0);
}