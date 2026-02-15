#ifndef _JPEG_RENDER_H_
#define _JPEG_RENDER_H_

#include <JPEGDEC.h>

#include "device/IDisplay.h"

// Global pointer set before JPEG decode, used by the callback
static IDisplay *_jpegDisplay = nullptr;

static int jpegDrawCallback(JPEGDRAW *pDraw) {
  if (!_jpegDisplay) return 0;
  auto gfx = _jpegDisplay->gfx();
  gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth,
                            pDraw->iHeight);
  return 1;
}

#endif // _JPEG_RENDER_H_
