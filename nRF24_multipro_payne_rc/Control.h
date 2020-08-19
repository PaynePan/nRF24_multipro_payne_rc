#ifndef Control_H_
#define Control_H_
#include <Arduino.h>
#include "Ch.h"

class Control {
  protected:
    uint8_t pin;
    uint8_t cond_status;
    Ch * ch;

  public:
    Control();
    virtual void setPin(uint8_t p);
    virtual void handle() = 0;
    void setCh(Ch *c) { ch = c;} 
    void setCondStatus(uint8_t s) { cond_status = s;} 
	  uint8_t getCondStatus() { return cond_status; }
};

class BtnControl  : public Control {
protected:
    virtual bool isTriggered();
     long timeStamp = 0;
public:
    virtual void setPin(uint8_t p);    
    virtual void handle() = 0;
};

class FuncBtnControl  : public BtnControl {
private:
   void (*func)();
public:
    virtual void handle();
    void setFunc(void (*f)())  { func = f; }
};

class OffsetBtnControl  : public BtnControl {
private:
   bool dec;
   void (*longPressFunc)();
public:
    OffsetBtnControl();
    virtual void handle();

    void setDec(bool b) { dec = b; }
    virtual bool isTriggered();
 };

class ReverseBtnControl  : public BtnControl {
public:
    virtual void handle();
 };

class PosXBtnControl  : public BtnControl {
private:
    bool dec;
    uint8_t posCount;
public:
    PosXBtnControl();
    virtual void handle();

    void setDec(bool b) { dec = b; }
    void setPosCount(uint8_t v) { posCount = v; }
    virtual bool isTriggered();    
}; 

class AdjustBtnControl  : public BtnControl {
private:
    bool dec;
public:
    AdjustBtnControl();
    virtual void handle();

    void setDec(bool b) { dec = b; }
    virtual bool isTriggered();    
}; 


class MinMaxBtnControl  : public BtnControl {
public:
    virtual void handle();
};

class AnalogControl  : public Control {
private:
    
public:
    void setPin(uint8_t p);  
    void handle();
    void resetCenterAnalog();
    boolean resetPercent();
};

#ifdef BAT_ALART
class BatAlertControl  : public Control {
private:
public:
    virtual void handle();
};
#endif

#ifdef TEST_CHAN
class TestControl  : public Control {
public:
    TestControl();
    void handle();
};
#endif

#endif

