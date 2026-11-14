cmake_minimum_required(VERSION 3.24)
project(CodeRunHardTask LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# SDL3 – folosește versiunea instalată dacă există, altfel o descarcă
find_package(SDL3 QUIET CONFIG)
if(NOT SDL3_FOUND)
    FetchContent_Declare(SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG        release-3.2.24
        GIT_SHALLOW    TRUE)
    set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(SDL3)
endif()

# GLM (header-only)
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
    GIT_SHALLOW    TRUE)
FetchContent_MakeAvailable(glm)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.c)
if(NOT SOURCES)
    message(FATAL_ERROR "src/ e gol: copiază acolo fișierele din template-ul SDL3.")
endif()

add_executable(shooter ${SOURCES})
target_include_directories(shooter PRIVATE src)
target_link_libraries(shooter PRIVATE SDL3::SDL3 glm::glm)

# Copiază assets/ lângă executabil
add_custom_command(TARGET shooter POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:shooter>/assets)
