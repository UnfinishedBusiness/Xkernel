/** \file
 * Super simple font from Asteroids.
 *
 * http://www.edge-online.com/wp-content/uploads/edgeonline/oldfiles/images/feature_article/2009/05/asteroids2.jpg
 */
#include <stdio.h>
#include <stdint.h>
#include <render/asteroids_font.h>
#include <vector>
#include <string>

extern "C"{

	#define P(x,y)	((((x) & 0xF) << 4) | (((y) & 0xF) << 0))
	
	const asteroids_char_t asteroids_font[] = {
		{'0', P(0,0), P(8,0), P(8,12), P(0,12), P(0,0), P(8,12), FONT_LAST },
		{'1', P(4,0), P(4,12), P(3,10), FONT_LAST },
		{'2', P(0,12), P(8,12), P(8,7), P(0,5), P(0,0), P(8,0), FONT_LAST },
		{'3', P(0,12), P(8,12), P(8,0), P(0,0), FONT_UP, P(0,6), P(8,6), FONT_LAST },
		{'4', P(0,12), P(0,6), P(8,6), FONT_UP, P(8,12), P(8,0), FONT_LAST },
		{'5', P(0,0), P(8,0), P(8,6), P(0,7), P(0,12), P(8,12), FONT_LAST },
		{'6', P(0,12), P(0,0), P(8,0), P(8,5), P(0,7), FONT_LAST },
		{'7', P(0,12), P(8,12), P(8,6), P(4,0), FONT_LAST },
		{'8', P(0,0), P(8,0), P(8,12), P(0,12), P(0,0), FONT_UP, P(0,6), P(8,6), },
		{'9', P(8,0), P(8,12), P(0,12), P(0,7), P(8,5), FONT_LAST },
		{' ', FONT_LAST },
		{'.', P(3,0), P(4,0), FONT_LAST },
		{',', P(2,0), P(4,2), FONT_LAST },
		{'-', P(2,6), P(6,6), FONT_LAST },
		{'+', P(1,6), P(7,6), FONT_UP, P(4,9), P(4,3), FONT_LAST },
		{'!', P(4,0), P(3,2), P(5,2), P(4,0), FONT_UP, P(4,4), P(4,12), FONT_LAST },
		{'#', P(0,4), P(8,4), P(6,2), P(6,10), P(8,8), P(0,8), P(2,10), P(2,2) },
		{'^', P(2,6), P(4,12), P(6,6), FONT_LAST },
		{'=', P(1,4), P(7,4), FONT_UP, P(1,8), P(7,8), FONT_LAST },
		{'*', P(0,0), P(4,12), P(8,0), P(0,8), P(8,8), P(0,0), FONT_LAST },
		{'_', P(0,0), P(8,0), FONT_LAST },
		{'/', P(0,0), P(8,12), FONT_LAST },
		{'\\', P(0,12), P(8,0), FONT_LAST },
		{'@', P(8,4), P(4,0), P(0,4), P(0,8), P(4,12), P(8,8), P(4,4), P(3,6) },
		{'$', P(6,2), P(2,6), P(6,10), FONT_UP, P(4,12), P(4,0), FONT_LAST },
		{'&', P(8,0), P(4,12), P(8,8), P(0,4), P(4,0), P(8,4), FONT_LAST },
		{'[', P(6,0), P(2,0), P(2,12), P(6,12), FONT_LAST },
		{']', P(2,0), P(6,0), P(6,12), P(2,12), FONT_LAST },
		{'(', P(6,0), P(2,4), P(2,8), P(6,12), FONT_LAST },
		{')', P(2,0), P(6,4), P(6,8), P(2,12), FONT_LAST },
		{'{', P(6,0), P(4,2), P(4,10), P(6,12), FONT_UP, P(2,6), P(4,6), FONT_LAST },
		{'}', P(4,0), P(6,2), P(6,10), P(4,12), FONT_UP, P(6,6), P(8,6), FONT_LAST },
		{'%', P(0,0), P(8,12), FONT_UP, P(2,10), P(2,8), FONT_UP, P(6,4), P(6,2) },
		{'<', P(6,0), P(2,6), P(6,12), FONT_LAST },
		{'>', P(2,0), P(6,6), P(2,12), FONT_LAST },
		{'|', P(4,0), P(4,5), FONT_UP, P(4,6), P(4,12), FONT_LAST },
		{':', P(4,9), P(4,7), FONT_UP, P(4,5), P(4,3), FONT_LAST },
		{';', P(4,9), P(4,7), FONT_UP, P(4,5), P(1,2), FONT_LAST },
		{'"', P(2,10), P(2,6), FONT_UP, P(6,10), P(6,6), FONT_LAST },
		{'\\', P(2,6), P(6,10), FONT_LAST },
		{'`', P(2,10), P(6,6), FONT_LAST },
		{'~', P(0,4), P(2,8), P(6,4), P(8,8), FONT_LAST },
		{'?', P(0,8), P(4,12), P(8,8), P(4,4), FONT_UP, P(4,1), P(4,0), FONT_LAST },
		{'A', P(0,0), P(0,8), P(4,12), P(8,8), P(8,0), FONT_UP, P(0,4), P(8,4) },
		{'B', P(0,0), P(0,12), P(4,12), P(8,10), P(4,6), P(8,2), P(4,0), P(0,0) },
		{'C', P(8,0), P(0,0), P(0,12), P(8,12), FONT_LAST },
		{'D', P(0,0), P(0,12), P(4,12), P(8,8), P(8,4), P(4,0), P(0,0), FONT_LAST },
		{'E', P(8,0), P(0,0), P(0,12), P(8,12), FONT_UP, P(0,6), P(6,6), FONT_LAST },
		{'F', P(0,0), P(0,12), P(8,12), FONT_UP, P(0,6), P(6,6), FONT_LAST },
		{'G', P(6,6), P(8,4), P(8,0), P(0,0), P(0,12), P(8,12), FONT_LAST },
		{'H', P(0,0), P(0,12), FONT_UP, P(0,6), P(8,6), FONT_UP, P(8,12), P(8,0) },
		{'I', P(0,0), P(8,0), FONT_UP, P(4,0), P(4,12), FONT_UP, P(0,12), P(8,12) },
		{'J', P(0,4), P(4,0), P(8,0), P(8,12), FONT_LAST },
		{'K', P(0,0), P(0,12), FONT_UP, P(8,12), P(0,6), P(6,0), FONT_LAST },
		{'L', P(8,0), P(0,0), P(0,12), FONT_LAST },
		{'M', P(0,0), P(0,12), P(4,8), P(8,12), P(8,0), FONT_LAST },
		{'N', P(0,0), P(0,12), P(8,0), P(8,12), FONT_LAST },
		{'O', P(0,0), P(0,12), P(8,12), P(8,0), P(0,0), FONT_LAST },
		{'P', P(0,0), P(0,12), P(8,12), P(8,6), P(0,5), FONT_LAST },
		{'Q', P(0,0), P(0,12), P(8,12), P(8,4), P(0,0), FONT_UP, P(4,4), P(8,0) },
		{'R', P(0,0), P(0,12), P(8,12), P(8,6), P(0,5), FONT_UP, P(4,5), P(8,0) },
		{'S', P(0,2), P(2,0), P(8,0), P(8,5), P(0,7), P(0,12), P(6,12), P(8,10) },
		{'T', P(0,12), P(8,12), FONT_UP, P(4,12), P(4,0), FONT_LAST },
		{'U', P(0,12), P(0,2), P(4,0), P(8,2), P(8,12), FONT_LAST },
		{'V', P(0,12), P(4,0), P(8,12), FONT_LAST },
		{'W', P(0,12), P(2,0), P(4,4), P(6,0), P(8,12), FONT_LAST },
		{'X', P(0,0), P(8,12), FONT_UP, P(0,12), P(8,0), FONT_LAST },
		{'Y', P(0,12), P(4,6), P(8,12), FONT_UP, P(4,6), P(4,0), FONT_LAST },
		{'Z', P(0,12), P(8,12), P(0,0), P(8,0), FONT_UP, P(2,6), P(6,6), FONT_LAST },
		{NULL, FONT_LAST },
	};
};
std::vector<asteroid_line_t> character_to_lines(char c, int x, int y, int size)
{
	// Asteroids font only has upper case
	asteroid_line_t line;
	std::vector<asteroid_line_t> lines;
	if ('a' <= c && c <= 'z')
		c -= 'a' - 'A';

	int charactor_index = 0;
	while(asteroids_font[charactor_index].letter != NULL)
	{
		if (asteroids_font[charactor_index].letter == c) break; //charactor_index holds the array index of the charactor we are looking for!
		charactor_index++;
	}
	//printf("Found %c at index: %d\n", c, charactor_index);

	const uint8_t * const pts = asteroids_font[charactor_index].points;
	int next_moveto = 1;
	//printf("Charactor begin\n");
	for(int i = 0 ; i < 8 ; i++)
	{
		uint8_t delta = pts[i];
		if (delta == FONT_LAST)
			break;
		if (delta == FONT_UP)
		{
			next_moveto = 1;
			continue;
		}

		unsigned dx = ((delta >> 4) & 0xF) * size;
		unsigned dy = ((delta >> 0) & 0xF) * size;

		if (next_moveto)
		{
			//moveto(x + dx, y + dy);
			line.start.x = (x + dx);
			line.start.y = (y + dy);
			//printf("Move to: %d, %d\n", x + dx, y + dy);
		}
		else
		{
			//lineto(x + dx, y + dy);
			//printf("Line to: %d, %d\n", x + dx, y + dy);
			line.end.x = (x + dx);
			line.end.y = (y + dy);
			lines.push_back(line);
			line.start.x = line.end.x;
			line.start.y = line.end.y;
		}
		next_moveto = 0;
	}
	return lines;
}

std::vector<asteroid_line_t> string_to_lines(std::string text, int x, int y, int size)
{
	std::vector<asteroid_line_t> lines;
	for (size_t i = 0; i < text.size(); i++)
	{
		char c = text.c_str()[i];
		std::vector<asteroid_line_t> char_lines = character_to_lines(c, x, y, size);
		x += size * 10;
		for (size_t z = 0; z < char_lines.size(); z++)
		{
			lines.push_back(char_lines[z]);
		}
	}
	
	return lines;
}