cmake_minimum_required(VERSION 3.15)
project(projectM_Demos)

# Find parent projectM installation
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include projectM headers
include_directories(
    ${CMAKE_SOURCE_DIR}/../src
    ${CMAKE_SOURCE_DIR}/../src/libprojectM
    ${CMAKE_SOURCE_DIR}/../vendor/glm
)

# Link to projectM library (when built)
link_directories(${CMAKE_SOURCE_DIR}/../build/src/libprojectM)

# Demo 1: Boids Flocking
add_executable(BoidsDemo
    BoidsDemo.cpp
)
target_link_libraries(BoidsDemo
    # Note: In a real build, link against projectM library
    # For now, this is a demonstration
)

# Demo 2: Reaction-Diffusion Patterns
add_executable(ReactionDiffusionDemo
    ReactionDiffusionDemo.cpp
)
target_link_libraries(ReactionDiffusionDemo
    # Note: Link against projectM library
)

# Demo 3: 4D Hyperdimensional Visualization
add_executable(Hyperdimensional4DDemo
    Hyperdimensional4DDemo.cpp
)
target_link_libraries(Hyperdimensional4DDemo
    # Note: Link against projectM library
)

# Demo 4: Quaternion Julia Set Fractals
add_executable(QuaternionJuliaDemo
    QuaternionJuliaDemo.cpp
)
target_link_libraries(QuaternionJuliaDemo
    # Note: Link against projectM library
)

# Installation (optional)
install(TARGETS
    BoidsDemo
    ReactionDiffusionDemo
    Hyperdimensional4DDemo
    QuaternionJuliaDemo
    DESTINATION bin/demos
)

# Copy README
install(FILES README.md DESTINATION share/doc/projectm/demos)
