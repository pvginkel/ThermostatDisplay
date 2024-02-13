#pragma once

extern "C" {
// The temperature icon comes from Fontawesome. Include the "solid" TTF when
// generating fonts from https://lvgl.io/tools/fontconverter and include
// 0xf2c9 in the range.

// LV_FONT_DECLARE(lv_font_roboto_20);
LV_FONT_DECLARE(lv_font_roboto_30);
// LV_FONT_DECLARE(lv_font_roboto_64);
// LV_FONT_DECLARE(lv_font_roboto_80);
LV_FONT_DECLARE(lv_font_roboto_85);
}

void lv_obj_set_bounds(lv_obj_t* obj, int x, int y, int width, int height, lv_text_align_t align);

class LvglUI {
    lv_obj_t* _parent;
    int _screenWidth;
    int _screenHeight;

public:
    LvglUI(lv_obj_t* parent);
    LvglUI(const LvglUI& other) = delete;
    LvglUI(LvglUI&& other) noexcept = delete;
    LvglUI& operator=(const LvglUI& other) = delete;
    LvglUI& operator=(LvglUI&& other) noexcept = delete;
    virtual ~LvglUI() = default;

    void begin();

protected:
    void render();
    virtual void doRender(lv_obj_t* parent) = 0;
    virtual void doBegin() {}

    int screenWidth() const { return _screenWidth; }
    int screenHeight() const { return _screenHeight; }
    lv_coord_t pw(double value) const { return lv_coord_t(_screenWidth * (value / 100)); }
    lv_coord_t ph(double value) const { return lv_coord_t(_screenHeight * (value / 100)); }
};
