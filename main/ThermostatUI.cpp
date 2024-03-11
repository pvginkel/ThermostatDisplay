#define _USE_MATH_DEFINES

#include "includes.h"

#include "ThermostatUI.h"

#include "Messages.h"

#define SELF(e) ((ThermostatUI*)lv_event_get_user_data((e)))

constexpr auto ARC_X_OFFSET = 50;
constexpr auto ARC_Y_OFFSET = 57;
constexpr auto ARC_WIDTH = 9;
constexpr auto ARC_RADIUS = 52;
constexpr auto ARC_LOCAL_TEMPERATURE_CIRCLE_SIZE = 2.5;
constexpr auto ARC_SETPOINT_CIRCLE_SIZE = 5;

constexpr auto BUTTON_RADIUS = 6;
constexpr auto BUTTON_X_OFFSET = 90;
constexpr auto BUTTON_Y_RELATIVE_OFFSET = BUTTON_RADIUS * 2 + 2;

constexpr auto STATUS_Y_OFFSET = 33;
constexpr auto STATUS_WIDTH = 40;
constexpr auto STATUS_HEIGHT = 8;
constexpr auto SETPOINT_Y_OFFSET = 54;
constexpr auto LOCAL_TEMPERATURE_Y_OFFSET = 74;
constexpr auto LOCAL_TEMPERATURE_WIDTH = 40;
constexpr auto LOCAL_TEMPERATURE_HEIGHT = 8;

ThermostatUI::ThermostatUI(lv_obj_t* parent)
    : LvglUI(parent),
      _stateLabel(nullptr),
      _setpointLabel(nullptr),
      _setpointFractionLabel(nullptr),
      _localTemperatureLabel(nullptr),
      _setpointArc(nullptr),
      _setpointHeatingArc(nullptr),
      _localTemperatureCircle(nullptr),
      _setpointCircle(nullptr) {}

void ThermostatUI::setState(const ThermostatState& state) {
    _state = state;

    renderState();
}

void ThermostatUI::doBegin() { render(); }

void ThermostatUI::doRender(lv_obj_t* parent) {
    lv_obj_clear_flag(parent, LV_OBJ_FLAG_SCROLLABLE);

    lv_style_init(&_temperatureButtonStyle);
    lv_style_set_bg_color(&_temperatureButtonStyle, lv_color_black());
    lv_style_set_border_color(&_temperatureButtonStyle, lv_theme_get_color_primary(parent));
    lv_style_set_border_width(&_temperatureButtonStyle, 2);
    lv_style_set_text_font(&_temperatureButtonStyle, NORMAL_FONT);

    lv_style_init(&_normalLabelStyle);
    lv_style_set_text_font(&_normalLabelStyle, NORMAL_FONT);
    lv_style_set_text_align(&_normalLabelStyle, LV_TEXT_ALIGN_CENTER);

    lv_style_init(&_largeDigitsLabelStyle);
    lv_style_set_text_font(&_largeDigitsLabelStyle, LARGE_DIGITS_FONT);
    lv_style_set_text_align(&_largeDigitsLabelStyle, LV_TEXT_ALIGN_CENTER);

    _stateLabel = createLabel(parent, _normalLabelStyle, pw(50), ph(STATUS_Y_OFFSET), pw(STATUS_WIDTH),
                              ph(STATUS_HEIGHT), LV_TEXT_ALIGN_CENTER);

    createSetpointLabels(parent);

    _localTemperatureLabel =
        createLabel(parent, _normalLabelStyle, pw(50), ph(LOCAL_TEMPERATURE_Y_OFFSET), pw(LOCAL_TEMPERATURE_WIDTH),
                    ph(LOCAL_TEMPERATURE_HEIGHT), LV_TEXT_ALIGN_CENTER);

    createArcControl(parent);

    const auto downButton = createTemperatureButton(parent, LV_SYMBOL_MINUS, pw(BUTTON_X_OFFSET),
                                                    ph(50 + BUTTON_Y_RELATIVE_OFFSET), pw(BUTTON_RADIUS));
    lv_obj_add_event_cb(
        downButton, [](auto e) { SELF(e)->handleSetpointChange(-0.5); }, LV_EVENT_CLICKED, this);

    const auto upButton = createTemperatureButton(parent, LV_SYMBOL_PLUS, pw(BUTTON_X_OFFSET),
                                                  ph(50 - BUTTON_Y_RELATIVE_OFFSET), pw(BUTTON_RADIUS));
    lv_obj_add_event_cb(
        upButton, [](auto e) { SELF(e)->handleSetpointChange(0.5); }, LV_EVENT_CLICKED, this);

    renderState();
}

lv_obj_t* ThermostatUI::createTemperatureButton(lv_obj_t* parent, const char* image, int x, int y, int radius) {
    const auto button = lv_btn_create(parent);

    lv_obj_add_style(button, &_temperatureButtonStyle, 0);
    lv_obj_set_size(button, radius * 2, radius * 2);
    lv_obj_set_x(button, x - radius);
    lv_obj_set_y(button, y - radius);
    lv_obj_set_style_radius(button, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_img_src(button, image, 0);

    return button;
}

lv_obj_t* ThermostatUI::createLabel(lv_obj_t* parent, lv_style_t& style, int x, int y, int width, int height,
                                    lv_text_align_t align) {
    const auto label = lv_label_create(parent);

    lv_obj_add_style(label, &style, 0);
    lv_obj_set_style_text_align(label, align, LV_PART_MAIN);
    lv_obj_set_bounds(label, x, y, width, height, align);

    return label;
}

void ThermostatUI::createSetpointLabels(lv_obj_t* parent) {
    _setpointLabel = lv_label_create(parent);
    lv_obj_add_style(_setpointLabel, &_largeDigitsLabelStyle, 0);

    _setpointUnitLabel = lv_label_create(parent);
    lv_obj_add_style(_setpointUnitLabel, &_normalLabelStyle, 0);
    lv_label_set_text(_setpointUnitLabel, "°C");

    _setpointFractionLabel = lv_label_create(parent);
    lv_obj_add_style(_setpointFractionLabel, &_normalLabelStyle, 0);
}

void ThermostatUI::createArcControl(lv_obj_t* parent) {
    auto backgroundArc = createArcObject(parent, lv_color_make(41, 41, 41));

    _setpointArc = createArcObject(parent, lv_color_make(134, 65, 34));

    _setpointHeatingArc = createArcObject(parent, lv_color_make(252, 114, 52));

    _localTemperatureCircle = lv_obj_create(parent);
    lv_obj_set_style_radius(_localTemperatureCircle, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_size(_localTemperatureCircle, ph(ARC_LOCAL_TEMPERATURE_CIRCLE_SIZE),
                    ph(ARC_LOCAL_TEMPERATURE_CIRCLE_SIZE));

    _setpointCircle = lv_obj_create(parent);
    lv_obj_clear_flag(_setpointCircle, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(_setpointCircle, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_size(_setpointCircle, ph(ARC_SETPOINT_CIRCLE_SIZE), ph(ARC_SETPOINT_CIRCLE_SIZE));
    lv_obj_set_style_bg_color(_setpointCircle, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_border_color(_setpointCircle, lv_color_white(), LV_PART_MAIN);

    setupArcHitTesting(backgroundArc);
    setupArcHitTesting(_setpointArc);
    setupArcHitTesting(_setpointHeatingArc);
    setupArcHitTesting(_localTemperatureCircle);
    setupArcHitTesting(_setpointCircle);
}

void ThermostatUI::setupArcHitTesting(lv_obj_t* obj) {
    lv_obj_add_event_cb(
        obj, [](auto e) { SELF(e)->handleArcPressed(e); }, LV_EVENT_PRESSING, this);
    lv_obj_add_event_cb(
        obj, [](auto e) { SELF(e)->handleArcPressed(e); }, LV_EVENT_PRESSED, this);
    lv_obj_add_event_cb(
        obj, [](auto e) { SELF(e)->handleArcPressed(e); }, LV_EVENT_RELEASED, this);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_ADV_HITTEST);
}

void ThermostatUI::positionCircleOnArc(lv_obj_t* obj, int size, int angleDegrees) {
    auto centerX = pw(ARC_X_OFFSET);
    auto centerY = ph(ARC_Y_OFFSET);
    auto arcWidth = ph(ARC_WIDTH);
    auto radius = ph(ARC_RADIUS) - arcWidth / 2;

    auto angleRadians = (angleDegrees + 135) * (M_PI / 180);

    auto x = centerX + radius * cos(angleRadians) - size / 2;
    auto y = centerY + radius * sin(angleRadians) - size / 2;

    lv_obj_set_x(obj, int32_t(x));
    lv_obj_set_y(obj, int32_t(y));
}

lv_obj_t* ThermostatUI::createArcObject(lv_obj_t* parent, lv_color_t color) {
    auto arc = lv_arc_create(parent);

    lv_obj_set_style_arc_width(arc, ph(ARC_WIDTH), LV_PART_MAIN);
    lv_obj_set_style_bg_color(arc, color, LV_PART_KNOB);
    lv_obj_set_style_arc_color(arc, color, LV_PART_MAIN);

    lv_obj_set_style_arc_width(arc, 0, LV_PART_INDICATOR);
    lv_obj_set_style_pad_all(arc, 0, LV_PART_KNOB);

    lv_obj_set_bounds(arc, pw(ARC_X_OFFSET), ph(ARC_Y_OFFSET), ph(ARC_RADIUS * 2), ph(ARC_RADIUS * 2),
                      LV_TEXT_ALIGN_CENTER);
    lv_arc_set_rotation(arc, 135);

    lv_arc_set_bg_angles(arc, 0, 270);

    return arc;
}

void ThermostatUI::handleSetpointChange(double offset) { setSetpoint(_state.setpoint + offset); }

void ThermostatUI::setSetpoint(double setpoint) {
    _state.setpoint = roundSetpoint(setpoint);

    renderState();

    _setpointChanged.call(_state.setpoint);
}

double ThermostatUI::roundSetpoint(double setpoint) {
    if (isnan(setpoint)) {
        return setpoint;
    }

    auto setpointClamped = clamp(setpoint, TEMPERATURE_MIN, TEMPERATURE_MAX);

    return round(setpointClamped * 2) / 2.0;
}

void ThermostatUI::renderState() {
    lv_label_set_text(_stateLabel, _state.state == ThermostatRunningState::True ? Messages::heating : Messages::idle);

    if (isnan(_state.setpoint)) {
        lv_label_set_text(_setpointLabel, "NA");
        lv_label_set_text(_setpointFractionLabel, "");
    } else {
        const auto roundedSetpoint = roundSetpoint(_state.setpoint);

        const auto setpointLabel = format("%d", int(roundedSetpoint));
        lv_label_set_text(_setpointLabel, setpointLabel.c_str());

        const auto setpointFractionLabel = format(",%d", int(roundedSetpoint * 10) % 10);
        lv_label_set_text(_setpointFractionLabel, setpointFractionLabel.c_str());
    }

    // Reposition the labels.
    lv_point_t setpointLabelSize;
    lv_label_get_text_size(&setpointLabelSize, _setpointLabel, 0, 0, LV_COORD_MAX,
                    LV_TEXT_FLAG_EXPAND);
    lv_point_t setpointUnitLabelSize;
    lv_label_get_text_size(&setpointUnitLabelSize, _setpointUnitLabel, 0, 0, LV_COORD_MAX,
        LV_TEXT_FLAG_EXPAND);
    lv_point_t setpointFractionLabelSize;
    lv_label_get_text_size(&setpointFractionLabelSize, _setpointFractionLabel, 0, 0, LV_COORD_MAX,
        LV_TEXT_FLAG_EXPAND);

    const auto setpointFullWidth = setpointLabelSize.x + max(setpointUnitLabelSize.x, setpointFractionLabelSize.x);
    const auto setpointXOffset = pw(50) - setpointFullWidth / 2;

    const auto setpointYOffset = ph(SETPOINT_Y_OFFSET) - setpointLabelSize.y / 2;

    const auto setpointUnitLabelNudge = -6;
    const auto setpointFractionLabelNudge = 9;

    lv_obj_set_pos(_setpointLabel, setpointXOffset, setpointYOffset);
    lv_obj_set_pos(_setpointUnitLabel, setpointXOffset + setpointLabelSize.x, setpointYOffset + setpointUnitLabelNudge);
    lv_obj_set_pos(_setpointFractionLabel, setpointXOffset + setpointLabelSize.x,
                   setpointYOffset + setpointLabelSize.y - setpointFractionLabelSize.y + setpointFractionLabelNudge);

    if (isnan(_state.localTemperature)) {
        lv_label_set_text(_localTemperatureLabel, "NA");
    } else {
        auto localTemperatureLabel = format("\xef\x8b\x89 %.1f °C", _state.localTemperature);
        replace(localTemperatureLabel.begin(), localTemperatureLabel.end(), '.', ',');
        lv_label_set_text(_localTemperatureLabel, localTemperatureLabel.c_str());
    }

    double setpointOffset = 0;
    double localTemperatureOffset = 0;

    if (!isnan(_state.setpoint)) {
        setpointOffset = int((clamp(_state.setpoint, TEMPERATURE_MIN, TEMPERATURE_MAX) - TEMPERATURE_MIN) /
                             (TEMPERATURE_MAX - TEMPERATURE_MIN) * 270);
    }

    if (!isnan(_state.localTemperature)) {
        localTemperatureOffset =
            int((clamp(_state.localTemperature, TEMPERATURE_MIN, TEMPERATURE_MAX) - TEMPERATURE_MIN) /
                (TEMPERATURE_MAX - TEMPERATURE_MIN) * 270);
    }

    lv_arc_set_bg_angles(_setpointArc, 0, setpointOffset);

    if (setpointOffset <= localTemperatureOffset) {
        lv_obj_add_flag(_setpointHeatingArc, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_flag(_setpointHeatingArc, LV_OBJ_FLAG_HIDDEN);

        lv_arc_set_bg_angles(_setpointHeatingArc, localTemperatureOffset, setpointOffset);
    }

    positionCircleOnArc(_localTemperatureCircle, ph(ARC_LOCAL_TEMPERATURE_CIRCLE_SIZE), int(localTemperatureOffset));

    auto localTemperatureCircleColor =
        setpointOffset < localTemperatureOffset ? lv_color_make(132, 132, 132) : lv_color_make(134, 65, 34);

    lv_obj_set_style_bg_color(_localTemperatureCircle, localTemperatureCircleColor, LV_PART_MAIN);
    lv_obj_set_style_border_color(_localTemperatureCircle, localTemperatureCircleColor, LV_PART_MAIN);

    positionCircleOnArc(_setpointCircle, ph(ARC_SETPOINT_CIRCLE_SIZE), int(setpointOffset));
}

void ThermostatUI::handleArcPressed(lv_event_t* e) {
    switch (lv_event_get_code(e)) {
        case LV_EVENT_PRESSED:
            return;

        case LV_EVENT_RELEASED:
            setSetpoint(_state.setpoint);
            return;
    }

    auto inputDevice = lv_indev_get_act();
    auto inputDeviceType = lv_indev_get_type(inputDevice);
    if (inputDeviceType == LV_INDEV_TYPE_ENCODER || inputDeviceType == LV_INDEV_TYPE_KEYPAD) {
        return;
    }

    lv_point_t p;
    lv_indev_get_point(inputDevice, &p);

    // Calculate the angle from the coordinates.

    auto centerX = pw(ARC_X_OFFSET);
    auto centerY = ph(ARC_Y_OFFSET);

    auto angleRadians = atan2(p.y - centerY, p.x - centerX);
    auto angleDegrees = angleRadians * (180 / M_PI) - 90;

    if (angleDegrees < 0) {
        angleDegrees += 360;
    }

    // The arc starts at 45 and ends at 270. Account for this.

    angleDegrees = clamp(angleDegrees - 45, 0.0, 270.0);

    const auto oldAngleDegrees = ((_state.setpoint - TEMPERATURE_MIN) / (TEMPERATURE_MAX - TEMPERATURE_MIN)) * 270;

    if (int(angleDegrees) != int(oldAngleDegrees)) {
        _state.setpoint = TEMPERATURE_MIN + ((TEMPERATURE_MAX - TEMPERATURE_MIN) * (angleDegrees / 270));

        renderState();
    }
}
