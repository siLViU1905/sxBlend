cmake_minimum_required(VERSION 3.26.0)
project(sBlend)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if (MSVC AND CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(/O2 /fp:fast /arch:AVX2)
endif ()


set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib")



find_package(OpenGL REQUIRED)

link_directories(${LIB_DIR})

set(SOURCE_FILES
        include/GL/glad.cpp
        include/sblend/Application.cpp
        include/sblend/graphics/Shader.cpp
        include/imgui/imgui.cpp
        include/imgui/imgui_draw.cpp
        include/imgui/imgui_widgets.cpp
        include/imgui/imgui_tables.cpp
        include/imgui/imgui_impl_glfw.cpp
        include/imgui/imgui_impl_opengl3.cpp
        include/sblend/Menu.cpp
        include/sblend/graphics/Mesh.cpp
        include/sblend/MeshManager.cpp
        include/sblend/ShaderManager.cpp
        include/sblend/LightManager.cpp
        include/sblend/Camera.cpp
        include/sblend/graphics/Light.cpp
        include/sblend/graphics/Shadow.cpp
        include/sblend/physics/PhysicsWorld.cpp
        include/sblend/graphics/Model.cpp
        include/sblend/graphics/Reflection.cpp
        include/sblend/graphics/Skybox.cpp
        include/sblend/MeshBoolean.cpp
        include/sblend/Terrain.cpp
        src/appMain.cpp
)


add_executable(sBlend ${SOURCE_FILES})

set_target_properties(sBlend PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/output"
)

target_include_directories(sBlend PRIVATE
        ${INCLUDE_DIR}
)

if (WIN32)
    target_link_libraries(sBlend
            ${OPENGL_LIBRARIES}
            glfw3
            assimp-vc143-mtd
            manifold
    )
elseif (APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(CORE_VIDEO_LIBRARY CoreVideo)
    target_link_libraries(sBlend
            ${OPENGL_LIBRARIES}
            glfw
            ${COCOA_LIBRARY}
            ${IOKIT_LIBRARY}
            ${CORE_VIDEO_LIBRARY}
    )
else ()
    target_link_libraries(sBlend
            ${OPENGL_LIBRARIES}
            glfw
            dl
    )
endif ()

# If you want to use <> includes for your headers, you can also do:
# target_include_directories(sBlend SYSTEM PRIVATE ${INCLUDE_DIR})