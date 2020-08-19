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
 #include "ControlManager.h"

ControlManager::ControlManager() {
    char ctl_count = 0;
}

void ControlManager::add(Control *ctl){
    ctls[ctl_count++] = ctl;
}

void ControlManager::handle(uint8_t status){
  for( int i=0; i<ctl_count; i++) {
    if (ctls[i]->getCondStatus() == status)
      ctls[i]->handle();
  }
}

AnalogControlManager::AnalogControlManager() {
  char ctl_count = 0;
}

void AnalogControlManager::add(AnalogControl *ctl) {
  if ( ctl_count < ANA_CTL_MAX) 
    ctls[ctl_count++] = ctl;
}

void AnalogControlManager::resetCenterAnalog() {
  for ( int i = 0; i < ctl_count; i++)
    ctls[i]->resetCenterAnalog();
}

boolean AnalogControlManager::resetPercent() {
  boolean r = false;
  for ( int i = 0; i < ctl_count; i++) {
     if ( ctls[i]->resetPercent()) 
       r = true;
  }
  return r;
}
