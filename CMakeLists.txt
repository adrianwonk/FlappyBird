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

add_executable(exec src/main.cpp src/PlayerObject.cpp)

# Copy the assets folder to the output directory after building
add_custom_command(TARGET exec POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/src $<TARGET_FILE_DIR:exec>/src
)

target_compile_features(exec PRIVATE cxx_std_17)
target_link_libraries(exec PRIVATE SFML::Graphics)

