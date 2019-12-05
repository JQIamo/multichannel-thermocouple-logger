#include "AnalogReadings.h"
#include <SPI.h>

int write32(int cs, int sck, int sdo, int sdi, int data){
  int32_t tempBit = 0;
  digitalWriteFast(cs, LOW);
  delayMicroseconds(1);
  
  for (int i = 0; i<32; i++){
  tempBit <<= 1;
    if (digitalReadFast(sdo)) {
      tempBit |= 1;
    }
     if (data & 0x80000000){
      digitalWriteFast(sdi, HIGH);
     } else {
      digitalWriteFast(sdi, LOW);
     }
     
     data <<= 1;
     digitalWriteFast(sck, HIGH);
     digitalWriteFast(sck, HIGH);
     
     digitalWriteFast(sck, LOW);
     digitalWriteFast(sck, LOW);
     
 }

 digitalWriteFast(cs, HIGH);
 
 return tempBit;
}

void exit_standby(int cs, int sck, int sdi, int data){
  digitalWriteFast(cs, LOW);
  delayMicroseconds(2);
  
  for (int i = 0; i<16; i++){
     if (data & 0x80000000){
      digitalWriteFast(sdi, HIGH);
     } else {
      digitalWriteFast(sdi, LOW);
     }
     
     data <<= 1;
     digitalWriteFast(sck, HIGH);
     delayMicroseconds(1);
     digitalWriteFast(sck, LOW);
     delayMicroseconds(1);
 }

 digitalWriteFast(cs, HIGH);
 delayMicroseconds(20);
 
}
