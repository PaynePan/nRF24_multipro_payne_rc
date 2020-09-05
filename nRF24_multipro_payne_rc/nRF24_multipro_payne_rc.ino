// ###################################################################
// #####   MultiProtocol nRF24L01 Tx  With Payne_RC integrated  ######
// ###################################################################
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License.
// If not, see <http://www.gnu.org/licenses/>.


#include <util/atomic.h>
#include <EEPROM.h>
#include "iface_nrf24l01.h"

#include "Config.h"
#include "Ch.h"
#include "ControlManager.h"
#include "Settings.h"
#include <avr/eeprom.h>
#include <SPI.h>

#define PPM_MIN 1000
#define PPM_SAFE_THROTTLE 1050
#define PPM_MID 1500
#define PPM_MAX 2000
#define PPM_MIN_COMMAND 1300
#define PPM_MAX_COMMAND 1700
#define GET_FLAG(ch, mask) (ppm[ch] > PPM_MAX_COMMAND ? mask : 0)
#define GET_FLAG_INV(ch, mask) (ppm[ch] < PPM_MIN_COMMAND ? mask : 0)

#define PPM_FrLen 22500  //set the PPM frame length in microseconds (1ms = 1000μs)
#define PPM_PulseLen 300  //set the pulse length

Ch *chs[CHAN_SIZE + 1];
uint16_t ppms[CHAN_SIZE];
ControlManager controlManager;
AnalogControlManager anaCtlManager;
Settings settings;


#define CE_on
#define CE_off
#define CS_pin     10 // CS   - A1
#define CS_on PORTB |= _BV(2);
#define CS_off PORTB &= ~_BV(2);
#define ledPin A4

#define RF_POWER TX_POWER_80mW

// tune ppm input for "special" transmitters
// #define SPEKTRUM // TAER, 1100-1900, AIL & RUD reversed

// PPM stream settings
#define CHANNELS 12 // number of channels in ppm stream, 12 ideally
enum chan_order {
  AILERON,
  ELEVATOR,
  THROTTLE,
  RUDDER,
  AUX1,  // (CH5)  led light, or 3 pos. rate on CX-10, H7, or inverted flight on H101
  AUX2,  // (CH6)  flip control
  AUX3,  // (CH7)  still camera (snapshot)
  AUX4,  // (CH8)  video camera
  AUX5,  // (CH9)  headless
  AUX6,  // (CH10) calibrate Y (V2x2), pitch trim (H7), RTH (Bayang, H20), 360deg flip mode (H8-3D, H22)
  AUX7,  // (CH11) calibrate X (V2x2), roll trim (H7), emergency stop (Bayang, Silverware)
  AUX8,  // (CH12) Reset / Rebind
};



// supported protocols
enum {
  PROTO_V2X2 = 0,     // WLToys V2x2, JXD JD38x, JD39x, JJRC H6C, Yizhan Tarantula X6 ...
  PROTO_CG023,        // EAchine CG023, CG032, 3D X4
  PROTO_CX10_BLUE,    // Cheerson CX-10 blue board, newer red board, CX-10A, CX-10C, Floureon FX-10, CX-Stars (todo: add DM007 variant)
  PROTO_CX10_GREEN,   // Cheerson CX-10 green board
  PROTO_H7,           // EAchine H7, MoonTop M99xx
  PROTO_BAYANG,       // EAchine H8(C) mini, H10, BayangToys X6, X7, X9, JJRC JJ850, Floureon H101
  PROTO_SYMAX5C1,     // Syma X5C-1 (not older X5C), X11, X11C, X12
  PROTO_YD829,        // YD-829, YD-829C, YD-822 ...
  PROTO_H8_3D,        // EAchine H8 mini 3D, JJRC H20, H22
  PROTO_MJX,          // MJX X600 (can be changed to Weilihua WLH08, X800 or H26D)
  PROTO_SYMAXOLD,     // Syma X5C, X2
  PROTO_HISKY,        // HiSky RXs, HFP80, HCP80/100, FBL70/80/90/100, FF120, HMX120, WLToys v933/944/955 ...
  PROTO_KN,           // KN (WLToys variant) V930/931/939/966/977/988
  PROTO_YD717,        // Cheerson CX-10 red (older version)/CX11/CX205/CX30, JXD389/390/391/393, SH6057/6043/6044/6046/6047, FY326Q7, WLToys v252 Pro/v343, XinXun X28/X30/X33/X39/X40
  PROTO_FQ777124,     // FQ777-124 pocket drone
  PROTO_E010,         // EAchine E010, NiHui NH-010, JJRC H36 mini
  PROTO_BAYANG_SILVERWARE, // Bayang for Silverware with frsky telemetry
  PROTO_END
};

// EEPROM locationss
enum {
  ee_PROTOCOL_ID = 0,
  ee_TXID0,
  ee_TXID1,
  ee_TXID2,
  ee_TXID3
};

struct {
  uint16_t volt1;
  uint16_t rssi;
  uint8_t updated;
  uint32_t lastUpdate;
} telemetry_data;

uint8_t transmitterID[4];
uint8_t current_protocol;
static volatile bool ppm_ok = false;
uint8_t packet[32];
static bool reset = true;
//volatile uint16_t Servo_data[12];
static uint16_t ppm[12] = {PPM_MIN, PPM_MIN, PPM_MIN, PPM_MIN, PPM_MID, PPM_MID,
                           PPM_MID, PPM_MID, PPM_MID, PPM_MID, PPM_MID, PPM_MID,
                          };

void setup()
{  
  randomSeed((analogRead(A4) & 0x1F) | (analogRead(A5) << 5));  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); //start LED off
  pinMode(CS_pin, OUTPUT);
  //pinMode(CE_pin, OUTPUT);

  // Initialize SPI
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  CE_off
  CS_on

  //frskyInit();
  set_txid(false);
  payne_rc_setup();
  tone(Tone_pin, 1400, 80); 
  delay(200); 
}

void payne_rc_setup() {
  pinMode(F1, INPUT_PULLUP);
  pinMode(F2, INPUT_PULLUP);

#if defined(DEBUG)
  Serial.begin(57600);//for debug
  Serial.println("start");
#endif
  // init channel
  for (int i = 1; i <= CHAN_SIZE; i++) {
    chs[i] = new Ch();
  }

  settings.load();
  for (int i = 1; i <= CHAN_SIZE; i++) {
    chs[i]->bind(settings.getChData(i));
  }
  for (int i = 5; i <= CHAN_SIZE; i++) {
    chs[i]->setRawValue(PWM_MIN);
  }


  Control *c;
#include "pins_atmel.h"

#ifdef BAT_ALART
  c = new BatAlertControl(); controlManager.add(c);
#endif

#include "ppm_init.h"

}

void payne_rc_handle_special() {
  static long last = 0;
  long now = millis();

  if ( now - last < 300 )
    return;
  last = now;

  if (digitalRead(Btn1) == LOW && analogRead(A7) > 850) {
    settings.loadDefaults();
    tone(Tone_pin, 2300, 80);
    return;
  } else {
    if ( anaCtlManager.resetPercent() ) {
      tone(Tone_pin, 2400, 80);
    }
  }
}

void payne_rc_loop() {
  uint8_t f1 = digitalRead(F1);
  uint8_t f2 = digitalRead(F2);
  uint8_t status = (f1 << 1) | f2;

  if ( status == STATUS_F1_F2_DOWN ) {
    payne_rc_handle_special();
  }

  controlManager.handle(status);
  for (int i = 0; i < CHAN_SIZE; i++) {
    ppms[i] = chs[i + 1]->getValue();
  }
  settings.saveIfUpdate();
}



void update_ppm()
{
  payne_rc_loop();  
  for (uint8_t ch = 0; ch < CHAN_SIZE; ch++) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      ppm[ch] = ppms[ch];
    }
  }
  ppm_ok = true;
}

void loop()
{
  uint32_t timeout = 0;

  // reset / rebind
  if (reset || ppm[AUX8] > PPM_MAX_COMMAND) {
    reset = false;
    selectProtocol();
    tone(Tone_pin, 1800, 80);    
    NRF24L01_Reset();
    NRF24L01_Initialize();
    init_protocol();
    tone(Tone_pin, 2200, 80);    
  }
  //telemetry_data.updated = 0;
  // process protocol
  switch (current_protocol) {
    case PROTO_CG023:
    case PROTO_YD829:
      timeout = process_CG023();
      break;
    case PROTO_V2X2:
      timeout = process_V2x2();
      break;
    case PROTO_CX10_GREEN:
    case PROTO_CX10_BLUE:
      timeout = process_CX10();
      break;
    case PROTO_H7:
      timeout = process_H7();
      break;
    case PROTO_BAYANG:
    case PROTO_BAYANG_SILVERWARE:
      timeout = process_Bayang();
      break;
    case PROTO_SYMAX5C1:
    case PROTO_SYMAXOLD:
      timeout = process_SymaX();
      break;
    case PROTO_H8_3D:
      timeout = process_H8_3D();
      break;
    case PROTO_MJX:
    case PROTO_E010:
      timeout = process_MJX();
      break;
    case PROTO_HISKY:
      timeout = process_HiSky();
      break;
    case PROTO_KN:
      timeout = process_KN();
      break;
    case PROTO_YD717:
      timeout = process_YD717();
      break;
    case PROTO_FQ777124:
      timeout = process_FQ777124();
      break;
  }

  update_ppm();
  while (micros() < timeout) {
    delayMicroseconds(100);
    /*
      if(telemetry_data.updated) {
        frskyUpdate();
      }
    */
  }
  //telemetry_data.updated = 0;
}

void set_txid(bool renew)
{
  uint8_t i;
  for (i = 0; i < 4; i++)
    transmitterID[i] = EEPROM.read(ee_TXID0 + i);
  if (renew || (transmitterID[0] == 0xFF && transmitterID[1] == 0x0FF)) {
    for (i = 0; i < 4; i++) {
      transmitterID[i] = random() & 0xFF;
      EEPROM.update(ee_TXID0 + i, transmitterID[i]);
    }
  }
}

void selectProtocol()
{
  update_ppm();

  // startup stick commands (protocol selection / renew transmitter ID)

  if (ppm[RUDDER] < PPM_MIN_COMMAND && ppm[AILERON] < PPM_MIN_COMMAND) // rudder left + aileron left
    current_protocol = PROTO_BAYANG_SILVERWARE; // Bayang protocol for Silverware with frsky telemetry

  else if (ppm[RUDDER] < PPM_MIN_COMMAND) { // Rudder left
    set_txid(true);                      // Renew Transmitter ID
    current_protocol = constrain(EEPROM.read(ee_PROTOCOL_ID), 0, PROTO_END - 1);
  } else if (ppm[RUDDER] > PPM_MAX_COMMAND && ppm[AILERON] > PPM_MAX_COMMAND && ppm[ELEVATOR] < PPM_MIN_COMMAND)
    // Rudder right + Aileron right + Elevator down
    current_protocol = PROTO_E010; // EAchine E010, NiHui NH-010, JJRC H36 mini

  // Rudder right + Aileron right + Elevator up
  else if (ppm[RUDDER] > PPM_MAX_COMMAND && ppm[AILERON] > PPM_MAX_COMMAND && ppm[ELEVATOR] > PPM_MAX_COMMAND)
    current_protocol = PROTO_FQ777124; // FQ-777-124

  // Rudder right + Aileron left + Elevator up
  else if (ppm[RUDDER] > PPM_MAX_COMMAND && ppm[AILERON] < PPM_MIN_COMMAND && ppm[ELEVATOR] > PPM_MAX_COMMAND)
    current_protocol = PROTO_YD717; // Cheerson CX-10 red (older version)/CX11/CX205/CX30, JXD389/390/391/393, SH6057/6043/6044/6046/6047, FY326Q7, WLToys v252 Pro/v343, XinXun X28/X30/X33/X39/X40

  // Rudder right + Aileron left + Elevator down
  else if (ppm[RUDDER] > PPM_MAX_COMMAND && ppm[AILERON] < PPM_MIN_COMMAND && ppm[ELEVATOR] < PPM_MIN_COMMAND)
    current_protocol = PROTO_KN; // KN (WLToys variant) V930/931/939/966/977/988

  // Rudder right + Elevator down
  else if (ppm[RUDDER] > PPM_MAX_COMMAND && ppm[ELEVATOR] < PPM_MIN_COMMAND)
    current_protocol = PROTO_HISKY; // HiSky RXs, HFP80, HCP80/100, FBL70/80/90/100, FF120, HMX120, WLToys v933/944/955 ...

  // Rudder right + Elevator up
  else if (ppm[RUDDER] > PPM_MAX_COMMAND && ppm[ELEVATOR] > PPM_MAX_COMMAND)
    current_protocol = PROTO_SYMAXOLD; // Syma X5C, X2 ...

  // Rudder right + Aileron right
  else if (ppm[RUDDER] > PPM_MAX_COMMAND && ppm[AILERON] > PPM_MAX_COMMAND)
    current_protocol = PROTO_MJX; // MJX X600, other sub protocols can be set in code

  // Rudder right + Aileron left
  else if (ppm[RUDDER] > PPM_MAX_COMMAND && ppm[AILERON] < PPM_MIN_COMMAND)
    current_protocol = PROTO_H8_3D; // H8 mini 3D, H20 ...

  // Elevator down + Aileron right
  else if (ppm[ELEVATOR] < PPM_MIN_COMMAND && ppm[AILERON] > PPM_MAX_COMMAND)
    current_protocol = PROTO_YD829; // YD-829, YD-829C, YD-822 ...

  // Elevator down + Aileron left
  else if (ppm[ELEVATOR] < PPM_MIN_COMMAND && ppm[AILERON] < PPM_MIN_COMMAND)
    current_protocol = PROTO_SYMAX5C1; // Syma X5C-1, X11, X11C, X12

  // Elevator up + Aileron right
  else if (ppm[ELEVATOR] > PPM_MAX_COMMAND && ppm[AILERON] > PPM_MAX_COMMAND)
    current_protocol = PROTO_BAYANG;    // EAchine H8(C) mini, BayangToys X6/X7/X9, JJRC JJ850 ...

  // Elevator up + Aileron left
  else if (ppm[ELEVATOR] > PPM_MAX_COMMAND && ppm[AILERON] < PPM_MIN_COMMAND)
    current_protocol = PROTO_H7;        // EAchine H7, MT99xx

  // Elevator up
  else if (ppm[ELEVATOR] > PPM_MAX_COMMAND)
    current_protocol = PROTO_V2X2;       // WLToys V202/252/272, JXD 385/388, JJRC H6C ...

  // Elevator down
  else if (ppm[ELEVATOR] < PPM_MIN_COMMAND)
    current_protocol = PROTO_CG023;      // EAchine CG023/CG031/3D X4, (todo :ATTOP YD-836/YD-836C) ...

  // Aileron right
  else if (ppm[AILERON] > PPM_MAX_COMMAND)
    current_protocol = PROTO_CX10_BLUE;  // Cheerson CX10(blue pcb, newer red pcb)/CX10-A/CX11/CX12 ...

  // Aileron left
  else if (ppm[AILERON] < PPM_MIN_COMMAND)
    current_protocol = PROTO_CX10_GREEN;  // Cheerson CX10(green pcb)...

  // read last used protocol from eeprom
  else
    current_protocol = constrain(EEPROM.read(ee_PROTOCOL_ID), 0, PROTO_END - 1);

  // update eeprom
  EEPROM.update(ee_PROTOCOL_ID, current_protocol);
  // wait for safe throttle
  while (ppm[THROTTLE] > PPM_SAFE_THROTTLE) {
    delay(100);
    update_ppm();
  }
}

void init_protocol()
{
  switch (current_protocol) {
    case PROTO_CG023:
    case PROTO_YD829:
      CG023_init();
      CG023_bind();
      break;
    case PROTO_V2X2:
      V2x2_init();
      V2x2_bind();
      break;
    case PROTO_CX10_GREEN:
    case PROTO_CX10_BLUE:
      CX10_init();
      CX10_bind();
      break;
    case PROTO_H7:
      H7_init();
      H7_bind();
      break;
    case PROTO_BAYANG:
    case PROTO_BAYANG_SILVERWARE:
      Bayang_init();
      Bayang_bind();
      break;
    case PROTO_SYMAX5C1:
    case PROTO_SYMAXOLD:
      Symax_init();
      break;
    case PROTO_H8_3D:
      H8_3D_init();
      H8_3D_bind();
      break;
    case PROTO_MJX:
    case PROTO_E010:
      MJX_init();
      MJX_bind();
      break;
    case PROTO_HISKY:
      HiSky_init();
      break;
    case PROTO_KN:
      kn_start_tx(true); // autobind
      break;
    case PROTO_YD717:
      YD717_init();
      break;
    case PROTO_FQ777124:
      FQ777124_init();
      FQ777124_bind();
      break;
  }
}

#include "ppm_exec.h"
