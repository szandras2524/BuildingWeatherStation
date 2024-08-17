#ifndef _OBSERVER_H_
#define _OBSERVER_H_

#include <Arduino.h>


class Observer {
public:
    virtual void notify(const char* topic, byte* payload, unsigned int length) = 0;
};


#endif