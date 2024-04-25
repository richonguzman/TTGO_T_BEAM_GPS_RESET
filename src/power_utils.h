#ifndef POWER_UTILS_H_
#define POWER_UTILS_H_

#include <Arduino.h>
#include "XPowersLib.h"

namespace POWER_Utils {

    void activateGPS();

    bool begin(TwoWire &port);
    void setup();
  
}

#endif
