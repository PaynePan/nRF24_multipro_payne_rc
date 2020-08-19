/*
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License.
 If not, see <http://www.gnu.org/licenses/>.
 */
#include "Ch.h"
#include "Config.h"
#include "payne_rc.h"

Ch::Ch() {
    increment = OFFSET_STEP;
    raw_value = 1500;
}

void Ch::setReversed(bool b) {
  if (  data->reversed != b ) {
      data->reversed = b;
      settings.refreshUpdateTime();
  }
}

void Ch::toggleReversed() {
  data->reversed = !data->reversed;
  tone(Tone_pin,2500,40);
  settings.refreshUpdateTime();
}
void Ch::setPercent(uint8_t v) { 
  data->percent = v; 
  settings.refreshUpdateTime();  
}

void Ch::setCenterAnalog(uint16_t v) {
  data->centerAnalog = v; 
  settings.refreshUpdateTime();  
}

void Ch::changeOffset(bool dec) {
  if (offsetReverse)
    dec = !dec;
  
   if (dec) {
      if (data->offset - increment < -OFFSET_RANGE) {
          return;
      } else {
        data->offset -=increment;
      }
   } else {
      if ( data->offset + increment > OFFSET_RANGE) {
          return;
      } else {
        data->offset  += increment;
      }
   }
   
   if ( data->offset == 0 ) {
      tone(Tone_pin,1800,80);
   } else {
      tone(Tone_pin,1000,40);
   }
   settings.refreshUpdateTime();
}

uint16_t Ch::getValue() {
    int32_t v  = raw_value;
    v =  1500 + (v-1500) * data->percent/100;
    v = constrain( v + data->offset, PWM_MIN, PWM_MAX);
    if (data->reversed)
      v = PWM_MIN  + (PWM_MAX - v);
    return v;
}
