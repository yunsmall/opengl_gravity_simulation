cmake_minimum_required(VERSION 3.27)
project(planet3d)

set(CMAKE_CXX_STANDARD 23)

include_directories(src)
include_directories(src/physics)

add_compile_definitions(_USE_MATH_DEFINES)

file(GLOB_RECURSE SRC_SIMLULATION_FILES src/physics/*.cpp)
file(GLOB SRC_VISION_FILES src/*.cpp)

add_executable(${PROJECT_NAME} main.cpp ${SRC_SIMLULATION_FILES} ${SRC_VISION_FILES})
add_executable(${PROJECT_NAME}_console main_console.cpp ${SRC_SIMLULATION_FILES})

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS locale)


target_link_libraries(${PROJECT_NAME} PRIVATE glad::glad)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog_header_only)
target_include_directories(${PROJECT_NAME} PRIVATE ${Stb_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
target_link_libraries(${PROJECT_NAME} PRIVATE Eigen3::Eigen)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::boost Boost::locale)

target_link_libraries(${PROJECT_NAME}_console PRIVATE spdlog::spdlog_header_only)
target_link_libraries(${PROJECT_NAME}_console PRIVATE Eigen3::Eigen)