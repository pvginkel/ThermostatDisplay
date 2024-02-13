idf_component_register(
    SRCS "main.c" "Application.cpp" "MQTTConnection.cpp" "support.cpp" "WifiConnection.cpp"
    "lv_font_roboto_30.c" "lv_font_roboto_85.c" "LvglUI.cpp" "Messages.cpp" "LoadingUI.cpp" "ThermostatUI.cpp"
    INCLUDE_DIRS "." "../components/espressif__esp_lcd_touch_gt911/include"
)

if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers -Wno-switch -Wno-deprecated-enum-enum-conversion")
endif()
