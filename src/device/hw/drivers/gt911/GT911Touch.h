#ifndef _GT911_TOUCH_H_
#define _GT911_TOUCH_H_

#include <Arduino.h>
#include <Wire.h>

#include "BoardConfig.h"
#include "device/ITouch.h"
#include "device/types/TouchLocation.h"
#include "events/types/TouchEvent.h"

// GT911 I2C address — 0x5D when INT is LOW during reset (set by Device::init)
#define GT911_ADDR 0x5D

// GT911 registers
#define GT911_REG_STATUS 0x814E
#define GT911_REG_POINT1 0x814F

class GT911Touch : public ITouch {
public:
  GT911Touch() {}

  void init() override {
    writeReg(GT911_REG_STATUS, 0);
  }

  void pollEvent(EventHandler<InputEvent> *handler) override;

private:
  bool _wasTouched = false;
  TouchLocation _touchStart = {};
  TouchLocation _lastPos = {};

  void writeReg(uint16_t reg, uint8_t val) {
    Wire.beginTransmission(GT911_ADDR);
    Wire.write(reg >> 8);
    Wire.write(reg & 0xFF);
    Wire.write(val);
    Wire.endTransmission();
  }

  uint8_t readReg(uint16_t reg) {
    Wire.beginTransmission(GT911_ADDR);
    Wire.write(reg >> 8);
    Wire.write(reg & 0xFF);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)GT911_ADDR, (uint8_t)1);
    return Wire.available() ? Wire.read() : 0;
  }

  void readRegs(uint16_t reg, uint8_t *buf, uint8_t len) {
    Wire.beginTransmission(GT911_ADDR);
    Wire.write(reg >> 8);
    Wire.write(reg & 0xFF);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)GT911_ADDR, len);
    for (uint8_t i = 0; i < len && Wire.available(); i++) {
      buf[i] = Wire.read();
    }
  }
};

#endif // _GT911_TOUCH_H_
