set(SETTINGS_SOURCES
        SettingsManager.cpp
        SettingsManager.hpp
        GeneralSettings.hpp
        AudioSettings.hpp
        BoidsSettings.hpp
        ReactionDiffusionSettings.hpp
        Hyperdimensional4DSettings.hpp
        JuliaSetSettings.hpp
)

# Add ImGui UI if available
if(TARGET imgui::imgui)
    list(APPEND SETTINGS_SOURCES
            SettingsUI.cpp
            SettingsUI.hpp
    )
    set(ENABLE_SETTINGS_UI ON)
else()
    set(ENABLE_SETTINGS_UI OFF)
    message(STATUS "ImGui not found - Settings UI disabled. Run 'git submodule update --init vendor/imgui' to enable.")
endif()

add_library(Settings OBJECT ${SETTINGS_SOURCES})

target_include_directories(Settings
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/.."
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        )

target_link_libraries(Settings
        PRIVATE
        libprojectM::API # For export header
        )

# Link ImGui if UI is enabled
if(ENABLE_SETTINGS_UI)
    target_link_libraries(Settings
            PUBLIC
            imgui::imgui
    )
    target_compile_definitions(Settings
            PUBLIC
            ENABLE_SETTINGS_UI=1
    )
endif()

set_target_properties(Settings PROPERTIES
        FOLDER libprojectM
        )
