cmake_minimum_required(VERSION 2.8)

project(a0)

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
  # set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -WX")
endif()

set (A0_LIBS ${OPENGL_gl_LIBRARY})

# GLFW
set(GLFW_INSTALL OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
add_subdirectory(glfw)
list(APPEND A0_LIBS glfw)
list(APPEND A0_INCLUDES glfw/include)

# GLEW - not needed on OS X
# we add glew source/header directly to the build, no glew library build.
if (NOT APPLE)
  add_definitions(-DGLEW_STATIC)
  list(APPEND A0_INCLUDES glew/include)
  list(APPEND A0_SRC glew/src/glew.c)
  SOURCE_GROUP(GLEW FILES glew/src/glew.c)
endif()

# vecmath include directory
include_directories(vecmath/include)
add_subdirectory(vecmath)
list (APPEND A0_LIBS vecmath)
list (APPEND A0_INCLUDES vecmath/include)
list (APPEND A0_SRC
  src/main.cpp
  src/starter0_util.cpp
  src/starter0_util.h
  src/recorder.cpp
)
list (APPEND A0_HEADER
  src/recorder.h
  src/teapot.h
  src/gl.h
)

add_executable(a0 ${A0_SRC} ${A0_HEADER})
target_include_directories(a0 PUBLIC ${A0_INCLUDES})
target_link_libraries(a0 ${A0_LIBS})
