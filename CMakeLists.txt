# dependencies (in ubuntu 24.04 installation):
# * clang++ version 18.1.3
# * libglm-dev/noble,noble,now 0.9.9.8+ds-7
# * libtbb-dev/noble,now 2021.11.0-2ubuntu2
# * libsdl2-dev/noble,now 2.30.0+dfsg-1build3
# * libsdl2-gfx-dev/noble,now 1.0.4+dfsg-5build1
# * libsdl2-image-dev/noble,now 2.8.2+dfsg-1build2
# * libsdl2-ttf-dev/noble,now 2.22.0+dfsg-1
# * opengl related packages

cmake_minimum_required(VERSION 3.7)
project(glos)
set(CMAKE_CXX_STANDARD 23)
set(OpenGL_GL_PREFERENCE GLVND)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(TBB REQUIRED)
add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion)
add_compile_options(-Wno-changes-meaning -Wno-unused-variable -Wno-unused-function -Wno-unused-parameter)
add_executable(glos src/main.cpp)
target_link_libraries(glos SDL2::SDL2 SDL2_image SDL2_ttf OpenGL::OpenGL glm::glm TBB::tbb)
