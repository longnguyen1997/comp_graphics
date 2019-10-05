cmake_minimum_required(VERSION 2.8)

project(a3)

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

set (A3_LIBS ${OPENGL_gl_LIBRARY})

# GLFW
set(GLFW_INSTALL OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL " " FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL " " FORCE)
add_subdirectory(3rd_party/glfw)
list(APPEND A3_LIBS glfw)
list(APPEND A3_INCLUDES 3rd_party/glfw/include)

# GLEW - not needed on OS X
# we add glew source/header directly to the build, no glew library build.
if (NOT APPLE)
  add_definitions(-DGLEW_STATIC)
  list(APPEND A3_INCLUDES 3rd_party/glew/include)
  list(APPEND A3_SRC 3rd_party/glew/src/glew.c)
  SOURCE_GROUP(GLEW FILES 3rd_party/glew/src/glew.c)
endif()


# vecmath include directory
include_directories(vecmath/include)
add_subdirectory(vecmath)
list (APPEND A3_LIBS vecmath)
list (APPEND A3_INCLUDES vecmath/include)
list (APPEND A3_SRC
  src/main.cpp
  src/starter3_util.cpp
  src/camera.cpp
  src/vertexrecorder.cpp
  src/clothsystem.cpp
  src/timestepper.cpp
  src/particlesystem.cpp
  src/pendulumsystem.cpp
  src/simplesystem.cpp
)
list (APPEND A3_HEADER
  src/gl.h
  src/starter3_util.h
  src/camera.h
  src/vertexrecorder.h
  src/clothsystem.h
  src/timestepper.h
  src/particlesystem.h
  src/pendulumsystem.h
  src/simplesystem.h
)

add_executable(a3 ${A3_SRC} ${A3_HEADER})
target_include_directories(a3 PUBLIC ${A3_INCLUDES})
target_link_libraries(a3 ${A3_LIBS})
