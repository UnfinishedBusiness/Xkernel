/** \file
 * Asteroids simple font.
 */
#ifndef _asteroids_font_h_
#define _asteroids_font_h_

#include <stdint.h>
#include <vector>

typedef struct
{
	uint8_t points[8]; // 4 bits x, 4 bits y
} asteroids_char_t;

struct asteroid_point_t{
	int x;
	int y;
};
struct asteroid_line_t{
	asteroid_point_t start;
	asteroid_point_t end;
};
std::vector<asteroid_line_t> character_to_lines(char c, int x, int y, int size);
#define FONT_UP 0xFE
#define FONT_LAST 0xFF

#ifdef __cplusplus
extern "C" {
#endif

extern const asteroids_char_t asteroids_font[];

#ifdef __cplusplus
}
#endif

#endif