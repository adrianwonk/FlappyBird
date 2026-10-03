cmake_minimum_required(VERSION 3.28)
project(FlappyBird)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable( run_program )
add_subdirectory(src)

# Copy the assets folder to the output directory after building
add_custom_command(TARGET run_program POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/src/resx $<TARGET_FILE_DIR:run_program>/src/resx
)

target_compile_features(run_program PRIVATE cxx_std_17)
target_link_libraries(run_program PRIVATE SFML::Graphics)
