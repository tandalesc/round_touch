#ifndef _JPEGDEC_SHIM_H_
#define _JPEGDEC_SHIM_H_

#include <cstdint>

// Scale constants
#define JPEG_SCALE_HALF    2
#define JPEG_SCALE_QUARTER 4
#define JPEG_SCALE_EIGHTH  8
#define RGB565_BIG_ENDIAN  1

// Callback types
typedef struct {
  int x, y;
  int iWidth, iHeight;
  uint16_t *pPixels;
} JPEGDRAW;

typedef struct {
  void *fHandle;
} JPEGFILE;

typedef void *(*JPEG_OPEN_CALLBACK)(const char *, int32_t *);
typedef void (*JPEG_CLOSE_CALLBACK)(void *);
typedef int32_t (*JPEG_READ_CALLBACK)(JPEGFILE *, uint8_t *, int32_t);
typedef int32_t (*JPEG_SEEK_CALLBACK)(JPEGFILE *, int32_t);
typedef int (*JPEG_DRAW_CALLBACK)(JPEGDRAW *);

class JPEGDEC {
public:
  int open(const char *, JPEG_OPEN_CALLBACK, JPEG_CLOSE_CALLBACK,
           JPEG_READ_CALLBACK, JPEG_SEEK_CALLBACK, JPEG_DRAW_CALLBACK) {
    return 0; // fail - no JPEG support in simulator
  }
  int getHeight() { return 0; }
  int getWidth() { return 0; }
  void setMaxOutputSize(int) {}
  void setPixelType(int) {}
  int decode(int, int, int) { return 0; }
  void close() {}
};

#endif // _JPEGDEC_SHIM_H_
