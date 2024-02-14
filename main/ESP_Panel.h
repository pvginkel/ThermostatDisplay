#pragma once

class ESP_Panel {  // we use two semaphores to sync the VSYNC event and the LVGL task, to avoid potential tearing effect
    static void lvgl_touch_cb(lv_indev_drv_t *drv, lv_indev_data_t *data);
    static bool on_vsync_event(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *event_data,
                               void *user_data);
    static void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map);
    static void increase_lvgl_tick(void *arg);

#if CONFIG_DISPLAY_AVOID_TEAR_EFFECT_WITH_SEM
    SemaphoreHandle_t sem_vsync_end;
    SemaphoreHandle_t sem_gui_ready;
#endif

public:
    lv_disp_t *begin();
    void process();

private:
    esp_err_t i2c_master_init();
};
