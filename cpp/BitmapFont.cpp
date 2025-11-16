#include "BitmapFont.hpp"
#include <FL/fl_draw.H>
// https://github.com/spacerace/romfont/tree/master/font-headers
#include "IBM_VGA_8x16.h"

void BitmapFont::drawText(const std::string& text, int x, int y, int scale)
{
	fl_color(60, 60, 60);

	int cur_x = x;
	int cur_y = y;

	for (size_t i = 0; i < text.length(); i++)
	{
		unsigned char ch = (unsigned char)text[i];
		const uint8_t* char_data = &IBM_VGA_8x16[ch * 16];

		for (int row = 0; row < CHAR_HEIGHT; row++)
		{
			uint8_t byte = char_data[row];
			int py = cur_y + row * scale;
			int col = 0;

			while (col < CHAR_WIDTH)
			{
				if (byte & (0x80 >> col))
				{
					int start_col = col;
					while (col < CHAR_WIDTH && (byte & (0x80 >> col)))
						col++;
					int px_start = cur_x + start_col * scale;
					int px_end = cur_x + col * scale;
					if (scale > 1)
						fl_rectf(px_start, py, px_end - px_start, scale);
					else
						fl_line(px_start, py, px_end - 1, py);
				}
				else
					col++;
			}
		}
		cur_x += (CHAR_WIDTH + 1) * scale;
	}
}
