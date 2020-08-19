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
#include "Settings.h"
#include "payne_rc.h"

#include <arduino.h>

Settings::Settings() {
  updateTime = saveTime = millis();
}

static uint8_t calculate_sum(uint8_t *cb , uint8_t siz) {
    uint8_t sum=0x55;  // checksum init
    while(--siz) sum += *cb++;  // calculate checksum (without checksum byte)
    return sum;
}

void Settings::loadDefaults() {
    for(int i=1;i<=CHAN_SIZE;i++) {
      data.ch[i].offset = 0;
      data.ch[i].reversed = false;
      data.ch[i].percent = 100;
      data.ch[i].centerAnalog = ANA_CENTER;      
  }
   store();   
}

// EEPROM locationss
/* the first 5 byte , store below data by multipro
enum{
    ee_PROTOCOL_ID = 0,
    ee_TXID0,
    ee_TXID1,
    ee_TXID2,
    ee_TXID3
};
*/

bool Settings::load() {
  eeprom_read_block((void*)&data, 5, sizeof(data));
  if(calculate_sum((uint8_t*)&data, sizeof(data)) != data.checksum) {
    loadDefaults();                 // force load defaults
      return false;
  }
 return true;
}

void Settings::store() {
  data.checksum = calculate_sum((uint8_t*)&data, sizeof(data));
  eeprom_write_block((void*)&data, 5, sizeof(data));
}

boolean Settings::saveIfUpdate() {
  if ( updateTime != saveTime  &&  (millis() - updateTime) > 2000) {
      store();
      saveTime = updateTime; 
      return true;
  }
  return false;
}
