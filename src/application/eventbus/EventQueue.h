#ifndef _EVENT_QUEUE_H_
#define _EVENT_QUEUE_H_

#include <vector>

template <typename T> using Callback = void (*)(T &);

// basic generic message queue
template <typename T> class EventQueue {
private:
  std::vector<T> queue;
  std::vector<Callback<T>> callbackQueue;

public:
  void post(T event) {
    queue.push_back(event);
    for (Callback<T> callback : callbackQueue) {
      callback(event);
    }
  };
  
  void subscribe(Callback<T> callback) {
    callbackQueue.push_back(callback);
  };
};

#endif // _EVENT_QUEUE_H_