#include "ShiftRegTemperature.h"

void setup() {
  
  Serial.begin(9600);
  pinMode(maxSO,INPUT);
  pinMode(maxSCK, OUTPUT);
  init_shift_reg(DSO, SCKSHIFT);

}

void loop() {  
  digitalWriteFast(DSO, LOW);
  step_shift_reg(DSO, SCKSHIFT);

  int tempData[N_CHANNELS];
  shift_reg_temp(DSO, SCKSHIFT, maxSO, maxSCK, tempData);

  for (int i = 0; i < N_CHANNELS; i++){
    double result;
    if (readCelsius(tempData[i], &result) == 0){
      Serial.print(result);
      Serial.print(" Celsius, ");
      Serial.print(readFahrenheit(result));
      Serial.println(" Fahrenheit");
    }
  }

}
