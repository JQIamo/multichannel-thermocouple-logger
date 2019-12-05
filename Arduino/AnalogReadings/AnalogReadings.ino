#include "AnalogReadings.h"

void setup() {
  delay(2000);
  pinMode(analogCLK, OUTPUT);
  pinMode(analogSDI,OUTPUT);
  pinMode(analogCS, OUTPUT);
  pinMode(analogSDO, INPUT);
  int com, result;
  

  digitalWriteFast(analogCLK, LOW);
  digitalWriteFast(analogCS, HIGH);
  
  
  com = adc_rst;
  exit_standby(analogCS, analogCLK, analogSDI, com);
  
  com = auto_chan;
  com <<= 16;
  result=write32(analogCS, analogCLK, analogSDO, analogSDI, com);

  
}

void loop() {
  int result, com;
  float volt, data[8];
  com = no_op;
  com <<= 16;

  
  for (int i=0; i < 8; i++){
    
    result=write32(analogCS, analogCLK, analogSDO, analogSDI, com);

    volt = 0.0003125 * (float(result) - 32768);
    data[i] = volt;
    }
  
  for (int i=0; i < 8; i++){
    Serial.println(i);
    Serial.println(data[i]);
  }

 
}
