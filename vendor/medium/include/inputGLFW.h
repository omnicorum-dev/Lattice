/**
 * @file input_glfw.h
 * @brief GLFW backend implementation of the @ref Input interface.
 *
 * Provides @ref InputGLFW, a concrete @ref Input subclass that wires GLFW
 * callbacks into the engine's event and global callback system. Instantiate
 * and pass to your @ref Medium after the GLFW window is created.
 */

//
// Created by Nico Russo on 4/18/26.
//

#ifndef MEDIUM_INPUTGLFW_H
#define MEDIUM_INPUTGLFW_H

#include <input.h>
#include <GLFW/glfw3.h>

/**
 * @brief GLFW-backed input handler.
 *
 * Installs GLFW callbacks on the native window obtained from @ref Medium and
 * forwards them into the @ref Input callback system. Key and mouse button
 * events fire both the per-(object, type) callback (via @ref eventCallbackImpl)
 * and the global callback (via @ref globalCallbackImpl). Scroll events fire
 * only the global callback with object code @ref MED_MOUSE_SCROLL.
 *
 * @note Only one @ref InputGLFW should be active per `GLFWwindow` at a time,
 *       since it overwrites the window's user pointer.
 */
class InputGLFW : public Input {
public:

    /**
     * @brief Binds this handler to @p medium_ and installs GLFW callbacks.
     *
     * Sets the GLFW window user pointer to `this`, then registers
     * key, mouse button, and scroll callbacks. Must be called after the
     * GLFW window has been created.
     *
     * @param medium_  The owning @ref Medium whose native window is used.
     */
    void initializeInput(Medium* medium_) override {
        medium = medium_;
        auto* window = static_cast<GLFWwindow*>(medium->getNativeWindow());
        glfwSetWindowUserPointer(window, this);
        glfwSetKeyCallback(window, glfwKeyCallback);
        glfwSetMouseButtonCallback(window, glfwMouseButtonCallback);
        glfwSetScrollCallback(window, glfwScrollCallback);
    }

    /**
     * @brief Returns whether a keyboard key is currently pressed or repeating.
     * @param keycode  A `MED_KEY_*` constant.
     * @return `true` if the key state is `MED_PRESS` or `MED_REPEAT`.
     */
    bool isKeyPressed(int keycode) override {
        auto* window = static_cast<GLFWwindow*>(medium->getNativeWindow());
        const int state = glfwGetKey(window, keycode);
        return state == MED_PRESS || state == MED_REPEAT;
    }

    /**
     * @brief Returns whether a mouse button is currently pressed.
     * @param button  A `MED_MOUSE_BUTTON_*` constant.
     * @return `true` if the button state is `MED_PRESS`.
     */
    bool isMouseButtonPressed(int button) override {
        auto* window = static_cast<GLFWwindow*>(medium->getNativeWindow());
        const int state = glfwGetMouseButton(window, button);
        return state == MED_PRESS;
    }

    /**
     * @brief Returns the current cursor position in window coordinates.
     * @return `{x, y}` in pixels relative to the top-left of the window.
     */
    std::pair<float, float> getMousePosition() override {
        auto* window = static_cast<GLFWwindow*>(medium->getNativeWindow());
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        return { x, y };
    }

    /** @brief Returns the horizontal cursor position in window coordinates. */
    float getMouseX() override { return getMousePosition().first; }

    /** @brief Returns the vertical cursor position in window coordinates. */
    float getMouseY() override { return getMousePosition().second; }

private:
    /**
     * @brief GLFW scroll callback — forwards scroll deltas as a global event.
     *
     * Fires @ref globalCallbackImpl with object `MED_MOUSE_SCROLL`, action `0`,
     * mods `0`, and `x`/`y` set to the horizontal and vertical scroll offsets.
     *
     * @param window    The originating GLFW window.
     * @param x_offset  Horizontal scroll delta.
     * @param y_offset  Vertical scroll delta.
     */
    static void glfwScrollCallback(GLFWwindow* window, const double x_offset, const double y_offset) {
        auto* self = static_cast<InputGLFW*>(glfwGetWindowUserPointer(window));
        self->globalCallbackImpl(MED_MOUSE_SCROLL, 0, 0, x_offset, y_offset);
    }

    /**
     * @brief GLFW key callback — dispatches per-event and global callbacks.
     *
     * Fires @ref eventCallbackImpl with `(key, action)`, then
     * @ref globalCallbackImpl with `(key, action, mods, 0, 0)`.
     *
     * @param window    The originating GLFW window.
     * @param key       `MED_KEY_*` keycode.
     * @param scancode  Platform-specific scancode (not forwarded).
     * @param action    `MED_PRESS`, `MED_RELEASE`, or `MED_REPEAT`.
     * @param mods      Active modifier bitmask (see @ref modifier_flags).
     */
    static void glfwKeyCallback(GLFWwindow* window, const int key, int scancode, const int action, const int mods) {
        auto* self = static_cast<InputGLFW*>(glfwGetWindowUserPointer(window));
        self->eventCallbackImpl(key, action);
        self->globalCallbackImpl(key, action, mods, 0, 0);
    }

    /**
     * @brief GLFW mouse button callback — dispatches per-event and global callbacks.
     *
     * Fires @ref eventCallbackImpl with `(button, action)`, then
     * @ref globalCallbackImpl with `(button, action, mods, 0, 0)`.
     *
     * @param window  The originating GLFW window.
     * @param button  `MED_MOUSE_BUTTON_*` code.
     * @param action  `MED_PRESS` or `MED_RELEASE`.
     * @param mods    Active modifier bitmask (see @ref modifier_flags).
     */
    static void glfwMouseButtonCallback(GLFWwindow* window, const int button, const int action, const int mods) {
        auto* self = static_cast<InputGLFW*>(glfwGetWindowUserPointer(window));
        self->eventCallbackImpl(button, action);
        self->globalCallbackImpl(button, action, mods, 0, 0);
    }
};

#endif //MEDIUM_INPUTGLFW_H