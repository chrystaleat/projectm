if(NOT ENABLE_SYSTEM_GLM)
    add_subdirectory(glm)
endif()
add_subdirectory(hlslparser)
if(NOT TARGET projectM::Eval)
    add_subdirectory(projectm-eval)
endif()
add_subdirectory(SOIL2)

# ImGui for settings UI
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/imgui/CMakeLists.txt)
    add_subdirectory(imgui)
endif()
