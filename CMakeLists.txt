cmake_minimum_required(VERSION 3.0)
project(CPSC_587_A1)

#[ OpenGL ]
find_package(OpenGL REQUIRED)

#[ GLFW ]
set(GLFW_DIR external/glfw)
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
add_subdirectory(${GLFW_DIR})

#[ glad ]
set(GLAD_DIR external/glad)
set(GLAD_SOURCES external/glad/src/glad.c)
add_library(glad ${GLAD_SOURCES})
target_include_directories(glad PRIVATE ${GLAD_DIR}/include)


#[ irrklang ]
set(IRRKLANG_DIR external/irrKlang)
FIND_LIBRARY(IRRKLANG_LIBRARY
        NAMES libIrrKlang.so
        PATHS "${IRRKLANG_DIR}/bin/linux-gcc-64/")


#[ Headers ]
set(HEADERS
    include/geometry/curve.h
    include/geometry/curvefileio.h

    include/math/vec3f.h
    include/math/mat4f.h

    include/opengl/program.h
    include/opengl/shader.h
    include/opengl/openglmatrix.h
    include/opengl/GraphicsProgram.h
    include/opengl/RenderingEngine.h
    include/opengl/Geometry.h
    include/opengl/CoasterPhysics.h

    include/scene/camera.h
    include/scene/Model.h
    )

#[ Sources ]
set(SOURCES
    src/geometry/curve.cpp
    src/geometry/curvefileio.cpp

    src/math/vec3f.cpp
    src/math/mat4f.cpp

    src/opengl/program.cpp
    src/opengl/shader.cpp
    src/opengl/openglmatrix.cpp
    src/opengl/GraphicsProgram.cpp
    src/opengl/RenderingEngine.cpp
    src/opengl/Geometry.cpp
    src/opengl/CoasterPhysics.cpp
    src/opengl/main.cpp

    src/scene/camera.cpp
    src/scene/Model.cpp
    )

#[ Resource ]
set(RESOURCE_FILES
    # CURVES
    resources/curves/rollerCoaster.obj


    # MODELS
    resources/models/coasterCar.obj
    resources/models/floor.obj
    resources/models/gate.obj


    # SOUNDS
    resources/sounds/lift.wav
    resources/sounds/roar.wav
    #reosurces/sounds/Sound_Effects.txt


    # SHADERS
    resources/shaders/phong_fs.glsl
    resources/shaders/phong_vs.glsl
    )

# [ Configure file locations ]
# track
configure_file(resources/curves/rollerCoaster.obj curves/rollerCoaster.obj COPYONLY)

# models
configure_file(resources/models/coasterCar.obj models/coasterCar.obj COPYONLY)
configure_file(resources/models/floor.obj models/floor.obj COPYONLY)
configure_file(resources/models/gate.obj models/gate.obj COPYONLY)

# sounds
configure_file(resources/sounds/lift.wav sounds/lift.wav COPYONLY)
configure_file(resources/sounds/roar.wav sounds/roar.wav COPYONLY)
#configure_file(resources/sounds/Sound_Effects.txt sounds/Sound_Effects.txt COPYONLY)

# shaders
configure_file(resources/shaders/phong_fs.glsl phong_fs.glsl COPYONLY)
configure_file(resources/shaders/phong_vs.glsl phong_vs.glsl COPYONLY)





#[ Executable ]
add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})

#[ Definitions ]
target_compile_definitions(${PROJECT_NAME}
    PRIVATE -DGLFW_INCLUDE_NONE
    )

if(MSVC)
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE -D_USE_MATH_DEFINES
        )
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    MACOSX_BUNDLE TRUE
    MACOSX_FRAMEWORK_IDENTIFIER org.cmake.${PROJECT_NAME}
    )

target_compile_definitions(${PROJECT_NAME}
    PRIVATE GLFW_INCLUDE_NONE
    )

target_link_libraries(${PROJECT_NAME}
    PRIVATE ${OPENGL_gl_LIBRARY}
    PRIVATE glfw
    PRIVATE ${GLFW_LIBRARIES}
    PRIVATE glad
    PRIVATE ${GLAD_LIBRARIES}
    PRIVATE ${IRRKLANG_LIBRARY}
    PRIVATE ${CMAKE_DL_LIBS}
    )

#include_directories(
#    ${IRRKLANG_DIR}/include
#    )

target_include_directories(${PROJECT_NAME}
    PRIVATE include
    PRIVATE include/geometry
    PRIVATE include/math
    PRIVATE include/opengl
    PRIVATE include/scene
    PRIVATE external
    PRIVATE ${GLFW_DIR}/include
    PRIVATE ${GLAD_DIR}/include
    PRIVATE ${GLM_DIR}
    PRIVATE ${IRRKLANG_DIR}/include
    )
