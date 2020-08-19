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
 #include "Control.h"
#include "Config.h"

Control::Control() {
  cond_status = STATUS_NORMAL;
}

void Control::setPin(uint8_t p) {
  pin =p; 
} 


void AnalogControl::setPin(uint8_t p) {
  pin = p;
}

void AnalogControl::handle() {
  uint16_t v = analogRead(pin);

  uint16_t c = ch->getCenterAnalog();
  if (v < c) {
    v = map(v, 0, c, 1000, 1500);
  } else {
    v = map(v, c, ANA_MAX, 1500, 2000);
  }
  ch->setRawValue(v);
}

void AnalogControl::resetCenterAnalog() {
  uint16_t v = analogRead(pin);
  if ( abs((int)v-ANA_CENTER) < 100)
      ch->setCenterAnalog(v);
}

boolean AnalogControl::resetPercent() {
  uint16_t v = analogRead(pin);
  uint16_t c = ch->getCenterAnalog();
  uint8_t p = 100;
  if (v < c) {
    p = (c - v) * 100 / c;
  } else {
    p = (v - c) * 100 / (ANA_MAX - c);
  }
 if ( p >= 20) {
    ch->setPercent(p);
    return true;
  }
  return false;
}

void BtnControl::setPin(uint8_t p) {
  pin =p; 
  pinMode(pin,INPUT_PULLUP);
} 

boolean BtnControl::isTriggered() {
  
  if ( digitalRead(pin) == LOW) {
    delayMicroseconds(1);
    timeStamp = millis();
    return 1;
  }
  return  0;
}

void FuncBtnControl::handle() {
  if (isTriggered()) {
    func();
  }
}

OffsetBtnControl::OffsetBtnControl() {
  dec = false;
}

boolean OffsetBtnControl::isTriggered() {
  static long last = 0;
  long now = millis(); 

  if ( now - last > 200 && digitalRead(pin) == LOW) {
    delayMicroseconds(1);
    last = now;
    return 1;
  }
  return  0;
}

void OffsetBtnControl::handle() {
  if (isTriggered()) {
    ch->changeOffset(dec);
  }
}

void ReverseBtnControl::handle() {
   if (  millis() - timeStamp < 350)  // NOT Allow  Value jump , when user hold the key
    return;
  if (isTriggered()) {
     ch->toggleReversed();
  }
}

PosXBtnControl::PosXBtnControl() {
  dec = false;
  posCount = 6;
}


boolean PosXBtnControl::isTriggered() {
  static long last = 0;
  long now = millis(); 

  if ( now - last > 200 && digitalRead(pin) == LOW) {
    delayMicroseconds(1);
    last = now;
    return 1;
  }
  return  0;
}

void PosXBtnControl::handle() {
  if (isTriggered()) {
      uint16_t r = ch->getRawValue();
      if ( abs(r-1500) < 10)
        r = 1500;

    uint16_t seg = 1000/posCount;

     uint16_t v = dec? -seg: seg;
     v = r+v;
     if ( v > PWM_MIN  && v < PWM_MAX) {
         ch->setRawValue(v);
       if (abs(v -1500) < 20)  tone(Tone_pin,1800,80);
       else tone(Tone_pin,1200,20);
     }
  }
}


AdjustBtnControl::AdjustBtnControl() {
  dec = false;
}


boolean AdjustBtnControl::isTriggered() {
  static long last = 0;
  long now = millis(); 

  if ( now - last > 40 && digitalRead(pin) == LOW) {
    delayMicroseconds(1);
    last = now;
    return 1;
  }
  return  0;
}

void AdjustBtnControl::handle() {
    static boolean alarm = true;
  if (isTriggered()) {
    uint16_t r = ch->getRawValue();
     uint16_t v = dec? -25: 25;
     v = r+v;
     if ( v > PWM_MIN  && v < PWM_MAX) {
         alarm = true;
         ch->setRawValue(v);
       if (abs(v -1500) < 20)  tone(Tone_pin,1800,80);
     } else {
       if ( alarm ) {
        tone(Tone_pin,1200,10);
        alarm = false;
       }
     }
  }
}

void MinMaxBtnControl::handle() {
   if (  millis() - timeStamp < 350)  // NOT Allow  Value jump , when user hold the key
    return;

    if (ch->getRawValue() < 1750) {
      ch->setRawValue(1000);
    } else {
      ch->setRawValue(2000);
    }
    
  if (isTriggered()) {
    if (ch->getRawValue() == 1000) {
        ch->setRawValue(2000);
        tone(Tone_pin,1300,40);
    } else if (ch->getRawValue() == 2000) {
        ch->setRawValue(1000);
        tone(Tone_pin,1100,40);        
    }
  }
}

// =====================================
#ifdef BAT_ALART  
#define BAT_FACTOR  2
#define BAT_BASE  3.3
#define BAT_MIN  3.6

void BatAlertControl::handle() {
  static long last = 0;
  long now = millis(); 
  if ( now - last > 1000*10) {  //every 10 seconds check
       float v = (float)(analogRead(BAT_PIN) * BAT_FACTOR)/ANA_MAX * BAT_BASE;
       if (v < BAT_MIN)  { // 1.1 factor only for 1S
      tone(Tone_pin,1600,800);
      }
      last = now;      
  }
}
#endif  

#ifdef TEST_CHAN
TestControl::TestControl() {
}

void TestControl::handle() {
  static long last = 0;
  uint16_t v;
  long now = millis(); 
  if ( now - last > 10) {
    v = ch->getRawValue() + 20;
    if (v >= PWM_MAX) {
       v = PWM_MIN;
    }
    ch->setRawValue(v);
    last = now;
  }
}
#endif
