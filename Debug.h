#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>

class Debug
{
  public:
    bool enabled = false;
    bool serial = false;
    bool store = false;
    String data;

    void log(String data);
    String get();
    void waiting();
    void waitingFinished();
};

#endif
