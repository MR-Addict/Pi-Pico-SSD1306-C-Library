#include <stdio.h>

#include "bitmap.h"
#include "lib/OLED/OLED.h"
#include "lib/OLED/font/Cherry_Cream_Soda_Regular_16.h"
#include "pico/stdlib.h"

int main() {
    // SCL, SDA, Width, Height, Frequency, I2C Port
    OLED oled(5, 4, 128, 64, 400000, i2c0);
    // Draw two bitmaps
    oled.drawBitmap(0, 0, 40, 32, pressure_40x32);
    oled.drawBitmap(45, 3, 32, 32, temperature_32x32);
    // Draw two circles
    oled.drawCircle(100, 16, 14);
    oled.drawFilledCircle(100, 16, 10);
    // Print defaule font string
    uint8_t string1[] = "Hello";
    oled.print(0, 35, string1);
    // Print custom font string
    oled.setFont(&Cherry_Cream_Soda_Regular_16);
    uint8_t string2[] = "world!";
    oled.print(60, 32, string2);
    // Draw a line
    oled.drawFastHLine(0, 60, 128);
    oled.show();
    // Turn scroll ON
    oled.setScrollDir(true);
    oled.isScroll(true);
    while (true) {
        tight_loop_contents();
    }
    return 0;
}