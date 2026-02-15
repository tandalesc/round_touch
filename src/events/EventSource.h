#pragma once

#include "events/EventHandler.h"

template <typename T> class EventSource {
public:
  virtual void pollEvent(EventHandler<T> *handler) = 0;
};