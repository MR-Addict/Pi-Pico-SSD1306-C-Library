add_library(OLED
    OLED.h
    OLED.cpp
)

target_link_libraries(OLED
    pico_stdlib
    pico_binary_info
    hardware_i2c
)
