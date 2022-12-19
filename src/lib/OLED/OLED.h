#ifndef _OLED_H_
#define _OLED_H_

#include "hardware/i2c.h"
#include "pico/stdlib.h"

#define OLED_ADDRESS 0x3C

#define SET_CONTRAST 0x81
#define SET_ENTIRE_ON 0xA4
#define SET_NORM_INV 0xA6
#define SET_DISP 0xAE
#define SET_MEM_ADDR 0x20
#define SET_COL_ADDR 0x21
#define SET_PAGE_ADDR 0x22
#define SET_DISP_START_LINE 0x40
#define SET_SEG_REMAP 0xA0
#define SET_MUX_RATIO 0xA8
#define SET_COM_OUT_DIR 0xC8
#define SET_DISP_OFFSET 0xD3
#define SET_COM_PIN_CFG 0xDA
#define SET_DISP_CLK_DIV 0xD5
#define SET_PRECHARGE 0xD9
#define SET_VCOM_DESEL 0xDB
#define SET_CHARGE_PUMP 0x8D
#define SET_SCROLL 0x2E
#define SET_HOR_SCROLL 0x26
#define SET_COM_OUT_DIR_REVERSE 0xC0

struct GFXglyph {
    uint16_t bitmapOffset;  ///< Pointer into GFXfont->bitmap
    uint8_t width;          ///< Bitmap dimensions in pixels
    uint8_t height;         ///< Bitmap dimensions in pixels
    uint8_t xAdvance;       ///< Distance to advance cursor (x axis)
    int8_t xOffset;         ///< X dist from cursor pos to UL corner
    int8_t yOffset;         ///< Y dist from cursor pos to UL corner
};

struct GFXfont {
    uint8_t* bitmap;   ///< Glyph bitmaps, concatenated
    GFXglyph* glyph;   ///< Glyph array
    uint8_t first;     ///< ASCII extents (first char)
    uint8_t last;      ///< ASCII extents (last char)
    uint8_t yAdvance;  ///< Newline distance (y axis)
};

class OLED {
   private:
    uint32_t FREQUENCY;
    i2c_inst_t* I2C_PORT;

    // Pin Definition
    uint8_t OLED_SDA_PIN;
    uint8_t OLED_SCL_PIN;

    uint8_t WIDTH;
    uint8_t HEIGHT;
    uint8_t PAGES;
    uint16_t BUFFERSIZE;
    bool reversed;
    uint8_t BUFFER[1024];
    const GFXfont* myFont;

    void init();
    void write_cmd(uint8_t cmd);
    void write_data(uint8_t data);
    void swap(uint8_t* x1, uint8_t* x2);
    bool bitRead(uint8_t character, uint8_t index);
    void drawPixel(uint8_t x, uint8_t y);

   public:
    OLED(uint8_t scl,
         uint8_t sda,
         uint8_t width,
         uint8_t height,
         uint32_t freq,
         bool reversed,
         i2c_inst_t* i2c);
    ~OLED();
    void show();
    void clear();
    void isDisplay(bool inverse);
    void isInverse(bool inverse);
    void setContrast(uint8_t contrast);

    void drawFastHLine(uint8_t x, uint8_t y, uint8_t width);
    void drawFastVLine(uint8_t x, uint8_t y, uint8_t height);
    void drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
    void drawRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
    void drawFilledRectangle(uint8_t x,
                             uint8_t y,
                             uint8_t width,
                             uint8_t height);
    void drawCircle(int16_t xc, int16_t yc, uint16_t r);
    void drawFilledCircle(int16_t xc, int16_t yc, uint16_t r);

    void setScrollDir(bool direction);
    void isScroll(bool isEnable);
    void setFont(const GFXfont* font);
    void printChar(uint8_t x, uint8_t y, uint8_t character);
    void print(uint8_t x, uint8_t y, uint8_t* string);
    void drawBitmap(uint8_t x,
                    uint8_t y,
                    uint8_t width,
                    uint8_t height,
                    const uint8_t* image);
};

#endif