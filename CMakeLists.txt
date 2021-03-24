cmake_minimum_required (VERSION 3.8)

project ("opengl_starter")

add_subdirectory ("opengl_starter")

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/opengl_starter)