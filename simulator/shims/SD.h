#ifndef _SD_SHIM_H_
#define _SD_SHIM_H_

#include <cstdint>
#include <cstddef>
#include "SPI.h"

// --- Stub File class ---
class File {
public:
  File() {}
  operator bool() const { return false; }
  size_t size() { return 0; }
  size_t read(uint8_t *, size_t) { return 0; }
  bool seek(int32_t) { return false; }
  void close() {}
};

// --- Stub SD filesystem ---
class SDFileSystemClass {
public:
  bool begin(int = -1) { return false; }
  File open(const char *, const char * = "r") { return File(); }
};

static SDFileSystemClass SD;

#endif // _SD_SHIM_H_
