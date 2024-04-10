# CMakeList.txt : CMake project for antialias, include source and define
# project specific logic here.
cmake_minimum_required(VERSION 3.12)

project(COMP371OpenGLAA)

# Set build type for debug
set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)

# Add source to this project's executable.
add_executable (antialias "main.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET antialias PROPERTY CXX_STANDARD 20)
endif()

find_package(OpenGL REQUIRED)

add_definitions(
    -DGLEW_STATIC
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

# for windows
include_directories(C:/glfw-3.3.9.bin.WIN64/include/)
include_directories(C:/glew-2.1.0/include/)
include_directories(C:/glew-2.1.0/include/GL/)
include_directories(C:/GLM_0.9.9.8/g-truc-glm-bf71a83/)

link_directories(C:/glew-2.1.0/lib/Release/x64/)
link_directories(C:/glfw-3.3.9.bin.WIN64/lib-vc2019/)

set(GLEW_LIBRARY_PATH "C:/glew-2.1.0/lib/Release/x64/glew32s.lib")
set(GLFW_LIBRARY_PATH "C:/glfw-3.3.9.bin.WIN64/lib-vc2019/glfw3.lib")

target_link_libraries(antialias ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARY_PATH})
target_link_libraries(antialias ${OPENGL_gl_LIBRARY} ${GLFW_LIBRARY_PATH})

# Adds vendors, textures, shaders
target_include_directories(antialias PRIVATE vendors)
target_include_directories(antialias PRIVATE textures)
target_include_directories(antialias PRIVATE shaders)

target_sources(antialias PRIVATE
src/Camera.cpp
src/Camera.h
src/IndexBuffer.cpp
src/IndexBuffer.h
src/Renderer.cpp
src/Renderer.h
src/Shader.cpp
src/Shader.h
src/Texture.cpp
src/Texture.h
src/VertexArray.cpp
src/VertexArray.h
src/VertexBuffer.cpp
src/VertexBuffer.h
src/VertexBufferLayout.cpp
src/VertexBufferLayout.h
src/AntiAliasing.cpp
src/AntiAliasing.h
vendors/stb_image.cpp
vendors/stb_image.h
textures/basketball.png
shaders/Basic.shader
)