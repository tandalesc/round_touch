#ifndef _SPI_SHIM_H_
#define _SPI_SHIM_H_

#include <cstdint>

class SPIClass {
public:
  void begin(int = -1, int = -1, int = -1, int = -1) {}
  void end() {}
};

static SPIClass SPI;

#endif // _SPI_SHIM_H_
