#define _USE_MATH_DEFINES

#include "includes.h"

#include "ThermostatUI.h"

#include "Messages.h"

#define SELF(e) ((ThermostatUI*)lv_event_get_user_data((e)))

constexpr auto ARC_X_OFFSET = 50;
constexpr auto ARC_Y_OFFSET = 50;
constexpr auto ARC_WIDTH = 7.5;
constexpr auto ARC_RADIUS = 44;
constexpr auto ARC_LOCAL_TEMPERATURE_CIRCLE_SIZE = 2.5;
constexpr auto ARC_SETPOINT_CIRCLE_SIZE = 5;

constexpr auto BUTTON_X_RELATIVE_OFFSET = 7;
constexpr auto BUTTON_Y_OFFSET = 87;

constexpr auto STATUS_Y_OFFSET = 34;
constexpr auto SETPOINT_Y_OFFSET = 52;
constexpr auto LOCAL_TEMPERATURE_Y_OFFSET = 66;

ThermostatUI::ThermostatUI(lv_obj_t* parent)
    : LvglUI(parent),
      _temperatureButtonStyle(),
      _normalLabelStyle(),
      _largeLabelStyle(),
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
    const auto fontNormal = &lv_font_roboto_30;
    const auto fontLarge = &lv_font_roboto_85;

    lv_style_init(&_temperatureButtonStyle);
    lv_style_set_bg_color(&_temperatureButtonStyle, lv_color_black());
    lv_style_set_border_color(&_temperatureButtonStyle, lv_theme_get_color_primary(parent));
    lv_style_set_border_width(&_temperatureButtonStyle, 2);

    lv_style_init(&_normalLabelStyle);
    lv_style_set_text_font(&_normalLabelStyle, fontNormal);
    lv_style_set_text_align(&_normalLabelStyle, LV_TEXT_ALIGN_CENTER);

    lv_style_init(&_largeLabelStyle);
    lv_style_set_text_font(&_largeLabelStyle, fontLarge);
    lv_style_set_text_align(&_largeLabelStyle, LV_TEXT_ALIGN_CENTER);

    _stateLabel =
        createLabel(parent, _normalLabelStyle, pw(50), ph(STATUS_Y_OFFSET), pw(40), ph(5), LV_TEXT_ALIGN_CENTER);

    createSetpointLabels(parent);

    _localTemperatureLabel = createLabel(parent, _normalLabelStyle, pw(50), ph(LOCAL_TEMPERATURE_Y_OFFSET), pw(40),
                                         ph(5), LV_TEXT_ALIGN_CENTER);

    createArcControl(parent);

    const auto downButton =
        createTemperatureButton(parent, LV_SYMBOL_MINUS, pw(50 - BUTTON_X_RELATIVE_OFFSET), ph(BUTTON_Y_OFFSET), pw(4));
    lv_obj_add_event_cb(
        downButton, [](auto e) { SELF(e)->handleSetpointChange(-0.5); }, LV_EVENT_CLICKED, this);

    const auto upButton =
        createTemperatureButton(parent, LV_SYMBOL_PLUS, pw(50 + BUTTON_X_RELATIVE_OFFSET), ph(BUTTON_Y_OFFSET), pw(4));
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
    lv_obj_set_style_text_font(button, lv_theme_get_font_large(button), 0);

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
    static lv_coord_t columnDesc[] = {pw(20), pw(20), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t rowDesc[] = {ph(8), ph(8), LV_GRID_TEMPLATE_LAST};

    auto setpointContainer = lv_obj_create(parent);
    lv_obj_remove_style_all(setpointContainer);
    lv_obj_set_grid_dsc_array(setpointContainer, columnDesc, rowDesc);
    lv_obj_set_style_pad_row(setpointContainer, 0, 0);
    lv_obj_set_style_pad_column(setpointContainer, 0, 0);
    lv_obj_set_bounds(setpointContainer, pw(50), ph(SETPOINT_Y_OFFSET), pw(35), ph(19), LV_TEXT_ALIGN_CENTER);

    _setpointLabel = lv_label_create(setpointContainer);
    lv_obj_set_size(_setpointLabel, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_grid_cell(_setpointLabel, LV_GRID_ALIGN_END, 0, 1, LV_GRID_ALIGN_CENTER, 0, 2);
    lv_obj_add_style(_setpointLabel, &_largeLabelStyle, 0);
    lv_obj_set_style_pad_top(_setpointLabel, ph(2), LV_PART_MAIN);

    auto setpointUnitLabel = lv_label_create(setpointContainer);
    lv_obj_set_size(setpointUnitLabel, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_grid_cell(setpointUnitLabel, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_END, 0, 1);
    lv_obj_add_style(setpointUnitLabel, &_normalLabelStyle, 0);
    lv_label_set_text(setpointUnitLabel, "°C");

    _setpointFractionLabel = lv_label_create(setpointContainer);
    lv_obj_set_size(_setpointFractionLabel, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_grid_cell(_setpointFractionLabel, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 1, 1);
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
