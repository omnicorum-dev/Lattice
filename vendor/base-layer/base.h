//
// Created by Nico Russo on 4/12/26.
//

#ifndef BASE_H
#define BASE_H

#include <complex>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

/** @brief Path to the active log output file. */
inline std::filesystem::path logPath;

/** @defgroup log_levels Log Level Constants
 *  @brief Severity levels for log filtering, from most to least critical.
 *  @{
 */
#define LOG_LEVEL_FATAL 0 ///< Unrecoverable error; program must terminate.
#define LOG_LEVEL_ERROR 1 ///< Recoverable error; operation failed.
#define LOG_LEVEL_WARN  2 ///< Unexpected condition that is not yet an error.
#define LOG_LEVEL_INFO  3 ///< General informational message.
#define LOG_LEVEL_DEBUG 4 ///< Diagnostic detail useful during development.
#define LOG_LEVEL_TRACE 5 ///< Fine-grained tracing; highest verbosity.
/** @} */

/**
 * @brief A single log entry capturing a severity level and message.
 */
struct LogEvent {
    int level;           ///< Severity level (see @ref log_levels).
    std::string message; ///< Human-readable log message.

    /**
     * @brief Constructs a LogEvent.
     * @param level   Severity level (e.g. @ref LOG_LEVEL_ERROR).
     * @param message Log message; moved into the struct.
     */
    LogEvent(const int level, std::string message)
        : level(level), message(std::move(message)) {}
};

/** @brief Global in-memory log buffer. Append @ref LogEvent entries here. */
inline std::vector<LogEvent> logger;

// ========================================================

/**
 * @namespace omni::basic
 * @brief Fundamental type aliases, numeric constants, and general-purpose math utilities.
 */
namespace omni::basic {

    /** @defgroup basic_types Integer & Float Type Aliases
     *  @ingroup omni_basic
     *  @brief Fixed-width integer and floating-point aliases for brevity and clarity.
     *  @{
     */
    using i8  = int8_t;   ///< Signed  8-bit integer.
    using i16 = int16_t;  ///< Signed 16-bit integer.
    using i32 = int32_t;  ///< Signed 32-bit integer.
    using i64 = int64_t;  ///< Signed 64-bit integer.

    using u8  = uint8_t;  ///< Unsigned  8-bit integer.
    using u16 = uint16_t; ///< Unsigned 16-bit integer.
    using u32 = uint32_t; ///< Unsigned 32-bit integer.
    using u64 = uint64_t; ///< Unsigned 64-bit integer.

    using f32 = float;    ///< 32-bit IEEE 754 floating-point.
    using f64 = double;   ///< 64-bit IEEE 754 floating-point.
    /** @} */

    /** @defgroup basic_constants Numeric Constants
     *  @ingroup omni_basic
     *  @{
     */

    using namespace std::complex_literals;
    constexpr std::complex<float> I = 1if; ///< Imaginary unit \f$i\f$.

    // --- Integer limits ---
    constexpr i8  min_i8  = static_cast<i8> (0x80);             ///< Minimum value of i8.
    constexpr i16 min_i16 = static_cast<i16>(0x8000);           ///< Minimum value of i16.
    constexpr i32 min_i32 = static_cast<i32>(0x80000000);       ///< Minimum value of i32.
    constexpr i64 min_i64 = static_cast<i64>(0x8000000000000000); ///< Minimum value of i64.

    constexpr i8  max_i8  = 0x7F;               ///< Maximum value of i8.
    constexpr i16 max_i16 = 0x7FFF;             ///< Maximum value of i16.
    constexpr i32 max_i32 = 0x7FFFFFFF;         ///< Maximum value of i32.
    constexpr i64 max_i64 = 0x7FFFFFFFFFFFFFFF; ///< Maximum value of i64.

    constexpr u8  max_u8  = 0xFF;               ///< Maximum value of u8.
    constexpr u16 max_u16 = 0xFFFF;             ///< Maximum value of u16.
    constexpr u32 max_u32 = 0xFFFFFFFF;         ///< Maximum value of u32.
    constexpr u64 max_u64 = 0xFFFFFFFFFFFFFFFF; ///< Maximum value of u64.

    // --- f32 constants ---
    constexpr f32 machine_epsilon_f32 = 1.1920929e-7f;  ///< Smallest f32 \f$\varepsilon\f$ s.t. \f$1+\varepsilon \neq 1\f$.
    constexpr f32 pi_f32              = 3.14159265359f; ///< \f$\pi\f$ (f32).
    constexpr f32 tau_f32             = 6.28318530718f; ///< \f$\tau = 2\pi\f$ (f32).
    constexpr f32 e_f32               = 2.71828182846f; ///< Euler's number \f$e\f$ (f32).
    constexpr f32 gold_big_f32        = 1.61803398875f; ///< Golden ratio \f$\phi\f$ (f32).
    constexpr f32 gold_small_f32      = 0.61803398875f; ///< Reciprocal golden ratio \f$1/\phi\f$ (f32).

    // --- f64 constants ---
    constexpr f64 machine_epsilon_f64 = 2.220446e-16;   ///< Smallest f64 \f$\varepsilon\f$ s.t. \f$1+\varepsilon \neq 1\f$.
    constexpr f64 pi_f64              = 3.14159265359;  ///< \f$\pi\f$ (f64).
    constexpr f64 tau_f64             = 6.28318530718;  ///< \f$\tau = 2\pi\f$ (f64).
    constexpr f64 two_pi_f64          = 6.28318530718;  ///< Alias for @ref tau_f64.
    constexpr f64 e_f64               = 2.71828182846;  ///< Euler's number \f$e\f$ (f64).
    constexpr f64 gold_big_f64        = 1.61803398875;  ///< Golden ratio \f$\phi\f$ (f64).
    constexpr f64 gold_small_f64      = 0.61803398875;  ///< Reciprocal golden ratio \f$1/\phi\f$ (f64).
    /** @} */

    /** @defgroup basic_math Math Utilities
     *  @ingroup omni_basic
     *  @{
     */

    /**
     * @brief Linearly interpolates between two values.
     * @tparam T    Any numeric or vector type supporting `+`, `-`, and scalar `*`.
     * @param  a    Start value (returned when @p t = 0).
     * @param  b    End value   (returned when @p t = 1).
     * @param  t    Interpolation factor; clamping is the caller's responsibility.
     * @return      \f$a + (b - a) \cdot t\f$
     */
    template<typename T>
    static T lerp(const T a, const T b, const f32 t) { return a + (b - a) * t; }

    /**
     * @brief Inverse of @ref lerp — maps a value back to its interpolation factor.
     * @tparam T  Same constraints as @ref lerp.
     * @param  a  Start of the range.
     * @param  b  End of the range.
     * @param  x  Value to invert.
     * @return    \f$(x - a)/(b - a)\f$, or `0` if @p a == @p b (division guard).
     */
    template<typename T>
    static T unlerp(const T a, const T b, const f32 x) { if (a != b) { return (x - a) / (b - a); } return 0.f; }

    /**
     * @brief Evaluates a quadratic Bézier curve at parameter @p t.
     * @tparam T  Any numeric or vector type.
     * @param  p1 Start control point.
     * @param  p2 Mid control point (pulls the curve).
     * @param  p3 End control point.
     * @param  t  Curve parameter in [0, 1].
     * @return    Point on the curve at @p t.
     */
    template<typename T>
    T bezierQuadratic(T p1, T p2, T p3, const f32 t) {
        const f32 t2 = t * t;
        return p1 - 2*t*p1 + t2*p1 + 2*t*p2 - 2*t2*p2 + t2*p3;
    }

    /**
     * @brief Returns a uniformly distributed random f32 in [0, 1].
     * @note  Wraps `std::rand()`; seed with `std::srand()` before use.
     * @return Random value in \f$[0, 1]\f$.
     */
    inline f32 rand_f32() { return static_cast<f32>(std::rand()) / static_cast<f32>(RAND_MAX); }

    /**
     * @brief Returns a uniformly distributed random i32 in [@p min, @p max].
     * @param min Inclusive lower bound.
     * @param max Inclusive upper bound.
     * @return    Random integer in \f$[\text{min}, \text{max}]\f$.
     */
    inline i32 rand_i32(const i32 min, const i32 max) { return min + std::rand() % ((max - min) + 1); }

    /**
     * @brief Maps a signed 32-bit integer to a normalised float in [-1, 1].
     * @param  x  Integer to convert.
     * @return    \f$x / \text{max\_i32}\f$ as f32.
     */
    inline f32 intToFloat(const i32 x) { return static_cast<f32>(x) / static_cast<f32>(max_i32); }

    /**
     * @brief Evaluates a Catmull-Rom spline segment at parameter @p t.
     *
     * Produces a smooth curve passing through @p p1 and @p p2, using @p p0 and
     * @p p3 as the surrounding context points.
     *
     * @param p0 Point before the segment start (context).
     * @param p1 Segment start point (curve passes through this).
     * @param p2 Segment end point   (curve passes through this).
     * @param p3 Point after the segment end (context).
     * @param t  Curve parameter in [0, 1].
     * @return   Interpolated value at @p t.
     */
    static f32 catmullRomSpline(const f32 p0, const f32 p1, const f32 p2, const f32 p3, const f32 t) {
        const f32 t2 = t * t;
        const f32 t3 = t2 * t;
        return 0.5f * (
            2.0f * p1 +
            (-p0 + p2) * t +
            (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
            (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3
        );
    }
    /** @} */

}

// ========================================================
// PRINTING AND LOGGING
// ========================================================

/** @defgroup ansi_colors ANSI Color & Style Codes
 *  @brief Terminal escape sequences for colored and styled console output.
 *  @note  These have no effect when output is redirected to a file.
 *  @{
 */
#define ANSI_BLACK     "\033[30m" ///< Black foreground.
#define ANSI_RED       "\033[31m" ///< Red foreground.
#define ANSI_GREEN     "\033[32m" ///< Green foreground.
#define ANSI_YELLOW    "\033[33m" ///< Yellow foreground.
#define ANSI_BLUE      "\033[34m" ///< Blue foreground.
#define ANSI_MAGENTA   "\033[35m" ///< Magenta foreground.
#define ANSI_CYAN      "\033[36m" ///< Cyan foreground.
#define ANSI_WHITE     "\033[37m" ///< White foreground.
#define ANSI_GREY      "\033[90m" ///< Bright-black (grey) foreground.

#define ANSI_BOLD      "\033[1m"  ///< Bold text.
#define ANSI_ITALIC    "\033[3m"  ///< Italic text.
#define ANSI_UNDERLINE "\033[4m"  ///< Underlined text.

#define ANSI_RESET     "\033[0m"  ///< Reset all attributes to terminal default.
/** @} */

namespace omni::basic {

    /** @defgroup printing Printing & Formatting
     *  @brief Lightweight `{}`-style format string utilities, similar to Python's `str.format()`.
     *
     *  Placeholders are bare `{}` tokens substituted left-to-right with the supplied arguments.
     *  Mismatches between placeholder count and argument count throw `std::runtime_error`.
     *  @{
     */

    /**
     * @brief Base case for fStringPrint — flushes the remaining format string tail.
     *
     * Called when all arguments have been consumed. Raises if any `{}` placeholder
     * remains in the unprocessed tail.
     *
     * @param ss   Accumulator stream built by the recursive overload.
     * @param fmt  Original format string.
     * @param pos  Byte offset of the unconsumed tail.
     * @return     Fully formatted string.
     * @throws std::runtime_error If unmatched `{}` placeholders remain.
     */
    inline std::string fStringPrint(std::stringstream& ss, const std::string& fmt, size_t pos) {
        if (fmt.find("{}", pos) != std::string::npos) {
            throw std::runtime_error("Too few arguments for format string");
        }
        ss << fmt.substr(pos);
        return ss.str();
    }

    /**
     * @brief Recursive workhorse for @ref stringPrint — substitutes one `{}` per call.
     * @tparam T      Type of the next argument; must be streamable via `operator<<`.
     * @tparam Args   Remaining argument types.
     * @param  ss     Accumulator stream.
     * @param  fmt    Original format string.
     * @param  pos    Byte offset of the unconsumed tail.
     * @param  value  Value to substitute into the next `{}`.
     * @param  args   Remaining values.
     * @return        Fully formatted string once recursion bottoms out.
     * @throws std::runtime_error If there are more arguments than `{}` placeholders.
     */
    template<typename T, typename... Args>
    std::string fStringPrint(std::stringstream& ss, const std::string& fmt, size_t pos, T value, Args... args) {
        const size_t brace = fmt.find("{}", pos);
        if (brace == std::string::npos) {
            throw std::runtime_error("Too many arguments for format string");
        }
        ss << fmt.substr(pos, brace - pos);
        ss << value;
        return fStringPrint(ss, fmt, brace + 2, args...);
    }

    /**
     * @brief Formats a string with `{}`-style placeholders and returns it.
     *
     * @code
     * auto s = stringPrint("Hello, {}! You are {} years old.", name, age);
     * @endcode
     *
     * @tparam Args  Argument types; each must be streamable via `operator<<`.
     * @param  fmt   Format string containing zero or more `{}` tokens.
     * @param  args  Values substituted left-to-right into each `{}`.
     * @return       Formatted string.
     * @throws std::runtime_error On placeholder/argument count mismatch.
     */
    template<typename... Args>
    std::string stringPrint(const std::string& fmt, Args... args) {
        std::stringstream ss;
        return fStringPrint(ss, fmt, 0, args...);
    }

    /**
     * @brief Base case for @ref print / @ref println — writes the format string tail to @p os.
     * @param os   Output stream.
     * @param fmt  Original format string.
     * @param pos  Byte offset of the unconsumed tail.
     * @throws std::runtime_error If unmatched `{}` placeholders remain.
     */
    inline void fprint_impl(std::ostream& os, const std::string& fmt, size_t pos) {
        if (fmt.find("{}", pos) != std::string::npos) {
            throw std::runtime_error("Too few arguments for format string");
        }
        os << fmt.substr(pos);
    }

    /**
     * @brief Recursive workhorse for @ref print / @ref println.
     * @tparam T      Type of the next argument.
     * @tparam Args   Remaining argument types.
     * @param  os     Destination stream.
     * @param  fmt    Format string.
     * @param  pos    Byte offset of the unconsumed tail.
     * @param  value  Value for the next `{}`.
     * @param  args   Remaining values.
     * @throws std::runtime_error On placeholder/argument count mismatch.
     */
    template<typename T, typename... Args>
    void fprint_impl(std::ostream& os, const std::string& fmt, size_t pos, T value, Args... args) {
        const size_t brace = fmt.find("{}", pos);
        if (brace == std::string::npos) {
            throw std::runtime_error("Too many arguments for format string");
        }
        os << fmt.substr(pos, brace - pos);
        os << value;
        fprint_impl(os, fmt, brace + 2, args...);
    }

    /**
     * @brief Prints a formatted string to `stdout` with no trailing newline.
     * @tparam Args  Streamable argument types.
     * @param  fmt   `{}`-style format string.
     * @param  args  Substitution values.
     */
    template<typename... Args>
    void print(const std::string& fmt, Args... args) {
        fprint_impl(std::cout, fmt, 0, args...);
    }

    /**
     * @brief Prints a formatted string to @p os with no trailing newline.
     * @tparam Args  Streamable argument types.
     * @param  os    Destination stream.
     * @param  fmt   `{}`-style format string.
     * @param  args  Substitution values.
     */
    template<typename... Args>
    void print(std::ostream& os, const std::string& fmt, Args... args) {
        fprint_impl(os, fmt, 0, args...);
    }

    /**
     * @brief Prints a formatted string to `stdout` followed by `std::endl`.
     * @tparam Args  Streamable argument types.
     * @param  fmt   `{}`-style format string.
     * @param  args  Substitution values.
     */
    template<typename... Args>
    void println(const std::string& fmt, Args... args) {
        fprint_impl(std::cout, fmt, 0, args...);
        std::cout << std::endl;
    }

    /**
     * @brief Prints a formatted string to @p os followed by `std::endl`.
     * @tparam Args  Streamable argument types.
     * @param  os    Destination stream.
     * @param  fmt   `{}`-style format string.
     * @param  args  Substitution values.
     */
    template<typename... Args>
    void println(std::ostream& os, const std::string& fmt, Args... args) {
        fprint_impl(os, fmt, 0, args...);
        os << std::endl;
    }

    /** @} */ // end group printing

    /** @defgroup logging Logging Macros & Helpers
     *  @brief Structured, levelled logging to `stderr` and optionally a file (@ref logPath).
     *
     *  Use the `LOG_*` macros rather than the `_impl_log_*` functions directly; the macros
     *  inject `__FILE__` and `__LINE__` automatically.
     *
     *  Individual levels can be silenced at compile time:
     *  | Preprocessor symbol | Suppressed level      |
     *  |---------------------|-----------------------|
     *  | `DISABLE_WARN`      | @ref LOG_LEVEL_WARN   |
     *  | `DISABLE_INFO`      | @ref LOG_LEVEL_INFO   |
     *  | `DISABLE_DEBUG`     | @ref LOG_LEVEL_DEBUG  |
     *  | `DISABLE_TRACE`     | @ref LOG_LEVEL_TRACE  |
     *
     *  FATAL and ERROR are always emitted and cannot be disabled.
     *  @{
     */

    /**
     * @brief Implementation backing @ref LOG_FATAL. Prefer the macro.
     *
     * Prints to `stderr` in bold red, appends to @ref logger, and writes to
     * @ref logPath if set.
     *
     * @tparam Args  Streamable argument types.
     * @param  fmt   `{}`-style format string.
     * @param  file  Source file (`__FILE__`).
     * @param  line  Source line (`__LINE__`).
     * @param  args  Format arguments.
     */
    template<typename... Args>
    void _impl_log_fatal(const std::string& fmt, const char* file, const int line, const Args&... args) {
        std::cerr << file << ":" << line << ANSI_RED << ANSI_BOLD << " [FATAL] ";
        print(std::cerr, fmt, args...);
        std::cerr << ANSI_RESET << std::endl;
        logger.emplace_back(LOG_LEVEL_FATAL, stringPrint(fmt, args...));
        if (!logPath.empty()) {
            std::ofstream os(logPath, std::ios_base::app);
            os << file << ":" << line << " [FATAL] ";
            print(os, fmt, args...);
            os << std::endl;
            os.close();
        }
    }

    /** @brief Logs a fatal message with source location. Always active.
     *  @param fmt `{}`-style format string. @param ... Format arguments. */
    #define LOG_FATAL(fmt, ...) _impl_log_fatal(fmt, __FILE__, __LINE__, ##__VA_ARGS__)

    /**
     * @brief Implementation backing @ref LOG_ERROR. Prefer the macro.
     *
     * Prints to `stderr` in red italic, appends to @ref logger, and writes to
     * @ref logPath if set.
     *
     * @tparam Args  Streamable argument types.
     * @param  fmt   `{}`-style format string.
     * @param  file  Source file (`__FILE__`).
     * @param  line  Source line (`__LINE__`).
     * @param  args  Format arguments.
     */
    template<typename... Args>
    void _impl_log_error(const std::string& fmt, const char* file, const int line, const Args&... args) {
        std::cerr << file << ":" << line << ANSI_RED << " [ERROR] " << ANSI_ITALIC;
        print(std::cerr, fmt, args...);
        std::cerr << ANSI_RESET << '\n';
        logger.emplace_back(LOG_LEVEL_ERROR, stringPrint(fmt, args...));
        if (!logPath.empty()) {
            std::ofstream os(logPath, std::ios_base::app);
            os << file << ":" << line << " [ERROR] ";
            print(os, fmt, args...);
            os << std::endl;
            os.close();
        }
    }

    /** @brief Logs a recoverable error with source location. Always active.
     *  @param fmt `{}`-style format string. @param ... Format arguments. */
    #define LOG_ERROR(fmt, ...) _impl_log_error(fmt, __FILE__, __LINE__, ##__VA_ARGS__)

    /**
     * @brief Implementation backing @ref LOG_WARN. Prefer the macro.
     *
     * No-op when `DISABLE_WARN` is defined. Otherwise prints to `stderr` in
     * magenta, appends to @ref logger, and writes to @ref logPath if set.
     *
     * @tparam Args  Streamable argument types.
     * @param  fmt   `{}`-style format string.
     * @param  file  Source file (`__FILE__`).
     * @param  line  Source line (`__LINE__`).
     * @param  args  Format arguments.
     */
    template<typename... Args>
    void _impl_log_warn(const std::string& fmt, const char* file, const int line, const Args&... args) {
        #ifndef DISABLE_WARN
        std::cerr << file << ":" << line << ANSI_MAGENTA << "  [WARN] ";
        print(std::cerr, fmt, args...);
        std::cerr << ANSI_RESET << std::endl;
        logger.emplace_back(LOG_LEVEL_WARN, stringPrint(fmt, args...));
        if (!logPath.empty()) {
            std::ofstream os(logPath, std::ios_base::app);
            os << file << ":" << line << "  [WARN] ";
            print(os, fmt, args...);
            os << std::endl;
            os.close();
        }
        #endif
    }

    /** @brief Logs a warning with source location. Suppressed if `DISABLE_WARN` is defined.
     *  @param fmt `{}`-style format string. @param ... Format arguments. */
    #define LOG_WARN(fmt, ...) _impl_log_warn(fmt, __FILE__, __LINE__, ##__VA_ARGS__)

    /**
     * @brief Implementation backing @ref LOG_INFO. Prefer the macro.
     *
     * No-op when `DISABLE_INFO` is defined. Otherwise prints to `stderr` in
     * green, appends to @ref logger, and writes to @ref logPath if set.
     *
     * @tparam Args  Streamable argument types.
     * @param  fmt   `{}`-style format string.
     * @param  file  Source file (`__FILE__`).
     * @param  line  Source line (`__LINE__`).
     * @param  args  Format arguments.
     */
    template<typename... Args>
    void _impl_log_info(const std::string& fmt, const char* file, const int line, const Args&... args) {
        #ifndef DISABLE_INFO
        std::cerr << file << ":" << line << ANSI_GREEN << "  [INFO] ";
        print(std::cerr, fmt, args...);
        std::cerr << ANSI_RESET << std::endl;
        logger.emplace_back(LOG_LEVEL_INFO, stringPrint(fmt, args...));
        if (!logPath.empty()) {
            std::ofstream os(logPath, std::ios_base::app);
            os << file << ":" << line << "  [INFO] ";
            print(os, fmt, args...);
            os << std::endl;
            os.close();
        }
        #endif
    }

    /** @brief Logs an informational message with source location. Suppressed if `DISABLE_INFO` is defined.
     *  @param fmt `{}`-style format string. @param ... Format arguments. */
    #define LOG_INFO(fmt, ...) _impl_log_info(fmt, __FILE__, __LINE__, ##__VA_ARGS__)

    /**
     * @brief Implementation backing @ref LOG_DEBUG. Prefer the macro.
     *
     * No-op when `DISABLE_DEBUG` is defined. Otherwise prints to `stderr` in
     * cyan italic, appends to @ref logger, and writes to @ref logPath if set.
     *
     * @tparam Args  Streamable argument types.
     * @param  fmt   `{}`-style format string.
     * @param  file  Source file (`__FILE__`).
     * @param  line  Source line (`__LINE__`).
     * @param  args  Format arguments.
     */
    template<typename... Args>
    void _impl_log_debug(const std::string& fmt, const char* file, const int line, const Args&... args) {
        #ifndef DISABLE_DEBUG
        std::cerr << file << ":" << line << ANSI_CYAN << " [DEBUG] " << ANSI_ITALIC;
        print(std::cerr, fmt, args...);
        std::cerr << ANSI_RESET << std::endl;
        logger.emplace_back(LOG_LEVEL_DEBUG, stringPrint(fmt, args...));
        if (!logPath.empty()) {
            std::ofstream os(logPath, std::ios_base::app);
            os << file << ":" << line << " [DEBUG] ";
            print(os, fmt, args...);
            os << std::endl;
            os.close();
        }
        #endif
    }

    /** @brief Logs a debug diagnostic with source location. Suppressed if `DISABLE_DEBUG` is defined.
     *  @param fmt `{}`-style format string. @param ... Format arguments. */
    #define LOG_DEBUG(fmt, ...) _impl_log_debug(fmt, __FILE__, __LINE__, ##__VA_ARGS__)

    /**
     * @brief Implementation backing @ref LOG_TRACE. Prefer the macro.
     *
     * No-op when `DISABLE_TRACE` is defined. Otherwise prints to `stderr` in
     * grey italic, appends to @ref logger, and writes to @ref logPath if set.
     *
     * @tparam Args  Streamable argument types.
     * @param  fmt   `{}`-style format string.
     * @param  file  Source file (`__FILE__`).
     * @param  line  Source line (`__LINE__`).
     * @param  args  Format arguments.
     */
    template<typename... Args>
    void _impl_log_trace(const std::string& fmt, const char* file, const int line, const Args&... args) {
        #ifndef DISABLE_TRACE
        std::cerr << file << ":" << line << ANSI_GREY << " [TRACE] " << ANSI_ITALIC;
        print(std::cerr, fmt, args...);
        std::cerr << ANSI_RESET << std::endl;
        logger.emplace_back(LOG_LEVEL_TRACE, stringPrint(fmt, args...));
        if (!logPath.empty()) {
            std::ofstream os(logPath, std::ios_base::app);
            os << file << ":" << line << " [TRACE] ";
            print(os, fmt, args...);
            os << std::endl;
            os.close();
        }
        #endif
    }

    /** @brief Logs a fine-grained trace message with source location. Suppressed if `DISABLE_TRACE` is defined.
     *  @param fmt `{}`-style format string. @param ... Format arguments. */
    #define LOG_TRACE(fmt, ...) _impl_log_trace(fmt, __FILE__, __LINE__, ##__VA_ARGS__)

    /**
     * @brief Logs a fatal message then calls `std::terminate()`.
     *
     * Use when a truly unrecoverable state has been reached. Prefer throwing
     * an exception where recovery is possible.
     *
     * @tparam Args  Streamable argument types.
     * @param  fmt   `{}`-style format string.
     * @param  args  Format arguments.
     */
    template<typename... Args>
    void panic(const std::string& fmt, Args... args) {
        LOG_FATAL(fmt, args...);
        std::terminate();
    }

    /**
     * @brief Marks an unimplemented code path with a debug-level log.
     *
     * Drop a `TODO("message")` anywhere a feature is not yet written.
     * The call is recorded in @ref logger at @ref LOG_LEVEL_DEBUG so
     * incomplete paths are visible in debug builds without causing a hard failure.
     *
     * @tparam Args  Streamable argument types.
     * @param  fmt   `{}`-style description of what still needs doing.
     * @param  args  Optional format arguments.
     */
    template<typename... Args>
    void TODO(const std::string& fmt, Args... args) {
        LOG_DEBUG(fmt, args...);
    }

    /** @} */ // end group logging

} // namespace omni::basic

// ========================================================
// USEFUL MACROS
// ========================================================
namespace omni::basic {

    /** @defgroup macros Utility Macros
     *  @brief General-purpose macros for assertions, array sizing, and pointer/offset arithmetic.
     *  @{
     */

    /**
     * @brief Hard assertion — calls @ref panic if @p c is false.
     *
     * Unlike `assert()`, this is never stripped in release builds.
     * @param c  Condition to test. Program terminates if false.
     */
    #define Assert(c) \
        do { \
            if (!(c)) { \
                panic("Assertion failed: " #c); \
            } \
        } while (0)

    /**
     * @brief Emits a @ref LOG_WARN if @p c is false; otherwise does nothing.
     * @param c    Condition to test.
     * @param msg  `{}`-style format string for the warning message.
     * @param ...  Optional format arguments.
     */
    #define WarnIfNot(c, msg, ...) \
        do { \
            if (!(c)) { \
                omni::LOG_WARN(msg, ##__VA_ARGS__); \
            } \
        } while (0)

    /**
     * @brief Returns the number of elements in a stack-allocated array.
     * @param a  A stack-allocated array (not a pointer).
     * @return   Element count as `size_t`.
     */
    #define ArrayCount(a) (sizeof(a) / sizeof(a[0]))

    /**
     * @brief Converts a pointer to an unsigned integer (its byte offset from null).
     * @param p  Pointer to convert.
     * @return   `unsigned long long` address value.
     */
    #define IntFromPtr(p) (unsigned long long)((char*)p - (char*)0)

    /**
     * @brief Converts an unsigned integer back to a `void*`.
     * @param n  Integer address produced by @ref IntFromPtr.
     * @return   `void*` at that address.
     */
    #define PtrFromInt(n) (void*)((char*)0 + (n))

    /**
     * @brief Produces a null-pointer-based lvalue for member @p m of type @p T.
     * @note  Used only as an operand to `sizeof` or @ref OffsetOfMember; never dereferenced.
     * @param T  Struct or class type.
     * @param m  Member name.
     */
    #define Member(T, m) (((T*)0)->m)

    /**
     * @brief Returns the byte offset of member @p m within type @p T.
     *
     * Equivalent to `offsetof(T, m)` but works with the project's pointer helpers.
     * @param T  Struct or class type.
     * @param m  Member name.
     * @return   Byte offset as `unsigned long long`.
     */
    #define OffsetOfMember(T, m) IntFromPtr(&Member(T, m))

    /** @} */ // end group macros

} // namespace omni::basic

// ========================================================
// COMPOUND TYPES
// ========================================================
namespace omni {
    using namespace basic;

    /**
     * @brief Two-component vector with multiple named accessors.
     *
     * The two components are accessible as:
     * - `x`, `y` — spatial coordinates
     * - `L`, `R` — left / right (e.g. stereo audio)
     * - `u`, `v` — texture coordinates
     * - `data[0]`, `data[1]` — raw indexed access
     *
     * All standard arithmetic operators are defined component-wise.
     * Mixed scalar/vector forms are provided for `+`, `-`, `*`, `/`.
     *
     * @tparam T  Component type. Must support arithmetic operators and `sqrt`.
     */
    template<typename T>
    struct Vec2 {
        union {
            struct { T x, y; }; ///< Spatial coordinate access.
            struct { T L, R; }; ///< Left / right access.
            struct { T u, v; }; ///< Texture coordinate access.
            T data[2];          ///< Raw indexed access.
        };

        /** @brief Default constructor — zero-initialises both components. */
        Vec2() : x(T()), y(T()) {}

        /**
         * @brief Constructs a Vec2 from explicit component values.
         * @param x  First component.
         * @param y  Second component.
         */
        Vec2(const T x, const T y) : x(x), y(y) {}

        /**
         * @brief Indexed component access (mutable).
         * @param i  Index; must be 0 or 1. Calls `exit(1)` if out of range.
         * @return   Reference to the component.
         */
        T& operator[](const size_t i) {
            if (i > 1) exit(1);
            return data[i];
        }

        /**
         * @brief Indexed component access (const).
         * @param i  Index; must be 0 or 1. Calls `exit(1)` if out of range.
         * @return   Const reference to the component.
         */
        const T& operator[](const size_t i) const {
            if (i > 1) exit(1);
            return data[i];
        }

        /**
         * @brief Streams the vector as `"x, y"`.
         * @param os  Output stream.
         * @param v   Vector to print.
         * @return    @p os for chaining.
         */
        friend std::ostream& operator<<(std::ostream& os, const Vec2<T>& v) {
            return os << v.x << ", " << v.y;
        }

        /** @brief Component-wise addition of two Vec2 values. */
        friend Vec2<T> operator+(const Vec2<T>& a, const Vec2<T>& b) {
            return Vec2<T>(a.x + b.x, a.y + b.y);
        }
        /** @brief Adds scalar @p b to each component of @p a. */
        template<typename E>
        friend Vec2<T> operator+(const Vec2<T>& a, E b) {
            return Vec2<T>(a.x + b, a.y + b);
        }
        /** @brief Adds scalar @p b to each component of @p a (commutative form). */
        template<typename E>
        friend Vec2<T> operator+(E b, const Vec2<T>& a) {
            return Vec2<T>(a.x + b, a.y + b);
        }

        /** @brief Component-wise subtraction of two Vec2 values. */
        friend Vec2<T> operator-(const Vec2<T>& a, const Vec2<T>& b) {
            return Vec2<T>(a.x - b.x, a.y - b.y);
        }
        /** @brief Subtracts scalar @p b from each component of @p a. */
        template<typename E>
        friend Vec2<T> operator-(const Vec2<T>& a, E b) {
            return Vec2<T>(a.x - b, a.y - b);
        }
        /** @brief Subtracts each component of @p b from scalar @p a. */
        template<typename E>
        friend Vec2<T> operator-(E a, const Vec2<T>& b) {
            return Vec2<T>(a - b.x, b - a.y);
        }

        /** @brief Component-wise multiplication of two Vec2 values. */
        friend Vec2<T> operator*(const Vec2<T>& a, const Vec2<T>& b) {
            return Vec2<T>(a.x * b.x, a.y * b.y);
        }
        /** @brief Multiplies each component of @p a by scalar @p b. */
        template<typename E>
        friend Vec2<T> operator*(const Vec2<T>& a, const E& b) {
            return Vec2<T>(a.x * b, a.y * b);
        }
        /** @brief Multiplies each component of @p a by scalar @p b (commutative form). */
        template<typename E>
        friend Vec2<T> operator*(const E& b, const Vec2<T>& a) {
            return Vec2<T>(a.x * b, a.y * b);
        }

        /** @brief Component-wise division of two Vec2 values. */
        friend Vec2<T> operator/(const Vec2<T>& a, const Vec2<T>& b) {
            return Vec2<T>(a.x / b.x, a.y / b.y);
        }
        /** @brief Divides each component of @p a by scalar @p b. */
        template<typename E>
        friend Vec2<T> operator/(const Vec2<T>& a, E b) {
            return Vec2<T>(a.x / b, a.y / b);
        }
        /** @brief Divides scalar @p a by each component of @p b. */
        template<typename E>
        friend Vec2<T> operator/(E a, const Vec2<T>& b) {
            return Vec2<T>(a / b.x, b / a.y);
        }

        /**
         * @brief Computes the dot product \f$a \cdot b = a_x b_x + a_y b_y\f$.
         * @return Scalar dot product.
         */
        friend constexpr T dot(const Vec2<T>& a, const Vec2<T>& b) {
            return a.x * b.x + a.y * b.y;
        }

        /**
         * @brief Computes the 2D cross product (scalar) \f$a_x b_y - a_y b_x\f$.
         *
         * The sign indicates the turn direction from @p a to @p b:
         * positive = counter-clockwise, negative = clockwise.
         *
         * @return Signed scalar magnitude of the cross product.
         */
        friend constexpr T cross(const Vec2<T>& a, const Vec2<T>& b) {
            return a.x * b.y - a.y * b.x;
        }

        /**
         * @brief Returns the Euclidean length \f$\|a\| = \sqrt{a \cdot a}\f$.
         * @return Non-negative scalar length.
         */
        friend inline T length(const Vec2<T>& a) {
            return sqrt(dot(a, a));
        }

        /**
         * @brief Returns a unit vector in the direction of @p a.
         *
         * Returns @p a unchanged if its length is zero (avoids division by zero).
         *
         * @return Normalised vector, or @p a if it is the zero vector.
         */
        friend inline Vec2<T> normalize(const Vec2<T>& a) {
            T len = length(a);
            if (len == T(0)) return a;
            return Vec2<T>(a.x / len, a.y / len);
        }
    };
/**
     * @brief Three-component vector with multiple named accessors.
     *
     * The three components are accessible as:
     * - `x`, `y`, `z` — spatial coordinates
     * - `r`, `g`, `b` — color channels
     * - `L`, `R`, `C` — left / right / center
     * - `u`, `v`, `w` — texture coordinates
     * - `data[0..2]`  — raw indexed access
     *
     * @tparam T  Component type. Must support arithmetic operators and `sqrt`.
     */
    template<typename T>
    struct Vec3 {
        union {
            struct { T x, y, z; }; ///< Spatial coordinate access.
            struct { T r, g, b; }; ///< Color channel access.
            struct { T L, R, C; }; ///< Left / right / center access.
            struct { T u, v, w; }; ///< Texture coordinate access.
            T data[3];             ///< Raw indexed access.
        };

        /** @brief Default constructor — zero-initialises all three components. */
        Vec3() : x(T()), y(T()), z(T()) {}

        /**
         * @brief Constructs a Vec3 from explicit component values.
         * @param x  First component.
         * @param y  Second component.
         * @param z  Third component.
         */
        Vec3(const T x, const T y, const T z) : x(x), y(y), z(z) {}

        /**
         * @brief Indexed component access (mutable).
         * @param i  Index in [0, 2]. Calls `exit(1)` if out of range.
         * @return   Reference to the component.
         */
        T& operator[](const size_t i) {
            if (i > 2) exit(1);
            return data[i];
        }

        /**
         * @brief Indexed component access (const).
         * @param i  Index in [0, 2]. Calls `exit(1)` if out of range.
         * @return   Const reference to the component.
         */
        const T& operator[](const size_t i) const {
            if (i > 2) exit(1);
            return data[i];
        }

        /**
         * @brief Streams the vector as `"x, y, z"`.
         * @param os  Output stream.
         * @param v   Vector to print.
         * @return    @p os for chaining.
         */
        friend std::ostream& operator<<(std::ostream& os, const Vec3<T>& v) {
            return os << v.x << ", " << v.y << ", " << v.z;
        }

        /** @brief Component-wise addition of two Vec3 values. */
        friend Vec3<T> operator+(const Vec3<T>& a, const Vec3<T>& b) {
            return Vec3<T>(a.x + b.x, a.y + b.y, a.z + b.z);
        }
        /** @brief Adds scalar @p b to each component of @p a. */
        template<typename E>
        friend Vec3<T> operator+(const Vec3<T>& a, E b) {
            return Vec3<T>(a.x + b, a.y + b, a.z + b);
        }
        /** @brief Adds scalar @p b to each component of @p a (commutative form). */
        template<typename E>
        friend Vec3<T> operator+(E b, const Vec3<T>& a) {
            return Vec3<T>(a.x + b, a.y + b, a.z + b);
        }

        /** @brief Component-wise subtraction of two Vec3 values. */
        friend Vec3<T> operator-(const Vec3<T>& a, const Vec3<T>& b) {
            return Vec3<T>(a.x - b.x, a.y - b.y, a.z - b.z);
        }
        /** @brief Subtracts scalar @p b from each component of @p a. */
        template<typename E>
        friend Vec3<T> operator-(const Vec3<T>& a, E b) {
            return Vec3<T>(a.x - b, a.y - b, a.z - b);
        }
        /** @brief Subtracts each component of @p b from scalar @p a. */
        template<typename E>
        friend Vec3<T> operator-(E a, const Vec3<T>& b) {
            return Vec3<T>(a - b.x, b - a.y, a - b.z);
        }

        /** @brief Component-wise multiplication of two Vec3 values. */
        friend Vec3<T> operator*(const Vec3<T>& a, const Vec3<T>& b) {
            return Vec3<T>(a.x * b.x, a.y * b.y, a.z * b.z);
        }
        /** @brief Multiplies each component of @p a by scalar @p b. */
        template<typename E>
        friend Vec3<T> operator*(const Vec3<T>& a, const E& b) {
            return Vec3<T>(a.x * b, a.y * b, a.z * b);
        }
        /** @brief Multiplies each component of @p a by scalar @p b (commutative form). */
        template<typename E>
        friend Vec3<T> operator*(const E& b, const Vec3<T>& a) {
            return Vec3<T>(a.x * b, a.y * b, a.z * b);
        }

        /** @brief Component-wise division of two Vec3 values. */
        friend Vec3<T> operator/(const Vec3<T>& a, const Vec3<T>& b) {
            return Vec3<T>(a.x / b.x, a.y / b.y, a.z / b.z);
        }
        /** @brief Divides each component of @p a by scalar @p b. */
        template<typename E>
        friend Vec3<T> operator/(const Vec3<T>& a, E b) {
            return Vec3<T>(a.x / b, a.y / b, a.z / b);
        }
        /** @brief Divides scalar @p a by each component of @p b. */
        template<typename E>
        friend Vec3<T> operator/(E a, const Vec3<T>& b) {
            return Vec3<T>(a / b.x, b / a.y, b / a.z);
        }

        /**
         * @brief Computes the dot product \f$a \cdot b = a_x b_x + a_y b_y + a_z b_z\f$.
         * @return Scalar dot product.
         */
        friend inline constexpr T dot(const Vec3<T>& a, const Vec3<T>& b) {
            return a.x * b.x + a.y * b.y + a.z * b.z;
        }

        /**
         * @brief Computes the 3D cross product \f$a \times b\f$.
         *
         * The result is perpendicular to both @p a and @p b, with direction
         * determined by the right-hand rule.
         *
         * @return Vector perpendicular to both inputs.
         */
        friend inline Vec3<T> cross(const Vec3<T>& a, const Vec3<T>& b) {
            return {
                a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x
            };
        }

        /**
         * @brief Returns the Euclidean length \f$\|a\| = \sqrt{a \cdot a}\f$.
         * @return Non-negative scalar length.
         */
        friend inline T length(const Vec3<T>& a) {
            return sqrt(dot(a, a));
        }

        /**
         * @brief Returns a unit vector in the direction of @p a.
         *
         * Returns @p a unchanged if its length is zero (avoids division by zero).
         *
         * @return Normalised vector, or @p a if it is the zero vector.
         */
        friend inline Vec3<T> normalize(const Vec3<T>& a) {
            T len = length(a);
            if (len == T(0)) return a;
            return Vec3<T>(a.x / len, a.y / len, a.z / len);
        }
    };

    /**
     * @brief Four-component vector with multiple named accessors.
     *
     * The four components are accessible as:
     * - `x`, `y`, `z`, `w` — homogeneous spatial coordinates
     * - `r`, `g`, `b`, `a` — color channels with alpha
     * - `data[0..3]`        — raw indexed access
     *
     * @note `normalize` preserves `w` unchanged — it normalises only the
     *       `xyz` magnitude. This is intentional for homogeneous coordinates
     *       but may be surprising for pure 4D vectors.
     *
     * @tparam T  Component type. Must support arithmetic operators and `sqrt`.
     */
    template<typename T>
    struct Vec4 {
        union {
            struct { T x, y, z, w; }; ///< Homogeneous spatial coordinate access.
            struct { T r, g, b, a; }; ///< Color channel access (with alpha).
            T data[4];                ///< Raw indexed access.
        };

        /** @brief Default constructor — zero-initialises all four components. */
        Vec4() : x(T()), y(T()), z(T()), w(T()) {}

        /**
         * @brief Constructs a Vec4 from explicit component values.
         * @param x  First component.
         * @param y  Second component.
         * @param z  Third component.
         * @param w  Fourth component.
         */
        Vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

        /**
         * @brief Indexed component access (mutable).
         * @param i  Index in [0, 3]. Calls `exit(1)` if out of range.
         * @return   Reference to the component.
         */
        T& operator[](const size_t i) {
            if (i > 3) exit(1);
            return data[i];
        }

        /**
         * @brief Indexed component access (const).
         * @param i  Index in [0, 3]. Calls `exit(1)` if out of range.
         * @return   Const reference to the component.
         */
        const T& operator[](const size_t i) const {
            if (i > 3) exit(1);
            return data[i];
        }

        /**
         * @brief Streams the vector as `"x, y, z, w"`.
         * @param os  Output stream.
         * @param v   Vector to print.
         * @return    @p os for chaining.
         */
        friend std::ostream& operator<<(std::ostream& os, const Vec4<T>& v) {
            return os << v.x << ", " << v.y << ", " << v.z << ", " << v.w;
        }

        /** @brief Component-wise addition of two Vec4 values. */
        friend Vec4<T> operator+(const Vec4<T>& a, const Vec4<T>& b) {
            return Vec4<T>(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
        }

        /** @brief Component-wise subtraction of two Vec4 values. */
        friend Vec4<T> operator-(const Vec4<T>& a, const Vec4<T>& b) {
            return Vec4<T>(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
        }

        /** @brief Component-wise multiplication of two Vec4 values. */
        friend Vec4<T> operator*(const Vec4<T>& a, const Vec4<T>& b) {
            return Vec4<T>(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
        }

        /** @brief Component-wise division of two Vec4 values. */
        friend Vec4<T> operator/(const Vec4<T>& a, const Vec4<T>& b) {
            return Vec4<T>(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
        }

        /**
         * @brief Computes the dot product \f$a \cdot b = a_x b_x + a_y b_y + a_z b_z + a_w b_w\f$.
         * @return Scalar dot product.
         */
        friend inline constexpr T dot(const Vec4<T>& a, const Vec4<T>& b) {
            return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
        }

        /**
         * @brief Returns the Euclidean length \f$\|a\| = \sqrt{a \cdot a}\f$.
         * @return Non-negative scalar length.
         */
        friend inline T length(const Vec4<T>& a) {
            return sqrt(dot(a, a));
        }

        /**
         * @brief Returns a vector with the `xyz` components normalised; `w` is preserved.
         *
         * Returns @p a unchanged if its length is zero (avoids division by zero).
         *
         * @return Vector with unit-length `xyz`, original `w`.
         */
        friend inline Vec4<T> normalize(const Vec4<T>& a) {
            T len = length(a);
            if (len == T(0)) return a;
            return Vec4<T>(a.x / len, a.y / len, a.z / len, a.w);
        }
    };
/**
     * @brief A heterogeneous key-value pair with multiple named accessors.
     *
     * The two members are accessible as:
     * - `first`, `second` — standard pair convention
     * - `a`, `b`          — generic aliases
     * - `x`, `y`          — coordinate-style aliases
     * - `key`, `value`    — map-style aliases
     *
     * Equality comparison is keyed on `a` / `first` only, making Pair
     * suitable for use in associative containers.
     *
     * @tparam T  Type of the first element.
     * @tparam E  Type of the second element.
     */
    template<typename T, typename E>
    struct Pair {
        union {
            struct { T first;  E second; }; ///< Standard pair access.
            struct { T a;      E b;      }; ///< Generic alias.
            struct { T x;      E y;      }; ///< Coordinate-style alias.
            struct { T key;    E value;  }; ///< Map-style alias.
        };

        /** @brief Default constructor — value-initialises both members. */
        Pair() : a(T()), b(E()) {}

        /**
         * @brief Constructs a Pair from explicit values.
         * @param a  First element.
         * @param b  Second element.
         */
        Pair(T a, E b) : a(a), b(b) {}

        /**
         * @brief Streams the pair as `"[a, b]"`.
         * @param os  Output stream.
         * @param p   Pair to print.
         * @return    @p os for chaining.
         */
        friend std::ostream& operator<<(std::ostream& os, const Pair<T, E>& p) {
            return os << "[" << p.a << ", " << p.b << "]";
        }

        /**
         * @brief Equality comparison keyed on the first element only.
         *
         * Two pairs are considered equal if their `a` / `first` / `key`
         * members compare equal, regardless of the second element. This
         * matches the behaviour expected for associative lookups.
         *
         * @param p  Pair to compare against.
         * @return   `true` if `a == p.a`.
         */
        bool operator==(const Pair& p) const {
            return a == p.a;
        }
    };

} // namespace omni

// ========================================================
// BENCHMARKING AND PROFILING
// ========================================================

/**
 * @defgroup benchmarking Benchmarking & Profiling
 * @brief Lightweight wall-clock timers and a Chrome-trace-format profiler.
 *
 * **Timing:**
 * @code
 * omni::basic::Timer t;
 * // ... do stuff ...
 * double ms  = t.elapsed_ms();
 * double sec = t.elapsed_sec();
 * t.reset();  // restart the clock
 *
 * {
 *     omni::basic::ScopedTimer t("my operation");
 *     // ... do stuff ...
 * }  // prints: "my operation took 3.142 ms"
 * @endcode
 *
 * **Profiling** (Chrome `chrome://tracing` format):
 * @code
 * #define PROFILING 1
 *
 * omni::basic::Benchmark::Get().BeginSession("my session", "results.json");
 *
 * void someFunction() {
 *     PROFILE_FUNCTION();      // trace the whole function by name
 * }
 *
 * void otherFunction() {
 *     PROFILE_SCOPE("label");  // trace with a custom name
 * }
 *
 * omni::basic::Benchmark::Get().EndSession();
 * @endcode
 */

namespace omni::basic {

    /**
     * @brief A single recorded profiling interval.
     * @ingroup benchmarking
     */
    struct ProfileResult {
        std::string name; ///< Label for the timed scope.
        i64 start;        ///< Scope start time in microseconds since session start.
        i64 end;          ///< Scope end time in microseconds since session start.
    };

    /**
     * @brief Metadata for an active profiling session.
     * @ingroup benchmarking
     */
    struct BenchmarkSession {
        std::string name; ///< Human-readable session name passed to @ref Benchmark::BeginSession.
    };

    /**
     * @brief Singleton profiler that writes a Chrome-trace-format JSON file.
     * @ingroup benchmarking
     *
     * Obtain the instance via @ref Get, open a session with @ref BeginSession,
     * instrument code with @ref PROFILE_FUNCTION or @ref PROFILE_SCOPE, then
     * close with @ref EndSession. The resulting JSON can be loaded directly
     * in `chrome://tracing`.
     *
     * @note Not thread-safe. Concurrent @ref WriteProfile calls require
     *       external synchronisation.
     */
    class Benchmark {
    private:
        BenchmarkSession* current_session; ///< Heap-allocated session metadata, or `nullptr`.
        std::ofstream output_stream;       ///< Open output file during an active session.
        i32 profile_count;                 ///< Number of profiles written; used to manage JSON commas.
        std::chrono::time_point<std::chrono::high_resolution_clock> session_start_time; ///< Wall-clock time at @ref BeginSession.

    public:
        /** @brief Constructs the profiler in an idle state (no active session). */
        Benchmark() : current_session(nullptr), profile_count(0) {}

        /**
         * @brief Opens a new profiling session and begins writing to @p filepath.
         *
         * Creates or truncates the output file and writes the JSON header.
         * Only one session may be active at a time; call @ref EndSession first
         * if one is already open.
         *
         * @param name      Human-readable session label embedded in the output.
         * @param filepath  Path to the output JSON file (default: `"results.json"`).
         */
        void BeginSession(const std::string& name, const std::string& filepath = "results.json") {
            session_start_time = std::chrono::high_resolution_clock::now();
            output_stream.open(filepath);
            if (!output_stream.is_open()) {
                LOG_WARN("Failed to open file '{}'", filepath);
            }
            WriteHeader();
            current_session = new BenchmarkSession{name};
        }
/**
         * @brief Closes the active session and finalises the JSON output file.
         *
         * Writes the JSON footer, closes the file, frees the session object,
         * and resets internal state so a new session can be started.
         */
        void EndSession() {
            WriteFooter();
            output_stream.close();
            delete current_session;
            current_session = nullptr;
            profile_count = 0;
        }

        /**
         * @brief Returns microseconds elapsed since @ref BeginSession was called.
         * @return Elapsed time as a signed 64-bit integer in microseconds.
         */
        i64 GetTimeSinceStartMicroseconds() const {
            const auto now = std::chrono::high_resolution_clock::now();
            return std::chrono::duration_cast<std::chrono::microseconds>(now - session_start_time).count();
        }

        /**
         * @brief Appends a single profiling interval to the JSON output.
         *
         * Emits a Chrome-trace `"X"` (complete event) object. Double-quote
         * characters in the name are replaced with single quotes to keep the
         * JSON valid.
         *
         * @param result  The profiling interval to record.
         */
        void WriteProfile(const ProfileResult& result) {
            if (profile_count++ > 0) {
                output_stream << ",";
            }

            std::string name = result.name;
            std::replace(name.begin(), name.end(), '"', '\'');

            output_stream << "{";
            output_stream << "\"cat\":\"function\",";
            output_stream << "\"dur\":" << (result.end - result.start) << ',';
            output_stream << "\"name\":\"" << name << "\",";
            output_stream << "\"ph\":\"X\",";
            output_stream << "\"pid\":0,";
            output_stream << "\"tid\":0,";
            output_stream << "\"ts\":" << result.start;
            output_stream << "}";

            output_stream.flush();
        }

        /**
         * @brief Writes the opening JSON structure to the output file.
         * @note  Called automatically by @ref BeginSession.
         */
        void WriteHeader() {
            output_stream << "{\"otherData\": {},\"traceEvents\":[";
            output_stream.flush();
        }

        /**
         * @brief Writes the closing JSON structure to the output file.
         * @note  Called automatically by @ref EndSession.
         */
        void WriteFooter() {
            output_stream << "]}";
            output_stream.flush();
        }

        /**
         * @brief Returns the global Benchmark singleton.
         *
         * The instance is heap-allocated on first call and lives for the
         * duration of the process.
         *
         * @return Reference to the singleton @ref Benchmark.
         */
        static Benchmark& Get() {
            static Benchmark* instance = new Benchmark();
            return *instance;
        }
    };

    /**
     * @brief RAII scope guard that records one @ref ProfileResult on destruction.
     * @ingroup benchmarking
     *
     * Constructed with a label; on destruction writes the elapsed interval to
     * @ref Benchmark::Get(). Use via @ref PROFILE_SCOPE or @ref PROFILE_FUNCTION
     * rather than directly.
     */
    struct ScopedBenchmarkTimer {
        const char* label; ///< Human-readable name for this scope.
        i64 start;         ///< Microseconds since session start at construction time.

        /**
         * @brief Starts the timer.
         * @param label  Name shown in the trace viewer for this scope.
         */
        explicit ScopedBenchmarkTimer(const char* label)
            : label(label), start(Benchmark::Get().GetTimeSinceStartMicroseconds()) {}

        /** @brief Stops the timer and writes the result to @ref Benchmark. */
        ~ScopedBenchmarkTimer() {
            const i64 end = Benchmark::Get().GetTimeSinceStartMicroseconds();
            Benchmark::Get().WriteProfile({label, start, end});
        }
    };

    /**
     * @brief Instruments the enclosing scope for Chrome-trace profiling.
     * @ingroup benchmarking
     *
     * Expands to a @ref ScopedBenchmarkTimer with a unique variable name.
     * Compiled away to nothing when `PROFILING` is not defined or is `0`.
     *
     * @param name  String literal label shown in `chrome://tracing`.
     */
    #define PROFILE_SCOPE(name)

    /**
     * @brief Instruments the enclosing function using its compiler-decorated name.
     * @ingroup benchmarking
     *
     * Equivalent to `PROFILE_SCOPE(__PRETTY_FUNCTION__)`.
     * Compiled away to nothing when `PROFILING` is not defined or is `0`.
     */
    #define PROFILE_FUNCTION()

    //#define PROFILING 1
    #if PROFILING
    #undef PROFILE_SCOPE
    #undef PROFILE_FUNCTION
    #define PROFILE_SCOPE(name) omni::ScopedBenchmarkTimer timer##__LINE__(name)
    #define PROFILE_FUNCTION()  PROFILE_SCOPE(__PRETTY_FUNCTION__)
    #endif

    /**
     * @brief RAII scope guard that logs elapsed time on destruction via @ref LOG_INFO.
     * @ingroup benchmarking
     *
     * Lighter-weight than @ref ScopedBenchmarkTimer — no session required,
     * output goes straight to the log.
     *
     * @code
     * {
     *     omni::basic::ScopedTimer t("my operation");
     *     // ... do stuff ...
     * }  // prints: "my operation took 3.142 ms"
     * @endcode
     */
    struct ScopedTimer {
        const char* label; ///< Label printed in the log message.
        std::chrono::high_resolution_clock::time_point start; ///< Wall-clock time at construction.

        /**
         * @brief Starts the timer.
         * @param label  Label included in the log message on destruction.
         */
        explicit ScopedTimer(const char* label)
            : label(label), start(std::chrono::high_resolution_clock::now()) {}

        /** @brief Stops the timer and logs `"<label> took <ms> ms"` at INFO level. */
        ~ScopedTimer() {
            const std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::high_resolution_clock::now();
            f64 ms = std::chrono::duration<double, std::milli>(end - start).count();
            LOG_INFO("{} took {:.3f} ms", label, ms);
        }
    };

    /**
     * @brief Simple reusable wall-clock timer.
     * @ingroup benchmarking
     *
     * @code
     * omni::basic::Timer t;
     * // ... do stuff ...
     * double ms  = t.elapsed_ms();
     * double sec = t.elapsed_sec();
     * t.reset();
     * @endcode
     */
    struct Timer {
        using Clock = std::chrono::high_resolution_clock; ///< Underlying clock type.
        Clock::time_point start; ///< Time point set by the constructor and @ref reset.

        /** @brief Constructs the timer and starts it immediately. */
        Timer() { reset(); }

        /** @brief Restarts the timer from now. */
        void reset() {
            start = Clock::now();
        }

        /**
         * @brief Returns milliseconds elapsed since construction or last @ref reset.
         * @return Elapsed time in milliseconds as a `double`.
         */
        [[nodiscard]]
        double elapsed_ms() const {
            return std::chrono::duration<double, std::milli>(Clock::now() - start).count();
        }

        /**
         * @brief Returns seconds elapsed since construction or last @ref reset.
         * @return Elapsed time in seconds as a `double`.
         */
        [[nodiscard]]
        double elapsed_sec() const {
            return std::chrono::duration<double>(Clock::now() - start).count();
        }
    };

} // namespace omni::basic


#endif //BASE_H
