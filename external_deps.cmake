# spdlog 1.9.2 or later
if (NOT DEFINED SPDLOG_DIR)

    message(FATAL_ERROR "SPDLOG_DIR not defined!")

endif()

add_subdirectory(${SPDLOG_DIR} "spdlog")


# mimalloc 2.0.2 or later
if (DEFINED MIMALLOC_DIR)

    add_subdirectory(${MIMALLOC_DIR} "mimalloc")
    add_compile_definitions(USELIB_MIMALLOC)

else()

    message(STATUS "Define MIMALLOC_DIR to utilize mimalloc lib")

endif()


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


#enkiTS
if (DEFINED ENKITS_DIR)

    add_subdirectory(${ENKITS_DIR} "enkiTS")
    add_compile_definitions(USELIB_ENKITS)

else()

    message(STATUS "Define ENKITS_DIR to build experiments that utilize enkiTS threading/tasks library")

endif()