/**
* @file input.h
 * @brief Abstract input interface and keycode utilities.
 *
 * Defines the @ref Input base class for polling keyboard and mouse state and
 * registering event callbacks, along with the full set of `MED_KEY_*`,
 * `MED_MOUSE_BUTTON_*`, `MED_GAMEPAD_*`, and related input code constants.
 *
 * Implement @ref Input for a specific windowing backend (e.g. GLFW) by
 * subclassing and providing the pure virtual methods. Use @ref keycodeToChar
 * to convert raw keycodes and modifier state into printable characters.
 * @author Nico Russo
 * @date   2026-04-14
 */

#ifndef MEDIUM_INPUT_H
#define MEDIUM_INPUT_H

#include <map>
#include <medium.h>

/** @defgroup input_codes Input Codes
 *  @brief Numeric constants for keyboard, mouse, gamepad, and joystick input.
 *
 *  Values mirror GLFW's defines and are safe to compare directly against
 *  GLFW callback parameters.
 *  @{
 */

/** @defgroup mouse_buttons Mouse Buttons
 *  @ingroup input_codes
 *  @{
 */
#define MED_MOUSE_BUTTON_1      0 ///< Mouse button 1.
#define MED_MOUSE_BUTTON_2      1 ///< Mouse button 2.
#define MED_MOUSE_BUTTON_3      2 ///< Mouse button 3.
#define MED_MOUSE_BUTTON_4      3 ///< Mouse button 4.
#define MED_MOUSE_BUTTON_5      4 ///< Mouse button 5.
#define MED_MOUSE_BUTTON_6      5 ///< Mouse button 6.
#define MED_MOUSE_BUTTON_7      6 ///< Mouse button 7.
#define MED_MOUSE_BUTTON_8      7 ///< Mouse button 8.
#define MED_MOUSE_BUTTON_LAST   MED_MOUSE_BUTTON_8 ///< Highest numbered mouse button.
#define MED_MOUSE_BUTTON_LEFT   MED_MOUSE_BUTTON_1 ///< Primary (left) mouse button.
#define MED_MOUSE_BUTTON_RIGHT  MED_MOUSE_BUTTON_2 ///< Secondary (right) mouse button.
#define MED_MOUSE_BUTTON_MIDDLE MED_MOUSE_BUTTON_3 ///< Middle mouse button.
#define MED_MOUSE_SCROLL        8                  ///< Synthetic code for scroll wheel events.
/** @} */

/** @defgroup keyboard_keys Keyboard Keys
 *  @ingroup input_codes
 *  @{
 */

/** @defgroup printable_keys Printable Keys
 *  @ingroup keyboard_keys
 *  @{
 */
#define MED_KEY_SPACE         32  ///< Space bar.
#define MED_KEY_APOSTROPHE    39  ///< `'`
#define MED_KEY_COMMA         44  ///< `,`
#define MED_KEY_MINUS         45  ///< `-`
#define MED_KEY_PERIOD        46  ///< `.`
#define MED_KEY_SLASH         47  ///< `/`
#define MED_KEY_0             48  ///< `0`
#define MED_KEY_1             49  ///< `1`
#define MED_KEY_2             50  ///< `2`
#define MED_KEY_3             51  ///< `3`
#define MED_KEY_4             52  ///< `4`
#define MED_KEY_5             53  ///< `5`
#define MED_KEY_6             54  ///< `6`
#define MED_KEY_7             55  ///< `7`
#define MED_KEY_8             56  ///< `8`
#define MED_KEY_9             57  ///< `9`
#define MED_KEY_SEMICOLON     59  ///< `;`
#define MED_KEY_EQUAL         61  ///< `=`
#define MED_KEY_A             65  ///< `A`
#define MED_KEY_B             66  ///< `B`
#define MED_KEY_C             67  ///< `C`
#define MED_KEY_D             68  ///< `D`
#define MED_KEY_E             69  ///< `E`
#define MED_KEY_F             70  ///< `F`
#define MED_KEY_G             71  ///< `G`
#define MED_KEY_H             72  ///< `H`
#define MED_KEY_I             73  ///< `I`
#define MED_KEY_J             74  ///< `J`
#define MED_KEY_K             75  ///< `K`
#define MED_KEY_L             76  ///< `L`
#define MED_KEY_M             77  ///< `M`
#define MED_KEY_N             78  ///< `N`
#define MED_KEY_O             79  ///< `O`
#define MED_KEY_P             80  ///< `P`
#define MED_KEY_Q             81  ///< `Q`
#define MED_KEY_R             82  ///< `R`
#define MED_KEY_S             83  ///< `S`
#define MED_KEY_T             84  ///< `T`
#define MED_KEY_U             85  ///< `U`
#define MED_KEY_V             86  ///< `V`
#define MED_KEY_W             87  ///< `W`
#define MED_KEY_X             88  ///< `X`
#define MED_KEY_Y             89  ///< `Y`
#define MED_KEY_Z             90  ///< `Z`
#define MED_KEY_LEFT_BRACKET  91  ///< `[`
#define MED_KEY_BACKSLASH     92  ///< `\`
#define MED_KEY_RIGHT_BRACKET 93  ///< `]`
#define MED_KEY_GRAVE_ACCENT  96  ///< `` ` ``
#define MED_KEY_WORLD_1       161 ///< Non-US key 1 (locale-dependent).
#define MED_KEY_WORLD_2       162 ///< Non-US key 2 (locale-dependent).
/** @} */

/** @defgroup function_keys Function & Control Keys
 *  @ingroup keyboard_keys
 *  @{
 */
#define MED_KEY_ESCAPE        256 ///< Escape.
#define MED_KEY_ENTER         257 ///< Enter / Return.
#define MED_KEY_TAB           258 ///< Tab.
#define MED_KEY_BACKSPACE     259 ///< Backspace.
#define MED_KEY_INSERT        260 ///< Insert.
#define MED_KEY_DELETE        261 ///< Delete.
#define MED_KEY_RIGHT         262 ///< Right arrow.
#define MED_KEY_LEFT          263 ///< Left arrow.
#define MED_KEY_DOWN          264 ///< Down arrow.
#define MED_KEY_UP            265 ///< Up arrow.
#define MED_KEY_PAGE_UP       266 ///< Page Up.
#define MED_KEY_PAGE_DOWN     267 ///< Page Down.
#define MED_KEY_HOME          268 ///< Home.
#define MED_KEY_END           269 ///< End.
#define MED_KEY_CAPS_LOCK     280 ///< Caps Lock.
#define MED_KEY_SCROLL_LOCK   281 ///< Scroll Lock.
#define MED_KEY_NUM_LOCK      282 ///< Num Lock.
#define MED_KEY_PRINT_SCREEN  283 ///< Print Screen.
#define MED_KEY_PAUSE         284 ///< Pause / Break.
#define MED_KEY_F1            290 ///< F1.
#define MED_KEY_F2            291 ///< F2.
#define MED_KEY_F3            292 ///< F3.
#define MED_KEY_F4            293 ///< F4.
#define MED_KEY_F5            294 ///< F5.
#define MED_KEY_F6            295 ///< F6.
#define MED_KEY_F7            296 ///< F7.
#define MED_KEY_F8            297 ///< F8.
#define MED_KEY_F9            298 ///< F9.
#define MED_KEY_F10           299 ///< F10.
#define MED_KEY_F11           300 ///< F11.
#define MED_KEY_F12           301 ///< F12.
#define MED_KEY_F13           302 ///< F13.
#define MED_KEY_F14           303 ///< F14.
#define MED_KEY_F15           304 ///< F15.
#define MED_KEY_F16           305 ///< F16.
#define MED_KEY_F17           306 ///< F17.
#define MED_KEY_F18           307 ///< F18.
#define MED_KEY_F19           308 ///< F19.
#define MED_KEY_F20           309 ///< F20.
#define MED_KEY_F21           310 ///< F21.
#define MED_KEY_F22           311 ///< F22.
#define MED_KEY_F23           312 ///< F23.
#define MED_KEY_F24           313 ///< F24.
#define MED_KEY_F25           314 ///< F25.
/** @} */

/** @defgroup keypad_keys Keypad Keys
 *  @ingroup keyboard_keys
 *  @{
 */
#define MED_KEY_KP_0          320 ///< Keypad 0.
#define MED_KEY_KP_1          321 ///< Keypad 1.
#define MED_KEY_KP_2          322 ///< Keypad 2.
#define MED_KEY_KP_3          323 ///< Keypad 3.
#define MED_KEY_KP_4          324 ///< Keypad 4.
#define MED_KEY_KP_5          325 ///< Keypad 5.
#define MED_KEY_KP_6          326 ///< Keypad 6.
#define MED_KEY_KP_7          327 ///< Keypad 7.
#define MED_KEY_KP_8          328 ///< Keypad 8.
#define MED_KEY_KP_9          329 ///< Keypad 9.
#define MED_KEY_KP_DECIMAL    330 ///< Keypad decimal point.
#define MED_KEY_KP_DIVIDE     331 ///< Keypad `/`.
#define MED_KEY_KP_MULTIPLY   332 ///< Keypad `*`.
#define MED_KEY_KP_SUBTRACT   333 ///< Keypad `-`.
#define MED_KEY_KP_ADD        334 ///< Keypad `+`.
#define MED_KEY_KP_ENTER      335 ///< Keypad Enter.
#define MED_KEY_KP_EQUAL      336 ///< Keypad `=`.
/** @} */

/** @defgroup modifier_keys Modifier Keys
 *  @ingroup keyboard_keys
 *  @{
 */
#define MED_KEY_LEFT_SHIFT    340 ///< Left Shift.
#define MED_KEY_LEFT_CONTROL  341 ///< Left Control.
#define MED_KEY_LEFT_ALT      342 ///< Left Alt.
#define MED_KEY_LEFT_SUPER    343 ///< Left Super (Windows / Command key).
#define MED_KEY_RIGHT_SHIFT   344 ///< Right Shift.
#define MED_KEY_RIGHT_CONTROL 345 ///< Right Control.
#define MED_KEY_RIGHT_ALT     346 ///< Right Alt.
#define MED_KEY_RIGHT_SUPER   347 ///< Right Super (Windows / Command key).
#define MED_KEY_MENU          348 ///< Menu key.
/** @} */

/** @defgroup modifier_flags Modifier Flag Bits
 *  @ingroup input_codes
 *  @brief Bitmask flags reported alongside key/mouse events.
 *
 *  Combine with bitwise OR to test for multiple held modifiers.
 *  @{
 */
#define MED_MOD_SHIFT     0x0001 ///< Shift is held.
#define MED_MOD_CONTROL   0x0002 ///< Control is held.
#define MED_MOD_ALT       0x0004 ///< Alt is held.
#define MED_MOD_SUPER     0x0008 ///< Super (Windows / Command) is held.
#define MED_MOD_CAPS_LOCK 0x0010 ///< Caps Lock is active.
#define MED_MOD_NUM_LOCK  0x0020 ///< Num Lock is active.
/** @} */

/** @defgroup joystick Joystick & Gamepad
 *  @ingroup input_codes
 *  @{
 */

/** @defgroup joystick_ids Joystick IDs
 *  @ingroup joystick
 *  @{
 */
#define MED_JOYSTICK_1    0  ///< Joystick 1.
#define MED_JOYSTICK_2    1  ///< Joystick 2.
#define MED_JOYSTICK_3    2  ///< Joystick 3.
#define MED_JOYSTICK_4    3  ///< Joystick 4.
#define MED_JOYSTICK_5    4  ///< Joystick 5.
#define MED_JOYSTICK_6    5  ///< Joystick 6.
#define MED_JOYSTICK_7    6  ///< Joystick 7.
#define MED_JOYSTICK_8    7  ///< Joystick 8.
#define MED_JOYSTICK_9    8  ///< Joystick 9.
#define MED_JOYSTICK_10   9  ///< Joystick 10.
#define MED_JOYSTICK_11   10 ///< Joystick 11.
#define MED_JOYSTICK_12   11 ///< Joystick 12.
#define MED_JOYSTICK_13   12 ///< Joystick 13.
#define MED_JOYSTICK_14   13 ///< Joystick 14.
#define MED_JOYSTICK_15   14 ///< Joystick 15.
#define MED_JOYSTICK_16   15 ///< Joystick 16.
#define MED_JOYSTICK_LAST MED_JOYSTICK_16 ///< Highest joystick ID.
/** @} */

/** @defgroup gamepad_buttons Gamepad Buttons
 *  @ingroup joystick
 *  @{
 */
#define MED_GAMEPAD_BUTTON_A            0  ///< A button (Xbox) / Cross (PlayStation).
#define MED_GAMEPAD_BUTTON_B            1  ///< B button (Xbox) / Circle (PlayStation).
#define MED_GAMEPAD_BUTTON_X            2  ///< X button (Xbox) / Square (PlayStation).
#define MED_GAMEPAD_BUTTON_Y            3  ///< Y button (Xbox) / Triangle (PlayStation).
#define MED_GAMEPAD_BUTTON_LEFT_BUMPER  4  ///< Left bumper (LB / L1).
#define MED_GAMEPAD_BUTTON_RIGHT_BUMPER 5  ///< Right bumper (RB / R1).
#define MED_GAMEPAD_BUTTON_BACK         6  ///< Back / Select button.
#define MED_GAMEPAD_BUTTON_START        7  ///< Start button.
#define MED_GAMEPAD_BUTTON_GUIDE        8  ///< Guide / Home button.
#define MED_GAMEPAD_BUTTON_LEFT_THUMB   9  ///< Left thumbstick click (L3).
#define MED_GAMEPAD_BUTTON_RIGHT_THUMB  10 ///< Right thumbstick click (R3).
#define MED_GAMEPAD_BUTTON_DPAD_UP      11 ///< D-pad up.
#define MED_GAMEPAD_BUTTON_DPAD_RIGHT   12 ///< D-pad right.
#define MED_GAMEPAD_BUTTON_DPAD_DOWN    13 ///< D-pad down.
#define MED_GAMEPAD_BUTTON_DPAD_LEFT    14 ///< D-pad left.
#define MED_GAMEPAD_BUTTON_LAST         MED_GAMEPAD_BUTTON_DPAD_LEFT ///< Highest gamepad button ID.

// PlayStation aliases
#define MED_GAMEPAD_BUTTON_CROSS    MED_GAMEPAD_BUTTON_A ///< PlayStation alias for @ref MED_GAMEPAD_BUTTON_A.
#define MED_GAMEPAD_BUTTON_CIRCLE   MED_GAMEPAD_BUTTON_B ///< PlayStation alias for @ref MED_GAMEPAD_BUTTON_B.
#define MED_GAMEPAD_BUTTON_SQUARE   MED_GAMEPAD_BUTTON_X ///< PlayStation alias for @ref MED_GAMEPAD_BUTTON_X.
#define MED_GAMEPAD_BUTTON_TRIANGLE MED_GAMEPAD_BUTTON_Y ///< PlayStation alias for @ref MED_GAMEPAD_BUTTON_Y.
/** @} */

/** @defgroup gamepad_axes Gamepad Axes
 *  @ingroup joystick
 *  @brief Axis indices reported in the range [-1.0, 1.0] (triggers: [0.0, 1.0]).
 *  @{
 */
#define MED_GAMEPAD_AXIS_LEFT_X        0 ///< Left thumbstick, horizontal axis.
#define MED_GAMEPAD_AXIS_LEFT_Y        1 ///< Left thumbstick, vertical axis.
#define MED_GAMEPAD_AXIS_RIGHT_X       2 ///< Right thumbstick, horizontal axis.
#define MED_GAMEPAD_AXIS_RIGHT_Y       3 ///< Right thumbstick, vertical axis.
#define MED_GAMEPAD_AXIS_LEFT_TRIGGER  4 ///< Left trigger (LT / L2).
#define MED_GAMEPAD_AXIS_RIGHT_TRIGGER 5 ///< Right trigger (RT / R2).
#define MED_GAMEPAD_AXIS_LAST          MED_GAMEPAD_AXIS_RIGHT_TRIGGER ///< Highest axis index.
/** @} */

/** @} */ // end group joystick

/** @defgroup event_types Event Types & Hat States
 *  @ingroup input_codes
 *  @{
 */
#define MED_RELEASE 0 ///< Key or button was released.
#define MED_PRESS   1 ///< Key or button was pressed.
#define MED_REPEAT  2 ///< Key is being held and the OS is repeating it.

/** @defgroup hat_states Joystick Hat States
 *  @ingroup event_types
 *  @brief Bitmask values for joystick hat (D-pad) position.
 *
 *  Diagonal states are bitwise OR combinations of the cardinal directions.
 *  @{
 */
#define MED_HAT_CENTERED   0                           ///< Hat is centred (no direction).
#define MED_HAT_UP         1                           ///< Hat is up.
#define MED_HAT_RIGHT      2                           ///< Hat is right.
#define MED_HAT_DOWN       4                           ///< Hat is down.
#define MED_HAT_LEFT       8                           ///< Hat is left.
#define MED_HAT_RIGHT_UP   (MED_HAT_RIGHT | MED_HAT_UP)   ///< Hat is up-right.
#define MED_HAT_RIGHT_DOWN (MED_HAT_RIGHT | MED_HAT_DOWN) ///< Hat is down-right.
#define MED_HAT_LEFT_UP    (MED_HAT_LEFT  | MED_HAT_UP)   ///< Hat is up-left.
#define MED_HAT_LEFT_DOWN  (MED_HAT_LEFT  | MED_HAT_DOWN) ///< Hat is down-left.
/** @} */

/** @} */ // end group event_types
/** @} */ // end group input_codes
/**
 * @brief Abstract input interface for keyboard, mouse, and event handling.
 *
 * Subclass this and implement the pure virtual methods for a specific
 * windowing backend (e.g. GLFW). Event callbacks are registered per
 * (object, type) pair via @ref registerEventCallback, or globally via
 * @ref registerGlobalCallback.
 */
class Input {
public:
    Medium* medium = nullptr; ///< Owning window/context; set by @ref initializeInput.

    /** @brief Callback for a specific (object, type) input event. */
    using EventCallback = std::function<void()>;

    /**
     * @brief Callback for any input event with full context.
     *
     * Parameters: `(object, action, mods, x, y)` where `x`/`y` are cursor
     * coordinates and `mods` is a @ref modifier_flags bitmask.
     */
    using GlobalEventCallback = std::function<void(int, int, int, double, double)>;

    /**
     * @brief Binds this input handler to a window context.
     * @param medium_  The @ref Medium this handler will poll.
     */
    virtual void initializeInput(Medium* medium_) {
        medium = medium_;
    }

    virtual ~Input() = default;

    /**
     * @brief Returns whether the key identified by @p keycode is currently held.
     * @param keycode  A `MED_KEY_*` constant.
     * @return `true` if the key is down.
     */
    virtual bool isKeyPressed(int keycode) = 0;

    /**
     * @brief Returns whether the mouse button identified by @p button is currently held.
     * @param button  A `MED_MOUSE_BUTTON_*` constant.
     * @return `true` if the button is down.
     */
    virtual bool isMouseButtonPressed(int button) = 0;

    /**
     * @brief Returns the current cursor position in window coordinates.
     * @return `{x, y}` in pixels relative to the top-left of the window.
     */
    virtual std::pair<float, float> getMousePosition() = 0;

    /**
     * @brief Returns the horizontal cursor position in window coordinates.
     * @return X coordinate in pixels.
     */
    virtual float getMouseX() = 0;

    /**
     * @brief Returns the vertical cursor position in window coordinates.
     * @return Y coordinate in pixels.
     */
    virtual float getMouseY() = 0;

    /**
     * @brief Registers a callback for a specific (object, type) input event.
     *
     * Only one callback is stored per `(eventObject, eventType)` pair;
     * registering again overwrites the previous callback.
     *
     * @param eventObject  The input object (e.g. a `MED_KEY_*` or `MED_MOUSE_BUTTON_*` code).
     * @param eventType    The event type (`MED_PRESS`, `MED_RELEASE`, or `MED_REPEAT`).
     * @param callback     Zero-argument callable invoked when the event fires.
     */
    void registerEventCallback(int eventObject, int eventType, const EventCallback& callback) {
        eventCallbacks[{eventObject, eventType}] = callback;
    }

    /**
     * @brief Registers a catch-all callback invoked for every input event.
     *
     * Replaces any previously registered global callback. The callback
     * receives `(object, action, mods, x, y)`.
     *
     * @param callback  The global event handler.
     */
    void registerGlobalCallback(const GlobalEventCallback& callback) {
        globalCallback = callback;
    }

protected:
    /**
     * @brief Dispatches a registered per-event callback if one exists.
     *
     * Looks up `(eventObject, eventType)` in @ref eventCallbacks and
     * invokes the stored callable. Does nothing if no callback is registered.
     *
     * @param eventObject  Input object code.
     * @param eventType    Event type code.
     */
    virtual void eventCallbackImpl(int eventObject, int eventType) {
        auto it = eventCallbacks.find({eventObject, eventType});
        if (it != eventCallbacks.end()) {
            it->second();
        }
    }

    /**
     * @brief Invokes the global callback if one is registered.
     * @param object  Input object code.
     * @param action  Event type (`MED_PRESS`, `MED_RELEASE`, `MED_REPEAT`).
     * @param mods    Active modifier bitmask (see @ref modifier_flags).
     * @param x       Cursor X position at the time of the event.
     * @param y       Cursor Y position at the time of the event.
     */
    virtual void globalCallbackImpl(int object, int action, int mods, double x, double y) {
        if (globalCallback)
            globalCallback(object, action, mods, x, y);
    }

    GlobalEventCallback globalCallback; ///< Optional catch-all event handler.
    std::map<std::pair<int, int>, EventCallback> eventCallbacks; ///< Per-(object, type) callbacks.
};

/**
 * @brief Converts a `MED_KEY_*` keycode and modifier state to a printable character.
 *
 * Handles the full US-QWERTY layout including:
 * - Letters (Shift XOR Caps Lock = uppercase)
 * - Number row with shifted symbols (`!`, `@`, `#`, …)
 * - Punctuation with shifted variants
 * - Keypad keys (Num Lock assumed active)
 *
 * @param keycode  A `MED_KEY_*` constant for the pressed key.
 * @param mods     Active modifier bitmask (see @ref modifier_flags);
 *                 only @ref MED_MOD_SHIFT and @ref MED_MOD_CAPS_LOCK are examined.
 * @return         The printable `char` the key produces, or `'\0'` for
 *                 non-printable keys (arrows, F-keys, etc.).
 */
inline char keycodeToChar(const int keycode, const int mods)
{
    const bool shift = (mods & MED_MOD_SHIFT) != 0;
    const bool caps  = (mods & MED_MOD_CAPS_LOCK) != 0;

    // Letters: Shift XOR Caps Lock = uppercase
    if (keycode >= MED_KEY_A && keycode <= MED_KEY_Z)
    {
        bool upper = shift ^ caps;
        return static_cast<char>((upper ? 'A' : 'a') + (keycode - MED_KEY_A));
    }

    // Number row and punctuation
    switch (keycode)
    {
        case MED_KEY_0: return shift ? ')' : '0';
        case MED_KEY_1: return shift ? '!' : '1';
        case MED_KEY_2: return shift ? '@' : '2';
        case MED_KEY_3: return shift ? '#' : '3';
        case MED_KEY_4: return shift ? '$' : '4';
        case MED_KEY_5: return shift ? '%' : '5';
        case MED_KEY_6: return shift ? '^' : '6';
        case MED_KEY_7: return shift ? '&' : '7';
        case MED_KEY_8: return shift ? '*' : '8';
        case MED_KEY_9: return shift ? '(' : '9';

        case MED_KEY_SPACE:         return ' ';
        case MED_KEY_APOSTROPHE:    return shift ? '"'  : '\'';
        case MED_KEY_COMMA:         return shift ? '<'  : ',';
        case MED_KEY_MINUS:         return shift ? '_'  : '-';
        case MED_KEY_PERIOD:        return shift ? '>'  : '.';
        case MED_KEY_SLASH:         return shift ? '?'  : '/';
        case MED_KEY_SEMICOLON:     return shift ? ':'  : ';';
        case MED_KEY_EQUAL:         return shift ? '+'  : '=';
        case MED_KEY_LEFT_BRACKET:  return shift ? '{'  : '[';
        case MED_KEY_BACKSLASH:     return shift ? '|'  : '\\';
        case MED_KEY_RIGHT_BRACKET: return shift ? '}'  : ']';
        case MED_KEY_GRAVE_ACCENT:  return shift ? '~'  : '`';

        // Keypad (Num Lock assumed on)
        case MED_KEY_KP_0:        return '0';
        case MED_KEY_KP_1:        return '1';
        case MED_KEY_KP_2:        return '2';
        case MED_KEY_KP_3:        return '3';
        case MED_KEY_KP_4:        return '4';
        case MED_KEY_KP_5:        return '5';
        case MED_KEY_KP_6:        return '6';
        case MED_KEY_KP_7:        return '7';
        case MED_KEY_KP_8:        return '8';
        case MED_KEY_KP_9:        return '9';
        case MED_KEY_KP_DECIMAL:  return '.';
        case MED_KEY_KP_DIVIDE:   return '/';
        case MED_KEY_KP_MULTIPLY: return '*';
        case MED_KEY_KP_SUBTRACT: return '-';
        case MED_KEY_KP_ADD:      return '+';
        case MED_KEY_KP_EQUAL:    return '=';

        default: return '\0'; ///< Non-printable key (arrows, F-keys, modifiers, etc.).
    }
}

#endif //MEDIUM_INPUT_H
