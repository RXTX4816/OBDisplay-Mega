#include "Display.h"

#include <Arduino.h>
#include <stdio.h>
#include <string.h>

// Row/col pixel lookup tables (16 px pitch, 1-px inset)
const int16_t rows[20] = {1,   17,  33,  49,  65,  81,  97,  113, 129, 145,
                          161, 177, 193, 209, 225, 241, 257, 273, 289, 305};
const int16_t cols[30] = {1,   17,  33,  49,  65,  81,  97,  113, 129, 145,
                          161, 177, 193, 209, 225, 241, 257, 273, 289, 305,
                          321, 337, 353, 369, 385, 401, 417, 433, 449, 465};

Display display;

void Display::begin()
{
    uint16_t id = tft_.readID();
    // ILI9486 may report 0x9486 or aliases; fall back if autodetect gives 0
    if (id == 0 || id == 0xFFFF)
        id = 0x9486;
    tft_.begin(id);
    tft_.setRotation(1); // landscape
    tft_.fillScreen(bg_);
    tft_.setTextColor(fg_, bg_);
    tft_.setTextSize(2); // FONT_BIG default
}

void Display::setColor(uint16_t fg)
{
    fg_ = fg;
    tft_.setTextColor(fg_, bg_);
}

void Display::setBackColor(uint16_t bg)
{
    bg_ = bg;
    tft_.setTextColor(fg_, bg_);
}

void Display::setFont(uint8_t font)
{
    font_ = font;
    applyFont();
}

void Display::applyFont()
{
    tft_.setFont(nullptr); // use built-in bitmap font
    switch (font_)
    {
        case FONT_SMALL:
            tft_.setTextSize(1);
            break;
        case FONT_SEVENSEG:
            tft_.setTextSize(4);
            break;
        case FONT_BIG:
        default:
            tft_.setTextSize(2);
            break;
    }
}

uint8_t Display::charWidth() const
{
    switch (font_)
    {
        case FONT_SMALL:
            return 6;
        case FONT_SEVENSEG:
            return 24;
        case FONT_BIG:
        default:
            return 12;
    }
}

uint8_t Display::charHeight() const
{
    switch (font_)
    {
        case FONT_SMALL:
            return 8;
        case FONT_SEVENSEG:
            return 32;
        case FONT_BIG:
        default:
            return 16;
    }
}

int16_t Display::alignX(int16_t x, uint8_t strLen) const
{
    if (x == DISP_CENTER)
        return (DISP_WIDTH - strLen * charWidth()) / 2;
    if (x == DISP_RIGHT)
        return DISP_WIDTH - strLen * charWidth() - 1;
    return x; // DISP_LEFT or explicit pixel
}

void Display::print(const char* str, int16_t x, int16_t y)
{
    applyFont();
    tft_.setTextColor(fg_, bg_);
    tft_.setCursor(alignX(x, strlen(str)), y);
    tft_.print(str);
}

void Display::print(const __FlashStringHelper* str, int16_t x, int16_t y)
{
    // Measure length by iterating PROGMEM
    const char* p = reinterpret_cast<const char*>(str);
    uint8_t len = 0;
    while (pgm_read_byte(p + len))
        len++;
    applyFont();
    tft_.setTextColor(fg_, bg_);
    tft_.setCursor(alignX(x, len), y);
    tft_.print(str);
}

void Display::printNumI(int32_t n, int16_t x, int16_t y, uint8_t len, char fill)
{
    char buf[14];
    ltoa(n, buf, 10);

    // Pad to `len` with fill character (left-pad)
    if (len > 0)
    {
        uint8_t numLen = strlen(buf);
        if (numLen < len)
        {
            char padded[14] = {};
            uint8_t padding = len - numLen;
            memset(padded, fill, padding);
            strcpy(padded + padding, buf);
            print(padded, x, y);
            return;
        }
    }
    print(buf, x, y);
}

void Display::printNumF(float n, uint8_t dec, int16_t x, int16_t y, char /*dp*/, uint8_t len,
                        char fill)
{
    // dtostrf: width is total field width (including sign and decimal point)
    char buf[16];
    dtostrf(n, (int8_t)(len > 0 ? len : (dec + 4)), dec, buf);

    // Replace default space padding with fill char
    if (fill != ' ')
    {
        for (uint8_t i = 0; buf[i] == ' '; i++)
            buf[i] = fill;
    }
    print(buf, x, y);
}

void Display::fillScreen(uint16_t color)
{
    tft_.fillScreen(color);
}

void Display::drawHLine(int16_t x, int16_t y, int16_t len)
{
    tft_.drawFastHLine(x, y, len, fg_);
}

void Display::drawVLine(int16_t x, int16_t y, int16_t len)
{
    tft_.drawFastVLine(x, y, len, fg_);
}

void Display::drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
    tft_.drawLine(x1, y1, x2, y2, fg_);
}

void Display::drawRect(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
    tft_.drawRect(x1, y1, x2 - x1, y2 - y1, fg_);
}

void Display::fillRect(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
    tft_.fillRect(x1, y1, x2 - x1, y2 - y1, fg_);
}

void Display::drawPixel(int16_t x, int16_t y)
{
    tft_.drawPixel(x, y, fg_);
}

void Display::clearRow(uint8_t row)
{
    uint16_t saved = fg_;
    fg_ = bg_;
    // 30 spaces clears the full row at BigFont width (30 * 12 = 360 px < 480)
    print("                              ", DISP_LEFT, rows[row]);
    fg_ = saved;
    tft_.setTextColor(fg_, bg_);
}
