#pragma once

void lv_label_get_text_size(lv_point_t* size_res, const lv_obj_t* obj, int32_t letter_space, int32_t line_space,
                            int32_t max_width, lv_text_flag_t flag);
void lv_write_color(uint8_t** p, uint32_t x, uint32_t y, lv_color_t color);
void lv_image_create_radial_dsc(lv_image_dsc_t* image_dsc, lv_color_t start_color, lv_color_t end_color);
