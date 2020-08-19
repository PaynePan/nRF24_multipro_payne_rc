#ifndef TYPES_H_
#define TYPES_H_

#include "Config.h"
#include <arduino.h>

enum {
  STATUS_NORMAL = 3,
  STATUS_F1_DOWN = 1,
  STATUS_F2_DOWN = 2,
  STATUS_F1_F2_DOWN = 0,
};

typedef struct {
    int16_t offset;
    boolean reversed;
    uint8_t  percent;
    uint16_t centerAnalog;	
} Ch_t;

typedef struct {
   Ch_t ch[CHAN_SIZE+1];	
   uint8_t checksum;   
} Settings_t;

#endif
