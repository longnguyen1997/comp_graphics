cmake_minimum_required(VERSION 2.8)

project(a2)

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
  add_definitions(/D "_CRT_SECURE_NO_WARNINGS")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -WX")
endif()

set (A2_LIBS ${OPENGL_gl_LIBRARY})

# GLFW
set(GLFW_INSTALL OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(3rd_party/glfw)
list(APPEND A2_LIBS glfw)
list(APPEND A2_INCLUDES 3rd_party/glfw/include)

# GLEW - not needed on OS X
# we add glew source/header directly to the build, no glew library build.
if (NOT APPLE)
  add_definitions(-DGLEW_STATIC)
  list(APPEND A2_INCLUDES 3rd_party/glew/include)
  list(APPEND A2_SRC 3rd_party/glew/src/glew.c)
  SOURCE_GROUP(GLEW FILES 3rd_party/glew/src/glew.c)
endif()

# NanoGui
set(NANOGUI_BUILD_EXAMPLE OFF)
set(NANOGUI_BUILD_SHARED OFF)
set(NANOGUI_BUILD_PYTHON OFF)
set(NANOGUI_USE_GLAD ${NANOGUI_USE_GLAD_DEFAULT})
#set(NANOGUI_USE_GLAD OFF)
set(NANOGUI_INSTALL OFF)

add_subdirectory(3rd_party/nanogui)
list(APPEND A2_LIBS nanogui)
list(APPEND A2_INCLUDES 3rd_party/nanogui/include 3rd_party/nanogui/ext/eigen 3rd_party/nanogui/ext/nanovg/src)

# lodepng
list (APPEND A2_INCLUDES 3rd_party/lodepng)
list (APPEND A2_SRC 3rd_party/lodepng/lodepng.cpp)
list (APPEND A2_HEADER 3rd_party/lodepng/lodepng.h)

source_group(lodepng FILES 
  3rd_party/lodepng/lodepng.h
  3rd_party/lodepng/lodepng.cpp
)


# vecmath include directory
include_directories(vecmath/include)
add_subdirectory(vecmath)
list (APPEND A2_LIBS vecmath)
list (APPEND A2_INCLUDES vecmath/include)
list (APPEND A2_SRC
  src/main.cpp
  src/starter2_util.cpp
  src/camera.cpp
  src/vertexrecorder.cpp
  src/matrixstack.cpp
  src/joint.cpp
  src/mesh.cpp
  src/skeletalmodel.cpp
)
list (APPEND A2_HEADER
  src/gl.h
  src/starter2_util.h
  src/camera.h
  src/vertexrecorder.h
  src/tuple.h
  src/matrixstack.h
  src/joint.h
  src/mesh.h
  src/skeletalmodel.h
)

add_executable(a2 ${A2_SRC} ${A2_HEADER})
target_include_directories(a2 PUBLIC ${A2_INCLUDES})
target_link_libraries(a2 ${A2_LIBS})
