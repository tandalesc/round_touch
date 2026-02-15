#ifndef _ISTORAGE_H_
#define _ISTORAGE_H_

#include <SD.h>

class IStorage {
public:
  virtual ~IStorage() {}
  virtual void init() = 0;
  virtual bool hasError() = 0;
  virtual File open_r(const char *fileName) = 0;
};

#endif // _ISTORAGE_H_
