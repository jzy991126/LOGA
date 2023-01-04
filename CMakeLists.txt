cmake_minimum_required(VERSION 3.14 FATAL_ERROR)


project(breakout VERSION 0.0.1)

include_directories(include)
include_directories(third)
aux_source_directory(src SRC_LIST)

find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME}  glfw imgui::imgui glad::glad glm::glm)