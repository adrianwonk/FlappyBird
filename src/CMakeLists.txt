target_sources(run_program
    PRIVATE
        main.cpp
        ObstacleObject.cpp
        PlayerObject.cpp
)

target_include_directories(run_program
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
