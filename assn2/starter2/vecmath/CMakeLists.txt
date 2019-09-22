set(LIB_NAME vecmath)

set(CPP_FILES
    Matrix2f.cpp
    Matrix3f.cpp
    Matrix4f.cpp
    Quat4f.cpp
    Vector2f.cpp
    Vector3f.cpp
    Vector4f.cpp
    )

set(CPP_HEADER_DIR include)

set(CPP_HEADERS
    ${CPP_HEADER_DIR}/Matrix2f.h
    ${CPP_HEADER_DIR}/Matrix3f.h
    ${CPP_HEADER_DIR}/Matrix4f.h
    ${CPP_HEADER_DIR}/Quat4f.h
    ${CPP_HEADER_DIR}/Vector2f.h
    ${CPP_HEADER_DIR}/Vector3f.h
    ${CPP_HEADER_DIR}/Vector4f.h
    ${CPP_HEADER_DIR}/vecmath.h
    )

add_library(${LIB_NAME} STATIC ${CPP_FILES} ${CPP_HEADERS})
