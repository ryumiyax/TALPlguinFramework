cmake_minimum_required(VERSION 3.25)

project(TaikoArcadeLoaderPlugin VERSION 1.0.0 LANGUAGES C CXX)

# Set C and C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set warning level
if(MSVC)
    add_compile_options(/W3)
    # Set the source file encoding to UTF-8
    add_compile_options(/utf-8)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Add project definitions (Add ASYNC_UPDATE/ASYNC_IO to enable new async feature [experimental])
# add_definitions(-DNOMINMAX -DLTC_NO_PROTOTYPES -D_CRT_SECURE_NO_WARNINGS -DASYNC_UPDATE)
add_definitions(-DNOMINMAX -DLTC_NO_PROTOTYPES -D_CRT_SECURE_NO_WARNINGS)

# Include FetchContent module
include(FetchContent)

# Fetch tomlc99 (no built-in CMake support)
# FetchContent_Declare(
#     tomlc99
#     GIT_REPOSITORY https://github.com/cktan/tomlc99.git
#     GIT_TAG master
# )
# FetchContent_GetProperties(tomlc99)
# if(NOT tomlc99_POPULATED)
#     FetchContent_MakeAvailable(tomlc99)
#     add_library(tomlc99 STATIC ${tomlc99_SOURCE_DIR}/toml.c)
#     target_include_directories(tomlc99 PUBLIC ${tomlc99_SOURCE_DIR})
# endif()

# Source files
set(SOURCES
    src/dllmain.cpp
)

# Create the library
add_library(plugin SHARED ${SOURCES})

# Remove 'lib' prefix from the library name
set_target_properties(plugin PROPERTIES PREFIX "")

# Include directories
target_include_directories(plugin PRIVATE
    src
    # ${tomlc99_SOURCE_DIR}
)

# Compiler definitions
target_compile_definitions(plugin PRIVATE
    _WIN32_WINNT=_WIN32_WINNT_WIN10
)

# Add link options
if(NOT MSVC)
    target_link_options(plugin PRIVATE -Wl,--allow-multiple-definition)
endif()

# Link libraries
target_link_libraries(plugin PRIVATE
    # tomlc99
    ws2_32
    ntdll
)

# Define log path; used to make the file path relative in the log calls.
# Last character (-) to remove the trailing slash in the log path
add_compile_definitions("SOURCE_ROOT=${CMAKE_CURRENT_SOURCE_DIR}/src-")

# Set runtime library to static
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Define the .def file
set(DEF_FILE "${CMAKE_CURRENT_SOURCE_DIR}/exports.def")

if (MSVC)
    # Add the .def file to the target properties
    set_target_properties(plugin PROPERTIES LINK_FLAGS "/DEF:${DEF_FILE}")
endif()

add_custom_command(
    TARGET plugin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/plugin.dll"
        "${CMAKE_SOURCE_DIR}/out/plugin.dll"
    COMMENT "Copying plugin.dll to out directory"
)

# Set a default target
add_custom_target(default ALL DEPENDS plugin)