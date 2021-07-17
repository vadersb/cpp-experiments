# raylib 3.7.0 or later
# RAYLIB_DIR to be specified
if(NOT DEFINED RAYLIB_DIR)
    message(FATAL_ERROR "RAYLIB_DIR not defined!")
endif()

add_subdirectory(${RAYLIB_DIR} "raylib")

# raygui
# todo add later
