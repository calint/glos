# dependencies (in ubuntu installation):
# * libglm-dev/mantic,mantic,now 0.9.9.8+ds-7 all: opengl math
# * libtbb-dev/mantic 2021.9.0-2ubuntu1 i386: for parallel unsequenced `for_each`
# * libsdl2-dev/mantic,now 2.28.3+dfsg-2 amd64
# * libsdl2-gfx-dev/mantic,now 1.0.4+dfsg-5 amd64
# * libsdl2-image-dev/mantic,now 2.6.3+dfsg-2 amd64
# * opengl related packages

cmake_minimum_required(VERSION 3.7)
project(glos)
set(CMAKE_CXX_STANDARD 20)
set(OpenGL_GL_PREFERENCE GLVND)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(TBB REQUIRED)
add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion)
add_compile_options(-Wno-changes-meaning -Wno-unused-variable -Wno-unused-function -Wno-unused-parameter)
add_executable(glos src/main.cpp src/application/application.cpp)
target_link_libraries(glos SDL2::SDL2 SDL2_image SDL2_ttf OpenGL::OpenGL glm::glm TBB::tbb)
