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

# --- Codul din training (10 noiembrie) ---
file(GLOB TRAINING_SOURCES CONFIGURE_DEPENDS training/*.cpp)
add_executable(training ${TRAINING_SOURCES})
target_include_directories(training PRIVATE training)
target_link_libraries(training PRIVATE SDL3::SDL3 glm::glm)

# --- Jocul (Hard Task) – se construiește doar când src/ are cod ---
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.c)
if(SOURCES)
    add_executable(shooter ${SOURCES})
    target_include_directories(shooter PRIVATE src)
    target_link_libraries(shooter PRIVATE SDL3::SDL3 glm::glm)

    # Copiază assets/ lângă executabil
    add_custom_command(TARGET shooter POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:shooter>/assets)
else()
    message(STATUS "src/ e gol: se construiește doar ținta 'training'.")
endif()

# Pe Windows, copiază SDL3.dll lângă executabile
if(WIN32)
    foreach(t training shooter)
        if(TARGET ${t})
            add_custom_command(TARGET ${t} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        $<TARGET_FILE:SDL3::SDL3> $<TARGET_FILE_DIR:${t}>)
        endif()
    endforeach()
endif()
