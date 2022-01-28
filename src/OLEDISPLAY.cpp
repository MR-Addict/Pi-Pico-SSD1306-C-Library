#include <stdio.h>

#include "bitmap.h"
#include "lib/OLED/OLED.h"
#include "lib/OLED/font/Cherry_Cream_Soda_Regular_16.h"
#include "pico/stdlib.h"

int main() {
    OLED oled;
    oled.drawBitmap(0, 0, 40, 32, pressure_40x32);
    oled.drawBitmap(45, 3, 32, 32, temperature_32x32);
    oled.drawCircle(100, 16, 14);
    oled.drawFilledCircle(100, 16, 10);
    uint8_t string1[] = "Hello";
    oled.print(0, 38, string1);
    oled.setFont(&Cherry_Cream_Soda_Regular_16);
    uint8_t string2[] = "world!";
    oled.print(60, 32, string2);
    oled.drawLine(0, 60, 128, 60);
    oled.show();
    oled.setScrollDir(true);
    oled.isScroll(true);
    while (true) {
        tight_loop_contents();
    }
    return 0;
}