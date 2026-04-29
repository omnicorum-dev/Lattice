# GRAPHITE

A lightweight, header-only C++ 2D software rasterizer.
Graphite gives you a pixel buffer you can draw into directly. No GPU. No windowing system. Ez.

Useful for game engines, emulators, tools, and any project that needs a simple CPU-side canvas.

---

## Features

- **Filled primitives** — rectangles, circles, ellipses, triangles
- **Line drawing** — anti-aliased single-pixel lines. Great for debugging.
- **Canvas blitting** — draw one canvas onto another with optional scaling and flipping
- **Image loading** — load PNG/JPG/BMP sprites via `stb_image`
- **Text rendering** — 8×8 bitmap font at any integer scale
- **Alpha blending** — optional; enabled at compile time via `#define ALPHA_BLEND`
- **Image export** — save any canvas to a `.ppm`, `.jpg`, `.png`, or `.bmp` file for quick debugging (or 'screenshotting')
- **Subcanvas views** — create a canvas that views a region of another without copying

---

## Installation

Don't even try at this point. I'm working on making it more user-friendly, but it's not there yet. Very much not so.

---

## Quick Start

```cpp
#include <graphite.h>
using namespace Graphite;

int main() {
    // Create a 800×600 canvas
    Canvas canvas(800, 600);

    // Fill the background black
    canvas.fillFast(Color(0, 0, 0));

    // Draw a red rectangle
    canvas.fillRect(50, 50, 200, 100, Color(255, 0, 0));

    // Draw a blue circle
    canvas.fillCircle(400, 300, 80, Color(0, 100, 255));

    // Write some text (font size 16)
    canvas.writeString("Hello, Graphite!", 10, 30, 16, Color(255, 255, 255));

    // Save to a JPG file
    canvas.saveToJPG("output.jpg");

    return 0;
}
```

---

## Linking to an Existing Pixel Buffer

If your windowing layer (SDL, Win32, etc.) owns the pixel buffer, link Graphite to it instead of allocating a new one:

```cpp
Canvas canvas;
canvas.linkCanvas(sdl_surface->pixels, width, height);
// Now draw directly into the SDL surface's memory
canvas.fillRect(0, 0, width, height, Color(0x1a1a2e));
```

---

## Loading a Sprite

```cpp
Canvas sprite("assets/player.png");

// Draw it onto the main canvas at (100, 200), scaled to 64×64
mainCanvas.drawCanvas(100, 200, 64, 64, sprite);

// Flip horizontally by passing a negative width
mainCanvas.drawCanvas(100, 200, -64, 64, sprite);
```

---

## Alpha Blending

By default, drawing is opaque. To enable alpha blending across all draw calls, define `ALPHA_BLEND` before including the header (or add it as a compiler flag):

```cpp
#define ALPHA_BLEND
#include <graphite.h>
```

With `ALPHA_BLEND` enabled, colors are mixed using the alpha channel

---

## API Reference

### `Canvas` constructors

| Constructor                     | Description                                                      |
|---------------------------------|------------------------------------------------------------------|
| `Canvas(width, height)`         | Allocates a new pixel buffer                                     |
| `Canvas(width, height, stride)` | As above, with a custom row stride                               |
| `Canvas(filepath)`              | Loads an image file (PNG, JPG, BMP, …)                           |
| `Canvas()`                      | Empty canvas; use `linkCanvas()` or `newCanvas()` before drawing |


### Drawing methods

| Method                                     | Description                                                   |
|--------------------------------------------|---------------------------------------------------------------|
| `fill(color)`                              | Fill the entire canvas                                        |
| `fillFast(color)`                          | Fill using `std::fill_n` (faster for solid colors)            |
| `fillRect(x, y, w, h, color)`              | Filled rectangle; clips to canvas bounds                      |
| `fillCircle(cx, cy, r, color)`             | Filled circle                                                 |
| `fillEllipse(cx, cy, rx, ry, color)`       | Filled ellipse                                                |
| `fillTriangle(x1,y1, x2,y2, x3,y3, color)` | Filled triangle                                               |
| `drawLine(x1, y1, x2, y2, color)`          | Anti-aliased line (Bresenham)                                 |
| `drawCanvas(x, y, w, h, src)`              | Blit and scale `src` onto this canvas; negative `w`/`h` flips |
| `blitCanvas(src)`                          | Scale `src` to fill this canvas (fixed-point, fast)           |
| `writeChar(c, x, y, size, color)`          | Render a single character                                     |
| `writeString(s, x, y, size, color)`        | Render a string left-to-right                                 |

### Saving methods
| Method                                     | Description                                                   |
|--------------------------------------------|---------------------------------------------------------------|
| `saveToPPM(filename)`                      | Write canvas to a binary PPM file                             |
| `saveToJPG(filename, optional_quality)`    | Write canvas to a binary JPG file                             |
| `saveToPNG(filename)`                      | Write canvas to a binary PNG file                             |
| `saveToBMP(filename)`                      | Write canvas to a binary BMP file                             |

### `Color`

```cpp
Color red(255, 0, 0);          // r, g, b
Color translucent(0, 255, 0, 128); // r, g, b, a (0 = transparent, 255 = opaque)
Color blue32(0xFFFF0000);  // raw ABGR word
```

Colors are stored as packed `uint32_t` in `r, g, b, a` byte order.

---

## Notes

- Coordinates are in pixels, with `(0, 0)` at the top-left.
- All draw calls clip silently to the canvas bounds — drawing outside never crashes.
- `subcanvas()` returns a view into a region of the parent canvas with shared pixel memory; do not use it after the parent is destroyed.
- `STB_IMAGE_IMPLEMENTATION` is defined inside `graphite.h`. If you include `stb_image.h` elsewhere in the same translation unit you will get a duplicate-symbol error — remove your own `#define STB_IMAGE_IMPLEMENTATION` in that case.

---

## License

MIT — see `LICENSE` for details.