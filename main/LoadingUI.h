#pragma once

#include <string>

#include "Callback.h"
#include "LvglUI.h"

using namespace std;

enum class LoadingUIState { None, Loading, Error };

class LoadingUI : public LvglUI {
    const char* _title;
    const char* _error;
    Callback _retryClicked;
    LoadingUIState _state;

public:
    LoadingUI(lv_obj_t* parent) : LvglUI(parent), _title(), _error(), _state() {}

    void setState(LoadingUIState state) { _state = state; }
    void setTitle(const char* title) { _title = title; }
    void setError(const char* error) { _error = error; }
    void redraw() { render(); }
    void onRetryClicked(Callback::Func func, uintptr_t data = 0) { _retryClicked.set(func, data); }

protected:
    void doRender(lv_obj_t* parent) override;
    void renderTitle(lv_obj_t* parent);
    void renderLoading(lv_obj_t* parent);
    void renderError(lv_obj_t* parent);
};
