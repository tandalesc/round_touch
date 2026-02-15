/*******************************************************************************
 * JPEGDEC related function
 *
 * Dependent libraries:
 * JPEGDEC: https://github.com/bitbank2/JPEGDEC.git
 ******************************************************************************/
#ifndef _JPEG_FUNCTIONS_H_
#define _JPEG_FUNCTIONS_H_

#include <JPEGDEC.h>

#include "config/Constants.h"
#include "lib/jpeg/JpegIO.h"
#include "lib/jpeg/JpegRender.h"

static JPEGDEC _jpeg;

static void drawJpeg(const char *filename, int x, int y, int widthLimit,
                     int heightLimit) {
  _jpeg.open(filename, jpegOpenFile, jpegCloseFile, jpegReadFile, jpegSeekFile,
             jpegDrawCallback);

  // scale to fit height
  int _scale;
  int iMaxMCUs;
  float ratio = (float)_jpeg.getHeight() / heightLimit;
  if (ratio <= 1) {
    _scale = 0;
    iMaxMCUs = widthLimit / 16;
  } else if (ratio <= 2) {
    _scale = JPEG_SCALE_HALF;
    iMaxMCUs = widthLimit / 8;
  } else if (ratio <= 4) {
    _scale = JPEG_SCALE_QUARTER;
    iMaxMCUs = widthLimit / 4;
  } else {
    _scale = JPEG_SCALE_EIGHTH;
    iMaxMCUs = widthLimit / 2;
  }
  _jpeg.setMaxOutputSize(iMaxMCUs);
  _jpeg.setPixelType(RGB565_BIG_ENDIAN);
  _jpeg.decode(x, y, _scale);
  _jpeg.close();
}

static void drawJpegFullscreen(const char *filename) {
  drawJpeg(filename, 0, 0, SCREEN_MAX_WIDTH, SCREEN_MAX_HEIGHT);
}

#endif // _JPEG_FUNCTIONS_H_