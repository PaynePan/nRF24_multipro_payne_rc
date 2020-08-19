#ifndef ControlManager_H_
#define ControlManager_H_
#include <Arduino.h>
#include "Control.h"

#define CTL_MAX   24
class ControlManager {
  public:
    Control *ctls[CTL_MAX];
    char ctl_count;

    void add(Control *ctl);
    ControlManager();
    void handle(uint8_t status);
};

#define ANA_CTL_MAX   4

class AnalogControlManager {
  // only allow 1,2,4  in AnalogControlManager
  public:
    AnalogControl *ctls[ANA_CTL_MAX];
    char ctl_count;
    
    void add(AnalogControl *ctl);
    AnalogControlManager();
    void resetCenterAnalog();
    boolean resetPercent();
};

#endif
