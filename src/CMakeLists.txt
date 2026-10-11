cmake_minimum_required(VERSION 3.16)

project(ArcwellGame2D LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(ArcwellGame2D
    # System
    fixed_math.h
    g_constants.h
    typedefs.h
    pc_main.c

    # Game modules
    g_entity.c
    g_entity.h
    g_gamestate.c
    g_gamestate.h
    g_map.c
    g_map.h

    # Input and physics
    h_input.c
    h_input.h
    p_physics.c
    p_physics.h

    # Hardware abstraction layer
    i_arcwell.c
    i_arcwell.h
    i_sdl.c
    i_system.h

    # Resource loader
    l_arcloader.c
    l_arcloader.h

    # Render
    r_renderer.c
    r_renderer.h
)

target_compile_definitions(ArcwellGame2D PRIVATE
    PM_PC
    SDL2_X64
)

include(GNUInstallDirs)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
target_include_directories(ArcwellGame2D PRIVATE ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})
target_link_libraries(ArcwellGame2D PRIVATE SDL2::SDL2 SDL2_image::SDL2_image)

install(TARGETS ArcwellGame2D
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
