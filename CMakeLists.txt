cmake_minimum_required(VERSION 3.26.0)
project(sBlend)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if(MSVC AND CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(/O2 /fp:fast /arch:AVX2)
endif()

# Set directories
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib")

# Find required packages
find_package(OpenGL REQUIRED)

# Add library directories
link_directories(${LIB_DIR})

# Define source files
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
    src/appMain.cpp
        include/sblend/graphics/Reflection.cpp
        include/sblend/graphics/Skybox.cpp
)

# Create the executable target FIRST
add_executable(sBlend ${SOURCE_FILES})


# Set output directory for the executable
set_target_properties(sBlend PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/output"
)

target_include_directories(sBlend PRIVATE 
${INCLUDE_DIR}
)

# Now you can link libraries to the target
if(WIN32)
    target_link_libraries(sBlend
        ${OPENGL_LIBRARIES}
        glfw3
        assimp-vc143-mtd
    )
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(CORE_VIDEO_LIBRARY CoreVideo)
    target_link_libraries(sBlend
        ${OPENGL_LIBRARIES}
        glfw
        ${COCOA_LIBRARY}
        ${IOKIT_LIBRARY}
        ${CORE_VIDEO_LIBRARY}

        # Note: BULLET_LIBRARIES wasn't defined anywhere
    )
else()
    target_link_libraries(sBlend
        ${OPENGL_LIBRARIES}
        glfw
        dl
    )
endif()

# If you want to use <> includes for your headers, you can also do:
# target_include_directories(sBlend SYSTEM PRIVATE ${INCLUDE_DIR})