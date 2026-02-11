#ifndef _ARDUINO_SHIM_H_
#define _ARDUINO_SHIM_H_

#include <SDL2/SDL.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <string>

// --- Types ---
typedef uint8_t byte;

// --- Pin modes ---
#define OUTPUT 1
#define INPUT 0
#define INPUT_PULLUP 2
#define HIGH 1
#define LOW 0

inline void pinMode(int, int) {}
inline void digitalWrite(int, int) {}
inline int digitalRead(int) { return 0; }
inline int analogRead(int) { return 0; }

// --- Timing ---
inline unsigned long millis() { return SDL_GetTicks(); }
inline unsigned long micros() { return SDL_GetTicks() * 1000; }

inline void delay(unsigned long ms) {
  unsigned long start = SDL_GetTicks();
  while (SDL_GetTicks() - start < ms) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) exit(0);
    }
    SDL_Delay(1);
  }
}

inline void delayMicroseconds(unsigned long) {}

// --- Math ---
using std::min;
using std::max;

// --- F() macro ---
#define F(x) (x)

// --- Color constants (RGB565) ---
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define YELLOW  0xFFE0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define ORANGE  0xFD20
#define DARKGREY 0x7BEF
#define LIGHTGREY 0xC618

// --- Serial ---
struct SerialShim {
  void begin(unsigned long) {}

  void print(const char *s) { printf("%s", s); }
  void print(char c) { printf("%c", c); }
  void print(int v) { printf("%d", v); }
  void print(unsigned int v) { printf("%u", v); }
  void print(long v) { printf("%ld", v); }
  void print(unsigned long v) { printf("%lu", v); }
  void print(float v) { printf("%f", v); }
  void print(double v) { printf("%f", v); }

  void println() { printf("\n"); }
  void println(const char *s) { printf("%s\n", s); }
  void println(char c) { printf("%c\n", c); }
  void println(int v) { printf("%d\n", v); }
  void println(unsigned int v) { printf("%u\n", v); }
  void println(long v) { printf("%ld\n", v); }
  void println(unsigned long v) { printf("%lu\n", v); }
  void println(float v) { printf("%f\n", v); }
  void println(double v) { printf("%f\n", v); }
};

// Forward declare so String can use it
class String;

static SerialShim Serial;

// --- String class (minimal Arduino-compatible) ---
class String {
  std::string _s;
public:
  String() {}
  String(const char *s) : _s(s ? s : "") {}
  String(const std::string &s) : _s(s) {}
  String(int val) : _s(std::to_string(val)) {}

  bool operator==(const char *other) const { return _s == other; }
  bool operator==(const String &other) const { return _s == other._s; }
  bool operator!=(const char *other) const { return _s != other; }
  bool operator!=(const String &other) const { return _s != other._s; }

  String operator+(const String &other) const { return String(_s + other._s); }
  String operator+(const char *other) const { return String(_s + other); }

  const char *c_str() const { return _s.c_str(); }
  int length() const { return (int)_s.length(); }

  // Allow Serial.print(String)
  operator const char *() const { return _s.c_str(); }
};

// --- ESP32 define ---
// Do NOT define ESP32 so the SDCard.h uses FILE_READ path
// #define ESP32

#define FILE_READ "r"

// --- SPI constants ---
#define HSPI 0

#endif // _ARDUINO_SHIM_H_
