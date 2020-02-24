cmake_minimum_required(VERSION 3.5)

project(SFML_exp)


set(SDL2_INCLUDE_DIR Dependencies/SFML/include)
set(SDL2_LIB_DIR Dependencies/SFML/lib)
set(SOURCE_FILES exp.cpp)

include_directories(${SDL2_INCLUDE_DIR})
link_directories(${SDL2_LIB_DIR})

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} sfml-graphics.lib sfml-window.lib sfml-system.lib)
