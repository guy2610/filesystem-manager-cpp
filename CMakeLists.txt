
cmake_minimum_required(VERSION 3.16)

project(FilesystemManager
    VERSION 0.1.0
    LANGUAGES CXX
)

add_executable(filesystem_demo
    src/main.cpp
)

target_compile_features(filesystem_demo
    PRIVATE cxx_std_17
)

set_target_properties(filesystem_demo PROPERTIES
    CXX_EXTENSIONS OFF
)

target_include_directories(filesystem_demo PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(filesystem_demo PRIVATE
        /W4
    )
else()
    target_compile_options(filesystem_demo PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()
