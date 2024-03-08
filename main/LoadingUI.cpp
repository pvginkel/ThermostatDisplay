#include "includes.h"

#include "LoadingUI.h"

#include "Messages.h"

#define SELF(e) ((LoadingUI*)lv_event_get_user_data((e)))

void LoadingUI::doRender(lv_obj_t* parent) {
    switch (_state) {
        case LoadingUIState::Loading:
            renderLoading(parent);
            break;
        case LoadingUIState::Error:
            renderError(parent);
            break;
    }
}

void LoadingUI::renderTitle(lv_obj_t* parent) {
    auto label = lv_label_create(parent);
    lv_label_set_text(label, _title);
    lv_obj_set_style_text_font(label, &lv_font_roboto_30, LV_PART_MAIN);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_bounds(label, pw(50), ph(13), pw(90), ph(10), LV_TEXT_ALIGN_CENTER);
}

void LoadingUI::renderLoading(lv_obj_t* parent) {
    renderTitle(parent);

    auto spinner = lv_spinner_create(parent, 2000, 60);
    lv_obj_set_bounds(spinner, pw(50), ph(57), ph(70), ph(70), LV_TEXT_ALIGN_CENTER);
    lv_obj_set_style_arc_width(spinner, ph(5), LV_PART_MAIN);
    lv_obj_set_style_arc_width(spinner, ph(5), LV_PART_INDICATOR);
}

void LoadingUI::renderError(lv_obj_t* parent) {
    renderTitle(parent);

    auto error = lv_label_create(parent);
    lv_label_set_text(error, _error);
    lv_obj_set_style_text_font(error, &lv_font_roboto_30, LV_PART_MAIN);
    lv_obj_set_style_text_align(error, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_bounds(error, pw(50), ph(30), pw(90), ph(15), LV_TEXT_ALIGN_CENTER);

    auto button = lv_btn_create(parent);
    lv_obj_set_style_text_font(button, &lv_font_roboto_30, LV_PART_MAIN);
    lv_obj_set_style_text_align(button, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_bounds(button, pw(50), ph(60), pw(30), ph(10), LV_TEXT_ALIGN_CENTER);
    lv_obj_add_event_cb(
        button, [](auto e) { SELF(e)->_retryClicked.call(); }, LV_EVENT_CLICKED, this);

    auto buttonLabel = lv_label_create(button);
    lv_label_set_text(buttonLabel, Messages::retry);
    lv_obj_center(buttonLabel);
}
