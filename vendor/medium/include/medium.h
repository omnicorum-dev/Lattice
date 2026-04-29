/**
 * @file medium.h
 * @brief Platform abstraction layer for windowing, rendering, and the main loop.
 *
 * Defines the @ref Medium base class, which represents a native window and
 * its associated rendering surface. Subclass Medium for each target platform
 * (e.g. GLFW/OpenGL, Metal, SDL) and implement the pure virtual methods.
 * Optional virtual methods log an error and no-op by default, so platforms
 * can adopt features incrementally.
 */

//
// Created by Nico Russo on 4/15/26.
//

#ifndef MEDIUM_H
#define MEDIUM_H

#include <base.h>
#include <../graphite/include/graphite.h>

#include <utility>

using namespace omni::basic;

/**
 * @brief Abstract platform window and rendering context.
 *
 * Medium owns the native window lifetime and drives the main loop via
 * @ref mediumRun. Concrete subclasses provide platform-specific
 * implementations of startup, shutdown, canvas rendering, cursor control,
 * and file-system root paths.
 *
 * Typical usage:
 * @code
 * auto med = std::make_unique<MediumGLFW>(1280, 720, 320, 180, "My Game");
 * med->mediumStartup();
 * med->mediumRun([](f32 dt) {
 *     // game update
 * });
 * med->mediumShutdown();
 * @endcode
 */
class Medium {
public:
    u32 SCREEN_WIDTH  = 0; ///< Native window width in pixels.
    u32 SCREEN_HEIGHT = 0; ///< Native window height in pixels.
    u32 GAME_WIDTH    = 0; ///< Internal render target width in pixels.
    u32 GAME_HEIGHT   = 0; ///< Internal render target height in pixels.

    float deltaTime = 0.0f; ///< Seconds elapsed since the previous frame; updated each tick.

    std::string windowName = "Graphite"; ///< Title shown in the OS window chrome.

    /**
     * @brief Constructs a Medium with the given dimensions and window title.
     * @param consoleWidth   Native window width in pixels.
     * @param consoleHeight  Native window height in pixels.
     * @param gameWidth      Internal render target width in pixels.
     * @param gameHeight     Internal render target height in pixels.
     * @param windowName_    Window title string (default: `"Graphite"`).
     */
    Medium(const u32 consoleWidth,
           const u32 consoleHeight,
           const u32 gameWidth,
           const u32 gameHeight,
           std::string windowName_ = "Graphite")
        : SCREEN_WIDTH(consoleWidth), SCREEN_HEIGHT(consoleHeight),
          GAME_WIDTH(gameWidth), GAME_HEIGHT(gameHeight),
          windowName(std::move(windowName_)) {}

    virtual ~Medium() = default;

    /**
     * @brief Updates the OS window title.
     *
     * The base implementation stores the name and logs an error; override
     * to apply it to the native window.
     *
     * @param _windowName  New title string.
     */
    virtual void setWindowName(const std::string& _windowName) {
        windowName = _windowName;
        LOG_ERROR("setWindowName() not implemented for current platform");
    }

    /**
     * @brief Initialises the native window and rendering context.
     * @return `0` on success, non-zero on failure.
     */
    virtual u32 mediumStartup() = 0;

    /**
     * @brief Enters the main loop, calling @p gameUpdate each frame.
     *
     * Blocks until the window is closed or @ref mediumShutdown is requested.
     * @ref deltaTime is updated before each call to @p gameUpdate.
     *
     * @param gameUpdate  Per-frame callback receiving the delta time in seconds.
     */
    virtual void mediumRun(std::function<void(f32)> gameUpdate) = 0;

    /**
     * @brief Tears down the rendering context and destroys the native window.
     * @return `0` on success, non-zero on failure.
     */
    virtual u32 mediumShutdown() = 0;

    /**
     * @brief Returns the platform-specific root directory for read-only assets.
     *
     * Typically the application bundle's `Resources/` directory on macOS or
     * the executable directory on other platforms.
     */
    virtual std::filesystem::path getAssetRoot() = 0;

    /**
     * @brief Returns the platform-specific root directory for writable save data.
     *
     * Typically `~/Library/Application Support/<app>` on macOS or
     * `%APPDATA%/<app>` on Windows.
     */
    virtual std::filesystem::path getSaveRoot() = 0;

    /**
     * @brief Blits a @ref Graphite::Canvas to the screen via an optional shader.
     *
     * The base implementation logs an error and does nothing; override to
     * perform the actual draw call.
     *
     * @param canvas        The canvas to display.
     * @param customShader  OpenGL program object to use, or `0` for the default.
     * @param x             Destination X offset in pixels (default: `0`).
     * @param y             Destination Y offset in pixels (default: `0`).
     * @param w             Destination width in pixels, or `-1` to fill (default: `-1`).
     * @param h             Destination height in pixels, or `-1` to fill (default: `-1`).
     */
    virtual void renderCanvas(const Graphite::Canvas& canvas,
                              u32 customShader = 0, int x = 0, int y = 0, int w = -1, int h = -1) {
        LOG_ERROR("renderCanvas() not implemented for current platform");
    }

    /**
     * @brief Replaces the full-screen blit shader.
     *
     * The base implementation logs an error and does nothing.
     *
     * @param shader  OpenGL program object to use as the screen shader.
     */
    virtual void setScreenShader(u32 shader) {
        LOG_ERROR("setScreenShader() not implemented for current platform");
    }

    /**
     * @brief Hides the OS cursor while it is over the window.
     *
     * The base implementation logs an error and does nothing.
     */
    virtual void cursorHide() {
        LOG_ERROR("cursorHide() not implemented for current platform");
    }

    /**
     * @brief Restores the OS cursor to its normal visible state.
     *
     * The base implementation logs an error and does nothing.
     */
    virtual void cursorShow() {
        LOG_ERROR("cursorShow() not implemented for current platform");
    }

    /**
     * @brief Captures the cursor, confining it to the window and hiding it.
     *
     * Useful for first-person camera control. The base implementation logs
     * an error and does nothing.
     */
    virtual void cursorCapture() {
        LOG_ERROR("cursorCapture() not implemented for current platform");
    }

    /**
     * @brief Returns a pointer to the underlying native window object.
     *
     * The concrete type depends on the platform (e.g. `GLFWwindow*` for the
     * GLFW backend). Cast at the call site.
     *
     * @return Opaque `void*` to the native window; never null after @ref mediumStartup.
     */
    [[nodiscard]] virtual void* getNativeWindow() const = 0;

private:
    /**
     * @brief Swaps the front and back buffers to display the current frame.
     *
     * Called internally at the end of each iteration of @ref mediumRun.
     * The base implementation logs an error and does nothing.
     */
    virtual void present() {
        LOG_ERROR("present() not implemented for current platform");
    }

    /**
     * @brief Uploads a destination rectangle to a shader program.
     *
     * Sets the uniforms that control where @ref renderCanvas draws on screen.
     * The base implementation logs an error and does nothing.
     *
     * @param program  OpenGL program object.
     * @param x        Destination X offset in pixels.
     * @param y        Destination Y offset in pixels.
     * @param w        Destination width in pixels.
     * @param h        Destination height in pixels.
     */
    virtual void applyRect(u32 program, int x, int y, int w, int h) {
        LOG_ERROR("applyRect() not implemented for current platform");
    }
};

#endif //MEDIUM_H
