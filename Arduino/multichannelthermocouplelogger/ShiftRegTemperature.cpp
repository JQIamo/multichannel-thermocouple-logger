#include "ShiftRegTemperature.h"
#include <SPI.h>
#include "digitalWriteFast.h"

void init_shift_reg(int dso, int sckshift){
  int i;
  pinMode(dso, OUTPUT);
  pinMode(sckshift, OUTPUT);

  digitalWriteFast(dso, HIGH);

  for( i = 0; i < 16; i++){
    digitalWriteFast(sckshift, HIGH);
    digitalWriteFast(sckshift, LOW);
  };

  digitalWriteFast(dso, LOW);
  
};


void step_shift_reg(int dso, int sckshift){
  digitalWriteFast(sckshift, HIGH);
  digitalWriteFast(sckshift, LOW);

  digitalWriteFast(dso, HIGH);
  
};


void shift_reg_temp(int dso, int sckshift, int maxso, int maxsck, int result[N_CHANNELS]){

  for (int i = 0; i < N_CHANNELS; i++){
    uint32_t tempBit = 0;
    step_shift_reg(dso, sckshift);
    if ((i%8)==0 && i!=0) {step_shift_reg(dso, sckshift);}
    delay(1);
      for (int j = 0; j < 32; j++){
        for (int k=0; k<100; k++){
          __asm__("nop\n\t");
        }
        tempBit <<= 1;
        if (digitalReadFast(maxso)) {
          tempBit |= 1;
        }
        digitalWriteFast(maxsck, HIGH);
        digitalWriteFast(maxsck, LOW);
      }
    result[i] = tempBit;
  }
  step_shift_reg(dso, sckshift);

}
/*
//Use this to read the internal temperature of the Max
int internalTemp(int encoding, double* result) {
  uint32_t v;
  v = encoding;
  int error;
  error = (int)((int)v & 0x0000000F);


  // ignore bottom 4 bits - they're just thermocouple data
  v >>= 4;

  // pull the bottom 11 bits off
  float internal = v & 0x7FF;
  // check sign bit!
  if (v & 0x800) {
    // Convert to negative value by extending sign and casting to signed type.
    int16_t tmp = 0xF800 | (v & 0x7FF);
    internal = tmp;
  }
  internal *= 0.0625; // LSB = 0.0625 degrees
  //Serial.print("\tInternal Temp: "); Serial.println(internal);
  *result = internal;
  return error;
}
*/

double readCelsius(int encoding, double* result) {
  int32_t v;
  v = encoding;
  int error;
  error = (int)((int)v & 0x0000000F);

  //Serial.print("0x"); Serial.println(v, HEX);


  if (v & 0x80000000) {
    // Negative value, drop the lower 18 bits and explicitly extend sign bits.
    v = 0xFFFFC000 | ((v >> 18) & 0x00003FFFF);
  }
  else {
    // Positive value, just drop the lower 18 bits.
    v >>= 18;
  }
  //Serial.println(v, HEX);
  
  double centigrade = v;

  // LSB = 0.25 degrees C
  centigrade *= 0.25;
  *result = centigrade;
  return error;
}

double readFahrenheit(double cels) {
  float f = cels;
  f *= 9.0;
  f /= 5.0;
  f += 32;
  return f;
}
