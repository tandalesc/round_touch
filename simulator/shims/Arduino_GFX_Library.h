#ifndef _ARDUINO_GFX_LIBRARY_SHIM_H_
#define _ARDUINO_GFX_LIBRARY_SHIM_H_

#include "Arduino.h"
#include "simulator/platform/SimDisplay.h"

// --- Arduino_GFX base class backed by SDL2 ---
class Arduino_GFX {
private:
  int _cursorX = 0;
  int _cursorY = 0;
  uint8_t _textSize = 1;
  uint16_t _textColor = WHITE;

public:
  Arduino_GFX() {}
  virtual ~Arduino_GFX() {}

  virtual void begin() {
    // SDL init is handled in main.cpp / SimDisplay
  }

  void fillScreen(uint16_t color) {
    SDL_Renderer *r = SimDisplay::renderer();
    SDL_Color c = SimDisplay::rgb565ToSDL(color);
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    SDL_RenderClear(r);
  }

  void setCursor(int x, int y) {
    _cursorX = x;
    _cursorY = y;
  }

  void setTextSize(uint8_t s) {
    _textSize = s;
  }

  void setTextColor(uint16_t c) {
    _textColor = c;
  }

  void print(const char *text) {
    if (!text) return;
    SDL_Renderer *r = SimDisplay::renderer();
    int x = _cursorX;
    for (const char *p = text; *p; p++) {
      if (*p == '\n') {
        _cursorX = 0;
        _cursorY += 8 * _textSize;
        x = _cursorX;
        continue;
      }
      SimDisplay::drawChar(r, x, _cursorY, *p, _textColor, _textSize);
      x += 6 * _textSize;
    }
    _cursorX = x;
  }

  void print(int val) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", val);
    print(buf);
  }

  void print(unsigned long val) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%lu", val);
    print(buf);
  }

  void println(const char *text) {
    print(text);
    _cursorY += 8 * _textSize;
    _cursorX = 0;
  }

  void println() {
    _cursorY += 8 * _textSize;
    _cursorX = 0;
  }

  void drawRect(int x, int y, int w, int h, uint16_t color) {
    SDL_Renderer *r = SimDisplay::renderer();
    SDL_Color c = SimDisplay::rgb565ToSDL(color);
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderDrawRect(r, &rect);
  }

  void fillRect(int x, int y, int w, int h, uint16_t color) {
    SDL_Renderer *r = SimDisplay::renderer();
    SDL_Color c = SimDisplay::rgb565ToSDL(color);
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(r, &rect);
  }

  void drawLine(int x0, int y0, int x1, int y1, uint16_t color) {
    SDL_Renderer *r = SimDisplay::renderer();
    SDL_Color c = SimDisplay::rgb565ToSDL(color);
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    SDL_RenderDrawLine(r, x0, y0, x1, y1);
  }

  void drawCircle(int x, int y, int radius, uint16_t color) {
    SDL_Renderer *r = SimDisplay::renderer();
    SDL_Color c = SimDisplay::rgb565ToSDL(color);
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    // Midpoint circle algorithm
    int dx = radius, dy = 0;
    int err = 1 - dx;
    while (dx >= dy) {
      SDL_RenderDrawPoint(r, x + dx, y + dy);
      SDL_RenderDrawPoint(r, x - dx, y + dy);
      SDL_RenderDrawPoint(r, x + dx, y - dy);
      SDL_RenderDrawPoint(r, x - dx, y - dy);
      SDL_RenderDrawPoint(r, x + dy, y + dx);
      SDL_RenderDrawPoint(r, x - dy, y + dx);
      SDL_RenderDrawPoint(r, x + dy, y - dx);
      SDL_RenderDrawPoint(r, x - dy, y - dx);
      dy++;
      if (err < 0) {
        err += 2 * dy + 1;
      } else {
        dx--;
        err += 2 * (dy - dx) + 1;
      }
    }
  }

  void draw16bitBeRGBBitmap(int x, int y, uint16_t *pixels, int w, int h) {
    SDL_Renderer *r = SimDisplay::renderer();
    for (int row = 0; row < h; row++) {
      for (int col = 0; col < w; col++) {
        uint16_t pixel = pixels[row * w + col];
        SDL_Color c = SimDisplay::rgb565ToSDL(pixel);
        SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
        SDL_RenderDrawPoint(r, x + col, y + row);
      }
    }
  }
};

// --- SPI bus stub ---
class Arduino_ESP32SPI {
public:
  Arduino_ESP32SPI(int, int, int, int, int, int = 0, bool = false) {}
};

// --- GC9A01 driver stub (delegates to base) ---
class Arduino_GC9A01 : public Arduino_GFX {
public:
  Arduino_GC9A01(Arduino_ESP32SPI *, int, int, bool) {}
};

#endif // _ARDUINO_GFX_LIBRARY_SHIM_H_
