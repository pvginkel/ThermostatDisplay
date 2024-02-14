#pragma once

constexpr uint8_t IO_EXPANDER_TOUCH_PANEL_RESET = 1 << 1;
constexpr uint8_t IO_EXPANDER_LCD_BACKLIGHT = 1 << 2;
constexpr uint8_t IO_EXPANDER_LCD_RESET = 1 << 3;
constexpr uint8_t IO_EXPANDER_SD_CS = 1 << 4;
constexpr uint8_t IO_EXPANDER_USB_SELECT = 1 << 5;

void set_ch422g_pins(uint8_t pins);
