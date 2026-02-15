#ifndef _JPEG_RENDER_H_
#define _JPEG_RENDER_H_

#include <JPEGDEC.h>

#include "device/hw/Display.h"

static int jpegDrawCallback(JPEGDRAW *pDraw) {
  auto gfx = Display::getInstance().gfx;
  gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth,
                            pDraw->iHeight);
  return 1;
}

#endif // _JPEG_RENDER_H_