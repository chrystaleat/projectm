# Settings System CMake Configuration

add_library(Settings OBJECT
    SettingsManager.cpp
    SettingsManager.hpp
)

target_include_directories(Settings
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/.."
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(Settings
    PRIVATE
        projectM::API
)

# Optionally compile SettingsUI if ImGui is available
if(TARGET imgui::imgui)
    message(STATUS "ImGui found - building SettingsUI")

    target_sources(Settings
        PRIVATE
            SettingsUI.cpp
            SettingsUI.hpp
    )

    target_link_libraries(Settings
        PRIVATE
            imgui::imgui
            SDL2::SDL2
    )

    target_compile_definitions(Settings
        PUBLIC
            ENABLE_SETTINGS_UI
    )
else()
    message(STATUS "ImGui not found - SettingsUI will not be built")
endif()

# Export for use in projectM library
set_target_properties(Settings PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
