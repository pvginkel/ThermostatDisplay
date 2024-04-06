#pragma once

#define _USE_MATH_DEFINES

#ifdef LV_SIMULATOR

#pragma warning(disable : 4200)

#endif

#include <algorithm>
#include <cmath>
#include <functional>
#include <string>
#include <vector>

#include "lvgl.h"

using namespace std;

#ifndef LV_SIMULATOR

#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>

#include <cctype>
#include <cstdarg>

#include "cJSON.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_app_format.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_touch_gt911.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_ota_ops.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "mqtt_client.h"
#include "nvs_flash.h"
#include "sdkconfig.h"

#define LV_COLOR_R(c) (c).ch.red
#define LV_COLOR_G(c) (c).ch.green
#define LV_COLOR_B(c) (c).ch.blue
#define LV_COLOR_A(c) (c).ch.alpha

#define _strdup strdup

#else

typedef void* QueueHandle_t;

#define lv_obj_set_style_bg_img_src lv_obj_set_style_bg_image_src
#define lv_msgbox_get_btns lv_msgbox_get_footer
#define LV_IMG_CF_TRUE_COLOR LV_COLOR_FORMAT_NATIVE
#define lv_color_to16 lv_color_to_u16
#define LV_COLOR_R(c) (c).red
#define LV_COLOR_G(c) (c).green
#define LV_COLOR_B(c) (c).blue
#define LV_COLOR_A(c) (c).alpha

static lv_obj_t* lv_msgbox_create(lv_obj_t* parent, const char* title, const char* text, const char** buttons,
                                  bool add_close) {
    auto msgbox = lv_msgbox_create(parent);

    for (int i = 0;; i++) {
        if (!buttons[i][0]) {
            break;
        }

        lv_msgbox_add_footer_button(msgbox, buttons[i]);
    }

    return msgbox;
}

static uint32_t lv_color_to_u32(lv_color32_t color) {
    return lv_color_to_u32(lv_color_make(color.red, color.green, color.blue));
}

#endif

#include "Callback.h"
#include "support.h"

#ifndef LV_SIMULATOR

#include "Mutex.h"

#endif
