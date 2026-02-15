#ifndef _JPEG_IO_H_
#define _JPEG_IO_H_

#include <JPEGDEC.h>

#include "device/IStorage.h"

// Global pointer set before JPEG decode, used by the callbacks
static IStorage *_jpegStorage = nullptr;
static File _f;

static void *jpegOpenFile(const char *szFilename, int32_t *pFileSize) {
  if (!_jpegStorage) return nullptr;
  _f = _jpegStorage->open_r(szFilename);
  *pFileSize = _f.size();
  return &_f;
}

static void jpegCloseFile(void *pHandle) {
  File *f = static_cast<File *>(pHandle);
  f->close();
}

static int32_t jpegReadFile(JPEGFILE *pFile, uint8_t *pBuf, int32_t iLen) {
  File *f = static_cast<File *>(pFile->fHandle);
  size_t r = f->read(pBuf, iLen);
  return r;
}

static int32_t jpegSeekFile(JPEGFILE *pFile, int32_t iPosition) {
  File *f = static_cast<File *>(pFile->fHandle);
  f->seek(iPosition);
  return iPosition;
}

#endif // _JPEG_IO_H_
