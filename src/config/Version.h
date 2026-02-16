#ifndef _VERSION_H_
#define _VERSION_H_

#define FIRMWARE_VERSION "0.0.0"

#ifdef BOARD_MAKERFABS_ROUND_128
  #define BOARD_ID "makerfabs_round_128"
#elif defined(BOARD_WAVESHARE_S3_LCD_7)
  #define BOARD_ID "waveshare_s3_lcd_7"
#elif defined(BOARD_SIMULATOR)
  #define BOARD_ID "simulator"
#else
  #define BOARD_ID "unknown"
#endif

#endif // _VERSION_H_
