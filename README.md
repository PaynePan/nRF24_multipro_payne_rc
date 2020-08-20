# nrf24-multipro integrated with payne_rc
nRF24L01 multi-protocol RC transmitter

[see the transmitter](https://www.moz8.com/data/attachment/forum/202008/19/215018ktpsusv6u6uu36k1.jpg)
[see the channel order](http://photo.5imxbbs.com/forum/202008/19/200636p7ok8e3odemeqnnp.jpg)

## Binding Procedure
- Power up quad or receiver.
- While holding the appropriate stick pattern listed below, power up the tranmitter. ( that means protocol selected) (pls confirm the channel not set reversed)
- Then move Throttle stick down ( enter binding) 
- when bind done the radio will beep, watch to see if thequad/receiver is in the right status.

#### Protocol is selected with stick position at startup:

- Rudder left + Aileron left = Bayang protocol without Silverware telemetry for OpenTX
- Rudder right + Aileron right + Elevator down = EAchine E010, NiHui NH-010, JJRC H36 mini  
- Rudder right + Aileron right + Elevator up = FQ-777-124 Pocket Drone  
- Rudder right + Aileron left + Elevator up = CX-10 older red PCB/CX11/CX205/CX30, JXD389/391/393, SH6057/6043/6044/6046/6047, FY326Q7, WLToys v252 Pro/v343, XinXun X28/X30/X33/X39/X40   
- Rudder right + Aileron left + Elevator down = WLToys V930/931/939/966/977/988  
- Rudder right + Elevator down = HiSky RXs, HFP80, HCP80/100, FBL70/80/90/100, FF120, HMX120, WLToys v933/944/955  
- Rudder right + Elevator up = Syma X5C (older model), X2 ...  
- Rudder right + Aileron right = MJX X600  
- Rudder right + Aileron left = EAchine H8 mini 3D, JJRC H20/H22   
- Elevator down + Aileron left = Syma X5C-1/X11/X11C/X12  
- Elevator down + Aileron right = Attop YD-822/YD-829/YD-829C ...  
- Elevator up + Aileron right = EAchine H8(C) mini, BayangToys X6/X7/X9, JJRC JJ850, Floureon H101, BWhoop B03 ...  
- Elevator up + Aileron left = EAchine H7  
- Elevator up = WLToys V202/252/272, JXD 385/388, JJRC H6C, Yizhan Tarantula X6 ...  
- Elevator down = EAchine CG023/CG031/3D X4  
- Aileron left = Cheerson CX-10 green pcb  
- Aileron right = Cheerson CX-10 blue pcb & some newer red pcb, CX-10A, CX-10C, CX11, CX12, Floureon FX10, JJRC DHD D1  
- Rudder left = Renew Transmitter ID

Last used protocol is automatically selected if stick is in neutral position ( throttle must in low postions)

#### Extra features (if available on aircraft):

- Channel 5: led light, 3 pos. rate on CX-10 and FQ777-124, H7, inverted flight on H101  
- Channel 6: flip control  
- Channel 7: still camera  
- Channel 8: video camera, pitch trim (FQ777-124)  

#### Control 4-8 Channel on Radio
- Channel 5: the knob
- Channel 6: holding F1 and F2 both, then presss down 2 btn on the right. do it again (the CH vale will be switch from PWM 1000 and 2000us)
- Channel 7: holding F1 and F2 both, then presss down 3 btn
- Channel 8: holding F1 and F2 both, then presss down 4 btn

#### Trim
- Channel 1 Aileron: use btn 3 or 4
- Channel 2 Elevator: use btn 1 or 2
- Channel 3 Throttle : holding F1 , then press 1 or 2
- Channel 4 Rudder :  holding F1 , then press 3 or 4

#### Channel reverse
- Channel 1 Aileron: holding F1 , then press 1   // high freq beep means reversed
- Channel 2 Elevator: holding F1 , then press 2
- Channel 3 Throttle :  to avoid confusion, not support reverse
- Channel 4 Rudder :  holding F1 , then press 4

#### Channel precent
- Channel 1 Aileron: move Stick of Aileron to 70% position (left or right is the same) , then press both F1 and F2 down. then range is reduce to 70%
- Channel 2 Elevator: just like Channel 1
- Channel 4 Rudder : just like Channel 1 

#### Channel Setting Reset (trim, reverse, precent)
Let Throttle stick up, after that, holding F1 and F2 both, then press 1. a beep will inform that reset is done.

#### hardware
atmegal328p + nrf24 with pa
[schematics](http://photo.5imxbbs.com/forum/202008/20/091810siyx6d2a0nu2ssyz.jpg)

#### to develop
Download from git  and open prj in Arduino IDE
Chose "Board: Arduino Pro or Pro mini (5v 16M)" to compile
upload firmware via ICSP port ( uspasp programmer needed)
Optional£º
upload bootloader via ICSP, then firmware can be uploaded via serial port (an usb-ttl with DTR pin needed)

[ICSP and Serial Port](http://photo.5imxbbs.com/forum/202008/20/091018xdfaujojfzo3833l.jpg)



