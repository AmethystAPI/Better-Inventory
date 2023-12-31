cmake_minimum_required(VERSION 3.12)
project(ItemInformation)
set(MOD_VERSION "1.2.1")

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD 20)

# Define only RelWithDebInfo as the available build configuration
set(CMAKE_CONFIGURATION_TYPES "RelWithDebInfo" CACHE STRING "Build configurations" FORCE)
set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo" FORCE)

# Project Configuration
set(AmethystFolder "$ENV{localappdata}/Packages/Microsoft.MinecraftUWP_8wekyb3d8bbwe/AC/Amethyst/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${AmethystFolder}/mods/${PROJECT_NAME}@${MOD_VERSION}")

find_library(AMETHYST_API AmethystAPI PATHS "${AmethystFolder}/lib")
include_directories(${AmethystFolder}/include)

file(GLOB_RECURSE CPP_SOURCES "src/*.cpp")
file(GLOB_RECURSE H_FILES "src/*.h")
add_library(${PROJECT_NAME} SHARED ${CPP_SOURCES} ${H_FILES})

target_compile_definitions(${PROJECT_NAME} PUBLIC ENTT_PACKED_PAGE=128)

set_property(
    TARGET ${PROJECT_NAME}
    PROPERTY MSVC_RUNTIME_LIBRARY
    # use /MD in debug mode (dynamic linking) and /MT in release mode (static linking)
    $<$<CONFIG:Debug>:MultiThreadedDLL>$<$<CONFIG:Release>:MultiThreaded>
)

target_link_libraries(${PROJECT_NAME} PRIVATE "${AmethystFolder}/lib/fmt.lib")
target_link_libraries(${PROJECT_NAME} PRIVATE "${AmethystFolder}/lib/libMinHook.x64.lib")
target_link_libraries(${PROJECT_NAME} PRIVATE ${AMETHYST_API})
