# CMakeList.txt : CMake project for clothsimulation, include source and define
# project specific logic here.
cmake_minimum_required(VERSION 3.12)

project(COMP371OpenGLAA)

# Set build type for debug
set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)

# Add source to this project's executable.
add_executable (clothsimulation "main.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET clothsimulation PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
  find_package(glm CONFIG REQUIRED)
  target_link_libraries(clothsimulation PRIVATE glm::glm)
  find_package(GLEW REQUIRED)
  target_link_libraries(clothsimulation PRIVATE GLEW::GLEW)
  find_package(glfw3 CONFIG REQUIRED)
  target_link_libraries(clothsimulation PRIVATE glfw)
  # find_package(imgui CONFIG REQUIRED)
  # target_link_libraries(clothsimulation PRIVATE imgui::imgui)
  find_package(Eigen3 CONFIG REQUIRED)
  target_link_libraries(clothsimulation PRIVATE Eigen3::Eigen)

  # Adds vendors, textures, shaders
	target_include_directories(clothsimulation PRIVATE vendors)
	target_include_directories(clothsimulation PRIVATE textures)
	target_include_directories(clothsimulation PRIVATE shaders)

   target_sources(clothsimulation PRIVATE
   Camera.cpp
   Camera.h
   Cloth.cpp
   Cloth.h
   VBuffer.cpp
   VBuffer.h
   IBuffer.cpp
   IBuffer.h
   Renderer.cpp
   Renderer.h
   Shader.cpp
   Shader.h
   Texture.cpp
   Texture.h
   VArray.cpp
   VArray.h
   VBuffer.cpp
   VBuffer.h
   VBufferLayout.cpp
   VBufferLayout.h
   vendors/stb_image.cpp
   vendors/stb_image.h
   textures/basketball.png
   textures/cloth.png
   shaders/Basic.shader
)