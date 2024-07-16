#ifndef _TIMER_H_
#define _TIMER_H_

#include <Arduino.h>

class Timer {
private:
  unsigned long timeout = 0;
  unsigned long interval;

public:
  Timer(unsigned long interval) : interval(interval) {};
  void start() { timeout = millis() + interval; }
  bool running() { return millis() < timeout; }
  bool stopped() { return !running(); }
};

#endif // _TIMER_H_