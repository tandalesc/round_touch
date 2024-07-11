#ifndef _SD_CARD_H_
#define _SD_CARD_H_

#include <SD.h>

#define SD_SCK 42
#define SD_MISO 41
#define SD_MOSI 2
#define SD_CS 1

class SDCard {
private:
  SDCard() {
    SPI.begin(SD_SCK, SD_MISO, SD_MOSI);
    if (!SD.begin(SD_CS)) {
      errorWhileInit = true;
    }
  }

public:
  bool errorWhileInit = false;
  static SDCard &getInstance() {
    static SDCard sdcard;
    return sdcard;
  }
  ~SDCard() {}
  SDCard(SDCard const &) = delete;
  void operator=(SDCard const &) = delete;
};

#endif // _SD_CARD_H_