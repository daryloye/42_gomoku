#ifndef CONFIG_HPP
#define CONFIG_HPP

constexpr int BOARD_SIZE = 19;
constexpr int CELL_SIZE = 40;
constexpr int OFFSET = 20;
constexpr int TEXT_MARGIN = 60;
constexpr int BOTTOM_MARGIN = 50;
constexpr int BUTTON_PANEL_WIDTH = 180;

constexpr int BOARD_WIDTH = BOARD_SIZE * CELL_SIZE;
constexpr int BOARD_HEIGHT = BOARD_SIZE * CELL_SIZE;
constexpr int WIN_WIDTH = BOARD_WIDTH + OFFSET + BUTTON_PANEL_WIDTH;
constexpr int WIN_HEIGHT = BOARD_HEIGHT + TEXT_MARGIN + OFFSET + BOTTOM_MARGIN;

#endif