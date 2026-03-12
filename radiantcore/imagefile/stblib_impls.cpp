// this source file includes the implementations of stb_image and stb_image_write
// having it in a separate source file allows optimizing it in debug builds (for faster load times)
// without hurting the debugability of the source files stb_image(_write) is used in

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

