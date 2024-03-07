idf_component_register(
    SRCS "DeviceConfiguration.cpp" "OTAManager.cpp" "CH422G.cpp" "Queue.cpp" "main.cpp" "Application.cpp" "MQTTConnection.cpp" "support.cpp" "WifiConnection.cpp"
    "lv_font_roboto_30.c" "lv_font_roboto_85.c" "LvglUI.cpp" "Messages.cpp" "LoadingUI.cpp" "ThermostatUI.cpp"
    "ESP_Panel.cpp"
    INCLUDE_DIRS "."
)

if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers -Wno-switch -Wno-deprecated-enum-enum-conversion")
endif()
