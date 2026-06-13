#pragma once

#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <stdint.h>

// Alignment constants (match UTFT convention)
#define DISP_LEFT 0
#define DISP_CENTER -3
#define DISP_RIGHT -1

// Font size tokens
#define FONT_SMALL 0
#define FONT_BIG 1
#define FONT_SEVENSEG 2

// 480x320 landscape, 16-pixel row pitch, 16-pixel col pitch
static const uint16_t DISP_WIDTH = 480;
static const uint16_t DISP_HEIGHT = 320;
static const uint16_t DISP_ROWS = 20;
static const uint16_t DISP_COLS = 30;

extern const int16_t rows[20];
extern const int16_t cols[30];

// Named colours (RGB565)
namespace Color
{
constexpr uint16_t Black = 0x0000;
constexpr uint16_t White = 0xFFFF;
constexpr uint16_t Cyan = 0x07FF;
constexpr uint16_t Red = 0xF800;
constexpr uint16_t Yellow = 0xFFE0;
constexpr uint16_t Green = 0x07E0;
constexpr uint16_t Blue = 0x001F;
constexpr uint16_t Orange = 0xFD20;
constexpr uint16_t DarkGrey = 0x39C7;
} // namespace Color

class Display
{
  public:
    void begin();

    // --- colour / font state ---
    void setColor(uint16_t fg);
    void setBackColor(uint16_t bg);
    void setFont(uint8_t font); // FONT_SMALL | FONT_BIG | FONT_SEVENSEG

    // Current colour accessors (used by screens to remember/restore)
    uint16_t getColor() const { return fg_; }
    uint16_t getBackColor() const { return bg_; }

    // --- text primitives ---
    // x: pixel position, or DISP_LEFT / DISP_CENTER / DISP_RIGHT
    void print(const char* str, int16_t x, int16_t y);
    void print(const __FlashStringHelper* str, int16_t x, int16_t y);

    // Print integer, right-padded to `len` chars with `fill` character
    void printNumI(int32_t n, int16_t x, int16_t y, uint8_t len = 0, char fill = ' ');

    // Print float with `dec` decimal places
    void printNumF(float n, uint8_t dec, int16_t x, int16_t y, char dp = '.', uint8_t len = 0,
                   char fill = ' ');

    // --- drawing primitives ---
    void fillScreen(uint16_t color);
    void drawHLine(int16_t x, int16_t y, int16_t len);
    void drawVLine(int16_t x, int16_t y, int16_t len);
    void drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
    void drawRect(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
    void fillRect(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
    void drawPixel(int16_t x, int16_t y);

    // --- convenience ---
    void clearRow(uint8_t row); // fill row with background colour
    void clearScreen() { fillScreen(bg_); }

    // Character dimensions for current font
    uint8_t charWidth() const;
    uint8_t charHeight() const;

    // Expose raw driver for advanced use (screen modules)
    MCUFRIEND_kbv& raw() { return tft_; }

  private:
    MCUFRIEND_kbv tft_;
    uint16_t fg_ = Color::Cyan;
    uint16_t bg_ = Color::Black;
    uint8_t font_ = FONT_BIG;

    // Compute pixel x from alignment constant + string length
    int16_t alignX(int16_t x, uint8_t strLen) const;
    void applyFont();
};

// Global singleton — accessed by all screen modules
extern Display display;
