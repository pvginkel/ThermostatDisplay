idf_component_register(SRCS "support.cpp" "Application.cpp" "WifiConnection.cpp" "main.cpp" "MQTTConnection.cpp"
                       INCLUDE_DIRS ".")

if ( CMAKE_COMPILER_IS_GNUCC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers")
endif()
