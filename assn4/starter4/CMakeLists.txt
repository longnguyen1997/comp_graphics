cmake_minimum_required(VERSION 2.8)

project(a4)

# Silence warnings about deprecated GLUT functions
if(APPLE)
    add_definitions("-Wno-deprecated-declarations")
endif()

if(UNIX)
    # Enable C++11
    add_definitions("-std=c++11")
elseif(MSVC)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -WX")
  add_definitions("-D_CRT_SECURE_NO_WARNINGS")
endif()

# vecmath include directory
include_directories(vecmath/include)
add_subdirectory(vecmath)
set (SRC_DIR "src/")

set(CPP_FILES
    ${SRC_DIR}main.cpp
    ${SRC_DIR}stb.cpp
    ${SRC_DIR}ArgParser.cpp
    ${SRC_DIR}Camera.cpp
    ${SRC_DIR}CubeMap.cpp
    ${SRC_DIR}Image.cpp
    ${SRC_DIR}Light.cpp
    ${SRC_DIR}Material.cpp
    ${SRC_DIR}Mesh.cpp
    ${SRC_DIR}Object3D.cpp
    ${SRC_DIR}Octree.cpp
    ${SRC_DIR}Renderer.cpp
    ${SRC_DIR}SceneParser.cpp
    ${SRC_DIR}VecUtils.cpp
    )

set(CPP_HEADERS
    ${SRC_DIR}ArgParser.h
    ${SRC_DIR}Camera.h
    ${SRC_DIR}CubeMap.h
    ${SRC_DIR}Image.h
    ${SRC_DIR}Ray.h
    ${SRC_DIR}Light.h
    ${SRC_DIR}Material.h
    ${SRC_DIR}Mesh.h
    ${SRC_DIR}Object3D.h
    ${SRC_DIR}Octree.h
    ${SRC_DIR}Renderer.h
    ${SRC_DIR}SceneParser.h
    ${SRC_DIR}VecUtils.h
    )
set (STB_SRC
   ${SRC_DIR}stb_image.h
   ${SRC_DIR}stb_image_write.h)
SOURCE_GROUP(stb FILES ${STB_SRC})


add_executable(a4 ${CPP_FILES} ${CPP_HEADERS} ${STB_SRC})
target_link_libraries(a4 vecmath)

