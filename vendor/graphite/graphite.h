/**
* @file graphite.h
 * @brief Graphite — a software-rasterization 2D/3D rendering library.
 *
 * Graphite provides a CPU-side pixel canvas with a full suite of 2D drawing
 * primitives (rectangles, circles, ellipses, lines, triangles, text) as well
 * as a lightweight 3D pipeline (perspective projection, texture mapping,
 * z-buffering, back-face culling, and diffuse shading) built on top of GLM.
 *
 * ### Quick-start
 * @code
 * #include <graphite.h>
 *
 * Graphite::Canvas c(800, 600);
 * c.fill(Graphite::Colors::Black);
 * c.fillCircle(400, 300, 100, Graphite::Colors::Red);
 * c.saveToPNG("out.png");
 * @endcode
 *
 * ### Optional compile-time macros
 * | Macro                        | Effect                                              |
 * |------------------------------|-----------------------------------------------------|
 * | `ALPHA_BLEND`                | Enable alpha-blending on all draw calls             |
 * | `NO_STB_IMPL`                | Skip STB implementation defines (if already defined elsewhere) |
 * | `GRAPHITE_NO_IMPLEMENTATION` | Include declarations only; skip inline bodies       |
 *
 * @author Nico Russo
 * @date   2026-04-14
 */


#ifndef GRAPHITE_H
#define GRAPHITE_H

#include <base.h>

#ifndef NO_STB_IMPL
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb_image.h>

#ifndef NO_STB_IMPL
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif
#include <stb_image_write.h>

#include <font8x8_basic.h>
#include <font_omni.h>

#include <random>
#include <cstdint>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

/**
 * @brief Returns a uniformly-distributed random 32-bit unsigned integer.
 *
 * Uses a thread-local Mersenne-Twister seeded from `std::random_device`.
 * The generator and distribution are static, so the first call pays the
 * seed cost; subsequent calls are cheap.
 *
 * @return A random value in [0, UINT32_MAX].
 */

inline uint32_t randomUint32()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);

    return dist(gen);
}

namespace Graphite {
    using namespace glm;

    /**
     * @brief Axis-aligned rectangle that has been clipped to canvas bounds.
     *
     * Used internally by draw routines to avoid per-pixel bounds checks.
     * After a successful call to normalizeRectangle(), `x1/y1/x2/y2` hold the
     * clipped (safe) extents and `ox1/oy1/ox2/oy2` hold the original,
     * unclipped extents (useful for computing UV offsets).
     */
    struct NormalizedRectangle {
        int32 x1, y1, x2, y2;   ///< Clipped pixel extents (always within canvas bounds).
        int32 ox1, oy1, ox2, oy2; ///< Original, un-clipped extents before canvas intersection.

        /**
         * @brief Clip a rectangle to the canvas and populate both sets of extents.
         *
         * @param x             Left edge of the requested rectangle (may be negative).
         * @param y             Top edge of the requested rectangle (may be negative).
         * @param w             Width of the requested rectangle (negative flips horizontal direction).
         * @param h             Height of the requested rectangle (negative flips vertical direction).
         * @param canvas_width  Width of the target canvas in pixels.
         * @param canvas_height Height of the target canvas in pixels.
         * @return `true` if any part of the rectangle is visible; `false` if it is
         *         entirely off-screen or degenerate (zero width/height).
         */
        bool normalizeRectangle(int32 x, int32 y, int32 w, int32 h, int32 canvas_width, int32 canvas_height);
    };


    // ==========================================
    //                  COLORS
    // ==========================================

    /**
     * @brief RGBA color value stored in a 32-bit word.
     *
     * The union allows the same four bytes to be read as:
     * - `uint32 color`  — packed RGBA word (layout: R in byte 0, G in byte 1, B in byte 2, A in byte 3)
     * - `float32 fColor` — raw float reinterpretation (used for depth buffer storage)
     * - `uint8 r, g, b, a` — individual byte channels
     * - `uint16 u, v`  — two-channel 16-bit representation (UV coordinates packed as color)
     *
     * Alpha defaults to 0xFF (fully opaque) in most constructors.
     *
     * @note When `ALPHA_BLEND` is defined, pixels with `a == 0` are skipped and pixels
     *       with intermediate alpha are blended with the destination.  Without the macro,
     *       every pixel is written regardless of alpha.
     */
    struct Color {
        union {
            uint32 color{};   ///< Packed RGBA word.
            float32 fColor;   ///< Raw float view (used for depth buffer values).
            struct { uint8 r, g, b, a; }; ///< Individual RGBA byte channels.
            struct { uint16 u, v; };      ///< Packed UV / two-channel 16-bit view.
        };

        /// @brief Construct from individual RGBA byte components.
        /// @param r Red channel (0–255).
        /// @param g Green channel (0–255).
        /// @param b Blue channel (0–255).
        /// @param a Alpha channel (0–255); defaults to 0xFF (fully opaque).
        constexpr Color(const uint8 r, const uint8 g, const uint8 b, const uint8 a = 0xFF)
            : r(r), g(g), b(b), a(a) {}

        /// @brief Construct from two packed 16-bit values (UV pair).
        /// @param u First 16-bit component.
        /// @param v Second 16-bit component.
        constexpr Color(const uint16 u, const uint16 v)
            : u(u), v(v) {}

        /// @brief Construct from a packed 32-bit RGBA word.
        /// @param c Packed color; defaults to opaque black (0xFF000000).
        constexpr Color(const uint32 c = 0xFF000000)
            : color(c) {}

        /// @brief Construct from a raw float (used to store depth values in a Canvas z-buffer).
        /// @param c Float value to store verbatim.
        explicit constexpr Color(const float32 c)
            : fColor(c) {}

        /// @brief Implicit conversion to packed `uint32`.
        operator uint32() const { return color; }
        /// @brief Explicit conversion to raw `float32`.
        explicit operator float32() const { return fColor; }

    };

    /**
     * @brief Predefined named Color constants.
     *
     * All constants are `inline constexpr` and fully opaque (alpha = 0xFF)
     * unless otherwise noted.  Use `Colors::Invisible` for a fully transparent
     * pixel (alpha = 0x00).
     */
    namespace Colors {
        inline constexpr Color Red       {0xff, 0x50, 0x50};
        inline constexpr Color Green     {0x50, 0xcc, 0x50};
        inline constexpr Color Blue      {0x50, 0x50, 0xff};
        inline constexpr Color LightBlue {0x88, 0x88, 0xff};
        inline constexpr Color White     {0xff, 0xff, 0xff};
        inline constexpr Color Black     {0x00, 0x00, 0x00};
        inline constexpr Color Yellow    {0xff, 0xff, 0x50};
        inline constexpr Color Pink      {0xff, 0x50, 0xff};
        inline constexpr Color Orange    {0xff, 0x99, 0x50};
        inline constexpr Color Cyan      {0x50, 0xcc, 0xff};
        inline constexpr Color Purple    {0xdd, 0x50, 0xee};
        inline constexpr Color Tan       {0xcc, 0xee, 0xaa};
        inline constexpr Color DarkGrey  {0x18, 0x18, 0x18};
        inline constexpr Color Grey      {0x60, 0x60, 0x60};
        inline constexpr Color LightGrey {0xcc, 0xcc, 0xcc};
        inline constexpr Color Brown     {0x8B, 0x45, 0x13};
        inline constexpr Color Invisible {static_cast<uint32_t>(0x00000000)};
    }

    /**
     * @brief Parse a color name string into a Color.
     *
     * Recognised names are the same identifiers as those in the Colors namespace
     * (case-insensitive, e.g. `"red"`, `"lightblue"`).
     *
     * @param str  Lowercase or mixed-case color name.
     * @return Corresponding Color, or an unspecified default if the name is unknown.
     */
    Color stringToColor(const std::string& str);

    /**
     * @brief Convert HSL color space values to an RGBA Color.
     *
     * @param hueAngleDeg Hue angle in degrees [0, 360).
     * @param satNorm     Saturation normalized to [0, 1].
     * @param lightNorm   Lightness normalized to [0, 1].
     * @param alpha       Alpha channel byte [0, 255].
     * @return Equivalent RGBA Color.
     */
    constexpr Color HSLtoRGB (float32 hueAngleDeg, float32 satNorm, float32 lightNorm, uint8 alpha);

    /**
     * @brief Convert HSV color space values to an RGBA Color.
     *
     * @param hueAngleDeg Hue angle in degrees [0, 360).
     * @param satNorm     Saturation normalized to [0, 1].
     * @param valueNorm   Value (brightness) normalized to [0, 1].
     * @param alpha       Alpha channel byte [0, 255].
     * @return Equivalent RGBA Color.
     */
    constexpr Color HSVtoRGB (float32 hueAngleDeg, float32 satNorm, float32 valueNorm, uint8 alpha);

    /**
     * @brief Alpha-blend a single color channel.
     *
     * Computes `c1 + (c2 - c1) * a / 255` clamped to [0, 255].
     *
     * @param c1 Source channel value (16-bit to avoid overflow).
     * @param c2 Destination channel value.
     * @param a  Alpha weight [0, 255].
     * @return   Blended channel value as uint8.
     */
    uint8_t mixComponent (uint16 c1, uint16 c2, uint16 a);

    /**
     * @brief Alpha-blend two colors using `c2`'s alpha channel.
     *
     * Each channel of the result is `mixComponent(c1.ch, c2.ch, c2.a)`.
     *
     * @param c1 Background color.
     * @param c2 Foreground color (alpha controls blend weight).
     * @return Blended color.
     */
    Color mixColors (Color c1, Color c2);

    /**
     * @brief Linearly interpolate between two colors.
     *
     * @param c1 Start color (`t == 0`).
     * @param c2 End color   (`t == 1`).
     * @param t  Blend factor in [0, 1].
     * @return Interpolated color.
     */
    Color lerpColors (Color c1, Color c2, float t);

    /**
     * @brief Barycentric blend of three colors.
     *
     * Computes `c1*w0 + c2*w1 + c3*w2`.  The weights should sum to 1 for a
     * true barycentric interpolation but are not enforced.
     *
     * @param c1 Color at vertex 0.
     * @param c2 Color at vertex 1.
     * @param c3 Color at vertex 2.
     * @param w0 Barycentric weight for vertex 0.
     * @param w1 Barycentric weight for vertex 1.
     * @param w2 Barycentric weight for vertex 2.
     * @return Blended color.
     */
    Color mixColorsTri (Color c1, Color c2, Color c3, float32 w0, float32 w1, float32 w2);

    /**
     * @brief Barycentric blend of three depth (float) values.
     *
     * Equivalent to `c1*w0 + c2*w1 + c3*w2` in floating point.
     *
     * @param c1 Depth at vertex 0.
     * @param c2 Depth at vertex 1.
     * @param c3 Depth at vertex 2.
     * @param w0 Barycentric weight for vertex 0.
     * @param w1 Barycentric weight for vertex 1.
     * @param w2 Barycentric weight for vertex 2.
     * @return Interpolated depth value.
     */
    float mixDepths(float32 c1, float32 c2, float32 c3, float32 w0, float32 w1, float32 w2);

    // ==========================================
    //                  CANVAS
    // ==========================================

    /**
     * @brief 2D pixel canvas — the central rendering surface.
     *
     * A Canvas owns (or links to) a contiguous array of RGBA pixels laid out in
     * row-major order with a configurable row stride.  All draw methods perform
     * software rasterization directly into this buffer.
     *
     * ### Memory ownership
     * | Construction path          | Who owns `pixels`?                          |
     * |---------------------------|----------------------------------------------|
     * | `Canvas(w, h)` / `(w,h,stride)` | Canvas allocates; destructor frees.  |
     * | `Canvas(filepath)`        | STB allocates; destructor calls `stbi_image_free`. |
     * | `linkCanvas(...)`         | Caller owns; destructor does **not** free.   |
     *
     * ### Alpha blending
     * When the `ALPHA_BLEND` preprocessor macro is defined, all draw calls
     * alpha-blend source pixels onto the existing destination.  Otherwise every
     * covered pixel is overwritten unconditionally.
     *
     * ### Z-buffering
     * A second Canvas whose pixel data is reinterpreted as `float` values can be
     * passed as a z-buffer to the triangle-fill and 3-D draw routines.  Pixels
     * that fail the depth test are discarded.  Inverse-Z (1/z) is stored so that
     * perspective-correct interpolation is a simple linear blend.
     */
    class Canvas {
    public:
        // ====================== CONSTRUCTORS ======================
        /**
         * @brief Allocate a new canvas with an explicit row stride.
         * @param width  Pixel width.
         * @param height Pixel height.
         * @param stride Number of pixels per row in memory (must be >= width).
         */
        Canvas(uint32 width, uint32 height, u32 stride);

        /**
         * @brief Allocate a new canvas with stride equal to width.
         * @param width  Pixel width.
         * @param height Pixel height.
         */
        Canvas(uint32 width, uint32 height);

        /**
         * @brief Load a canvas from an image file (PNG, JPG, BMP, …).
         *
         * Uses STB Image internally.  The resulting canvas dimensions match the
         * image dimensions.  Logs an error and leaves the canvas in an invalid
         * state if the file cannot be opened.
         *
         * @param spriteFilepath Path to the image file.
         */
        explicit Canvas(const std::string& spriteFilepath);

        /// @brief Construct an empty, zero-size canvas (pixels == nullptr).
        Canvas();

        /// @brief Destructor — releases owned pixel memory or STB image data.
        ~Canvas();

        // ====================== INITIALIZERS ======================

        /**
         * @brief Link this canvas to an externally-owned pixel buffer.
         *
         * The canvas does **not** free the buffer on destruction.  Useful for
         * wrapping a framebuffer or window surface.
         *
         * @param pixelLink Pointer to the first pixel; must remain valid for the canvas lifetime.
         * @param width     Buffer width in pixels.
         * @param height    Buffer height in pixels.
         */
        void linkCanvas(uint32* pixelLink, uint32 width, uint32 height);

        /**
         * @brief Reallocate the canvas to new dimensions (stride == width).
         *
         * Frees the previous allocation.  Any previously linked buffer is **not**
         * freed; call this only on owned canvases.
         *
         * @param width  New pixel width.
         * @param height New pixel height.
         */
        void newCanvas(uint32 width, uint32 height);

        /**
         * @brief Reallocate the canvas to new dimensions with an explicit stride.
         * @param width  New pixel width.
         * @param height New pixel height.
         * @param stride New row stride in pixels.
         */
        void newCanvas(uint32 width, uint32 height, uint32 stride);

        // ====================== ACCESSORS =========================
        [[nodiscard]] uint32 getWidth () const;  ///< @return Canvas width in pixels.
        [[nodiscard]] uint32 getHeight() const;  ///< @return Canvas height in pixels.
        [[nodiscard]] uint32 getStride () const; ///< @return Row stride in pixels (pixels per row in memory).
        void setWidth  (uint32 width);   ///< @brief Override the stored width (does not reallocate).
        void setHeight (uint32 height);  ///< @brief Override the stored height (does not reallocate).
        void setStride (uint32 stride);  ///< @brief Override the stored stride (does not reallocate).

        // ===================== SUB-CANVASING ======================
        /**
         * @brief Create a non-owning view into a rectangular region of this canvas.
         *
         * The returned Canvas shares the underlying pixel buffer; writes to it will
         * modify the parent canvas.  The sub-canvas is clipped to the parent bounds.
         *
         * @param x X coordinate of the top-left corner.
         * @param y Y coordinate of the top-left corner.
         * @param w Width of the sub-region (negative flips horizontal).
         * @param h Height of the sub-region (negative flips vertical).
         * @return A linked Canvas view, or an empty canvas if the region is entirely off-screen.
         */
        [[nodiscard]] Canvas subcanvas(int32 x, int32 y, int32 w, int32 h) const;

        /// @overload
        [[nodiscard]] Canvas subcanvas(glm::i32vec2 p, int32 w, int32 h) const;

        // ==================== FULL-CANVAS FILL ====================
        /**
         * @brief Fill the entire canvas with a single color.
         *
         * Uses `memset` when all bytes of the packed color are equal (e.g. solid
         * grey), and `std::fill_n` otherwise.
         *
         * @param color Fill color.
         */
        void fill(Color color) const;

        /**
         * @brief Scale-blit another canvas onto this canvas, covering it entirely.
         *
         * Uses nearest-neighbor sampling.  Transparent source pixels (alpha == 0)
         * are skipped; other pixels respect the `ALPHA_BLEND` macro.
         *
         * @param sourceCanvas The canvas to blit from.
         */
        void blitCanvas(const Canvas& sourceCanvas) const;

        /**
         * @brief Completely clear the canvas to 0x00000000
         */
        void clear() const;

        // ======================== DRAWING =========================

        /**
         * @brief Fill an axis-aligned rectangle with a solid color.
         * @param x0     Left edge.
         * @param y0     Top edge.
         * @param width  Rectangle width (negative flips horizontal direction).
         * @param height Rectangle height (negative flips vertical direction).
         * @param color  Fill color.
         */
        void fillRect(int32 x0, int32 y0, int32 width, int32 height, Color color) const;
        /// @overload
        void fillRect(glm::i32vec2 p0, glm::i32vec2 dim, Color color) const;

        /**
         * @brief Fill a solid circle.
         * @param cx     Center X.
         * @param cy     Center Y.
         * @param radius Radius in pixels.
         * @param color  Fill color.
         */
        void fillCircle(i32 cx, i32 cy, i32 radius, Color color) const;
        /// @overload
        void fillCircle(const glm::i32vec2& p, i32 radius, Color color) const;

        /**
         * @brief Draw the outline of a circle (Bresenham's algorithm).
         * @param cx     Center X.
         * @param cy     Center Y.
         * @param radius Radius in pixels.
         * @param color  Outline color.
         */
        void drawCircle(i32 cx, i32 cy, i32 radius, Color color) const;
        /// @overload
        void drawCircle(const glm::i32vec2& p, i32 radius, Color color) const;

        /**
         * @brief Fill a solid axis-aligned ellipse.
         * @param cx Center X.
         * @param cy Center Y.
         * @param rx Horizontal radius.
         * @param ry Vertical radius.
         * @param color Fill color.
         */
        void fillEllipse(i32 cx, i32 cy, i32 rx, i32 ry, Color color) const;
        /// @overload
        void fillEllipse(const glm::i32vec2& p, const glm::i32vec2& radii, Color color) const;

        /**
         * @brief Fill a triangle with a flat color (no z-buffer).
         *
         * Uses an edge-function rasterizer; both CW and CCW winding are accepted.
         *
         * @param x1,y1 First vertex.
         * @param x2,y2 Second vertex.
         * @param x3,y3 Third vertex.
         * @param color Fill color.
         */
        // 1. flat color, no z-buffer
        void fillTriangle(i32 x1, i32 y1, i32 x2, i32 y2, i32 x3, i32 y3, Color color) const;
        /// @overload
        void fillTriangle(const glm::ivec2& p1, const glm::ivec2& p2, const glm::ivec2& p3, Color color) const;

        /**
         * @brief Fill a triangle with a flat color and optional z-buffer test/write.
         *
         * Inverse-Z values (1/z) are interpolated barycentrically; a fragment is
         * discarded if its interpolated invZ is less than the current z-buffer value.
         *
         * @param x1,y1 First vertex.
         * @param x2,y2 Second vertex.
         * @param x3,y3 Third vertex.
         * @param color  Fill color.
         * @param invZ1  Inverse depth at vertex 1.
         * @param invZ2  Inverse depth at vertex 2.
         * @param invZ3  Inverse depth at vertex 3.
         * @param zBuffer Optional z-buffer canvas (pixels reinterpreted as float); pass nullptr to disable.
         */
        // 1.5. flat color, with z-buffer
        void fillTriangle(i32 x1, i32 y1, i32 x2, i32 y2, i32 x3, i32 y3, Color color,
                                     f32 invZ1, f32 invZ2, f32 invZ3, const Canvas* zBuffer = nullptr) const;
        /// @overload
        void fillTriangle(const glm::ivec2& p1, const glm::ivec2& p2, const glm::ivec2& p3, Color color,
                                     f32 invZ1, f32 invZ2, f32 invZ3, const Canvas* zBuffer = nullptr) const;

        /**
         * @brief Fill a triangle with per-vertex colors and optional z-buffer.
         *
         * Colors are barycentrically interpolated across the triangle face.
         * Pass `invZ1 == invZ2 == invZ3 == 0` and `zBuffer == nullptr` to skip
         * depth testing entirely.
         *
         * @param x1,y1 First vertex.
         * @param x2,y2 Second vertex.
         * @param x3,y3 Third vertex.
         * @param c1    Color at vertex 1.
         * @param c2    Color at vertex 2.
         * @param c3    Color at vertex 3.
         * @param invZ1 Inverse depth at vertex 1 (default 0).
         * @param invZ2 Inverse depth at vertex 2 (default 0).
         * @param invZ3 Inverse depth at vertex 3 (default 0).
         * @param zBuffer Optional z-buffer canvas; nullptr disables depth testing.
         */
        // 2. per-vertex color, optional z-buffer
        void fillTriangle(i32 x1, i32 y1, i32 x2, i32 y2, i32 x3, i32 y3,
                          Color c1, Color c2, Color c3,
                          f32 invZ1 = 0, f32 invZ2 = 0, f32 invZ3 = 0,
                          const Canvas* zBuffer = nullptr) const;
        /// @overload
        void fillTriangle(const glm::ivec2& p1, const glm::ivec2& p2, const glm::ivec2& p3,
                          Color c1, Color c2, Color c3,
                          f32 invZ1 = 0, f32 invZ2 = 0, f32 invZ3 = 0,
                          const Canvas* zBuffer = nullptr) const;

        /**
         * @brief Fill a perspective-correct textured triangle.
         *
         * UV coordinates are supplied pre-divided by Z (`uv/z`) so that
         * barycentric interpolation is linear in screen space.  The final UV
         * is recovered by dividing by the interpolated invZ.
         *
         * @param x1,y1       First screen vertex.
         * @param x2,y2       Second screen vertex.
         * @param x3,y3       Third screen vertex.
         * @param uvOverZ1    UV/Z at vertex 1.
         * @param uvOverZ2    UV/Z at vertex 2.
         * @param uvOverZ3    UV/Z at vertex 3.
         * @param invZ1       1/Z at vertex 1.
         * @param invZ2       1/Z at vertex 2.
         * @param invZ3       1/Z at vertex 3.
         * @param tex         Texture canvas to sample.
         * @param zBuffer     Optional z-buffer canvas; nullptr disables depth testing.
         * @param lerpColor   If >= 0, lerps each texel towards black by this factor (simple diffuse fog). Pass -1 to disable.
         */
        // 3. textured, optional z-buffer (perspective correct)
        void fillTriangleUV(i32 x1, i32 y1, i32 x2, i32 y2, i32 x3, i32 y3,
                            const glm::vec2& uvOverZ1, const glm::vec2& uvOverZ2, const glm::vec2& uvOverZ3,
                            f32 invZ1, f32 invZ2, f32 invZ3,
                            const Canvas& tex, const Canvas* zBuffer = nullptr, float lerpColor = -1.f) const;
        /// @overload
        void fillTriangleUV(const glm::ivec2& p1, const glm::ivec2& p2, const glm::ivec2& p3,
                            const glm::vec2& uvOverZ1, const glm::vec2& uvOverZ2, const glm::vec2& uvOverZ3,
                            f32 invZ1, f32 invZ2, f32 invZ3,
                            const Canvas& tex, const Canvas* zBuffer = nullptr, float lerpColor = -1.f) const;

        /**
         * @brief Compute the minimum distance from a point to a line segment.
         * @param p Query point.
         * @param a Segment start.
         * @param b Segment end.
         * @return Euclidean distance in pixels.
         */
        static float distToSegment(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b);

        /**
         * @brief Draw a filled square point centered at (x, y).
         * @param x      Center X.
         * @param y      Center Y.
         * @param radius Half the side length of the square (in pixels).
         * @param color  Fill color.
         */
        void drawPoint(i32 x, i32 y, i32 radius, Color color) const;
        /// @overload
        void drawPoint(const i32vec2& p, i32 radius, Color color) const;

        /**
         * @brief Draw a 1-pixel-wide line (Bresenham's algorithm).
         * @param x1,y1 Start point.
         * @param x2,y2 End point.
         * @param color Line color.
         */
        void drawLine(i32 x1, i32 y1, i32 x2, i32 y2, Color color) const;
        /// @overload
        void drawLine(const glm::ivec2& p1, const glm::ivec2& p2, Color color) const;

        /**
         * @brief Draw a thick line by testing per-pixel distance to the segment.
         * @param x1,y1     Start point.
         * @param x2,y2     End point.
         * @param color     Line color.
         * @param thickness Full thickness in pixels.
         */
        void drawLine(i32 x1, i32 y1, i32 x2, i32 y2, Color color, float thickness) const;
        /// @overload
        void drawLine(const glm::ivec2& p1, const glm::ivec2& p2, Color color, float thickness) const;

        /**
         * @brief Draw an arrow from one point to another.
         *
         * Draws a line shaft plus two arrowhead segments at the tip.
         *
         * @param fromX,fromY Arrow tail.
         * @param toX,toY     Arrow head tip.
         * @param color       Color.
         * @param thickness   Shaft and head line thickness in pixels.
         * @param headSize    Length of each arrowhead barb in pixels.
         */
        void drawArrow(i32 fromX, i32 fromY, i32 toX, i32 toY, Color color, float thickness, float headSize) const;
        /// @overload
        void drawArrow(const glm::fvec2& from, const glm::fvec2& to, Color color, float thickness, float headSize) const;

        /**
         * @brief Draw another canvas scaled into a rectangular region.
         *
         * Supports both up- and down-scaling via nearest-neighbor sampling.
         * Negative width/height values flip the source image horizontally or vertically.
         *
         * @param x0           Destination top-left X.
         * @param y0           Destination top-left Y.
         * @param width        Destination width (negative = flip horizontal).
         * @param height       Destination height (negative = flip vertical).
         * @param sourceCanvas Source image to sample from.
         */
        void drawCanvas(i32 x0, i32 y0, i32 width, i32 height, const Canvas& sourceCanvas) const;
        /// @overload
        void drawCanvas(const glm::ivec2& p, i32 width, i32 height, const Canvas& sourceCanvas) const;

        /**
         * @brief Render a single ASCII character at a baseline position using the 8×8 bitmap font.
         *
         * The character is scaled to `font_size` × `font_size` pixels.  The
         * baseline is at `y`; the glyph extends upward by `font_size` pixels.
         *
         * @param c         ASCII character to render.
         * @param x         Baseline left edge.
         * @param y         Baseline Y position.
         * @param font_size Pixel size of each font cell.
         * @param color     Glyph color.
         */
        void writeCharBaseline(char c, int32 x, int32 y, int32 font_size, Color color) const;
        /// @overload
        void writeCharBaseline(char c, const glm::i32vec2& p, int32 font_size, Color color) const;

        /**
         * @brief Render a string at a baseline position using the 8×8 bitmap font.
         *
         * Characters are rendered left-to-right with a fixed advance of `font_size`
         * pixels.  No wrapping is performed.
         *
         * @param s         String to render.
         * @param x         Baseline left edge.
         * @param y         Baseline Y position.
         * @param font_size Pixel size of each font cell.
         * @param color     Text color.
         */
        void writeStringBaseline(const std::string& s, int32 x, int32 y, int32 font_size, Color color) const;
        /// @overload
        void writeStringBaseline(const std::string& s, const glm::i32vec2& p, int32 font_size, Color color) const;

        /**
         * @brief Render a single ASCII character using the OmniFont (outline + shadow).
         *
         * Each glyph pixel is classified as outline (0), main fill (1), or shadow (2)
         * and painted with the corresponding color argument.
         *
         * @param c            ASCII character to render.
         * @param x            Baseline left edge.
         * @param y            Baseline Y position.
         * @param font_size    Scale factor (each glyph pixel becomes a font_size × font_size block).
         * @param color_outline Color for outline pixels.
         * @param color_main    Color for filled glyph pixels.
         * @param color_shadow  Color for shadow pixels.
         */
        void writeOmniCharBaseline(char c, int32 x, int32 y, int32 font_size, Color color_outline, Color color_main, Color color_shadow) const;

        /**
         * @brief Render a single OmniFont character with default colors (black outline, white fill, grey shadow).
         * @param c         ASCII character.
         * @param x         Baseline left edge.
         * @param y         Baseline Y position.
         * @param font_size Scale factor.
         */
        void writeOmniCharBaseline(char c, int32 x, int32 y, int32 font_size) const;

        /**
         * @brief Render a string using the OmniFont with default colors.
         *
         * Supports `\n` for newline; advances X by the glyph's proportional move
         * width rather than a fixed pitch, giving variable-width rendering.
         *
         * @param s         String to render.
         * @param x         Baseline left edge.
         * @param y         Baseline Y position.
         * @param font_size Scale factor.
         */
        void writeOmniStringBaseline(const std::string& s, int32 x, int32 y, int32 font_size) const;

        // ====================== FILE OUTPUT =======================
        /**
         * @brief Save the canvas to a PPM (binary P6) file.
         * @param filename Destination path.
         * @return `true` on success, `false` if the file could not be opened.
         */
        [[nodiscard]] bool saveToPPM(const std::string& filename) const;

        /**
         * @brief Save the canvas to a JPEG file via STB Image Write.
         * @param filename Destination path.
         * @param quality  JPEG quality [1, 100]; default 100.
         * @return `true` on success.
         */
        [[nodiscard]] bool saveToJPG(const std::string& filename, int quality = 100) const;

        /**
         * @brief Save the canvas to a PNG file via STB Image Write.
         * @param filename Destination path.
         * @return `true` on success.
         */
        [[nodiscard]] bool saveToPNG(const std::string& filename) const;

        /**
         * @brief Save the canvas to a BMP file via STB Image Write.
         * @param filename Destination path.
         * @return `true` on success.
         */
        [[nodiscard]] bool saveToBMP(const std::string& filename) const;

        /**
         * @brief Read the color of a single pixel.
         * @param x Pixel column (must be < getWidth()).
         * @param y Pixel row    (must be < getHeight()).
         * @return RGBA Color at that position.
         */
        [[nodiscard]] Color getPixelColor(uint32 x, uint32 y) const;

        /**
         * @brief Get a pointer to the raw pixel data
         * @return uint32* pointer to the raw pixel data
         */
        [[nodiscard]] uint32* getPixels() const { return pixels; }

    protected:
        uint32 *pixels; ///< Pointer to the raw pixel data (RGBA, row-major).
        uint32 WIDTH;   ///< Canvas width in pixels.
        uint32 HEIGHT;  ///< Canvas height in pixels.
        uint32 STRIDE;  ///< Row stride — number of pixels between the start of successive rows.

        bool linked = false;          ///< True if this canvas does not own `pixels`.
        unsigned char* rawData = nullptr; ///< Non-null when pixels were loaded by STB (freed via stbi_image_free).

        /// @brief Return a pointer to the pixel at (x, y). No bounds checking.
        [[nodiscard]] uint32* getPixel(uint32 x, uint32 y) const;
        /// @brief Return the flat array index for the pixel at (x, y). No bounds checking.
        [[nodiscard]] uint32 getPixelIndex(uint32 x, uint32 y) const;
    };

    // ==========================================
    //                3-DIMENSIONS
    // ==========================================

    /**
     * @brief Convert a normalized device coordinate to a screen pixel position.
     *
     * The NDC origin is at the canvas center; the shorter axis spans [-1, 1].
     * Y is flipped (positive NDC Y maps to smaller screen Y).
     *
     * @param p      NDC point (x, y in [-1, 1]).
     * @param WIDTH  Canvas width in pixels.
     * @param HEIGHT Canvas height in pixels.
     * @return Pixel-space integer coordinates.
     */
    glm::ivec2 normalizedToScreen(const glm::vec2& p, u32 WIDTH, u32 HEIGHT);

    /**
     * @brief Project a 3D camera-space point onto the 2D projection plane.
     *
     * Performs a simple perspective divide: returns `(x/z, y/z)`.
     *
     * @param p Camera-space point (z must be > 0).
     * @return Projected 2D NDC point.
     */
    glm::vec2 project(const glm::vec3& p);

    /**
     * @brief Translate a point by a vector.
     * @param p           Input point.
     * @param translation Translation vector.
     * @return `p + translation`.
     */
    glm::vec3 translate(const glm::vec3& p, const glm::vec3& translation);

    /**
     * @brief Rotate a point using Euler angles (pitch, yaw, roll).
     *
     * Rotation order is ZYX (roll → yaw → pitch applied right-to-left).
     *
     * @param p        Input point.
     * @param rotation `{pitch (around X), yaw (around Y), roll (around Z)}` in radians.
     * @return Rotated point.
     */
    // rotation: {pitch, yaw, roll} aka {around x, around y, around z}
    glm::vec3 rotate(const glm::vec3& p, const glm::vec3& rotation);

    /**
     * @brief Apply the inverse rotation (transpose of the rotation matrix).
     * @param p        Input point.
     * @param rotation Same convention as rotate().
     * @return Inversely rotated point.
     */
    glm::vec3 rotateInverse(const glm::vec3& p, const glm::vec3& rotation);

    /**
     * @brief Build a 3×3 rotation matrix from Euler angles.
     *
     * Equivalent to the matrix used internally by rotate().
     *
     * @param rotation `{pitch, yaw, roll}` in radians.
     * @return Combined rotation matrix (ZYX order).
     */
    glm::mat3 makeRotationMatrix(const glm::vec3& rotation);

    /**
     * @brief Canonical unit-vector directions in 3D world space.
     *
     * Coordinate convention: Y is up, Z is forward.
     */
    namespace Dir3D {
        inline constexpr f32vec3 UP       {0,  1,  0};
        inline constexpr f32vec3 DOWN     {0, -1,  0};
        inline constexpr f32vec3 RIGHT    {1,  0,  0};
        inline constexpr f32vec3 LEFT     {-1, 0,  0};
        inline constexpr f32vec3 FORWARD  {0,  0,  1};
        inline constexpr f32vec3 BACKWARD {0,  0,  -1};
    }

    /**
     * @brief References into an Object3D's attribute arrays for a single polygon vertex.
     *
     * Indices are 0-based and correspond to the `vertices`, `texCoords`, and
     * `normals` vectors of the owning Object3D.  `randColor` is a random per-face
     * color assigned when loading an OBJ, used by the vertex-color draw path.
     */
    struct FaceIndex {
        i32 vertexIdx{};    ///< Index into Object3D::vertices.
        i32 texCoordIdx{};  ///< Index into Object3D::texCoords.
        i32 normalIdx{};    ///< Index into Object3D::normals.
        Color randColor;    ///< Random color assigned at load time for per-face shading.
    };

    /**
     * @brief A 3D mesh with associated transform and optional texture.
     *
     * Vertices, normals, and texture coordinates are stored in parallel arrays;
     * `faces` contains one entry per polygon, each being a list of FaceIndex
     * values that index into those arrays.
     *
     * The mesh can be rendered via Camera::drawObject() and its variants.
     */
    struct Object3D {
        std::vector<glm::f32vec3> vertices;   ///< Mesh vertex positions in object space.
        std::vector<glm::f32vec3> normals;    ///< Per-vertex or per-face normals.
        std::vector<glm::f32vec2> texCoords;  ///< UV texture coordinates.
        std::vector<std::vector<FaceIndex>> faces; ///< Polygon face list; each face is a triangle fan.
        glm::f32vec3 position{};  ///< World-space translation of the object.
        glm::f32vec3 rotation{};  ///< Euler rotation `{pitch, yaw, roll}` in radians.
        glm::f32vec3 scale{};     ///< Per-axis scale factor (not yet applied by the rasterizer; reserved).
        Canvas* tex = nullptr;    ///< Optional texture; if non-null, drawObject() uses the UV path.
    };

    /**
     * @brief Load an OBJ mesh file into an Object3D.
     *
     * Parses vertex positions (`v`), texture coordinates (`vt`), normals (`vn`),
     * and face definitions (`f`).  Each face vertex receives a random color for
     * use with the vertex-color draw path.
     *
     * @param filePath Path to the `.obj` file.
     * @return Populated Object3D (empty on failure).
     */
    Object3D loadOBJ(const std::filesystem::path& filePath);

    /**
     * @brief Perspective camera used to project and rasterize 3D objects onto a Canvas.
     *
     * The camera is positioned at `position` and oriented by `rotation` (Euler angles).
     * All `draw*` methods accept an optional RenderOptions struct that controls
     * z-buffering, back-face culling, and diffuse lighting.
     *
     * ### Coordinate system
     * - Y is up, Z is forward in world space.
     * - Camera-space Z > 0 is in front of the camera.
     *
     * ### Typical usage
     * @code
     * Graphite::Camera cam{ {0, 0, -5}, {0, 0, 0} };
     * cam.drawObject(myObj, canvas);
     * @endcode
     */
    struct Camera {
        glm::f32vec3 position; ///< World-space camera position.
        glm::f32vec3 rotation; ///< Camera orientation as `{pitch, yaw, roll}` in radians.

        /**
         * @brief Draw the wireframe edges of a 3D object.
         * @param obj       Object to draw.
         * @param c         Target canvas.
         * @param color     Edge color.
         * @param thickness Line thickness in pixels.
         */
        void drawObjectWireframe(const Object3D& obj, const Canvas& c, Color color, f32 thickness) const;

        /**
         * @brief Draw each vertex of a 3D object as a filled square point.
         * @param obj       Object to draw.
         * @param c         Target canvas.
         * @param color     Point color.
         * @param pointSize Half side-length of the square point in pixels.
         */
        void drawObjectVertices(const Object3D& obj, const Canvas& c, Color color, int pointSize) const;

        /**
         * @brief Transform a world-space direction vector into 2D camera-screen space.
         *
         * Applies the inverse camera rotation and returns the X/Y components of
         * the resulting camera-space vector.
         *
         * @param direction World-space direction (does not need to be normalized).
         * @return 2D screen-space direction.
         */
        [[nodiscard]] inline vec2 transformDirection(const glm::vec3 direction) const {
            const glm::mat3 camRotInv = glm::transpose(makeRotationMatrix(rotation));
            const glm::vec3 camSpace  = camRotInv * direction;
            return {camSpace.x, camSpace.y};
        }

        /**
         * @brief Transform a direction from camera space into object (world) space.
         * @param direction Camera-space direction.
         * @return World-space direction.
         */
        [[nodiscard]] glm::vec3 directionObj(const glm::vec3& direction) const;

        /**
         * @brief Transform a direction from object (world) space into camera space.
         * @param direction World-space direction.
         * @return Camera-space direction.
         */
        [[nodiscard]] glm::vec3 directionCam(const glm::vec3& direction) const;

        /// @name Camera-space direction helpers
        /// Convenience accessors for the six cardinal directions in camera space.
        ///@{
        [[nodiscard]] glm::vec3 forwardCam()  const; ///< Camera's forward  (+Z) axis in world space.
        [[nodiscard]] glm::vec3 backwardCam() const; ///< Camera's backward (-Z) axis in world space.
        [[nodiscard]] glm::vec3 rightCam()    const; ///< Camera's right    (+X) axis in world space.
        [[nodiscard]] glm::vec3 leftCam()     const; ///< Camera's left     (-X) axis in world space.
        [[nodiscard]] glm::vec3 upCam()       const; ///< Camera's up       (+Y) axis in world space.
        [[nodiscard]] glm::vec3 downCam()     const; ///< Camera's down     (-Y) axis in world space.
        ///@}

        /// @name Object-space direction helpers
        /// Same six directions expressed in object / world space (inverse of camera-space helpers).
        ///@{
        [[nodiscard]] glm::vec3 forwardObj()  const; ///< World forward  vector from camera's perspective.
        [[nodiscard]] glm::vec3 backwardObj() const;
        [[nodiscard]] glm::vec3 rightObj()    const;
        [[nodiscard]] glm::vec3 leftObj()     const;
        [[nodiscard]] glm::vec3 upObj()       const;
        [[nodiscard]] glm::vec3 downObj()     const;
        ///@}

        /**
         * @brief Options controlling per-draw rendering behavior.
         */
        struct RenderOptions {
            const Canvas* zBuffer = nullptr; ///< Optional depth buffer (pixels reinterpreted as float). nullptr = no depth test.
            bool cullBackface = true;        ///< Discard back-facing triangles (CCW winding = front).
            bool diffuse = false;            ///< Apply simple diffuse (Lambert) shading using `sunVector`.
            fvec3 sunVector = {};            ///< Light direction for diffuse shading (should be normalized).
        };

        /**
         * @brief Draw a 3D object, automatically choosing textured or color-interpolated mode.
         *
         * Uses `drawObjectTexture()` when `obj.tex != nullptr`, otherwise falls
         * back to `drawObjectColor()`.
         *
         * @param obj           Object to render.
         * @param c             Target canvas.
         * @param renderOptions Rendering options.
         */
        void drawObject(const Object3D& obj, const Canvas& c, const RenderOptions& renderOptions) const;
        /// @overload Uses default RenderOptions.
        void drawObject(const Object3D& obj, const Canvas& c) const;

        /**
         * @brief Draw a 3D object with perspective-correct texture mapping.
         * @param obj           Object to render.
         * @param c             Target canvas.
         * @param renderOptions Rendering options.
         */
        void drawObjectTexture(const Object3D& obj, const Canvas& c, const RenderOptions &renderOptions) const;
        /// @overload Uses default RenderOptions.
        void drawObjectTexture(const Object3D& obj, const Canvas& c) const;

        /**
         * @brief Draw a 3D object using per-face random colors (set at load time).
         * @param obj           Object to render.
         * @param c             Target canvas.
         * @param renderOptions Rendering options.
         */
        void drawObjectColor(const Object3D& obj, const Canvas& c, const RenderOptions &renderOptions) const;
        /// @overload Uses default RenderOptions.
        void drawObjectColor(const Object3D& obj, const Canvas& c) const;

        /**
         * @brief Draw a 3D object shaded with per-vertex colors from the face list.
         * @param obj           Object to render.
         * @param c             Target canvas.
         * @param renderOptions Rendering options.
         */
        void drawObjectVertexColor(const Object3D& obj, const Canvas& c, const RenderOptions &renderOptions) const;
        /// @overload Uses default RenderOptions.
        void drawObjectVertexColor(const Object3D& obj, const Canvas& c) const;

        /**
         * @brief Render a depth-visualisation pass (greyscale, brighter = closer).
         *
         * Each pixel's brightness is proportional to its inverse-Z value clamped
         * to [0, 1].
         *
         * @param obj           Object to render.
         * @param c             Target canvas.
         * @param renderOptions Rendering options.
         */
        void drawObjectDepth(const Object3D& obj, const Canvas& c, const RenderOptions& renderOptions) const;
        /// @overload Uses default RenderOptions.
        void drawObjectDepth(const Object3D& obj, const Canvas& c) const;

        /**
         * @brief Draw a 3D object with a single flat color on every face.
         * @param obj           Object to render.
         * @param c             Target canvas.
         * @param color         Fill color.
         * @param renderOptions Rendering options.
         */
        void drawObjectSingleColor(const Object3D& obj, const Canvas& c, const Color& color, const RenderOptions &renderOptions) const;
        /// @overload Uses default RenderOptions.
        void drawObjectSingleColor(const Object3D& obj, const Canvas& c, const Color& color) const;

        /**
         * @brief Project a world-space point onto the canvas and return its NDC coordinates.
         *
         * Returns the raw 2D NDC result; use normalizedToScreen() to convert to pixels.
         *
         * @param positionWorld World-space position.
         * @return NDC (x, y) in [-1, 1].
         */
        [[nodiscard]] vec2 transformPoint(vec3 positionWorld) const;

        /**
         * @brief Convert a world-space sphere radius to a pixel radius at a given center.
         *
         * Useful for rendering billboards or impostor circles whose size should
         * match the apparent size of a 3D sphere.
         *
         * @param centerWorld   World-space center of the sphere.
         * @param radiusWorld   Radius in world units.
         * @param c             The target canvas (needed for aspect-ratio correction).
         * @return Approximate radius in pixels.
         */
        [[nodiscard]] float worldRadiusToPixels(vec3 centerWorld, float radiusWorld, const Canvas& c) const;

    private:
        /// @brief Transformed vertex ready for rasterization.
        struct Vtx {
            glm::ivec2 screen{}; ///< Pixel-space screen position.
            glm::vec3 world{};   ///< World-space position (used for normal computation).
            float invZ{};        ///< Inverse depth (1/z) in camera space.
            bool valid = false;  ///< False if the vertex is behind the camera (z <= 0).
        };

        /**
         * @brief Project all vertices of an object into screen space.
         * @param obj Object whose vertices should be transformed.
         * @param c   Target canvas (supplies dimensions for NDC→pixel conversion).
         * @return Vector of transformed Vtx; invalid entries have `valid == false`.
         */
        [[nodiscard]] std::vector<Vtx> transformVertices(const Object3D& obj, const Canvas& c) const;

        /**
         * @brief Test whether a triangle is back-facing (screen-space winding check).
         * @param v0 First vertex.
         * @param v1 Second vertex.
         * @param v2 Third vertex.
         * @return `true` if the triangle faces away from the camera.
         */
        static bool backfaceCull(const Vtx& v0, const Vtx& v1, const Vtx& v2);
    };



























































// ==========================================
//              IMPLEMENTATIONS
// ==========================================

#ifndef GRAPHITE_NO_IMPLEMENTATION

    inline bool NormalizedRectangle::normalizeRectangle(const int32 x, const int32 y, const int32 w, const int32 h, const int32 canvas_width, const int32 canvas_height) {
        if (w == 0 || h == 0)
            return false;

        // ---- STEP 1: compute ORIGINAL bounds (NO CLAMPING) ----
        ox1 = x; oy1 = y; ox2 = x + w; oy2 = y + h;

        if (ox1 > ox2) std::swap(ox1, ox2);
        if (oy1 > oy2) std::swap(oy1, oy2);

        // ---- STEP 2: cull using ORIGINAL bounds ----
        if (ox1 >= canvas_width || ox2 < 0 ||
            oy1 >= canvas_height || oy2 < 0)
            return false;

        // ---- STEP 3: compute CLIPPED bounds ----
        x1 = glm::max(0, glm::min(ox1, canvas_width - 1));
        y1 = glm::max(0, glm::min(oy1, canvas_height - 1));
        x2 = glm::max(0, glm::min(ox2, canvas_width - 1));
        y2 = glm::max(0, glm::min(oy2, canvas_height - 1));

        // ---- STEP 4: safety check (important) ----
        if (x1 > x2 || y1 > y2)
            return false;

        return true;
    }

    inline Color stringToColor(const std::string &str) {
        std::string lower = str;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if (lower == "red") { return Colors::Red; }
        if (lower == "green") { return Colors::Green; }
        if (lower == "blue") { return Colors::Blue; }
        if (lower == "lightblue") { return Colors::LightBlue; }
        if (lower == "white") { return Colors::White; }
        if (lower == "black") { return Colors::Black; }
        if (lower == "yellow") { return Colors::Yellow; }
        if (lower == "pink") { return Colors::Pink; }
        if (lower == "orange") { return Colors::Orange; }
        if (lower == "cyan") { return Colors::Cyan; }
        if (lower == "purple") { return Colors::Purple; }
        if (lower == "tan") { return Colors::Tan; }
        if (lower == "darkgrey") { return Colors::DarkGrey; }
        if (lower == "grey") { return Colors::Grey; }
        if (lower == "lightgrey") { return Colors::LightGrey; }
        if (lower == "brown") { return Colors::Brown; }
        if (lower == "invisible" ||
            lower == "invis" ||
            lower == "clear") { return Colors::Invisible; }
        omni::LOG_ERROR("invalid color: {}", str);
        return Colors::Pink;
    }

    constexpr Color HSLtoRGB (float32 hueAngleDeg, float32 satNorm, float32 lightNorm, const uint8 alpha = 255) {
        hueAngleDeg = glm::mod(hueAngleDeg, 360.0f);
        if (hueAngleDeg < 0.0f) hueAngleDeg += 360.0f;

        satNorm = glm::clamp(satNorm, 0.0f, 1.0f);
        lightNorm = glm::clamp(lightNorm, 0.0f, 1.0f);

        const float32 c = (1.0f - glm::abs(2.0f * lightNorm - 1.0f)) * satNorm;
        const float32 x = c * (1.0f - glm::abs(glm::mod(hueAngleDeg / 60.0f, 2.0f) - 1.0f));
        const float32 m = lightNorm - c * 0.5f;

        float32 r1 = 0, g1 = 0, b1 = 0;

        if      (hueAngleDeg < 60)  { r1 = c; g1 = x; b1 = 0; }
        else if (hueAngleDeg < 120) { r1 = x; g1 = c; b1 = 0; }
        else if (hueAngleDeg < 180) { r1 = 0; g1 = c; b1 = x; }
        else if (hueAngleDeg < 240) { r1 = 0; g1 = x; b1 = c; }
        else if (hueAngleDeg < 300) { r1 = x; g1 = 0; b1 = c; }
        else              { r1 = c; g1 = 0; b1 = x; }

        const auto r = static_cast<uint8>(glm::round((r1 + m) * 255.0f));
        const auto g = static_cast<uint8>(glm::round((g1 + m) * 255.0f));
        const auto b = static_cast<uint8>(glm::round((b1 + m) * 255.0f));

        return {r, g, b, alpha};
    }
    constexpr Color HSVtoRGB (float32 hueAngleDeg, float32 satNorm, float32 valueNorm, const uint8 alpha = 255) {
        hueAngleDeg = glm::mod(hueAngleDeg, 360.0f);
        if (hueAngleDeg < 0.0f) hueAngleDeg += 360.0f;

        satNorm = glm::clamp(satNorm, 0.0f, 1.0f);
        valueNorm = glm::clamp(valueNorm, 0.0f, 1.0f);

        const float32 c = valueNorm * satNorm;
        const float32 x = c * (1.0f - glm::abs(glm::mod(hueAngleDeg / 60.0f, 2.0f) - 1.0f));
        const float32 m = valueNorm - c;

        float32 r1 = 0, g1 = 0, b1 = 0;

        if      (hueAngleDeg < 60)  { r1 = c; g1 = x; b1 = 0; }
        else if (hueAngleDeg < 120) { r1 = x; g1 = c; b1 = 0; }
        else if (hueAngleDeg < 180) { r1 = 0; g1 = c; b1 = x; }
        else if (hueAngleDeg < 240) { r1 = 0; g1 = x; b1 = c; }
        else if (hueAngleDeg < 300) { r1 = x; g1 = 0; b1 = c; }
        else              { r1 = c; g1 = 0; b1 = x; }

        const auto r = static_cast<uint8>(std::lround((r1 + m) * 255.0f));
        const auto g = static_cast<uint8>(std::lround((g1 + m) * 255.0f));
        const auto b = static_cast<uint8>(std::lround((b1 + m) * 255.0f));

        return {r, g, b, alpha};
    }

    inline uint8_t mixComponent (const uint16 c1, const uint16 c2, const uint16 a) { return c1 + (c2 - c1)*a/255; }
    inline Color mixColors      (const Color c1, const Color c2) {
        return {
            mixComponent(c1.r, c2.r, c2.a),
            mixComponent(c1.g, c2.g, c2.a),
            mixComponent(c1.b, c2.b, c2.a),
            mixComponent(c1.a, c2.a, c2.a),
        };
    }
    inline Color lerpColors      (const Color c1, const Color c2, const float32 t) {
        return {
        omni::lerp(c1.r, c2.r, t),
        omni::lerp(c1.g, c2.g, t),
        omni::lerp(c1.b, c2.b, t),
        omni::lerp(c1.a, c2.a, t),
        };
    }
    inline Color mixColorsTri(const Color c1, const Color c2, const Color c3, const float32 w0, const float32 w1, const float32 w2) {
        const glm::vec4 v = glm::vec4(c1.r, c1.g, c1.b, c1.a) * w0
                          + glm::vec4(c2.r, c2.g, c2.b, c2.a) * w1
                          + glm::vec4(c3.r, c3.g, c3.b, c3.a) * w2;
        return { static_cast<uint8>(v.r), static_cast<uint8>(v.g),
                 static_cast<uint8>(v.b), static_cast<uint8>(v.a) };
    }
    inline float mixDepths      (const float32 c1, const float32 c2, const float32 c3, const float32 w0, const float32 w1, const float32 w2) {
        return c1 * w0 + c2 * w1 + c3 * w2;
    }

    inline Canvas::Canvas  (const uint32 width, const uint32 height, const uint32 stride) {
        pixels = new uint32[width * height];
        WIDTH = width;
        HEIGHT = height;
        STRIDE = stride;
    }
    inline Canvas::Canvas  (const uint32 width, const uint32 height) {
        pixels = new uint32[width * height];
        WIDTH = width;
        HEIGHT = height;
        STRIDE = width;
    }
    inline Canvas::Canvas  (const std::string& spriteFilepath) {
        int32 x;
        int32 y;
        int32 n;
        rawData = stbi_load(spriteFilepath.c_str(), &x, &y, &n, 4);
        if (!rawData) {
            omni::LOG_ERROR("failed to load sprite file {}", spriteFilepath);
            return;
        }
        pixels = reinterpret_cast<uint32*>(rawData);
        WIDTH = static_cast<uint32>(x);
        HEIGHT = static_cast<uint32>(y);
        STRIDE = static_cast<uint32>(x);
    }
    inline Canvas::Canvas  () {
        pixels = nullptr;
        WIDTH = 0;
        HEIGHT = 0;
        STRIDE = 0;
    }
    inline Canvas::~Canvas () {
        if (rawData != nullptr)
            stbi_image_free(rawData);
        else if (!linked)
            delete[] pixels;
    }

    inline void Canvas::linkCanvas (uint32* pixelLink, const uint32 width, const uint32 height) {
        linked = true;
        pixels = pixelLink;
        WIDTH = width;
        HEIGHT = height;
        STRIDE = width;
    }
    inline void Canvas::newCanvas  (const uint32 width, const uint32 height) {
        delete[] pixels;
        pixels = new uint32[width * height];
        WIDTH = width;
        HEIGHT = height;
        STRIDE = width;
    }
    inline void Canvas::newCanvas  (const uint32 width, const uint32 height, const uint32 stride) {
        delete[] pixels;
        pixels = new uint32[width * height];
        WIDTH = width;
        HEIGHT = height;
        STRIDE = stride;
    }

    inline uint32 Canvas::getWidth  () const { return WIDTH; }
    inline uint32 Canvas::getHeight () const { return HEIGHT; }
    inline uint32 Canvas::getStride () const { return STRIDE; }
    inline void   Canvas::setWidth  (const uint32 width) { WIDTH = width; }
    inline void   Canvas::setHeight (const uint32 height) { HEIGHT = height; }
    inline void   Canvas::setStride (const uint32 stride) { STRIDE = stride; }

    inline Canvas Canvas::subcanvas(const int32 x, const int32 y, const int32 w, const int32 h) const {
        NormalizedRectangle nr{};
        Canvas c(0, 0);
        if (!nr.normalizeRectangle(x, y, w, h, static_cast<int32>(WIDTH), static_cast<int32>(HEIGHT))) return c;
        c.pixels = getPixel(x, y);
        c.setWidth(nr.x2 - nr.x1 + 1);
        c.setHeight(nr.y2 - nr.y1 + 1);
        return c;
    }
    inline Canvas Canvas::subcanvas(const glm::i32vec2 p, const int32 w, const int32 h) const {
        return subcanvas(p.x, p.y, w, h);
    }

    inline void Canvas::fill(const Color color) const {
        if (const auto c = static_cast<uint32>(color); (c & 0xFF) == ((c >> 8) & 0xFF) &&
                                                         (c & 0xFF) == ((c >> 16) & 0xFF) &&
                                                         (c & 0xFF) == ((c >> 24) & 0xFF)) {
            memset(pixels, static_cast<int32>(c) & 0xFF, sizeof(uint32) * WIDTH * HEIGHT);
            } else {
                std::fill_n(pixels, WIDTH * HEIGHT, c);
            }
    }

    inline void Canvas::clear() const {
        memset(pixels, 0x00, sizeof(uint32) * WIDTH * HEIGHT);
    }

    inline void Canvas::fillRect(const int32 x0, const int32 y0, const int32 width, const int32 height, const Color color) const {
        NormalizedRectangle nr{};
        if (!nr.normalizeRectangle(x0, y0, width, height, static_cast<int32>(WIDTH), static_cast<int32>(HEIGHT))) return;
        const int32 rowWidth = nr.x2 - nr.x1 + 1;
        for (int32 y = nr.y1; y <= nr.y2; ++y) {
            uint32* row = pixels + y * STRIDE + nr.x1;
#ifdef ALPHA_BLEND
            if (color.a == 0xFF) {
                std::fill_n(row, rowWidth, static_cast<uint32>(color));
            } else {
                for (int32 x = 0; x < rowWidth; ++x)
                    row[x] = static_cast<uint32>(mixColors(Color(row[x]), color));
            }
#else
            std::fill_n(row, rowWidth, static_cast<uint32>(color));
#endif
        }
    }
    inline void Canvas::fillRect(const glm::i32vec2 p0, const glm::i32vec2 dim, const Color color) const {
        fillRect(p0.x, p0.y, dim.x, dim.y, color);
    }

    inline void Canvas::fillCircle(const i32 cx, const i32 cy, const i32 radius, const Color color) const {
        const i32 r2 = radius * radius;
        const i32 y1 = glm::max(0, cy - radius);
        const i32 y2 = glm::min(static_cast<i32>(HEIGHT) - 1, cy + radius);

        for (i32 y = y1; y <= y2; ++y) {
            const i32 dy  = y - cy;
            const i32 dy2 = dy * dy;
            const i32 xSpan = static_cast<i32>(sqrtf(static_cast<f32>(r2 - dy2)));

            const i32 xStart = glm::max(0,                          cx - xSpan);
            const i32 xEnd   = glm::min(static_cast<i32>(WIDTH) - 1, cx + xSpan);

            uint32* row = pixels + y * STRIDE;

#ifdef ALPHA_BLEND
            if (color.a == 0xFF) {
                std::fill_n(row + xStart, xEnd - xStart + 1, static_cast<u32>(color));
            } else {
                for (i32 x = xStart; x <= xEnd; ++x)
                    row[x] = static_cast<u32>(mixColors(Color(row[x]), color));
            }
#else
            std::fill_n(row + xStart, xEnd - xStart + 1, static_cast<u32>(color));
#endif
        }
    }
    inline void Canvas::fillCircle(const glm::i32vec2& p, const i32 radius, const Color color) const {
        fillCircle(p.x, p.y, radius, color);
    }

    inline void Canvas::drawCircle(const i32 cx, const i32 cy, const i32 radius, const Color color) const {
        auto writePixel = [&](i32 x, i32 y) {
            if (x < 0 || x >= static_cast<i32>(WIDTH))  return;
            if (y < 0 || y >= static_cast<i32>(HEIGHT)) return;
            uint32* row = pixels + y * STRIDE;
#ifdef ALPHA_BLEND
            if (color.a == 0xFF)
                row[x] = static_cast<u32>(color);
            else
                row[x] = static_cast<u32>(mixColors(Color(row[x]), color));
#else
            row[x] = static_cast<u32>(color);
#endif
        };

        // Mirror one octant point into all 8 octants
        auto plot8 = [&](i32 dx, i32 dy) {
            writePixel(cx + dx, cy + dy);
            writePixel(cx - dx, cy + dy);
            writePixel(cx + dx, cy - dy);
            writePixel(cx - dx, cy - dy);
            writePixel(cx + dy, cy + dx);
            writePixel(cx - dy, cy + dx);
            writePixel(cx + dy, cy - dx);
            writePixel(cx - dy, cy - dx);
        };

        i32 dx = 0;
        i32 dy = radius;
        i32 d  = 3 - 2 * radius;  // initial decision variable

        while (dx <= dy) {
            plot8(dx, dy);

            if (d < 0) {
                d += 4 * dx + 6;
            } else {
                d += 4 * (dx - dy) + 10;
                --dy;
            }
            ++dx;
        }
    }
    inline void Canvas::drawCircle (const glm::i32vec2& p, const i32 radius, const Color color) const {
        drawCircle(p.x, p.y, radius, color);
    }

    inline void Canvas::fillEllipse(const i32 cx, const i32 cy, const i32 rx, const i32 ry, const Color color) const {
        const i32 y1 = glm::max(0, cy - ry);
        const i32 y2 = glm::min(static_cast<i32>(HEIGHT) - 1, cy + ry);

        const f32 ry2 = static_cast<f32>(ry * ry);

        for (i32 y = y1; y <= y2; ++y) {
            const f32 dy = static_cast<f32>(y - cy);
            const i32 xSpan = static_cast<i32>(static_cast<f32>(rx) * sqrtf(glm::max(0.0f, 1.0f - (dy * dy) / ry2)));

            const i32 xStart = glm::max(0,                           cx - xSpan);
            const i32 xEnd   = glm::min(static_cast<i32>(WIDTH) - 1, cx + xSpan);

            uint32* row = pixels + y * STRIDE;

#ifdef ALPHA_BLEND
            if (color.a == 0xFF) {
                std::fill_n(row + xStart, xEnd - xStart + 1, static_cast<u32>(color));
            } else {
                for (i32 x = xStart; x <= xEnd; ++x)
                    row[x] = static_cast<u32>(mixColors(Color(row[x]), color));
            }
#else
            std::fill_n(row + xStart, xEnd - xStart + 1, static_cast<u32>(color));
#endif
        }
    }
    inline void Canvas::fillEllipse(const glm::i32vec2& p, const glm::i32vec2& radii, const Color color) const {
        fillEllipse(p.x, p.y, radii.x, radii.y, color);
    }

    inline Color sampleUV(const glm::vec2& uv, const Canvas& tex) {
        const f32 u = 1.0f - glm::clamp(uv.x, 0.001f, 0.999f);
        const f32 v = 1.0f - glm::clamp(uv.y, 0.001f, 0.999f);
        return tex.getPixelColor(
            static_cast<u32>(u * static_cast<f32>(tex.getWidth())),
            static_cast<u32>(v * static_cast<f32>(tex.getHeight()))
        );
    };

    inline void blendPixel(u32& px, const Color c) {
#ifdef ALPHA_BLEND
        if (c.a == 0xFF)
            px = static_cast<u32>(c);
        else
            px = static_cast<u32>(mixColors(Color(px), c));
#else
        px = static_cast<u32>(c);
#endif
    }

    inline void Canvas::fillTriangle(i32 x1, i32 y1, i32 x2, i32 y2, i32 x3, i32 y3, const Color color) const {
        auto edge = [](i32 ax, i32 ay, i32 bx, i32 by, i32 x, i32 y) {
            return (x - ax) * (by - ay) - (y - ay) * (bx - ax);
        };
        const int area = edge(x1, y1, x2, y2, x3, y3);
        if (area == 0) return;

        const int minX = std::max(0,                            std::min({x1, x2, x3}));
        const int maxX = std::min(static_cast<int>(WIDTH) - 1,  std::max({x1, x2, x3}));
        const int minY = std::max(0,                            std::min({y1, y2, y3}));
        const int maxY = std::min(static_cast<int>(HEIGHT) - 1, std::max({y1, y2, y3}));
        if (minX > maxX || minY > maxY) return;

        const int dy12 = y3 - y2, dy20 = y1 - y3, dy01 = y2 - y1;
        const int dx12 = x3 - x2, dx20 = x1 - x3, dx01 = x2 - x1;

        int w0_row = edge(x2, y2, x3, y3, minX, minY);
        int w1_row = edge(x3, y3, x1, y1, minX, minY);
        int w2_row = edge(x1, y1, x2, y2, minX, minY);

        for (int y = minY; y <= maxY; ++y) {
            int w0 = w0_row, w1 = w1_row, w2 = w2_row;
            u32* row = pixels + y * STRIDE;
            for (int x = minX; x <= maxX; ++x) {
                if ((w0|w1|w2) >= 0 || ((-w0)|(-w1)|(-w2)) >= 0) // see note
                if ((w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0)) {
                    blendPixel(row[x], color);
                } w0 += dy12; w1 += dy20; w2 += dy01;
            }
            w0_row -= dx12; w1_row -= dx20; w2_row -= dx01;
        }
    }
    inline void Canvas::fillTriangle(const glm::ivec2& p1, const glm::ivec2& p2, const glm::ivec2& p3, const Color color) const {
        fillTriangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, color);
    }

    inline void Canvas::fillTriangle(i32 x1, i32 y1, i32 x2, i32 y2, i32 x3, i32 y3,
                                     const Color color,
                                     const f32 invZ1, const f32 invZ2, const f32 invZ3,
                                     const Canvas* zBuffer) const {
        PROFILE_FUNCTION();
        auto edge = [](i32 ax, i32 ay, i32 bx, i32 by, i32 x, i32 y) {
            return (x - ax) * (by - ay) - (y - ay) * (bx - ax);
        };

        const int area = edge(x1, y1, x2, y2, x3, y3);
        if (area == 0) return;

        const float invArea = 1.0f / static_cast<f32>(area);

        const int minX = std::max(0,                            std::min({x1, x2, x3}));
        const int maxX = std::min(static_cast<int>(WIDTH) - 1,  std::max({x1, x2, x3}));
        const int minY = std::max(0,                            std::min({y1, y2, y3}));
        const int maxY = std::min(static_cast<int>(HEIGHT) - 1, std::max({y1, y2, y3}));
        if (minX > maxX || minY > maxY) return;

        const int dy12 = y3 - y2, dy20 = y1 - y3, dy01 = y2 - y1;
        const int dx12 = x3 - x2, dx20 = x1 - x3, dx01 = x2 - x1;

        int w0_row = edge(x2, y2, x3, y3, minX, minY);
        int w1_row = edge(x3, y3, x1, y1, minX, minY);
        int w2_row = edge(x1, y1, x2, y2, minX, minY);

        for (int y = minY; y <= maxY; ++y) {
            int w0 = w0_row, w1 = w1_row, w2 = w2_row;
            u32* row = pixels + y * STRIDE;
            f32* zRow = zBuffer ? reinterpret_cast<f32*>(zBuffer->pixels) + y * STRIDE : nullptr;

            for (int x = minX; x <= maxX; ++x) {
                if ((w0 >= 0 && w1 >= 0 && w2 >= 0) ||
                    (w0 <= 0 && w1 <= 0 && w2 <= 0)) {

                    if (zRow) {
                        const f32 b0 = static_cast<f32>(w0) * invArea;
                        const f32 b1 = static_cast<f32>(w1) * invArea;
                        const f32 b2 = static_cast<f32>(w2) * invArea;

                        const f32 invZ = invZ1 * b0 + invZ2 * b1 + invZ3 * b2;
                        if (invZ < zRow[x]) {
                            w0 += dy12; w1 += dy20; w2 += dy01;
                            continue;
                        }
                        zRow[x] = invZ;
                    }

                    blendPixel(row[x], color);
                }

                w0 += dy12;
                w1 += dy20;
                w2 += dy01;
            }

            w0_row -= dx12;
            w1_row -= dx20;
            w2_row -= dx01;
        }
    }
    inline void Canvas::fillTriangle(const glm::ivec2& p1, const glm::ivec2& p2, const glm::ivec2& p3,
                      const Color color,
                      const f32 invZ1, const f32 invZ2, const f32 invZ3,
                      const Canvas* zBuffer) const {
        fillTriangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, color, invZ1, invZ2, invZ3, zBuffer);
    }

    inline void Canvas::fillTriangle(i32 x1, i32 y1, i32 x2, i32 y2, i32 x3, i32 y3,
                      const Color c1, const Color c2, const Color c3,
                      const f32 invZ1, const f32 invZ2, const f32 invZ3,
                      const Canvas* zBuffer) const {
        auto edge = [](i32 ax, i32 ay, i32 bx, i32 by, i32 x, i32 y) {
            return (x - ax) * (by - ay) - (y - ay) * (bx - ax);
        };
        const int area = edge(x1, y1, x2, y2, x3, y3);
        if (area == 0) return;
        const float invArea = 1.0f / static_cast<f32>(area);

        const int minX = std::max(0,                            std::min({x1, x2, x3}));
        const int maxX = std::min(static_cast<int>(WIDTH) - 1,  std::max({x1, x2, x3}));
        const int minY = std::max(0,                            std::min({y1, y2, y3}));
        const int maxY = std::min(static_cast<int>(HEIGHT) - 1, std::max({y1, y2, y3}));
        if (minX > maxX || minY > maxY) return;

        const int dy12 = y3-y2, dy20 = y1-y3, dy01 = y2-y1;
        const int dx12 = x3-x2, dx20 = x1-x3, dx01 = x2-x1;

        int w0_row = edge(x2, y2, x3, y3, minX, minY);
        int w1_row = edge(x3, y3, x1, y1, minX, minY);
        int w2_row = edge(x1, y1, x2, y2, minX, minY);

        for (int y = minY; y <= maxY; ++y) {
            int w0 = w0_row, w1 = w1_row, w2 = w2_row;
            u32*  row  = pixels + y * STRIDE;
            f32*  zRow = zBuffer ? reinterpret_cast<f32*>(zBuffer->pixels) + y * STRIDE : nullptr;

            for (int x = minX; x <= maxX; ++x) {
                if ((w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0)) {
                    const float b0 = static_cast<f32>(w0) * invArea;
                    const float b1 = static_cast<f32>(w1) * invArea;
                    const float b2 = static_cast<f32>(w2) * invArea;

                    if (zRow) {
                        const f32 invZ = invZ1*b0 + invZ2*b1 + invZ3*b2;
                        if (invZ < zRow[x]) { w0+=dy12; w1+=dy20; w2+=dy01; continue; }
                        zRow[x] = invZ;
                    }

                    Color c = mixColorsTri(c1, c2, c3, b0, b1, b2);

                    if (c.a == 0) continue;

                    blendPixel(row[x], mixColorsTri(c1, c2, c3, b0, b1, b2));
                }
                w0 += dy12; w1 += dy20; w2 += dy01;
            }
            w0_row -= dx12; w1_row -= dx20; w2_row -= dx01;
        }
    }
    inline void Canvas::fillTriangle(const glm::ivec2& p1, const glm::ivec2& p2, const glm::ivec2& p3,
                      const Color c1, const Color c2, const Color c3,
                      const f32 invZ1, const f32 invZ2, const f32 invZ3,
                      const Canvas* zBuffer) const {
        fillTriangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, c1, c2, c3, invZ1, invZ2, invZ3, zBuffer);
    }


    inline void Canvas::fillTriangleUV(i32 x1, i32 y1, i32 x2, i32 y2, i32 x3, i32 y3,
                        const glm::vec2& uvOverZ1, const glm::vec2& uvOverZ2, const glm::vec2& uvOverZ3,
                        const f32 invZ1, const f32 invZ2, const f32 invZ3,
                        const Canvas& tex, const Canvas* zBuffer, float lerpColor) const {
        auto edge = [](i32 ax, i32 ay, i32 bx, i32 by, i32 x, i32 y) {
            return (x - ax) * (by - ay) - (y - ay) * (bx - ax);
        };
        const int area = edge(x1, y1, x2, y2, x3, y3);
        if (area == 0) return;
        const float invArea = 1.0f / static_cast<f32>(area);

        const int minX = std::max(0,                            std::min({x1, x2, x3}));
        const int maxX = std::min(static_cast<int>(WIDTH) - 1,  std::max({x1, x2, x3}));
        const int minY = std::max(0,                            std::min({y1, y2, y3}));
        const int maxY = std::min(static_cast<int>(HEIGHT) - 1, std::max({y1, y2, y3}));
        if (minX > maxX || minY > maxY) return;

        const int dy12 = y3-y2, dy20 = y1-y3, dy01 = y2-y1;
        const int dx12 = x3-x2, dx20 = x1-x3, dx01 = x2-x1;

        int w0_row = edge(x2, y2, x3, y3, minX, minY);
        int w1_row = edge(x3, y3, x1, y1, minX, minY);
        int w2_row = edge(x1, y1, x2, y2, minX, minY);

        for (int y = minY; y <= maxY; ++y) {
            int w0 = w0_row, w1 = w1_row, w2 = w2_row;
            u32* row  = pixels + y * STRIDE;
            f32* zRow = zBuffer ? reinterpret_cast<f32*>(zBuffer->pixels) + y * STRIDE : nullptr;

            for (int x = minX; x <= maxX; ++x) {
                if ((w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0)) {
                    const float b0 = static_cast<f32>(w0) * invArea;
                    const float b1 = static_cast<f32>(w1) * invArea;
                    const float b2 = static_cast<f32>(w2) * invArea;

                    const f32 invZ = invZ1*b0 + invZ2*b1 + invZ3*b2;

                    if (zRow) {
                        if (invZ < zRow[x]) { w0+=dy12; w1+=dy20; w2+=dy01; continue; }
                        zRow[x] = invZ;
                    }

                    const glm::vec2 uv = (uvOverZ1*b0 + uvOverZ2*b1 + uvOverZ3*b2) / invZ;
                    Color c = (lerpColor != -1) ? lerpColors(sampleUV(uv, tex), Colors::Black, lerpColor) : sampleUV(uv, tex);

                    blendPixel(row[x], c);
                }
                w0 += dy12; w1 += dy20; w2 += dy01;
            }
            w0_row -= dx12; w1_row -= dx20; w2_row -= dx01;
        }
    }

    /*
    inline void Canvas::fillTriangleUV(i32 x1_, i32 y1_, i32 x2_, i32 y2_, i32 x3_, i32 y3_,
                    const glm::vec2& uvOverZ1_, const glm::vec2& uvOverZ2_, const glm::vec2& uvOverZ3_,
                    const f32 invZ1_, const f32 invZ2_, const f32 invZ3_,
                    const Canvas& tex, const Canvas* zBuffer, float lerpColor) const {
    PROFILE_FUNCTION();
        auto edge = [](i32 ax, i32 ay, i32 bx, i32 by, i32 x, i32 y) {
            return (x - ax) * (by - ay) - (y - ay) * (bx - ax);
        };
        const int area = edge(x1_, y1_, x2_, y2_, x3_, y3_);
        if (area == 0) return;
        const float invArea = 1.0f / static_cast<f32>(area);

        i32 x1=x1_, y1=y1_, x2=x2_, y2=y2_, x3=x3_, y3=y3_;
        f32 invZ1=invZ1_, invZ2=invZ2_, invZ3=invZ3_;
        glm::vec2 uvOverZ1=uvOverZ1_, uvOverZ2=uvOverZ2_, uvOverZ3=uvOverZ3_;

        // Sort vertices by Y (top to bottom): p0 = top, p2 = bottom
        if (y1 > y2) { std::swap(x1,x2); std::swap(y1,y2); std::swap(invZ1,invZ2); std::swap(uvOverZ1,uvOverZ2); }
        if (y1 > y3) { std::swap(x1,x3); std::swap(y1,y3); std::swap(invZ1,invZ3); std::swap(uvOverZ1,uvOverZ3); }
        if (y2 > y3) { std::swap(x2,x3); std::swap(y2,y3); std::swap(invZ2,invZ3); std::swap(uvOverZ2,uvOverZ3); }
        // now y1 <= y2 <= y3

        const int minY = std::max(y1, 0);
        const int maxY = std::min(y3, static_cast<int>(HEIGHT) - 1);
        if (minY > maxY) return;

        // Attribute gradients (same derivation as before)
        const float dInvZ_dx  =  (invZ1*(float)(y3-y2)     + invZ2*(float)(y1-y3)     + invZ3*(float)(y2-y1))     * invArea;
        const float dInvZ_dy  = -(invZ1*(float)(x3-x2)     + invZ2*(float)(x1-x3)     + invZ3*(float)(x2-x1))     * invArea;
        const float dUVZx_dx  =  (uvOverZ1.x*(float)(y3-y2) + uvOverZ2.x*(float)(y1-y3) + uvOverZ3.x*(float)(y2-y1)) * invArea;
        const float dUVZx_dy  = -(uvOverZ1.x*(float)(x3-x2) + uvOverZ2.x*(float)(x1-x3) + uvOverZ3.x*(float)(x2-x1)) * invArea;
        const float dUVZy_dx  =  (uvOverZ1.y*(float)(y3-y2) + uvOverZ2.y*(float)(y1-y3) + uvOverZ3.y*(float)(y2-y1)) * invArea;
        const float dUVZy_dy  = -(uvOverZ1.y*(float)(x3-x2) + uvOverZ2.y*(float)(x1-x3) + uvOverZ3.y*(float)(x2-x1)) * invArea;

        // Attribute values at vertex 0 (top)
        const float invZ_v0  = invZ1;
        const float uvZx_v0  = uvOverZ1.x;
        const float uvZy_v0  = uvOverZ1.y;

        const bool doLerp = (lerpColor != -1.0f);
        const float texW  = static_cast<float>(tex.getWidth());
        const float texH  = static_cast<float>(tex.getHeight());

        // Edge slopes: x advances per y step along each edge
        // Long edge: v0->v2, upper short: v0->v1, lower short: v1->v2
        const float dyTotal = (float)(y3 - y1);
        const float dyUpper = (float)(y2 - y1);
        const float dyLower = (float)(y3 - y2);

        const float slopeLong  = dyTotal > 0 ? (float)(x3 - x1) / dyTotal : 0.0f;
        const float slopeUpper = dyUpper > 0 ? (float)(x2 - x1) / dyUpper : 0.0f;
        const float slopeLower = dyLower > 0 ? (float)(x3 - x2) / dyLower : 0.0f;

        // Determine which side the long edge is on
        // midpoint of long edge at y2, compare to x2
        const float xMid = x1 + slopeLong * dyUpper;
        const bool longOnRight = xMid >= (float)x2;

        // We'll walk two x cursors per row: xLeft, xRight
        // Upper half: y1..y2, Lower half: y2..y3
        for (int half = 0; half < 2; ++half) {
            const int   yStart  = (half == 0) ? y1   : y2;
            const int   yEnd    = (half == 0) ? y2   : y3;
            const float xShort0 = (half == 0) ? (float)x1 : (float)x2;
            const float slopeShort = (half == 0) ? slopeUpper : slopeLower;

            // Long edge x at yStart
            const float xLong0 = (float)x1 + slopeLong * (float)(yStart - y1);

            const int rowStart = std::max(yStart, minY);
            const int rowEnd   = std::min(yEnd,   static_cast<int>(HEIGHT) - 1);

            for (int y = rowStart; y <= rowEnd; ++y) {
                const float t = (float)(y - yStart);
                float xLong  = xLong0  + slopeLong  * t;
                float xShort = xShort0 + slopeShort * t;

                int xL = (int)(longOnRight ? xShort : xLong);
                int xR = (int)(longOnRight ? xLong  : xShort);
                xL = std::max(xL, 0);
                xR = std::min(xR, static_cast<int>(WIDTH) - 1);
                if (xL > xR) continue;

                // Compute attributes at (xL, y) from vertex 0
                const float dy = (float)(y  - y1);
                const float dx = (float)(xL - x1);
                float invZ_x = invZ_v0  + dInvZ_dx  * dx + dInvZ_dy  * dy;
                float uvZx_x = uvZx_v0  + dUVZx_dx  * dx + dUVZx_dy  * dy;
                float uvZy_x = uvZy_v0  + dUVZy_dx  * dx + dUVZy_dy  * dy;

                u32* row  = pixels + y * STRIDE;
                f32* zRow = zBuffer ? reinterpret_cast<f32*>(zBuffer->pixels) + y * STRIDE : nullptr;

                for (int x = xL; x <= xR; ++x) {
                    if (!zRow || invZ_x >= zRow[x]) {
                        if (zRow) zRow[x] = invZ_x;

                        const float z = 1.0f / invZ_x;
                        const float u = 1.0f - glm::clamp(uvZx_x * z, 0.001f, 0.999f);
                        const float v = 1.0f - glm::clamp(uvZy_x * z, 0.001f, 0.999f);

                        Color c = tex.getPixelColor(
                            static_cast<u32>(u * texW),
                            static_cast<u32>(v * texH)
                        );
                        if (doLerp) c = lerpColors(c, Colors::Black, lerpColor);
                        blendPixel(row[x], c);
                    }

                    invZ_x += dInvZ_dx;
                    uvZx_x += dUVZx_dx;
                    uvZy_x += dUVZy_dx;
                }
            }
        }
    }
    */

    inline void Canvas::fillTriangleUV(const glm::ivec2& p1, const glm::ivec2& p2, const glm::ivec2& p3,
                        const glm::vec2& uvOverZ1, const glm::vec2& uvOverZ2, const glm::vec2& uvOverZ3,
                        const f32 invZ1, const f32 invZ2, const f32 invZ3,
                        const Canvas& tex, const Canvas* zBuffer, float lerpColor) const {
        fillTriangleUV(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, uvOverZ1, uvOverZ2, uvOverZ3, invZ1, invZ2, invZ3, tex, zBuffer, lerpColor);
    }

    inline void Canvas::drawPoint(const i32 x, const i32 y, const i32 radius, const Color color) const {
        const i32 rx2 = radius * 2;
        fillRect(x - radius, y - radius, rx2, rx2, color);
    }
    inline void Canvas::drawPoint(const i32vec2& p, const i32 radius, const Color color) const {
        drawPoint(p.x, p.y, radius, color);
    }

    inline void Canvas::drawLine(i32 x1, i32 y1, const i32 x2, const i32 y2, const Color color) const {
        const i32 dx  =  glm::abs(x2 - x1);
        const i32 dy  =  glm::abs(y2 - y1);
        const i32 sx  = (x1 < x2) ? 1 : -1;
        const i32 sy  = (y1 < y2) ? 1 : -1;
        i32 err = dx - dy;

        while (true) {
            if (static_cast<u32>(x1) < WIDTH && static_cast<u32>(y1) < HEIGHT) {
                uint32& px = pixels[y1 * STRIDE + x1];
#ifdef ALPHA_BLEND
                if (color.a == 0xFF)
                    px = static_cast<u32>(color);
                else
                    px = static_cast<u32>(mixColors(Color(px), color));
#else
                px = static_cast<u32>(color);
#endif
            }

            if (x1 == x2 && y1 == y2) break;

            const i32 e2 = 2 * err;
            if (e2 > -dy) { err -= dy; x1 += sx; }
            if (e2 <  dx) { err += dx; y1 += sy; }
        }
    }
    inline void Canvas::drawLine(const glm::ivec2& p1, const glm::ivec2& p2, const Color color) const {
        drawLine(p1.x, p1.y, p2.x, p2.y, color);
    }

    inline float Canvas::distToSegment(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b) {
        const glm::vec2 v  = b - a;
        const glm::vec2 w  = p - a;
        const float     c1 = glm::dot(w, v);
        if (c1 <= 0) return glm::length(w);
        const float c2 = glm::dot(v, v);
        if (c2 <= c1) return glm::length(p - b);
        return glm::length(p - (a + (c1 / c2) * v));
    }
    inline void Canvas::drawLine(const i32 x1, const i32 y1, const i32 x2, const i32 y2, const Color color, const float thickness) const {
        const float r = thickness * 0.5f;

        const int minX = glm::max(0,                  (int)glm::min(x1, x2) - static_cast<int>(thickness));
        const int maxX = glm::min(static_cast<int>(WIDTH)  - 1,    (int)glm::max(x1, x2) + static_cast<int>(thickness));
        const int minY = glm::max(0,                  (int)glm::min(y1, y2) - static_cast<int>(thickness));
        const int maxY = glm::min(static_cast<int>(HEIGHT) - 1,    (int)glm::max(y1, y2) + static_cast<int>(thickness));

        const glm::vec2 a(x1, y1);
        const glm::vec2 b(x2, y2);
        const float r2 = r * r;

        const glm::vec2 v  = b - a;
        const float     c2 = glm::dot(v, v);

        for (int y = minY; y <= maxY; ++y) {
            uint32* row = pixels + y * STRIDE;
            //const glm::vec2 rowP(0.0f, static_cast<float>(y));
            for (int x = minX; x <= maxX; ++x) {
                const glm::vec2 w = glm::vec2(x, y) - a;
                const float c1 = glm::dot(w, v);
                float d2;
                if      (c1 <= 0)  d2 = glm::dot(w, w);
                else if (c2 <= c1) d2 = glm::dot(glm::vec2(x,y) - b, glm::vec2(x,y) - b);
                else               d2 = glm::dot(w, w) - (c1 * c1) / c2;

                if (d2 <= r2) {
                    uint32& px = row[x];
    #ifdef ALPHA_BLEND
                    if (color.a == 0xFF)
                        px = static_cast<u32>(color);
                    else
                        px = static_cast<u32>(mixColors(Color(px), color));
    #else
                    px = static_cast<u32>(color);
    #endif
                }
            }
        }
    }
    inline void Canvas::drawLine(const glm::ivec2& p1, const glm::ivec2& p2, const Color color, const float thickness) const {
        drawLine(p1.x, p1.y, p2.x, p2.y, color, thickness);
    }

    inline void Canvas::drawArrow(i32 fromX, i32 fromY, i32 toX, i32 toY, const Color color, const float thickness, const float headSize) const {
        drawArrow({fromX, fromY}, {toX, toY}, color, thickness, headSize);
    }

    inline void Canvas::drawArrow(const glm::fvec2 &from, const glm::fvec2 &to, const Color color, const float thickness, const float headSize) const {
        const glm::vec2 dir  = glm::normalize(to - from) * headSize;

        const glm::vec2 rotatedPos = glm::rotate(dir,  glm::radians(145.0f));
        const glm::vec2 rotatedNeg = glm::rotate(dir, -glm::radians(145.0f));

        drawLine(from, to, color, thickness);
        drawLine(to, to + rotatedPos, color, thickness);
        drawLine(to, to + rotatedNeg, color, thickness);
    }

    inline void Canvas::drawCanvas(const i32 x0, const i32 y0, const i32 width, const i32 height, const Canvas& sourceCanvas) const {
        if (sourceCanvas.getWidth() == 0 || sourceCanvas.getHeight() == 0) return;

        NormalizedRectangle nr{};
        if (!nr.normalizeRectangle(x0, y0, width, height, static_cast<i32>(WIDTH), static_cast<i32>(HEIGHT))) return;

        const i32 xa = (width  < 0) ? nr.ox2 : nr.ox1;
        const i32 ya = (height < 0) ? nr.oy2 : nr.oy1;

        const i32 srcW = static_cast<i32>(sourceCanvas.getWidth());
        const i32 srcH = static_cast<i32>(sourceCanvas.getHeight());

        for (i32 y = nr.y1; y <= nr.y2; ++y) {
            uint32* dstRow = pixels + y * STRIDE;
            u32 ny = static_cast<u32>((y - ya) * srcH / height);
            if (ny >= static_cast<u32>(srcH)) ny = srcH - 1;

            for (i32 x = nr.x1; x <= nr.x2; ++x) {
                u32 nx = static_cast<u32>((x - xa) * srcW / width);
                if (nx >= static_cast<u32>(srcW)) nx = srcW - 1;

                const Color sourcePixel = sourceCanvas.getPixelColor(nx, ny);
                if (sourcePixel.a == 0) continue;

                uint32& px = dstRow[x];
    #ifdef ALPHA_BLEND
                if (sourcePixel.a == 0xFF)
                    px = static_cast<u32>(sourcePixel);
                else
                    px = static_cast<u32>(mixColors(Color(px), sourcePixel));
    #else
                px = static_cast<u32>(sourcePixel);
    #endif
            }
        }
    }
    inline void Canvas::drawCanvas(const glm::ivec2& p, const i32 width, const i32 height, const Canvas& sourceCanvas) const {
        drawCanvas(p.x, p.y, width, height, sourceCanvas);
    }

    inline void Canvas::blitCanvas(const Canvas& sourceCanvas) const {
        const i32 srcW      = static_cast<i32>(sourceCanvas.getWidth());
        const i32 srcH      = static_cast<i32>(sourceCanvas.getHeight());
        const i32 dstW      = static_cast<i32>(WIDTH);
        const i32 dstH      = static_cast<i32>(HEIGHT);
        const i32 srcStride = static_cast<i32>(sourceCanvas.getStride());
        const i32 dstStride = static_cast<i32>(STRIDE);

        const i32 stepX = (srcW << 16) / dstW;
        const i32 stepY = (srcH << 16) / dstH;

        i32 srcY_fp = 0;
        for (i32 y = 0; y < dstH; ++y) {
            const u32* srcRow = sourceCanvas.pixels + (srcY_fp >> 16) * srcStride;
            u32*       dstRow = pixels              +  y              * dstStride;

            i32 srcX_fp = 0;
            for (i32 x = 0; x < dstW; ++x) {
                const Color sourcePixel = Color(srcRow[srcX_fp >> 16]);
                if (sourcePixel.a == 0) { srcX_fp += stepX; ++dstRow; continue; }

                uint32& px = dstRow[x];
    #ifdef ALPHA_BLEND
                if (sourcePixel.a == 0xFF)
                    px = static_cast<u32>(sourcePixel);
                else
                    px = static_cast<u32>(mixColors(Color(px), sourcePixel));
    #else
                px = static_cast<u32>(sourcePixel);
    #endif
                srcX_fp += stepX;
            }
            srcY_fp += stepY;
        }
    }

    inline void Canvas::writeOmniCharBaseline(const char c, int32 x, int32 y, int32 font_size, Color color_outline, Color color_main, Color color_shadow) const {
        const OmniFont::CharData charData = OmniFont::char_to_data(c);
        uint32 pen_x = x, pen_y = y;
        pen_y -= charData.baseline * font_size;  // scale baseline offset too

        for (int rel_y = 0; rel_y < charData.char_height; ++rel_y) {
            for (int rel_x = 0; rel_x < charData.char_width; ++rel_x) {
                const uint8_t value = charData.char_data[rel_y * charData.char_width + rel_x];
                Color col{};
                switch (value) {
                    case 0: col = color_outline; break;
                    case 1: col = color_main; break;
                    case 2: col = color_shadow; break;
                    default: continue;
                }

                // Draw a font_size x font_size block for each glyph pixel
                for (int sy = 0; sy < font_size; ++sy) {
                    for (int sx = 0; sx < font_size; ++sx) {
                        const int32 draw_x = static_cast<int32>(pen_x) + rel_x * font_size + sx;
                        const int32 draw_y = static_cast<int32>(pen_y) + rel_y * font_size + sy;

                        if (draw_x < 0 || draw_x >= WIDTH || draw_y < 0 || draw_y >= HEIGHT)
                            continue;

                        pixels[getPixelIndex(draw_x, draw_y)] = col;
                    }
                }
            }
        }
    }

    inline void Canvas::writeOmniCharBaseline(char c, int32 x, int32 y, int32 font_size) const {
        writeOmniCharBaseline(c, x, y, font_size, Colors::Black, Colors::White, Colors::Grey);
    }

    inline void Canvas::writeOmniStringBaseline(const std::string &s, int32 x, int32 y, int32 font_size) const {
        int32 working_x = x;
        int32 working_y = y;
        for (const char& c : s) {
            if (c == '\n') {
                working_y += 10 * font_size;
                working_x = x;
                continue;
            }
            const OmniFont::CharData charData = OmniFont::char_to_data(c);
            writeOmniCharBaseline(c, working_x, working_y, font_size);
            working_x += charData.move_width * font_size;
        }
    }

    inline void Canvas::writeCharBaseline(const char c, const int32 x, const int32 y, const int32 font_size, const Color color) const {
        const uint8* glyph = font8x8_basic[static_cast<uint8>(c)];

        for (int32 py = 0; py < font_size; ++py) {
            // Map output y to glyph row (0..7)
            const int32 glyph_row = (py * 8) / font_size;

            const uint8 bits = glyph[glyph_row];

            for (int32 px = 0; px < font_size; ++px) {
                const int32 draw_x = x + px;
                const int32 draw_y = y - font_size + py;
                if (draw_x < 0 || draw_x >= WIDTH || draw_y < 0 || draw_y >= HEIGHT) continue;

                // Map output x to glyph column (0..7)
                const int32 glyph_col = (px * 8) / font_size;

                if (bits & (1 << glyph_col)) {
                    pixels[getPixelIndex(draw_x, draw_y)] = static_cast<uint32>(color);
                }
            }
        }
    }
    inline void Canvas::writeCharBaseline(const char c, const glm::i32vec2& p, const int32 font_size, const Color color) const {
        writeCharBaseline(c, p.x, p.y, font_size, color);
    }
    inline void Canvas::writeStringBaseline(const std::string& s, const int32 x, const int32 y, const int32 font_size, const Color color) const {
        int32 cursor_x = x;

        for (int i = 0; s[i] != '\0'; ++i) {
            const char c = s[i];
            writeCharBaseline(c, cursor_x, y, font_size, color);
            cursor_x += font_size;
        }
    }
    inline void Canvas::writeStringBaseline(const std::string& s, const glm::i32vec2& p, const int32 font_size, const Color color) const {
        writeStringBaseline(s, p.x, p.y, font_size, color);
    }

    inline bool Canvas::saveToPPM (const std::string& filename) const {
            std::ofstream file(filename, std::ios::out | std::ios::binary);
            if (file.fail()) {
                omni::LOG_ERROR("Could not open file {}", filename);
                return false;
            }

            omni::println(file, "P6\n{} {} 255", WIDTH, HEIGHT);

            for (uint64 i = 0; i < WIDTH * HEIGHT; i++) {
                const uint32 pixel = pixels[i];
                const uint8 bytes[3] = {
                    static_cast<uint8>((pixel >> 0) & 0xff),
                    static_cast<uint8>((pixel >> 8) & 0xff),
                    static_cast<uint8>((pixel >> 16) & 0xff)
                };
                file << bytes[0] << bytes[1] << bytes[2];
            }

            file.close();
            return true;
        }
    inline bool Canvas::saveToJPG (const std::string& filename, const int quality) const {
        std::ofstream file(filename, std::ios::out | std::ios::binary);
        if (file.fail()) {
            omni::LOG_ERROR("Could not open file {}", filename);
            return false;
        }

        if (stbi_write_jpg(filename.c_str(), static_cast<int32>(WIDTH), static_cast<int32>(HEIGHT), 4, pixels, quality) != 1) {
            omni::LOG_ERROR("Could not write to file {}", filename);
            file.close();
            return false;
        }

        file.close();
        return true;
    }
    inline bool Canvas::saveToPNG (const std::string& filename) const {
        std::ofstream file(filename, std::ios::out | std::ios::binary);
        if (file.fail()) {
            omni::LOG_ERROR("Could not open file {}", filename);
            return false;
        }

        if (stbi_write_png(filename.c_str(),
            static_cast<int32>(WIDTH),static_cast<int32>(HEIGHT),
            4, pixels, static_cast<int32>(sizeof(pixels[0])*WIDTH)) != 1) {
            omni::LOG_ERROR("Could not write to file {}", filename);
            file.close();
            return false;
        }

        file.close();
        return true;
    }
    inline bool Canvas::saveToBMP (const std::string& filename) const {
        std::ofstream file(filename, std::ios::out | std::ios::binary);
        if (file.fail()) {
            omni::LOG_ERROR("Could not open file {}", filename);
            return false;
        }

        if (stbi_write_bmp(filename.c_str(), static_cast<int32>(WIDTH),static_cast<int32>(HEIGHT), 4, pixels) != 1) {
            omni::LOG_ERROR("Could not write to file {}", filename);
            file.close();
            return false;
        }

        file.close();
        return true;
    }

    inline uint32* Canvas::getPixel      (const uint32 x, const uint32 y) const {
        return &pixels[(y)*STRIDE + (x)];
    }
    inline uint32  Canvas::getPixelIndex (const uint32 x, const uint32 y) const {
        return (y)*STRIDE + (x);
    }
    inline Color   Canvas::getPixelColor (const uint32 x, const uint32 y) const {
        return {pixels[(y) * STRIDE + (x)]};
    }

    inline glm::ivec2 normalizedToScreen(const glm::vec2& p, const u32 WIDTH, const u32 HEIGHT) {
        const f32 w  = static_cast<f32>(WIDTH);
        const f32 h  = static_cast<f32>(HEIGHT);
        const f32 s  = glm::min(w, h);
        const f32 cx = w * 0.5f;
        const f32 cy = h * 0.5f;

        return {
            static_cast<i32>(cx + p.x * s * 0.5f),
            static_cast<i32>(cy - p.y * s * 0.5f)
        };
    }

    inline glm::vec2 project(const glm::vec3& p) {
        return glm::vec2(p.x, p.y) / p.z;
    }

    inline glm::vec3 translate(const glm::vec3& p, const glm::vec3& translation) {
        return p + translation;
    }

    // rotation: {pitch, yaw, roll} aka {around x, around y, around z}
    inline glm::vec3 rotate(const glm::vec3& p, const glm::vec3& rotation) {
        const auto rx = glm::mat3(glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1, 0, 0)));
        const auto ry = glm::mat3(glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0, 1, 0)));
        const auto rz = glm::mat3(glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0, 0, 1)));
        return rz * ry * rx * p;
    }

    inline glm::vec3 rotateInverse(const glm::vec3& p, const glm::vec3& rotation) {
        const auto rx = glm::mat3(glm::rotate(glm::mat4(1.0f), -rotation.x, glm::vec3(1, 0, 0)));
        const auto ry = glm::mat3(glm::rotate(glm::mat4(1.0f), -rotation.y, glm::vec3(0, 1, 0)));
        const auto rz = glm::mat3(glm::rotate(glm::mat4(1.0f), -rotation.z, glm::vec3(0, 0, 1)));
        return rx * ry * rz * p;
    }

    inline glm::mat3 makeRotationMatrix(const glm::vec3& rotation) {
        const auto rx = glm::mat3(glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1, 0, 0)));
        const auto ry = glm::mat3(glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0, 1, 0)));
        const auto rz = glm::mat3(glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0, 0, 1)));
        return rz * ry * rx;
    }

    inline void Camera::drawObjectWireframe(const Object3D& obj, const Canvas& c, const Color color, const f32 thickness) const {
        const glm::vec3 re_position = obj.position - position;

        const glm::mat3 objRotMatrix = makeRotationMatrix(obj.rotation);
        const glm::mat3 camRotInvMatrix = glm::transpose(makeRotationMatrix(rotation));

        for (const std::vector<FaceIndex>& face : obj.faces) {
            const int faceSize = static_cast<int>(face.size());
            for (int i = 0; i < faceSize; i++) {
                const glm::vec3& p0 = obj.vertices[face[i].vertexIdx];
                const glm::vec3& p1 = obj.vertices[face[(i+1) % faceSize].vertexIdx];

                const glm::vec3 p0CameraRelative = camRotInvMatrix * (objRotMatrix * p0 + re_position);
                const glm::vec3 p1CameraRelative = camRotInvMatrix * (objRotMatrix * p1 + re_position);

                if (p0CameraRelative.z < 0 || p1CameraRelative.z < 0) continue;

                const glm::ivec2 s0 = normalizedToScreen(project(p0CameraRelative), c.getWidth(), c.getHeight());
                const glm::ivec2 s1 = normalizedToScreen(project(p1CameraRelative), c.getWidth(), c.getHeight());
                c.drawLine(s0, s1, color, thickness);
            }
        }
    }

    inline glm::vec3 Camera::directionObj(const glm::vec3& direction) const {
        return makeRotationMatrix({0, rotation.y, 0}) * direction;
    }

    inline glm::vec3 Camera::directionCam(const glm::vec3& direction) const {
        return makeRotationMatrix(rotation) * direction;
    }

    inline glm::vec3 Camera::forwardCam()  const { return makeRotationMatrix(rotation) * Dir3D::FORWARD; }
    inline glm::vec3 Camera::backwardCam() const { return -forwardCam(); }

    inline glm::vec3 Camera::rightCam()  const { return makeRotationMatrix(rotation) * Dir3D::RIGHT; }
    inline glm::vec3 Camera::leftCam()   const { return -rightCam(); }

    inline glm::vec3 Camera::upCam()   const { return makeRotationMatrix(rotation) * Dir3D::UP; }
    inline glm::vec3 Camera::downCam() const { return -upCam(); }

    inline glm::vec3 Camera::forwardObj()  const { return makeRotationMatrix({0, rotation.y, 0}) * Dir3D::FORWARD; }
    inline glm::vec3 Camera::backwardObj() const { return -forwardObj(); }

    inline glm::vec3 Camera::rightObj()  const { return makeRotationMatrix({0, rotation.y, 0}) * Dir3D::RIGHT; }
    inline glm::vec3 Camera::leftObj()   const { return -rightObj(); }

    inline glm::vec3 Camera::upObj()   const { return makeRotationMatrix({0, rotation.y, 0}) * Dir3D::UP; }
    inline glm::vec3 Camera::downObj() const { return -upObj(); }

    inline Object3D loadOBJ(const std::filesystem::path& filePath) {
        Object3D obj;

        std::ifstream file(filePath);
        if (!file.is_open()) {
            omni::LOG_ERROR("Could not open file {}", filePath.string());
            return {};
        }

        std::string line;

        while (std::getline(file, line)) {
            std::istringstream ss(line);
            std::string prefix;
            ss >> prefix;

            // ---- Vertex positions ----
            if (prefix == "v") {
                f32vec3 v;
                ss >> v.x >> v.y >> v.z;

                obj.vertices.push_back(v);
            }

            // ---- Vertex normals ----
            if (prefix == "vn") {
                f32vec3 n;
                ss >> n.x >> n.y >> n.z;

                obj.normals.push_back(n);
            }

            // ---- Texture coordinates ----
            else if (prefix == "vt") {
                f32vec2 uv;
                ss >> uv.x >> uv.y;
                obj.texCoords.push_back(uv);
            }

            // ---- Faces ----
            else if (prefix == "f") {
                std::vector<FaceIndex> face;
                std::string vert;

                while (ss >> vert) {
                    // formats:
                    // v
                    // v/vt
                    // v/vt/vn
                    // v//vn

                    int vIndex  = -1;
                    int vnIndex = -1;
                    int vtIndex = -1;

                    size_t s1 = vert.find('/');
                    size_t s2 = std::string::npos;

                    if (s1 == s2) {
                        vIndex = std::stoi(vert) - 1;
                    } else {
                        vIndex = std::stoi(vert.substr(0, s1)) - 1;

                        s2 = vert.find('/', s1 + 1);

                        std::string vtStr =
                            (s2 == std::string::npos)
                            ? vert.substr(s1 + 1)
                            : vert.substr(s1 + 1, s2 - s1 - 1);

                        if (!vtStr.empty())
                            vtIndex = std::stoi(vtStr) - 1;
                    }

                    face.push_back({
                        vIndex, vtIndex, 0, static_cast<Color>(randomUint32())
                    });
                }

                if (face.size() >= 3)
                    obj.faces.push_back(face);
            }
        }

        return obj;
    }

#define TRANSFORM_OUT_OF_CAMERA_BOUNDS {-100, -100}

    inline vec2 Camera::transformPoint(const vec3 positionWorld) const {
        const glm::vec3 re_position = positionWorld - position;
        const glm::mat3 camRotInv = glm::transpose(makeRotationMatrix(rotation));
        const glm::vec3 cam       = camRotInv * re_position;
        if (cam.z <= 0.001f) return TRANSFORM_OUT_OF_CAMERA_BOUNDS;
        return project(cam);
    }

    inline float Camera::worldRadiusToPixels(const vec3 centerWorld, const float radiusWorld,
                                   const Canvas& c) const {
        const int screenWidth = static_cast<int>(c.getWidth());
        const int screenHeight = static_cast<int>(c.getHeight());

        // Project the center
        vec2 centerScreen = transformPoint(centerWorld);
        vec2 centerPixel  = normalizedToScreen(centerScreen, screenWidth, screenHeight);

        // Pick an offset point — use camera right axis to stay perpendicular to view
        const glm::mat3 camRot    = makeRotationMatrix(rotation);
        const glm::vec3 camRight  = glm::normalize(camRot[0]); // first column = right axis

        vec3 edgeWorld  = centerWorld + camRight * radiusWorld;
        vec2 edgeScreen = transformPoint(edgeWorld);
        vec2 edgePixel  = normalizedToScreen(edgeScreen, screenWidth, screenHeight);

        return glm::length(edgePixel - centerPixel);
    }

    inline std::vector<Camera::Vtx> Camera::transformVertices(const Object3D& obj, const Canvas& c) const {
        const glm::vec3 re_position = obj.position - position;
        const glm::mat3 objRot    = makeRotationMatrix(obj.rotation);
        const glm::mat3 camRotInv = glm::transpose(makeRotationMatrix(rotation));

        std::vector<Vtx> vtx(obj.vertices.size());
        for (size_t i = 0; i < obj.vertices.size(); i++) {
            glm::vec3 world = objRot * obj.vertices[i] + re_position;
            glm::vec3 cam   = camRotInv * world;

            if (cam.z <= 0.001f) continue;

            vtx[i].world  = world;
            vtx[i].invZ   = 1.0f / cam.z;
            vtx[i].screen = normalizedToScreen(project(cam), c.getWidth(), c.getHeight());
            vtx[i].valid  = true;
        }
        return vtx;
    }

    inline bool Camera::backfaceCull(const Vtx& v0, const Vtx& v1, const Vtx& v2) {
        const glm::ivec2 e1 = v1.screen - v0.screen;
        const glm::ivec2 e2 = v2.screen - v0.screen;
        return (e1.x * e2.y - e1.y * e2.x) < 0;
    }

    inline void Camera::drawObjectTexture(const Object3D& obj, const Canvas& c, const RenderOptions& renderOptions) const {
        const auto vtx = transformVertices(obj, c);

        for (const std::vector<FaceIndex>& face : obj.faces) {
            if (face.size() < 3) continue;

            const Vtx& v0 = vtx[face[0].vertexIdx];
            const Vtx& v1 = vtx[face[1].vertexIdx];
            const Vtx& v2 = vtx[face[2].vertexIdx];
            if (!v0.valid || !v1.valid || !v2.valid) continue;

            if (renderOptions.cullBackface && backfaceCull(v0, v1, v2)) continue;

            const glm::vec2 uv0(obj.texCoords[face[0].texCoordIdx].x, obj.texCoords[face[0].texCoordIdx].y);

            for (size_t i = 1; i + 1 < face.size(); i++) {
                const Vtx& a = vtx[face[i].vertexIdx];
                const Vtx& b = vtx[face[i + 1].vertexIdx];
                if (!a.valid || !b.valid) continue;

                const glm::vec2 auv(obj.texCoords[face[i].texCoordIdx].x,     obj.texCoords[face[i].texCoordIdx].y);
                const glm::vec2 buv(obj.texCoords[face[i+1].texCoordIdx].x,   obj.texCoords[face[i+1].texCoordIdx].y);

                double t = -1.0f;
                if (renderOptions.diffuse) {
                    fvec3 norm = glm::normalize(glm::cross(
                        a.world - v0.world,
                        b.world - v0.world
                    ));
                    t = (glm::dot(norm, renderOptions.sunVector) + 1.0) / 2;
                }

                c.fillTriangleUV(
                    v0.screen, a.screen, b.screen,
                    uv0 * v0.invZ, auv * a.invZ, buv * b.invZ,
                    v0.invZ, a.invZ, b.invZ,
                    *obj.tex, renderOptions.zBuffer, static_cast<float>(t)
                );
            }
        }
    }
    inline void Camera::drawObjectTexture(const Object3D& obj, const Canvas& c) const {
        drawObjectTexture(obj, c, {});
    }
    inline void Camera::drawObjectColor(const Object3D& obj, const Canvas& c, const RenderOptions& renderOptions) const {
        const auto vtx = transformVertices(obj, c);

        for (const std::vector<FaceIndex>& face : obj.faces) {
            if (face.size() < 3) continue;

            const Vtx& v0 = vtx[face[0].vertexIdx];
            const Vtx& v1 = vtx[face[1].vertexIdx];
            const Vtx& v2 = vtx[face[2].vertexIdx];
            if (!v0.valid || !v1.valid || !v2.valid) continue;

            if (renderOptions.cullBackface && backfaceCull(v0, v1, v2)) continue;

            Color c0 = face[0].randColor;

            for (size_t i = 1; i + 1 < face.size(); i++) {
                const Vtx& a = vtx[face[i].vertexIdx];
                const Vtx& b = vtx[face[i + 1].vertexIdx];
                if (!a.valid || !b.valid) continue;

                Color c1 = face[i].randColor;
                Color c2 = face[i+1].randColor;

                if (renderOptions.diffuse) {
                    fvec3 norm = glm::normalize(glm::cross(
                        a.world - v0.world,
                        b.world - v0.world
                    ));
                    const double t = (glm::dot(norm, renderOptions.sunVector) + 1.0) / 2;
                    const auto t_float = static_cast<float>(t);

                    c0 = lerpColors(c0, Colors::Black, t_float);
                    c1 = lerpColors(c1, Colors::Black, t_float);
                    c2 = lerpColors(c2, Colors::Black, t_float);
                }

                c.fillTriangle(
                    v0.screen, a.screen, b.screen,
                    c0, c1, c2,
                    v0.invZ, a.invZ, b.invZ,
                    renderOptions.zBuffer
                );
            }
        }
    }

    inline void Camera::drawObjectColor(const Object3D& obj, const Canvas& c) const {
        drawObjectColor(obj, c, {});
    }

    inline void Camera::drawObjectVertexColor(const Object3D& obj, const Canvas& c, const RenderOptions &renderOptions) const {
        const auto vtx = transformVertices(obj, c);

        for (const std::vector<FaceIndex>& face : obj.faces) {
            if (face.size() < 3) continue;

            const Vtx& v0 = vtx[face[0].vertexIdx];
            const Vtx& v1 = vtx[face[1].vertexIdx];
            const Vtx& v2 = vtx[face[2].vertexIdx];
            if (!v0.valid || !v1.valid || !v2.valid) continue;

            if (renderOptions.cullBackface && backfaceCull(v0, v1, v2)) continue;

            Color c0 = face[0].randColor;

            for (size_t i = 1; i + 1 < face.size(); i++) {
                const Vtx& a = vtx[face[i].vertexIdx];
                const Vtx& b = vtx[face[i + 1].vertexIdx];
                if (!a.valid || !b.valid) continue;

                if (renderOptions.diffuse) {
                    fvec3 norm = glm::normalize(glm::cross(
                        a.world - v0.world,
                        b.world - v0.world
                    ));
                    const double t = (glm::dot(norm, renderOptions.sunVector) + 1.0) / 2;

                    c0 = lerpColors(c0, Colors::Black, static_cast<float>(t));
                }

                c.fillTriangle(
                    v0.screen, a.screen, b.screen,
                    c0,
                    v0.invZ, a.invZ, b.invZ,
                    renderOptions.zBuffer
                );
            }
        }
    }

    inline void Camera::drawObjectVertexColor(const Object3D& obj, const Canvas& c) const {
        drawObjectVertexColor(obj, c, {});
    }


    inline void Camera::drawObjectSingleColor (const Object3D& obj, const Canvas& c, const Color& color, const RenderOptions &renderOptions) const {
        const auto vtx = transformVertices(obj, c);

        for (const std::vector<FaceIndex>& face : obj.faces) {
            if (face.size() < 3) continue;

            const Vtx& v0 = vtx[face[0].vertexIdx];
            const Vtx& v1 = vtx[face[1].vertexIdx];
            const Vtx& v2 = vtx[face[2].vertexIdx];
            if (!v0.valid || !v1.valid || !v2.valid) continue;

            if (renderOptions.cullBackface && backfaceCull(v0, v1, v2)) continue;

            Color c0 = color;
            if (c0.a == 0) continue;

            for (size_t i = 1; i + 1 < face.size(); i++) {
                const Vtx& a = vtx[face[i].vertexIdx];
                const Vtx& b = vtx[face[i + 1].vertexIdx];
                if (!a.valid || !b.valid) continue;

                if (renderOptions.diffuse) {
                    fvec3 norm = glm::normalize(glm::cross(
                        a.world - v0.world,
                        b.world - v0.world
                    ));
                    const double t = (glm::dot(norm, renderOptions.sunVector) + 1.0) / 2;

                    c0 = lerpColors(color, Colors::Black, static_cast<float>(t));
                }

                c.fillTriangle(
                    v0.screen, a.screen, b.screen,
                    c0,
                    v0.invZ, a.invZ, b.invZ,
                    renderOptions.zBuffer
                );
            }
        }
    }
    inline void Camera::drawObjectSingleColor (const Object3D& obj, const Canvas& c, const Color& color) const {
        drawObjectSingleColor(obj, c, color, {});
    }

    inline void Camera::drawObjectVertices(const Object3D &obj, const Canvas &c, const Color color, const int pointSize) const {
        const glm::vec3 re_position = obj.position - position;

        const glm::mat3 objRotMatrix = makeRotationMatrix(obj.rotation);
        const glm::mat3 camRotInvMatrix = glm::transpose(makeRotationMatrix(rotation));

        for (const f32vec3 vertex : obj.vertices) {
            const glm::vec3 p0CameraRelative = camRotInvMatrix * (objRotMatrix * vertex + re_position);
            if (p0CameraRelative.z < 0) continue;
            const glm::ivec2 s0 = normalizedToScreen(project(p0CameraRelative), c.getWidth(), c.getHeight());
            c.drawPoint(s0, pointSize, color);
        }
    }

    inline void Camera::drawObjectDepth(const Object3D& obj, const Canvas& c, const RenderOptions& renderOptions) const {
        const auto vtx = transformVertices(obj, c);

        for (const std::vector<FaceIndex>& face : obj.faces) {
            if (face.size() < 3) continue;

            const Vtx& v0 = vtx[face[0].vertexIdx];
            const Vtx& v1 = vtx[face[1].vertexIdx];
            const Vtx& v2 = vtx[face[2].vertexIdx];
            if (!v0.valid || !v1.valid || !v2.valid) continue;

            if (renderOptions.cullBackface && backfaceCull(v0, v1, v2)) continue;

            auto depthColor = [](float invZ) -> Color {
                const u8 z = static_cast<u8>(glm::clamp(invZ, 0.0f, 1.0f) * 255.0f);
                return {z, z, z, 0xff};
            };

            for (size_t i = 1; i + 1 < face.size(); i++) {
                const Vtx& a = vtx[face[i].vertexIdx];
                const Vtx& b = vtx[face[i + 1].vertexIdx];
                if (!a.valid || !b.valid) continue;

                Color c0 = depthColor(v0.invZ);
                Color c1 = depthColor(a.invZ);
                Color c2 = depthColor(b.invZ);

                if (renderOptions.diffuse) {
                    fvec3 norm = glm::normalize(glm::cross(
                        a.world - v0.world,
                        b.world - v0.world
                    ));
                    const double t = (glm::dot(norm, renderOptions.sunVector) + 1.0) / 2;
                    const auto t_float = static_cast<float>(t);

                    c0 = lerpColors(c0, Colors::Black, t_float);
                    c1 = lerpColors(c1, Colors::Black, t_float);
                    c2 = lerpColors(c2, Colors::Black, t_float);
                }

                c.fillTriangle(
                    v0.screen, a.screen, b.screen,
                    depthColor(v0.invZ), depthColor(a.invZ), depthColor(b.invZ),
                    v0.invZ, a.invZ, b.invZ,
                    renderOptions.zBuffer
                );
            }
        }
    }
    inline void Camera::drawObjectDepth(const Object3D& obj, const Canvas& c) const {
        drawObjectDepth(obj, c, {});
    }
    inline void Camera::drawObject(const Object3D& obj, const Canvas& c, const RenderOptions& renderOptions) const {
        if (obj.tex == nullptr)
            drawObjectColor(obj, c, renderOptions);
        else
            drawObjectTexture(obj, c, renderOptions);
    }

    inline void Camera::drawObject(const Object3D &obj, const Canvas &c) const {
        drawObject(obj, c, {});
    }

#endif

}

#endif //GRAPHITE_H
