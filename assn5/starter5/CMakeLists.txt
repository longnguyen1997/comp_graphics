cmake_minimum_required(VERSION 2.8)

project(a5)

find_package(OpenGL REQUIRED)

if (APPLE)
  set(CMAKE_MACOSX_RPATH 1)
endif()

if (UNIX)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=gnu++11")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-variable")
  # recommended but not set by default
  # set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
elseif(MSVC)
  # recommended but not set by default
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -WX")
endif()

set (A5_LIBS ${OPENGL_gl_LIBRARY})

# GLFW
set(GLFW_INSTALL OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL " " FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL " " FORCE)
add_subdirectory(3rd_party/glfw)
list(APPEND A5_LIBS glfw)
list(APPEND A5_INCLUDES 3rd_party/glfw/include)

# GLEW - not needed on OS X
# we add glew source/header directly to the build, no glew library build.
if (NOT APPLE)
  add_definitions(-DGLEW_STATIC)
  list(APPEND A5_INCLUDES 3rd_party/glew/include)
  list(APPEND A5_SRC 3rd_party/glew/src/glew.c)
  SOURCE_GROUP(GLEW FILES 3rd_party/glew/src/glew.c)
endif()

# lodepng
list (APPEND A5_INCLUDES 3rd_party/lodepng)
list (APPEND A5_SRC 3rd_party/lodepng/lodepng.cpp)
list (APPEND A5_HEADER 3rd_party/lodepng/lodepng.h)
source_group(lodepng FILES 3rd_party/lodepng/lodepng.h 3rd_party/lodepng/lodepng.cpp)

# vecmath include directory
include_directories(vecmath/include)
add_subdirectory(vecmath)
list (APPEND A5_LIBS vecmath)
list (APPEND A5_INCLUDES vecmath/include)

# shaders
list (APPEND SHADERFILES
  shaders/vertexshader.glsl
  shaders/fragmentshader_color.glsl
  shaders/fragmentshader_dirlight.glsl
  shaders/diffuse_nolight.glsl
)
source_group(shaders FILES ${SHADERFILES})

# src
list (APPEND A5_SRC
  src/main.cpp
  src/starter5_util.cpp
  src/camera.cpp
  src/vertexrecorder.cpp
  src/objparser.cpp
  src/stb.cpp
  src/renderer.cpp
)
list (APPEND A5_HEADER
  src/main.h
  src/gl.h
  src/starter5_util.h
  src/camera.h
  src/vertexrecorder.h
  src/objparser.h
  src/stb_image.h
  src/renderer.h
)

add_executable(a5 ${A5_SRC} ${A5_HEADER} ${SHADERFILES})
target_include_directories(a5 PUBLIC ${A5_INCLUDES})
target_link_libraries(a5 ${A5_LIBS})
