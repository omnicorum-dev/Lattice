cmake_minimum_required(VERSION 4.2)
project(Lattice)

set(CMAKE_CXX_STANDARD 17)

# COMPILE GLAD
add_library(glad STATIC vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC vendor/glad/include)

# COMPILE GLFW
add_subdirectory(vendor/glfw)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG_MODE)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DRELEASE_MODE)
endif()

# MAIN EXECUTABLE
add_executable(LatticeTesting testing/MINIMAL_TEST/minimal_test.cpp testing/MINIMAL_TEST/App.cpp)

# ALL INCLUDES USED
target_include_directories(LatticeTesting PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/other
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/base-layer
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw/include
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/graphite
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/Medium
)

# LINK THE LIBS FOR GLFW AND GLAD
target_link_libraries(LatticeTesting PRIVATE glfw glad)

# === Platform-specific system linking ===
if(APPLE)
    target_link_libraries(LatticeTesting PRIVATE
            "-framework Cocoa"
            "-framework OpenGL"
            "-framework IOKit"
            "-framework CoreVideo"
            "-framework QuartzCore"
    )
elseif(UNIX)
    target_link_libraries(LatticeTesting PRIVATE
            GL X11 Xrandr Xi pthread dl
    )
elseif(WIN32)
    target_link_libraries(LatticeTesting PRIVATE
            opengl32 gdi32 user32
    )
endif()

# COPY ASSETS FOLDER TO BUILD LOCATION
set(ASSET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)
add_custom_target(copy_lattice_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${ASSET_DIR}
        $<TARGET_FILE_DIR:LatticeTesting>/assets
)





if(FALSE)
add_subdirectory(vendor/Medium)

add_executable(LatticeTesting testing/MINIMAL_TEST/minimal_test.cpp testing/MINIMAL_TEST/App.cpp)
#add_executable(LatticeTesting testing/main.cpp)
target_include_directories(LatticeTesting PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(LatticeTesting PRIVATE Medium)

set(ASSET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)
add_custom_target(copy_lattice_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${ASSET_DIR}
        $<TARGET_FILE_DIR:LatticeTesting>/assets
)

add_dependencies(LatticeTesting copy_lattice_assets)
endif()