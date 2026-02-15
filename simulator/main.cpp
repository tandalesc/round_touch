#include <SDL2/SDL.h>
#include <cstdio>

#include "platform/SimDisplay.h"
#include "platform/SimTouch.h"

// These includes resolve against our shims due to include path ordering
#include "device/Device.h"
#include "application/Application.h"

int main(int, char *[]) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
    return 1;
  }

  // 2x scale for comfortable viewing on desktop
  SimDisplay::init(480, 480, 240, 240);

  Device device;
  Application app(&device);

  device.init();
  app.init();

  printf("Simulator running. Click to tap, click+drag to swipe.\n");

  bool running = true;
  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
        break;
      }
      SimTouch::processEvent(event);
    }

    app.loop();
    SimDisplay::present();
  }

  SimDisplay::shutdown();
  SDL_Quit();
  return 0;
}
