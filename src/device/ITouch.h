#ifndef _ITOUCH_H_
#define _ITOUCH_H_

#include "events/EventSource.h"
#include "events/types/InputEvent.h"

class ITouch : public EventSource<InputEvent> {
public:
  virtual ~ITouch() {}
  virtual void init() = 0;
};

#endif // _ITOUCH_H_
