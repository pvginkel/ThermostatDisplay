#include "includes.h"

#include "ESP_Panel.h"

#include "ESP_Panel_Conf.h"
#include "driver/i2c.h"
#include "esp_lcd_touch_gt911.h"

#define EXAMPLE_MAX_CHAR_SIZE 64

// The sample uses 160 (1/3d of 480), but we don't have that available.
#define DISPLAY_BUFFER_LINES (480 / 5)

// Pin assignments can be set in menuconfig, see "SD SPI Example Configuration" menu.
// You can also change the pin assignments here by changing the following 4 lines.
#define PIN_NUM_MISO CONFIG_DISPLAY_PIN_MISO
#define PIN_NUM_MOSI CONFIG_DISPLAY_PIN_MOSI
#define PIN_NUM_CLK CONFIG_DISPLAY_PIN_CLK
#define PIN_NUM_CS CONFIG_DISPLAY_PIN_CS

#define I2C_MASTER_SCL_IO 9 /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO 8 /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM                                                                                              \
    I2C_NUM_0 /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the \
                 chip */
#define I2C_MASTER_FREQ_HZ 400000   /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS 1000

static const char *TAG = "main";
uint8_t sd_flag = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your LCD spec //////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ ESP_PANEL_LCD_RGB_CLK_HZ
#define EXAMPLE_LCD_BK_LIGHT_ON_LEVEL 1
#define EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL !EXAMPLE_LCD_BK_LIGHT_ON_LEVEL
#define EXAMPLE_PIN_NUM_BK_LIGHT -1
#define EXAMPLE_PIN_NUM_HSYNC 46
#define EXAMPLE_PIN_NUM_VSYNC 3
#define EXAMPLE_PIN_NUM_DE 5
#define EXAMPLE_PIN_NUM_PCLK 7
#define EXAMPLE_PIN_NUM_DATA0 14   // B3
#define EXAMPLE_PIN_NUM_DATA1 38   // B4
#define EXAMPLE_PIN_NUM_DATA2 18   // B5
#define EXAMPLE_PIN_NUM_DATA3 17   // B6
#define EXAMPLE_PIN_NUM_DATA4 10   // B7
#define EXAMPLE_PIN_NUM_DATA5 39   // G2
#define EXAMPLE_PIN_NUM_DATA6 0    // G3
#define EXAMPLE_PIN_NUM_DATA7 45   // G4
#define EXAMPLE_PIN_NUM_DATA8 48   // G5
#define EXAMPLE_PIN_NUM_DATA9 47   // G6
#define EXAMPLE_PIN_NUM_DATA10 21  // G7
#define EXAMPLE_PIN_NUM_DATA11 1   // R3
#define EXAMPLE_PIN_NUM_DATA12 2   // R4
#define EXAMPLE_PIN_NUM_DATA13 42  // R5
#define EXAMPLE_PIN_NUM_DATA14 41  // R6
#define EXAMPLE_PIN_NUM_DATA15 40  // R7
#define EXAMPLE_PIN_NUM_DISP_EN -1

// The pixel number in horizontal and vertical
#define EXAMPLE_LCD_H_RES 800
#define EXAMPLE_LCD_V_RES 480

#if CONFIG_DISPLAY_DOUBLE_FB
#define EXAMPLE_LCD_NUM_FB 2
#else
#define EXAMPLE_LCD_NUM_FB 1
#endif  // CONFIG_DISPLAY_DOUBLE_FB

#define EXAMPLE_LVGL_TICK_PERIOD_MS 2

/**
 * @brief i2c master initialization
 */
esp_err_t ESP_Panel::i2c_master_init() {
    auto i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master{
            .clk_speed = I2C_MASTER_FREQ_HZ,
        },
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

// extern lv_obj_t *scr;
void ESP_Panel::lvgl_touch_cb(lv_indev_drv_t *drv, lv_indev_data_t *data) {
    uint16_t touchpad_x[1] = {0};
    uint16_t touchpad_y[1] = {0};
    uint8_t touchpad_cnt = 0;

    /* Read touch controller data */
    esp_lcd_touch_read_data((esp_lcd_touch_handle_t)drv->user_data);

    /* Get coordinates */
    bool touchpad_pressed = esp_lcd_touch_get_coordinates((esp_lcd_touch_handle_t)drv->user_data, touchpad_x,
                                                          touchpad_y, NULL, &touchpad_cnt, 1);

    if (touchpad_pressed && touchpad_cnt > 0) {
        data->point.x = touchpad_x[0];
        data->point.y = touchpad_y[0];
        data->state = LV_INDEV_STATE_PR;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

bool ESP_Panel::on_vsync_event(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *event_data,
                               void *user_data) {
    BaseType_t high_task_awoken = pdFALSE;
#if CONFIG_DISPLAY_AVOID_TEAR_EFFECT_WITH_SEM
    if (xSemaphoreTakeFromISR(sem_gui_ready, &high_task_awoken) == pdTRUE) {
        xSemaphoreGiveFromISR(sem_vsync_end, &high_task_awoken);
    }
#endif
    return high_task_awoken == pdTRUE;
}

void ESP_Panel::lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map) {
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)drv->user_data;
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;
#if CONFIG_DISPLAY_AVOID_TEAR_EFFECT_WITH_SEM
    xSemaphoreGive(sem_gui_ready);
    xSemaphoreTake(sem_vsync_end, portMAX_DELAY);
#endif
    // pass the draw buffer to the driver
    esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
    lv_disp_flush_ready(drv);
}

void ESP_Panel::increase_lvgl_tick(void *arg) {
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}

lv_disp_t *ESP_Panel::begin() {
    static lv_disp_draw_buf_t disp_buf;  // contains internal graphic buffer(s) called draw buffer(s)
    static lv_disp_drv_t disp_drv;       // contains callback functions

#if CONFIG_DISPLAY_AVOID_TEAR_EFFECT_WITH_SEM
    ESP_LOGI(TAG, "Create semaphores");
    sem_vsync_end = xSemaphoreCreateBinary();
    assert(sem_vsync_end);
    sem_gui_ready = xSemaphoreCreateBinary();
    assert(sem_gui_ready);
#endif

#if EXAMPLE_PIN_NUM_BK_LIGHT >= 0
    ESP_LOGI(TAG, "Turn off LCD backlight");
    gpio_config_t bk_gpio_config = {.mode = GPIO_MODE_OUTPUT, .pin_bit_mask = 1ULL << EXAMPLE_PIN_NUM_BK_LIGHT};
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));
#endif

    ESP_LOGI(TAG, "Install RGB LCD panel driver");
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_rgb_panel_config_t panel_config = {
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .timings =
            {
                .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
                .h_res = EXAMPLE_LCD_H_RES,
                .v_res = EXAMPLE_LCD_V_RES,
                // The following parameters should refer to LCD spec
                .hsync_pulse_width = ESP_PANEL_LCD_RGB_HPW,
                .hsync_back_porch = ESP_PANEL_LCD_RGB_HBP,
                .hsync_front_porch = ESP_PANEL_LCD_RGB_HFP,
                .vsync_pulse_width = ESP_PANEL_LCD_RGB_VPW,
                .vsync_back_porch = ESP_PANEL_LCD_RGB_VBP,
                .vsync_front_porch = ESP_PANEL_LCD_RGB_VFP,
                .flags =
                    {
                        .pclk_active_neg = ESP_PANEL_LCD_RGB_PCLK_ACTIVE_NEG,
                    },

            },
        .data_width = ESP_PANEL_LCD_RGB_DATA_WIDTH,
        .num_fbs = EXAMPLE_LCD_NUM_FB,
#if CONFIG_DISPLAY_USE_BOUNCE_BUFFER
        .bounce_buffer_size_px = 10 * EXAMPLE_LCD_H_RES,
#endif
        .psram_trans_align = 64,
        .hsync_gpio_num = EXAMPLE_PIN_NUM_HSYNC,
        .vsync_gpio_num = EXAMPLE_PIN_NUM_VSYNC,
        .de_gpio_num = EXAMPLE_PIN_NUM_DE,
        .pclk_gpio_num = EXAMPLE_PIN_NUM_PCLK,
        .disp_gpio_num = EXAMPLE_PIN_NUM_DISP_EN,
        .data_gpio_nums =
            {
                EXAMPLE_PIN_NUM_DATA0,
                EXAMPLE_PIN_NUM_DATA1,
                EXAMPLE_PIN_NUM_DATA2,
                EXAMPLE_PIN_NUM_DATA3,
                EXAMPLE_PIN_NUM_DATA4,
                EXAMPLE_PIN_NUM_DATA5,
                EXAMPLE_PIN_NUM_DATA6,
                EXAMPLE_PIN_NUM_DATA7,
                EXAMPLE_PIN_NUM_DATA8,
                EXAMPLE_PIN_NUM_DATA9,
                EXAMPLE_PIN_NUM_DATA10,
                EXAMPLE_PIN_NUM_DATA11,
                EXAMPLE_PIN_NUM_DATA12,
                EXAMPLE_PIN_NUM_DATA13,
                EXAMPLE_PIN_NUM_DATA14,
                EXAMPLE_PIN_NUM_DATA15,
            },
        .flags =
            {
                .fb_in_psram = true,  // allocate frame buffer in PSRAM
            },
    };
    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));

    ESP_LOGI(TAG, "Register event callbacks");
    esp_lcd_rgb_panel_event_callbacks_t cbs = {
        .on_vsync = on_vsync_event,
    };
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_register_event_callbacks(panel_handle, &cbs, &disp_drv));

    ESP_LOGI(TAG, "Initialize RGB LCD panel");
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

#if EXAMPLE_PIN_NUM_BK_LIGHT >= 0
    ESP_LOGI(TAG, "Turn on LCD backlight");
    gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, EXAMPLE_LCD_BK_LIGHT_ON_LEVEL);
#endif

    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(TAG, "I2C initialized successfully");

    uint8_t write_buf = 0x01;

    auto ret =
        i2c_master_write_to_device(I2C_MASTER_NUM, 0x24, &write_buf, 1, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "0x48 0x01 ret is %d", ret);

    write_buf = 0x0E;
    ret = i2c_master_write_to_device(I2C_MASTER_NUM, 0x38, &write_buf, 1, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "0x70 0x00 ret is %d", ret);

    esp_lcd_touch_handle_t tp = NULL;
    esp_lcd_panel_io_handle_t tp_io_handle = NULL;

    ESP_LOGI(TAG, "Initialize I2C");

    esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_GT911_CONFIG();

    ESP_LOGI(TAG, "Initialize touch IO (I2C)");
    /* Touch IO handle */
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)I2C_MASTER_NUM, &tp_io_config, &tp_io_handle));
    esp_lcd_touch_config_t tp_cfg = {
        .x_max = EXAMPLE_LCD_V_RES,
        .y_max = EXAMPLE_LCD_H_RES,
        .rst_gpio_num = GPIO_NUM_NC,
        .int_gpio_num = GPIO_NUM_NC,
        .flags =
            {
                .swap_xy = 0,
                .mirror_x = 0,
                .mirror_y = 0,
            },
    };
    /* Initialize touch */
    ESP_LOGI(TAG, "Initialize touch controller GT911");
    ESP_ERROR_CHECK(esp_lcd_touch_new_i2c_gt911(tp_io_handle, &tp_cfg, &tp));

    ESP_LOGI(TAG, "Initialize LVGL library");
    lv_init();
    void *buf1 = NULL;
#if CONFIG_DISPLAY_DOUBLE_FB
    void *buf2 = NULL;
    ESP_LOGI(TAG, "Use frame buffers as LVGL draw buffers");
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_get_frame_buffer(panel_handle, 2, &buf1, &buf2));
    // initialize LVGL draw buffers
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES);
#else
    size_t buf1size = EXAMPLE_LCD_H_RES * DISPLAY_BUFFER_LINES * sizeof(lv_color_t);
    ESP_LOGI(TAG, "Allocate separate LVGL draw buffers from PSRAM, available %d DMA capable %d, requesting %d",
             heap_caps_get_free_size(MALLOC_CAP_8BIT), heap_caps_get_free_size(MALLOC_CAP_DMA), buf1size);
    buf1 = heap_caps_malloc(EXAMPLE_LCD_H_RES * DISPLAY_BUFFER_LINES * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf1);
    // buf2 = heap_caps_malloc(EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    // assert(buf2);
    // initialize LVGL draw buffers
    lv_disp_draw_buf_init(&disp_buf, buf1, NULL, EXAMPLE_LCD_H_RES * DISPLAY_BUFFER_LINES);
#endif  // CONFIG_DISPLAY_DOUBLE_FB

    ESP_LOGI(TAG, "Register display driver to LVGL");
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = EXAMPLE_LCD_H_RES;
    disp_drv.ver_res = EXAMPLE_LCD_V_RES;
    disp_drv.flush_cb = lvgl_flush_cb;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.user_data = panel_handle;
#if CONFIG_DISPLAY_DOUBLE_FB
    disp_drv.full_refresh =
        true;  // the full_refresh mode can maintain the synchronization between the two frame buffers
#endif
    lv_disp_t *disp = lv_disp_drv_register(&disp_drv);

    ESP_LOGI(TAG, "Install LVGL tick timer");
    // Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
    const esp_timer_create_args_t lvgl_tick_timer_args = {.callback = &increase_lvgl_tick, .name = "lvgl_tick"};

    static lv_indev_drv_t indev_drv;  // Input device driver (Touch)
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.disp = disp;
    indev_drv.read_cb = lvgl_touch_cb;
    indev_drv.user_data = tp;

    lv_indev_drv_register(&indev_drv);

    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000));

    return disp;
}

void ESP_Panel::process() {  // raise the task priority of LVGL and/or reduce the handler period can improve the
                             // performance
    vTaskDelay(pdMS_TO_TICKS(10));
    // The task running lv_timer_handler should have lower priority than that running `lv_tick_inc`
    lv_timer_handler();
}