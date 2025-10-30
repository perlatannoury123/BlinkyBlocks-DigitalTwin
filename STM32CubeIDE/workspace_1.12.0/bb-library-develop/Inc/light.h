#ifndef __LIGHT_H__
#define __LIGHT_H__

#include <stdint.h>

//------------------------------------------------------------------------------
// define
//------------------------------------------------------------------------------
typedef enum __packed {
	RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, PURPLE, GREY, WHITE, DARK_RED, DARK_ORANGE, BROWN, DARK_GREEN, DARK_CYAN, DARK_BLUE, DARK_PURPLE, DARK_GREY, BLACK, NB_COLORS
} ColorName;
typedef struct _Color {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} Color;

void setColor(uint8_t color_index);

#endif // __LIGHT_H__
