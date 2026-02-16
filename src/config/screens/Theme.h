#ifndef _SCREEN_THEME_H_
#define _SCREEN_THEME_H_

// Zinc/neutral palette (Tailwind)
#define CLR_ZINC_50   0xFAFAFA
#define CLR_ZINC_100  0xF4F4F5
#define CLR_ZINC_200  0xE4E4E7
#define CLR_ZINC_300  0xD4D4D8
#define CLR_ZINC_400  0xA1A1AA
#define CLR_ZINC_500  0x71717A
#define CLR_ZINC_700  0x3F3F46
#define CLR_ZINC_800  0x27272A
#define CLR_ZINC_900  0x18181B
#define CLR_ZINC_950  0x09090B
// Green palette (Tailwind)
#define CLR_GREEN_200 0xBBF7D0
#define CLR_GREEN_700 0x15803D
#define CLR_GREEN_800 0x166534
// Semantic colors
#define CLR_GREEN     0x22C55E
#define CLR_BLUE      0x3B82F6
#define CLR_RED       0xEF4444
#define CLR_AMBER     0xF59E0B

// Main navigation tabs shared across screens
#define MAIN_TABS \
  {LV_SYMBOL_HOME, "Home", READY}, \
  {LV_SYMBOL_TINT, "Gauges", INFO1}, \
  {LV_SYMBOL_BARS, "Tuning", INFO2}, \
  {LV_SYMBOL_SETTINGS, "Settings", INFO3}, \
  {LV_SYMBOL_DRIVE, "Car", LIGHTS}

#endif // _SCREEN_THEME_H_
