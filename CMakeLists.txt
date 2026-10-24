cmake_minimum_required(VERSION 3.16)

project(FilesystemManager
    VERSION 0.1.0
    LANGUAGES CXX
)

add_library(filesystem_core
    src/Node.cpp
    src/File.cpp
    src/Directory.cpp
    src/Symlink.cpp
    src/FileSystemManager.cpp
)

target_compile_features(filesystem_core
    PUBLIC cxx_std_17
)

set_target_properties(filesystem_core PROPERTIES
    CXX_EXTENSIONS OFF
)

target_include_directories(filesystem_core PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

add_executable(filesystem_demo
    src/main.cpp
)

target_link_libraries(filesystem_demo PRIVATE
    filesystem_core
)

if(MSVC)
    target_compile_options(filesystem_core PRIVATE /W4)
else()
    target_compile_options(filesystem_core PRIVATE
        -Wall -Wextra -Wpedantic
    )
endif()
