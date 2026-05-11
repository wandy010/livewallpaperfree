cmake_minimum_required(VERSION 3.15)
project(WallpaperEngineClone VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Include directories ──────────────────────────────────────────────────
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)



# ── Source files ─────────────────────────────────────────────────────────
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} WIN32 ${SOURCES})

# ── mpv (Replaces Media Foundation) ───────────────────────────────────────
# Make sure you have mpv-1.dll and mpv.lib installed
target_link_libraries(${PROJECT_NAME}
    mpv
)

# ── Librerías de Windows ─────────────────────────────────────────────────
target_link_libraries(${PROJECT_NAME}
    user32        # ventanas, mensajes
    gdi32         # GDI
    shell32       # bandeja del sistema
    comctl32      # controles comunes
    ole32         # COM
    psapi         # GetModuleBaseName (process_monitor)
)

# ══════════════════════════════════════════════════════════════════════════
# INSTALL
# ══════════════════════════════════════════════════════════════════════════
include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY assets/
    DESTINATION ${CMAKE_INSTALL_BINDIR}/assets
)

# ── CPack: generar instalador NSIS ───────────────────────────────────────
if(WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_PACKAGE_NAME "WallpaperEngineClone")
    set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Dynamic wallpaper engine clone")
    set(CPACK_NSIS_DISPLAY_NAME "WallpaperEngineClone")
    set(CPACK_NSIS_HELP_LINK "https://github.com/user/WallpaperEngineClone")
    set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/user/WallpaperEngineClone")
    set(CPACK_NSIS_CREATE_ICONS_EXTRA
        "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\WallpaperEngineClone.lnk' '$INSTDIR\\\\bin\\\\WallpaperEngineClone.exe'"
    )
endif()

include(CPack)
