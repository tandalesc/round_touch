#ifndef _SPI_SD_CARD_H_
#define _SPI_SD_CARD_H_

#include <SD.h>
#include "device/IStorage.h"

class SPISDCard : public IStorage {
private:
  SDFileSystemClass *driver = nullptr;
  bool _hasError = false;

public:
  SPISDCard() {}
  ~SPISDCard() {}

  void init() override {
    SPI.begin(SD_SCK, SD_MISO, SD_MOSI);
    if (!SD.begin(SD_CS)) {
      _hasError = true;
    } else {
      driver = &SD;
    }
  }

  bool hasError() override { return _hasError; }

  File open_r(const char *fileName) override {
#if defined(ESP32)
    File f = driver->open(fileName, "r");
#else
    File f = driver->open(fileName, FILE_READ);
#endif
    return f;
  }
};

#endif // _SPI_SD_CARD_H_
