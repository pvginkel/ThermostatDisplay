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

#include "Mutex.h"

#else

typedef void* QueueHandle_t;

#endif

#include "Callback.h"
#include "support.h"
