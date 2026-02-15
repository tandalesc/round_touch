#ifndef _NO_STORAGE_H_
#define _NO_STORAGE_H_

#include "device/IStorage.h"

// Placeholder storage driver for boards where SD card is not yet supported
class NoStorage : public IStorage {
public:
  void init() override {}
  bool hasError() override { return true; }
  File open_r(const char *) override { return File(); }
};

#endif // _NO_STORAGE_H_
