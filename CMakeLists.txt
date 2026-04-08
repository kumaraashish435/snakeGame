cmake_minimum_required(VERSION 3.20)
project(SnakeGame VERSION 1.0.0 LANGUAGES CXX)

# ── C++20

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Sources 

set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Snake.cpp
    src/Food.cpp
    src/Renderer.cpp
    src/Audio.cpp
)

set(HEADERS
    src/Config.h
    src/Types.h
    src/Game.h
    src/Snake.h
    src/Food.h
    src/Renderer.h
    src/Audio.h
)

# ── SDL3 

find_package(SDL3 QUIET)

if(NOT SDL3_FOUND)
    message(STATUS "SDL3 not found on system – fetching via FetchContent")
    include(FetchContent)
    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG        main            
        
        GIT_SHALLOW    TRUE
        EXCLUDE_FROM_ALL
    )
    set(SDL_SHARED  OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC  ON  CACHE BOOL "" FORCE)
    set(SDL_TEST    OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(SDL3)
endif()

# ── Target ─────────────────────────────────────────────────────────────────
if(ANDROID)
    add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})
else()
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)



if(TARGET SDL3::SDL3main)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3main)
endif()

if(TARGET SDL3::SDL3-static)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3-static)
elseif(TARGET SDL3::SDL3)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
endif()

# ── Compiler warnings 

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ──  WebAssembly 
if(EMSCRIPTEN)
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
    target_link_options(${PROJECT_NAME} PRIVATE
        -sWASM=1
        -sALLOW_MEMORY_GROWTH=1
        -sSTACK_SIZE=1048576
        --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/web/shell.html
    )
endif()

# ── Windows
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE $<CONFIG:Release>
    )
endif()

# ── Install rules 
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
