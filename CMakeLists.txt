#title
cmake_minimum_required(VERSION 3.10.0)
project(SDLshooter VERSION 0.1.0 LANGUAGES C CXX)

#set c++ std
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

#add compile options
if(MSVC)
add_compile_options(/W4)
else()
add_compile_options(-g -Wall -Wextra)
endif()

#set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})

#set name
set(TARGET ${PROJECT_NAME}-${CMAKE_SYSTEM_NAME})

#preset
find_package(SDL2 REQUIRED)
find_Package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)


add_executable(${TARGET} "src/main.cpp" "src/game.cpp" "src/sceneMain")


target_link_libraries(${TARGET} ${SDL2_LIBRARIES}
    SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer SDL2_ttf::SDL2_ttf)

