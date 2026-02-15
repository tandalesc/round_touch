#include <SDL2/SDL.h>
#include <cstdio>

#include "lvgl.h"

#include "platform/SimTouch.h"

// These includes resolve against our shims due to include path ordering
#include "device/Device.h"
#include "application/Application.h"

int main(int, char *[]) {
  // Initialize LVGL
  lv_init();

  // Create LVGL SDL display (handles SDL_Init internally)
  lv_display_t *display = lv_sdl_window_create(SCREEN_WIDTH, SCREEN_HEIGHT);

  // Create LVGL SDL mouse input device
  lv_indev_t *mouse = lv_sdl_mouse_create();

  // Initialize application
  Device device;
  Application app(&device);

  device.init();
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
