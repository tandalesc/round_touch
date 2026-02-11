#ifndef _SIM_DISPLAY_H_
#define _SIM_DISPLAY_H_

#include <SDL2/SDL.h>
#include <cstdint>

class SimDisplay {
public:
  static void init(int windowWidth, int windowHeight, int logicalWidth,
                   int logicalHeight);
  static void shutdown();

  static SDL_Window *window();
  static SDL_Renderer *renderer();

  static SDL_Color rgb565ToSDL(uint16_t color);
  static void drawChar(SDL_Renderer *r, int x, int y, char c, uint16_t color,
                       uint8_t size);
  static void present();

  // Call when a frame has been rendered to the framebuffer
  static void markDirty();
};

#endif // _SIM_DISPLAY_H_
