cmake_minimum_required(VERSION 2.8)

project(a1)

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

set (A1_LIBS ${OPENGL_gl_LIBRARY})

# GLFW
set(GLFW_INSTALL OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
add_subdirectory(glfw)
list(APPEND A1_LIBS glfw)
list(APPEND A1_INCLUDES glfw/include)

# GLEW - not needed on OS X
# we add glew source/header directly to the build, no glew library build.
if (NOT APPLE)
  add_definitions(-DGLEW_STATIC)
  list(APPEND A1_INCLUDES glew/include)
  list(APPEND A1_SRC glew/src/glew.c)
  SOURCE_GROUP(GLEW FILES glew/src/glew.c)
endif()

# vecmath include directory
include_directories(vecmath/include)
add_subdirectory(vecmath)
list (APPEND A1_LIBS vecmath)
list (APPEND A1_INCLUDES vecmath/include)
list (APPEND A1_SRC
  src/main.cpp
  src/camera.cpp
  src/curve.cpp
  src/parse.cpp
  src/starter1_util.cpp
  src/surf.cpp
  src/vertexrecorder.cpp
)
list (APPEND A1_HEADER
  src/camera.h
  src/curve.h
  src/gl.h
  src/parse.h
  src/starter1_util.h
  src/vertexrecorder.h
  src/surf.h
  src/tuple.h
  src/gl.h
)

add_executable(a1 ${A1_SRC} ${A1_HEADER})
target_include_directories(a1 PUBLIC ${A1_INCLUDES})
target_link_libraries(a1 ${A1_LIBS})
