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

    bool infoLogs = true;
    bool warningLogs = true;
    bool errorLogs = true;

    void log(String data);
    void info(String data);
    void warn(String data);
    void error(String data);
    String get();
    void waiting();
    void waitingFinished();
};

#endif
