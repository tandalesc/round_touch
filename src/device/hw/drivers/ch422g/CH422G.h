#ifndef _CH422G_H_
#define _CH422G_H_

#include <Arduino.h>
#include <Wire.h>

#include "BoardConfig.h"

// CH422G I/O Expander - static utility class
// Each register type has its own I2C address (no internal register offset)
#define CH422G_WR_SET_ADDR 0x24  // System parameter / mode register
#define CH422G_WR_OC_ADDR  0x23  // Write OC output pins (8-11)
#define CH422G_WR_IO_ADDR  0x38  // Write IO output pins (0-7, includes EXIO1-4)
#define CH422G_RD_IO_ADDR  0x26  // Read IO input pins (0-7)

class CH422G {
public:
  // Call after Wire.begin() has been called externally
  static void init() {
    // CRITICAL: Write pin states BEFORE enabling output mode.
    // When IO_OE switches to 1 (output), pins take on whatever WR_IO holds.
    // If we set mode first, pins briefly default to 0xFF, driving IO0 (USB_SEL)
    // HIGH and killing USB serial.
    _ioState = 0xDF;  // EXIO1-4 high, IO5 (USB_SEL) LOW
    Wire.beginTransmission(CH422G_WR_IO_ADDR);
    Wire.write(_ioState);
    Wire.endTransmission();

    // Now enable output mode: IO_OE=1, OD_EN=0 (push-pull), SLEEP=0
    Wire.beginTransmission(CH422G_WR_SET_ADDR);
    Wire.write(0x01);
    Wire.endTransmission();
  }

  // Set a specific IO pin high or low
  // pin: 0-based bit position in the IO register (EXIO1=1, EXIO2=2, etc.)
  static void setPin(uint8_t pin, bool high) {
    if (pin == 5) return;  // IO5 = USB_SEL, never touch
    if (high) {
      _ioState |= (1 << pin);
    } else {
      _ioState &= ~(1 << pin);
    }
    Wire.beginTransmission(CH422G_WR_IO_ADDR);
    Wire.write(_ioState);
    Wire.endTransmission();
  }

private:
  static uint8_t _ioState;
};

uint8_t CH422G::_ioState = 0xDF;  // IO5 (USB_SEL) must stay LOW

#endif // _CH422G_H_
