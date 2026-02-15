#ifndef _CH422G_H_
#define _CH422G_H_

#include <Wire.h>

// CH422G I/O Expander - static utility class
// Must be initialized before LCD and touch (controls their reset pins)
class CH422G {
public:
  static void init() {
    Wire.begin(TOUCH_SDA, TOUCH_SCL);
  }

  // Set all EXIO pins to output mode (push-pull)
  static void setAllOutput() {
    Wire.beginTransmission(0x24);
    // Set mode register: OC push-pull, all output
    Wire.write(0x01);
    Wire.endTransmission();

    // Set output enable for EXIO pins
    Wire.beginTransmission(0x26);
    Wire.write(0x0F); // Enable EXIO0-3 as output
    Wire.endTransmission();
  }

  // Set a specific EXIO pin high or low
  // pin: 1-based EXIO pin number (1=EXIO1, 2=EXIO2, etc.)
  static void setPin(uint8_t pin, bool high) {
    // Read current state
    uint8_t state = readOutputState();
    if (high) {
      state |= (1 << (pin - 1));
    } else {
      state &= ~(1 << (pin - 1));
    }
    writeOutputState(state);
  }

private:
  static uint8_t _outputState;

  static uint8_t readOutputState() { return _outputState; }

  static void writeOutputState(uint8_t state) {
    _outputState = state;
    Wire.beginTransmission(0x26);
    Wire.write(state);
    Wire.endTransmission();
  }
};

uint8_t CH422G::_outputState = 0;

#endif // _CH422G_H_
