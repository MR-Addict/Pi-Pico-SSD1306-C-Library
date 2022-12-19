#include <cstdio>
#include <cstring>

#include "OLED.h"
#include "font/Dialog_bold_16.h"

void OLED::write_cmd(uint8_t cmd) {
    // 0x00 for write command
    uint8_t buff[] = {0x00, cmd};
    i2c_write_blocking(I2C_PORT, OLED_ADDRESS, buff, 2, false);
}

void OLED::write_data(uint8_t data) {
    // 0x40 for write data
    uint8_t buff[] = {0x40, data};
    i2c_write_blocking(I2C_PORT, OLED_ADDRESS, buff, 2, false);
}

void OLED::swap(uint8_t* x1, uint8_t* x2) {
    uint8_t temp = *x1;
    *x1 = *x2, *x2 = temp;
}

bool OLED::bitRead(uint8_t character, uint8_t index) {
    return bool((character >> index) & 0x01);
}

void OLED::init() {
    // Display init
    write_cmd(SET_DISP | 0x00);
    // Set horizontal address mode
    write_cmd(SET_MEM_ADDR);
    write_cmd(0x00);
    // Start line from 0
    write_cmd(SET_DISP_START_LINE);
    write_cmd(0x00);
    // Set seg-map
    if (reversed) {
        write_cmd(SET_SEG_REMAP);
    } else {
        write_cmd(SET_SEG_REMAP | 0x01);
    }
    // Set oled height
    write_cmd(SET_MUX_RATIO);
    write_cmd(HEIGHT - 1);
    // Set COM output scan directionscan from bottom up, COM[0] to COM[N-1]
    if (reversed) {
        write_cmd(SET_COM_OUT_DIR_REVERSE);
    } else {
        write_cmd(SET_COM_OUT_DIR);
    }
    
    // Set display offset
    write_cmd(SET_DISP_OFFSET);
    write_cmd(0x00);
    // Set COM pins hardware configuration,0x12 for 12864,and 0x02 for 12832
    write_cmd(SET_COM_PIN_CFG);
    if (HEIGHT == 64)
        write_cmd(0x12);
    else if (HEIGHT == 32)
        write_cmd(0x02);
    // Set display clock divide ratio
    write_cmd(SET_DISP_CLK_DIV);
    write_cmd(0x80);
    // Set per-charge period
    write_cmd(SET_PRECHARGE);
    write_cmd(0xF1);
    // Set VCOMH deselect level
    write_cmd(SET_VCOM_DESEL);
    write_cmd(0x30);
    // Contrast set 255
    write_cmd(SET_CONTRAST);
    write_cmd(0xFF);
    // Set oled on following from RAM
    write_cmd(SET_ENTIRE_ON);
    // NO inverse , which '0' for pixel off, '1' for pixel on
    write_cmd(SET_NORM_INV);
    // Set charge pump
    write_cmd(SET_CHARGE_PUMP);
    write_cmd(0x14);
    // Set scroll disable
    write_cmd(SET_SCROLL | 0x00);
    // Turn oled on
    write_cmd(SET_DISP | 0x01);
}

OLED::OLED(uint8_t scl,
           uint8_t sda,
           uint8_t width,
           uint8_t height,
           uint32_t freq,
           bool rev,
           i2c_inst_t* i2c) {
    // OLED object init

    WIDTH = width, HEIGHT = height;
    PAGES = height / 8, BUFFERSIZE = width * PAGES;
    OLED_SDA_PIN = sda, OLED_SCL_PIN = scl;
    FREQUENCY = freq, I2C_PORT = i2c;
    myFont = &Dialog_bold_16;
    reversed = rev;

    clear();
    // i2c init
    i2c_init(I2C_PORT, FREQUENCY);
    gpio_set_function(OLED_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(OLED_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(OLED_SDA_PIN);
    gpio_pull_up(OLED_SCL_PIN);
    // Display init
    init();
}

OLED::~OLED() {}

void OLED::isDisplay(bool display) {
    write_cmd(SET_DISP | display);
}

void OLED::setContrast(uint8_t contrast) {
    write_cmd(SET_CONTRAST);
    write_cmd(contrast);
}

void OLED::isInverse(bool inverse) {
    write_cmd(SET_NORM_INV | inverse);
}

void OLED::clear() {
    for (uint16_t i = 0; i < BUFFERSIZE; i++) {
        BUFFER[i] = 0x00;
    }
}

void OLED::show() {
    // Set col, row, and page address for sending data buffer
    write_cmd(SET_COL_ADDR);
    write_cmd(0);
    write_cmd(WIDTH - 1);
    write_cmd(SET_PAGE_ADDR);
    write_cmd(0);
    write_cmd(PAGES - 1);

    for (uint16_t i = 0; i < BUFFERSIZE; i++) {
        write_data(BUFFER[i]);
    }
}

void OLED::drawPixel(uint8_t x, uint8_t y) {
    if (x < WIDTH && y < HEIGHT)
        BUFFER[x + WIDTH * (y / 8)] |= 0x01 << (y % 8);
}

void OLED::drawFastHLine(uint8_t x, uint8_t y, uint8_t width) {
    for (uint8_t i = 0; i < width; i++) {
        drawPixel(x + i, y);
    }
}

void OLED::drawFastVLine(uint8_t x, uint8_t y, uint8_t height) {
    for (uint8_t i = 0; i < height; i++) {
        drawPixel(x, y + i);
    }
}

void OLED::drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
    if (x1 > x2) {
        swap(&x1, &x2);
        swap(&y1, &y2);
    }
    float m = (float)(y2 - y1) / (float)(x2 - x1);
    for (uint8_t x = x1; x <= x2; x++) {
        float y = m * (float)(x - x1) + (float)y1;
        drawPixel(x, y);
    }
}

void OLED::drawCircle(int16_t xc, int16_t yc, uint16_t r) {
    int16_t x = -r;
    int16_t y = 0;
    int16_t e = 2 - (2 * r);
    do {
        drawPixel(xc + x, yc - y);
        drawPixel(xc - x, yc + y);
        drawPixel(xc + y, yc + x);
        drawPixel(xc - y, yc - x);
        int16_t _e = e;
        if (_e <= y)
            e += (++y * 2) + 1;
        if ((_e > x) || (e > y))
            e += (++x * 2) + 1;
    } while (x < 0);
}

void OLED::drawFilledCircle(int16_t xc, int16_t yc, uint16_t r) {
    int16_t x = r;
    int16_t y = 0;
    int16_t e = 1 - x;
    while (x >= y) {
        drawLine(xc + x, yc + y, xc - x, yc + y);
        drawLine(xc + y, yc + x, xc - y, yc + x);
        drawLine(xc - x, yc - y, xc + x, yc - y);
        drawLine(xc - y, yc - x, xc + y, yc - x);
        ++y;
        if (e >= 0) {
            x--;
            e += 2 * ((y - x) + 1);
        } else
            e += (2 * y) + 1;
    }
}

void OLED::drawRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
    drawFastHLine(x, y, width);
    drawFastHLine(x, y + height - 1, width);
    drawFastVLine(x, y, height);
    drawFastVLine(x + width - 1, y, height);
}

void OLED::drawFilledRectangle(uint8_t x,
                               uint8_t y,
                               uint8_t width,
                               uint8_t height) {
    for (uint8_t i = 0; i < height; i++) {
        drawFastHLine(x, y+i, width);
    }
}

void OLED::setScrollDir(bool direction) {
    write_cmd(SET_HOR_SCROLL | direction);
    write_cmd(0x00);       // Dummy byte
    write_cmd(0);          // Start page
    write_cmd(0x06);       // Time inteval
    write_cmd(PAGES - 1);  // End page
    write_cmd(0x00);       // Dummy byte
    write_cmd(0xFF);       // Dummy byte
}

void OLED::isScroll(bool isEnable) {
    write_cmd(SET_SCROLL | isEnable);
}

void OLED::setFont(const GFXfont* font) {
    myFont = font;
}

void OLED::printChar(uint8_t x, uint8_t y, uint8_t character) {
    if (character < myFont->first || character > myFont->last)
        return;
    character -= myFont->first;
    GFXglyph* glyph = myFont->glyph + character;
    uint8_t* bitmap = myFont->bitmap;

    uint16_t bitmapOffset = glyph->bitmapOffset;
    uint8_t width = glyph->width, height = glyph->height;
    int8_t xOffset = glyph->xOffset;
    uint8_t yOffset = myFont->yAdvance + glyph->yOffset;
    uint8_t bits = 0, abit = 0;

    for (uint8_t i = 0; i < height; i++) {
        for (uint8_t j = 0; j < width; j++) {
            if (!(abit++ & 7)) {
                bits = bitmap[bitmapOffset++];
            }
            if (bits & 0x80) {
                drawPixel(x + xOffset + j, y + yOffset + i);
            }
            bits <<= 1;
        }
    }
}

void OLED::print(uint8_t x, uint8_t y, uint8_t* string) {
    for (uint8_t i = 0; string[i]; i++) {
        uint8_t character = string[i];
        GFXglyph* glyph = myFont->glyph + character - myFont->first;
        if (x + glyph->width + glyph->xOffset > WIDTH) {
            x = 0;
            y += myFont->yAdvance;
        }
        printChar(x, y, character);
        x += glyph->xAdvance;
    }
}

void OLED::drawBitmap(uint8_t x,
                      uint8_t y,
                      uint8_t width,
                      uint8_t height,
                      const uint8_t* image) {
    for (uint8_t i = 0; i < height; i++)
        for (uint8_t j = 0; j < width; j++) {
            bool value =
                bitRead(image[i * ((width - 1) / 8 + 1) + j / 8], 7 - j % 8);
            if (value) {
                drawPixel(x + j, y + i);
            }
        }
}