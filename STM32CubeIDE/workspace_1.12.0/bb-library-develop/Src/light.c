/*
 * light.c
 *
 *  Created on: 3 déc. 2021
 *      Author: flassabe
 */

#include <light.h>
#include <abstraction.h>

Color colors[NB_COLORS] = {
		{60, 0, 0}, {60, 30, 0}, {60, 60, 0}, {0, 60, 0}, {0, 60, 60},
		{0, 0, 60}, {47, 16, 57}, {30, 30, 30}, {60, 60, 60},
		{20, 0, 0}, {20, 10, 0}, {20, 20, 0}, {0, 20, 0}, {0, 20, 20},
		{0, 0, 20}, {16, 5, 19}, {8, 8, 8}, {0, 0, 0}};

void setColor(uint8_t color_index) {
	uint8_t idx = color_index%NB_COLORS;
	Color *color = &colors[idx];
	set_RGB(color->r, color->g, color->b);
}
