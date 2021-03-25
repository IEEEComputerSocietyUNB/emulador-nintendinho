cmake_minimum_required(VERSION 3.16.3)

project(emulador-nintendinho)

# Seta flags
set(CMAKE_C_STANDARD)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_C_FLAGS "-Wall -pedantic -W")
    set(CMAKE_C_FLAGS_DEBUG "-g")
endif()

# Inclui arquivos e dependencias
find_package(SDL2 REQUIRED)
include_directories(${SDL_INCLUDE_DIRS})

include_directories(src)
include_directories(include)
file(GLOB SRC "src/*.c")
file(GLOB INC "include/*.h")

# Compila
add_executable(nintendinho ${SRC} ${INC})
target_link_libraries(nintendinho ${SDL2_LIBRARIES})