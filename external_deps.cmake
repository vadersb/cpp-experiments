# spdlog 1.9.2 or later
if (NOT DEFINED SPDLOG_DIR)

    message(FATAL_ERROR "SPDLOG_DIR not defined!")

endif()

add_subdirectory(${SPDLOG_DIR} "spdlog")



# raylib 3.7.0 or later
# RAYLIB_DIR to be specified
if(DEFINED RAYLIB_DIR)

    add_subdirectory(${RAYLIB_DIR} "raylib")

else()

    message(STATUS "Define RAYLIB_DIR to build experiments that utilize Raylib")

endif()



#sokol
if (DEFINED SOKOL_DIR)

    set(sokol_header_files
            ${SOKOL_DIR}/sokol_app.h
            ${SOKOL_DIR}/sokol_gfx.h
            ${SOKOL_DIR}/sokol_glue.h)

    add_library(sokol INTERFACE)
    target_sources(sokol INTERFACE ${sokol_header_files})
    target_include_directories(sokol INTERFACE ${SOKOL_DIR}/)

else()

    message(STATUS "Define SOKOL_DIR to build experiments that utilize sokol lib")

endif()

