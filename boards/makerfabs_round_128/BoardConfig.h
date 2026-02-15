#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

// MakerFabs ESP32-S3 Round SPI TFT with Touch
// 1.28" GC9A01 240x240 circular display + CST816S touch

// Display
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240

// Display pins (SPI)
#define TFT_BLK 45
#define TFT_RES 11
#define TFT_CS 15
#define TFT_MOSI 13
#define TFT_MISO 12
#define TFT_SCLK 14
#define TFT_DC 21

// Touch pins (I2C)
#define TOUCH_SDA 38
#define TOUCH_SCL 39
#define TOUCH_RST 16
#define TOUCH_INT 40

// SD Card pins (SPI)
#define SD_SCK 42
#define SD_MISO 41
#define SD_MOSI 2
#define SD_CS 1

// Splash screen
#define SPLASH_SCREEN_JPEG_PATH "/logo_240240.jpg"

#endif // _BOARD_CONFIG_H_
