/**
 * @file lv_conf.h
 * Configuration file for LVGL v9.2.2
 * Based on lv_conf_template.h - only non-default values are set here.
 */

/* clang-format off */
#if 1 /*Enable content*/

#ifndef LV_CONF_H
#define LV_CONF_H

/*====================
   STDLIB WRAPPER SETTINGS
 *====================*/

/* Use standard C library functions instead of LVGL builtins */
#define LV_USE_STDLIB_MALLOC    LV_STDLIB_CLIB
#define LV_USE_STDLIB_STRING    LV_STDLIB_CLIB
#define LV_USE_STDLIB_SPRINTF   LV_STDLIB_CLIB

/*====================
   FEATURE CONFIGURATION
 *====================*/

/*Enable the log module*/
#define LV_USE_LOG 1
#if LV_USE_LOG
    #define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
#endif

/*==================
 * WIDGETS
 *================*/

/* Disable all widgets except label */
#define LV_USE_ANIMIMG    0
#define LV_USE_ARC        0
#define LV_USE_BAR        0
#define LV_USE_BUTTON     0
#define LV_USE_BUTTONMATRIX  0
#define LV_USE_CALENDAR   0
#define LV_USE_CANVAS     0
#define LV_USE_CHART      0
#define LV_USE_CHECKBOX   0
#define LV_USE_DROPDOWN   0
#define LV_USE_IMAGE      0
#define LV_USE_IMAGEBUTTON 0
#define LV_USE_KEYBOARD   0
/* LV_USE_LABEL defaults to 1 */
#define LV_USE_LED        0
#define LV_USE_LINE       0
#define LV_USE_LIST       0
#define LV_USE_LOTTIE     0
#define LV_USE_MENU       0
#define LV_USE_MSGBOX     0
#define LV_USE_ROLLER     0
#define LV_USE_SCALE      0
#define LV_USE_SLIDER     0
#define LV_USE_SPAN       0
#define LV_USE_SPINBOX    0
#define LV_USE_SPINNER    0
#define LV_USE_SWITCH     0
#define LV_USE_TEXTAREA   0
#define LV_USE_TABLE      0
#define LV_USE_TABVIEW    0
#define LV_USE_TILEVIEW   0
#define LV_USE_WIN        0

/*==================
 * LAYOUTS
 *==================*/

/* LV_USE_FLEX defaults to 1 */
#define LV_USE_GRID 0

/*==================
 * DEVICES
 *==================*/

/*Use SDL to open window on PC (simulator only)*/
#ifdef BOARD_SIMULATOR
    #define LV_USE_SDL              1
    #if LV_USE_SDL
        #define LV_SDL_INCLUDE_PATH     <SDL2/SDL.h>
        #define LV_SDL_RENDER_MODE      LV_DISPLAY_RENDER_MODE_DIRECT
        #define LV_SDL_BUF_COUNT        1
        #define LV_SDL_ACCELERATED      1
        #define LV_SDL_FULLSCREEN       0
        #define LV_SDL_DIRECT_EXIT      1
        #define LV_SDL_MOUSEWHEEL_MODE  LV_SDL_MOUSEWHEEL_MODE_ENCODER
    #endif
#else
    #define LV_USE_SDL              0
#endif

/*==================
 * EXAMPLES & DEMOS
 *==================*/

#define LV_BUILD_EXAMPLES 0

/*--END OF LV_CONF_H--*/

#endif /*LV_CONF_H*/

#endif /*End of "Content enable"*/
