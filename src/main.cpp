#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <cstdlib>
#include <filesystem>

#include "core/application.h"
#include "utils/logger.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    AllocConsole();
    FILE* dummy;
    freopen_s(&dummy, "CONOUT$", "w", stdout);
    freopen_s(&dummy, "CONOUT$", "w", stderr);

    Logger::instance().set_log_file("wallpaper_engine_clone.log");
    Logger::info("===== WallpaperEngineClone =====");

    {
        std::error_code ec;
        auto cwd = std::filesystem::current_path(ec);
        if (!ec) {
            Logger::info("Working directory: " + cwd.string());
        }
        bool has_assets = std::filesystem::exists("assets/config/settings.json");
        Logger::info(std::string("assets/config/settings.json: ") +
                     (has_assets ? "FOUND" : "NOT FOUND"));
        if (!has_assets) {
            Logger::warn("Run the exe from the build/Release/ directory!");
        }
    }

    Application app(hInstance);
    if (!app.Init()) {
        Logger::error("Application initialization failed");
        MessageBoxA(nullptr,
            "Failed to initialize WallpaperEngineClone.\n\n"
            "Check the console window for details.\n"
            "Common issues:\n"
            "  - Missing VLC installation\n"
            "  - Windows version incompatible with wallpaper attachment\n"
            "  - Missing assets/config files",
            "WallpaperEngineClone - Error",
            MB_OK | MB_ICONERROR);
        return 1;
    }

    {
        auto& cfg = app.GetConfig();
        Logger::info("Config:");
        Logger::info("  Video:         " + cfg.video_path());
        Logger::info("  Volume:        " + std::to_string(cfg.volume()));
        Logger::info("  Mute:          " + std::string(cfg.mute() ? "yes" : "no"));
        Logger::info("  Pause on FS:   " + std::string(cfg.pause_on_fullscreen() ? "yes" : "no"));
        Logger::info("  Pause hidden:  " + std::string(cfg.pause_when_hidden() ? "yes" : "no"));
        Logger::info("  FPS limit:     " + std::to_string(cfg.fps_limit()));
    }

    Logger::info("Wallpaper running — right-click tray icon to configure");

    int exit_code = app.Run();

    Logger::info("Application exited with code " + std::to_string(exit_code));
    return exit_code;
}
