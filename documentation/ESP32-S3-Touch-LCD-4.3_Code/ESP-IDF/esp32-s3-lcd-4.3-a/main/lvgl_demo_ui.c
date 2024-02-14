/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

// This demo UI is adapted from LVGL official example: https://docs.lvgl.io/master/examples.html#scatter-chart

#include "lvgl.h"

static void scene_next_task_cb(lv_timer_t * timer);

static int32_t scene_act = -1;
// static lv_obj_t * scene_bg;
static lv_style_t style_common;
static lv_obj_t *scr;

void example_lvgl_demo_ui(lv_disp_t *disp)
{    
    scr = lv_disp_get_scr_act(disp);
    lv_obj_set_style_bg_color(scr, lv_color_white() , 0);
    scene_act=1;
    scene_next_task_cb(NULL);
}

#define SCENE_TIME      2000      /*ms*/
extern uint8_t sd_flag;
static void scene_next_task_cb(lv_timer_t * timer)
{
    LV_UNUSED(timer);
    lv_obj_clean(scr);

    if(scene_act==1){
        lv_obj_set_style_bg_color(scr, lv_palette_darken(LV_PALETTE_RED,4), 0);
        // lv_obj_set_style_bg_color(scr, lv_color_red() , 0);
        scene_act=2;
    }
    else if(scene_act==2){
        lv_obj_set_style_bg_color(scr, lv_palette_darken(LV_PALETTE_GREEN,1), 0);
        // lv_obj_set_style_bg_color(scr, lv_color_green() , 0);
        scene_act=3;
    }else if(scene_act==3){
        lv_obj_set_style_bg_color(scr, lv_palette_darken(LV_PALETTE_BLUE,1), 0);
        // lv_obj_set_style_bg_color(scr, lv_color_blue() , 0);
        scene_act=4;
    }else{
        lv_obj_set_style_bg_color(scr, lv_color_white() , 0);
        scene_act=5;
    }

    if(scene_act==5){
        lv_obj_t * label1 = lv_label_create(lv_scr_act());
        lv_label_set_long_mode(label1, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
        lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
        lv_obj_set_width(label1, 150);  /*Set smaller width to make the lines wrap*/
        lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align(label1, LV_ALIGN_CENTER, 0, -40);
        if(sd_flag==0){
            lv_label_set_text(label1, "#0000ff SD Card # #ff00ff is OK");
        }else{
            lv_label_set_text(label1, "#0000ff SD Card # #ff00ff is failure");
        }
        scene_act = 6;
    }

    lv_timer_t * t = lv_timer_create(scene_next_task_cb, SCENE_TIME, NULL);
    lv_timer_set_repeat_count(t, 1);
}
