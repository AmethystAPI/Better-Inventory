cmake_minimum_required(VERSION 3.12)
project(BetterInventory CXX ASM_NASM)
include(FetchContent)

# Mod Options
set(MOD_VERSION "1.6.1")

# Amethyst Minecraft Folder
set(AmethystFolder "$ENV{localappdata}/Packages/Microsoft.MinecraftUWP_8wekyb3d8bbwe/LocalState/games/com.mojang/amethyst/")

# Configure to build within the Amethyst Folder
configure_file(mod.json.in "${AmethystFolder}/mods/${PROJECT_NAME}@${MOD_VERSION}/mod.json" @ONLY)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${AmethystFolder}/mods/${PROJECT_NAME}@${MOD_VERSION}") 

# Compiler Options
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CONFIGURATION_TYPES "RelWithDebInfo" CACHE STRING "Build configurations" FORCE)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /await")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: RelWithDebInfo" FORCE)

# Make the Mod
file(GLOB_RECURSE ${PROJECT_NAME}_All
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.asm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
) 

add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_All})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    AmethystAPI 
    libhat
    $ENV{amethyst_src}/AmethystAPI/lib/fmt.lib
)

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:/MP>
)

# EnTT Config Options
target_compile_definitions(${PROJECT_NAME} PUBLIC ENTT_PACKED_PAGE=128)

# Get the AmethystAPI Lib.
add_subdirectory("$ENV{amethyst_src}/AmethystAPI" "build") 
