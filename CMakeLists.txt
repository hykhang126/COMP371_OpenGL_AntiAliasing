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

# TODO: Add tests and install targets if needed.
  find_package(glm CONFIG REQUIRED)
  target_link_libraries(antialias PRIVATE glm::glm)
  find_package(GLEW REQUIRED)
  target_link_libraries(antialias PRIVATE GLEW::GLEW)
  find_package(glfw3 CONFIG REQUIRED)
  target_link_libraries(antialias PRIVATE glfw)
  # find_package(imgui CONFIG REQUIRED)
  # target_link_libraries(antialias PRIVATE imgui::imgui)
  find_package(Eigen3 CONFIG REQUIRED)
  target_link_libraries(antialias PRIVATE Eigen3::Eigen)

  # Adds vendors, textures, shaders
	target_include_directories(antialias PRIVATE vendors)
	target_include_directories(antialias PRIVATE textures)
	target_include_directories(antialias PRIVATE shaders)

   target_sources(antialias PRIVATE
   Camera.cpp
   Camera.h
   Cloth.cpp
   Cloth.h
   IndexBuffer.cpp
   IndexBuffer.h
   Renderer.cpp
   Renderer.h
   Shader.cpp
   Shader.h
   Texture.cpp
   Texture.h
   VertexArray.cpp
   VertexArray.h
   VertexBuffer.cpp
   VertexBuffer.h
   VertexBufferLayout.cpp
   VertexBufferLayout.h
   AntiAliasing.cpp
   AntiAliasing.h
   vendors/stb_image.cpp
   vendors/stb_image.h
   textures/basketball.png
   textures/cloth.png
   shaders/Basic.shader
)