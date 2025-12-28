#ifndef BITMAPFONT_HPP
#define BITMAPFONT_HPP

#include <string>
#include <FL/fl_draw.H>

#include "IBM_VGA_8x16.h"

class BitmapFont
{
public:
	static constexpr int CHAR_WIDTH = 8;
	static constexpr int CHAR_HEIGHT = 16;

	static void drawText(const std::string& text, int x, int y, int scale = 1);
};

#endif
