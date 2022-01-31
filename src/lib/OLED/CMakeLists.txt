add_library(OLED
    OLED.h
    OLED.cpp
)

target_link_libraries(OLED
    pico_stdlib
    hardware_i2c
)
