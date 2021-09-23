# spdlog 1.9.2 or later
if (NOT DEFINED SPDLOG_DIR)
    message(FATAL_ERROR "SPDLOG_DIR not defined!")
endif()

add_subdirectory(${SPDLOG_DIR} "spdlog")

# raylib 3.7.0 or later
#todo make optional
# RAYLIB_DIR to be specified
if(NOT DEFINED RAYLIB_DIR)
    message(FATAL_ERROR "RAYLIB_DIR not defined!")
endif()

add_subdirectory(${RAYLIB_DIR} "raylib")

# raygui
# todo add later (and make optional)


#sokol
if (DEFINED SOKOL_DIR)

    set(sokol_header_files
            ${SOKOL_DIR}/sokol_app.h
            ${SOKOL_DIR}/sokol_gfx.h
            ${SOKOL_DIR}/sokol_glue.h)

    add_library(sokol INTERFACE)
    target_sources(sokol INTERFACE ${sokol_header_files})
    target_include_directories(sokol INTERFACE ${SOKOL_DIR}/)

endif()

