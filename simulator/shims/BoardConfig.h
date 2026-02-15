#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

// Simulator board config
// Defaults to MakerFabs dimensions, can be changed at compile time

#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 240
#endif

#ifndef SCREEN_HEIGHT
#define SCREEN_HEIGHT 240
#endif

// Dummy pin defines (not used in simulator)
#define TFT_BLK 0
#define TFT_RES 0
#define TFT_CS 0
#define TFT_MOSI 0
#define TFT_MISO 0
#define TFT_SCLK 0
#define TFT_DC 0

#define TOUCH_SDA 0
#define TOUCH_SCL 0
#define TOUCH_RST 0
#define TOUCH_INT 0

#define SD_SCK 0
#define SD_MISO 0
#define SD_MOSI 0
#define SD_CS 0

#define SPLASH_SCREEN_JPEG_PATH "/logo_240240.jpg"

// BOARD_SIMULATOR is defined via CMake compile definition (add_compile_definitions)

#endif // _BOARD_CONFIG_H_
