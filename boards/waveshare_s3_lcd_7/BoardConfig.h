#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

// Waveshare ESP32-S3-Touch-LCD-7
// 7" 800x480 RGB parallel display + GT911 touch

// Display
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 480

// RGB Display data pins (RGB565 16-bit parallel)
#define LCD_R3 1
#define LCD_R4 2
#define LCD_R5 42
#define LCD_R6 41
#define LCD_R7 40
#define LCD_G2 39
#define LCD_G3 0
#define LCD_G4 45
#define LCD_G5 48
#define LCD_G6 47
#define LCD_G7 21
#define LCD_B3 14
#define LCD_B4 38
#define LCD_B5 18
#define LCD_B6 17
#define LCD_B7 10

// RGB Display control pins
#define LCD_HSYNC 46
#define LCD_VSYNC 3
#define LCD_PCLK 7
#define LCD_DE 5

// Touch pins (I2C)
#define TOUCH_SDA 8
#define TOUCH_SCL 9
#define TOUCH_INT 4
// Touch reset is via CH422G EXIO1

// SD Card pins (SPI)
// SD CS is via CH422G EXIO4
#define SD_MOSI 11
#define SD_SCK 12
#define SD_MISO 13

// CH422G I/O Expander
#define CH422G_I2C_ADDR 0x24
#define CH422G_EXIO_TP_RST 1
#define CH422G_EXIO_LCD_BL 2
#define CH422G_EXIO_LCD_RST 3
#define CH422G_EXIO_SD_CS 4

// Splash screen
#define SPLASH_SCREEN_JPEG_PATH "/logo_800480.jpg"

#endif // _BOARD_CONFIG_H_
