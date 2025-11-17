add_library(Settings OBJECT
        SettingsManager.cpp
        SettingsManager.hpp
        GeneralSettings.hpp
        AudioSettings.hpp
        BoidsSettings.hpp
        ReactionDiffusionSettings.hpp
        Hyperdimensional4DSettings.hpp
        JuliaSetSettings.hpp
        )

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

set_target_properties(Settings PROPERTIES
        FOLDER libprojectM
        )
