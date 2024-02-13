#pragma once

#include "LvglUI.h"
#include "ThermostatState.h"

class ThermostatUI : public LvglUI {
    static constexpr double TEMPERATURE_MIN = 9;
    static constexpr double TEMPERATURE_MAX = 35;

    lv_style_t _temperatureButtonStyle;
    lv_style_t _normalLabelStyle;
    lv_style_t _largeLabelStyle;
    lv_obj_t* _stateLabel;
    ThermostatState _state;
    lv_obj_t* _setpointLabel;
    lv_obj_t* _setpointFractionLabel;
    lv_obj_t* _localTemperatureLabel;
    lv_obj_t* _setpointArc;
    lv_obj_t* _setpointHeatingArc;
    lv_obj_t* _localTemperatureCircle;
    lv_obj_t* _setpointCircle;
    CallbackArgs<double> _setpointChanged;

public:
    ThermostatUI(lv_obj_t* parent);
    void setState(const ThermostatState& state);
    void onSetpointChanged(CallbackArgs<double>::Func func, uintptr_t data) { _setpointChanged.set(func, data); }

protected:
    void doRender(lv_obj_t* parent) override;
    void doBegin() override;

private:
    lv_obj_t* createTemperatureButton(lv_obj_t* parent, const char* image, int x, int y, int radius);
    lv_obj_t* createLabel(lv_obj_t* parent, lv_style_t& style, int x, int y, int width, int height,
                          lv_text_align_t align);
    void createSetpointLabels(lv_obj_t* parent);
    void createArcControl(lv_obj_t* parent);
    void setupArcHitTesting(lv_obj_t* obj);
    void positionCircleOnArc(lv_obj_t* obj, int size, int angleDegrees);
    lv_obj_t* createArcObject(lv_obj_t* parent, lv_color_t color);
    void handleSetpointChange(double offset);
    void setSetpoint(double setpoint);
    double roundSetpoint(double setpoint);
    void renderState();
    void handleArcPressed(lv_event_t* e);
};
