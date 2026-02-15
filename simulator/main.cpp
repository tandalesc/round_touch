#include <SDL2/SDL.h>
#include <cstdio>

#include "lvgl.h"

#include "platform/SimTouch.h"

// These includes resolve against our shims due to include path ordering
#include "device/Device.h"
#include "application/Application.h"

int main(int, char *[]) {
  // Initialize application (Device::init handles lv_init + LVGL display)
  Device device;
  Application app(&device);

  device.init();

  // Create LVGL SDL mouse input device (after lv_init in device.init)
  lv_sdl_mouse_create();

  app.init();

  printf("Simulator running. Click to tap, click+drag to swipe.\n");

  bool running = true;
  while (running) {
    // Check for quit via SDL
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
        break;
      }
      // Forward to SimTouch for gesture detection
      SimTouch::processEvent(event);
    }

    app.loop();

    // Small delay to prevent CPU spinning
    SDL_Delay(5);
  }

  lv_deinit();
  return 0;
}
