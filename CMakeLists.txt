cmake_minimum_required(VERSION 3.12)
project(Better-Inventory CXX ASM_NASM)
set(MOD_VERSION "1.6.1")
set(MOD_AUTHOR "FrederoxDev")

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /await")

# Amethyst Minecraft Folder
if (CI_CD_BUILD)
        configure_file(mod.json.in "${CMAKE_SOURCE_DIR}/dist/${PROJECT_NAME}@${MOD_VERSION}/mod.json" @ONLY)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_SOURCE_DIR}/dist/${PROJECT_NAME}@${MOD_VERSION}")
        set(AMETHYST_SRC "${CMAKE_SOURCE_DIR}/amethyst")
else()
        set(AmethystFolder "$ENV{localappdata}/Packages/Microsoft.MinecraftUWP_8wekyb3d8bbwe/LocalState/games/com.mojang/amethyst/") 
        configure_file(mod.json.in "${AmethystFolder}/mods/${PROJECT_NAME}@${MOD_VERSION}/mod.json" @ONLY)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${AmethystFolder}/mods/${PROJECT_NAME}@${MOD_VERSION}")
        set(AMETHYST_SRC "$ENV{amethyst_src}/amethyst")
endif()

# Define only RelWithDebInfo as the available build configuration
set(CMAKE_CXX_STANDARD 23)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CONFIGURATION_TYPES "RelWithDebInfo" CACHE STRING "Build configurations" FORCE)
set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: RelWithDebInfo" FORCE)

# Project Files
file(GLOB_RECURSE ${PROJECT_NAME}_All
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.asm
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_All})

# Pass mod options to the source code
target_compile_definitions(${PROJECT_NAME} PRIVATE MOD_VERSION="${MOD_VERSION}")
target_compile_definitions(${PROJECT_NAME} PRIVATE MOD_AUTHOR="${MOD_AUTHOR}")
target_compile_definitions(${PROJECT_NAME} PRIVATE MOD_NAME="${PROJECT_NAME}")

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
        AmethystAPI
        libhat
        ${AMETHYST_SRC}/AmethystAPI/lib/fmt.lib
)

# Enable multi processor compilation for C++, to make it go brrrrrr
target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:/MP>
)

# EnTT Config Options
target_compile_definitions(${PROJECT_NAME} PUBLIC ENTT_PACKED_PAGE=128)

# Get the AmethystAPI Lib
add_subdirectory("${AMETHYST_SRC}/AmethystAPI" "build")
