cmake_minimum_required(VERSION 3.15)
project(main VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Специальные флаги для Helgrind
set(HELGRIND_FLAGS "-g -O1 -fno-omit-frame-pointer -fno-inline")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${HELGRIND_FLAGS}")

find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME}
    main.cpp
    src/log.cpp
    src/logger.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

# Цель для запуска с Helgrind
add_custom_target(helgrind
    COMMAND valgrind --tool=helgrind ./${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    COMMENT "Running with Helgrind"
)