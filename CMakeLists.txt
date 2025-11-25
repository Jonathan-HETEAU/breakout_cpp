cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 23)

project(Breakout_cpp VERSION 0.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# Déclaration de Raylib
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5  # Dernière stable
)


# Déclaration de Flecs
FetchContent_Declare(
    flecs
    GIT_REPOSITORY https://github.com/SanderMertens/flecs.git
    GIT_TAG v4.1.2  # Version stable recommandée
)

FetchContent_MakeAvailable(raylib  flecs)


add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib flecs::flecs_static)

# === GESTION ASSETS ===
# 1. Copie assets/ vers build/assets/ (dev)
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

# 2. Macro ASSETS_PATH pour le code (absolue en dev, relative en release)
target_compile_definitions(${PROJECT_NAME} PRIVATE 
    ASSETS_PATH="${CMAKE_BINARY_DIR}/assets/"  # Dev: copie dans build
    $<$<CONFIG:Release>:ASSETS_PATH="./assets/">  # Release: relatif
)