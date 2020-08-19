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
     
    // adjust channel
    c = new AnalogControl();  c->setPin(A0);  c->setCh(chs[1]);  controlManager.add(c);
    anaCtlManager.add(c);
    c = new AnalogControl();  c->setPin(A1);  c->setCh(chs[2]);  controlManager.add(c);
    anaCtlManager.add(c);

    c = new AnalogControl();  c->setPin(A6);  c->setCh(chs[4]);  controlManager.add(c);
    anaCtlManager.add(c);
    c = new AnalogControl();  c->setPin(A7);  c->setCh(chs[3]);  controlManager.add(c);

    c = new AnalogControl();  c->setPin(A2);  c->setCh(chs[5]);  controlManager.add(c);
    

    c = new OffsetBtnControl();  c->setPin(Btn2);  c->setCh(chs[2]);  controlManager.add(c); ((OffsetBtnControl *)c)->setDec(true);
    c = new OffsetBtnControl();  c->setPin(Btn1);  c->setCh(chs[2]);  controlManager.add(c);
    c = new OffsetBtnControl();  c->setPin(Btn3);  c->setCh(chs[1]);  controlManager.add(c); ((OffsetBtnControl *)c)->setDec(true);
    c = new OffsetBtnControl();  c->setPin(Btn4);  c->setCh(chs[1]);  controlManager.add(c);

    c = new OffsetBtnControl();  c->setPin(Btn2);  c->setCondStatus(STATUS_F1_DOWN); c->setCh(chs[3]);  controlManager.add(c); ((OffsetBtnControl *)c)->setDec(true);
    c = new OffsetBtnControl();  c->setPin(Btn1);  c->setCondStatus(STATUS_F1_DOWN); c->setCh(chs[3]);  controlManager.add(c);
    c = new OffsetBtnControl();  c->setPin(Btn3);  c->setCondStatus(STATUS_F1_DOWN); c->setCh(chs[4]);  controlManager.add(c); ((OffsetBtnControl *)c)->setDec(true);
    c = new OffsetBtnControl();  c->setPin(Btn4);  c->setCondStatus(STATUS_F1_DOWN); c->setCh(chs[4]);  controlManager.add(c);

    c = new ReverseBtnControl();  c->setPin(Btn1); c->setCondStatus(STATUS_F2_DOWN); c->setCh(chs[1]);  controlManager.add(c);
    c = new ReverseBtnControl();  c->setPin(Btn2); c->setCondStatus(STATUS_F2_DOWN); c->setCh(chs[2]);  controlManager.add(c);
    //c = new ReverseBtnControl();  c->setPin(Btn3); c->setCondStatus(STATUS_F2_DOWN); c->setCh(chs[3]);  controlManager.add(c);
    c = new ReverseBtnControl();  c->setPin(Btn4); c->setCondStatus(STATUS_F2_DOWN); c->setCh(chs[4]);  controlManager.add(c);

    c = new MinMaxBtnControl();  c->setPin(Btn2);  c->setCondStatus(STATUS_F1_F2_DOWN); c->setCh(chs[6]); controlManager.add(c);
    c = new MinMaxBtnControl();  c->setPin(Btn3);  c->setCondStatus(STATUS_F1_F2_DOWN); c->setCh(chs[7]); controlManager.add(c);
    c = new MinMaxBtnControl();  c->setPin(Btn4);  c->setCondStatus(STATUS_F1_F2_DOWN); c->setCh(chs[8]); controlManager.add(c);
  
