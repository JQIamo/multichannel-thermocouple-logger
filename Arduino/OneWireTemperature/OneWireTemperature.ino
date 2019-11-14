#include <C:\Users\Justin\Documents\School\Porto Research\Code\Temperature_Sensor_3Channel\OneWire_3Channel.h>

// OneWire MAX31850 or DS18B20 example!
// Connect either/both kinds of sensors, it will automatically detect
// the type and decode properly

#define TYPE_DS18S20 0
#define TYPE_DS18B20 1
#define TYPE_DS18S22 2
#define TYPE_MAX31850 3

void PrintTemps(OneWire ds);
void init_shift_reg(int dso, int sckshift);
void step_shift_reg(int dso, int sckshfit);


//small board options
//OneWire  ds(2, 0, 1);
//OneWire ds(5, 4, 3);
//large board options
OneWire ds1(20,21,24);
OneWire ds2(17, 19, 25);

void setup(void) {
  Serial.begin(9600);
  init_shift_reg(18, 14);
}

void loop(void) {
  Serial.println("Channel 1");
  PrintTemps(ds1);
  //delay(250);
  Serial.println("Channel 2");
  PrintTemps(ds2);
  //delay(250);

  step_shift_reg(int dso, int sckshfit)

}

void PrintTemps(OneWire ds){
  byte i;
  byte present = 0;
  byte temptype;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;
  
  Serial.println(F("******************************************"));
  while ( ds.search(addr)) {
    //delay(250);
    Serial.print("ROM =");
    for( i = 0; i < 8; i++) {
      Serial.write(' ');
      Serial.print(addr[i], HEX);
    }
  
    if (OneWire::crc8(addr, 7) != addr[7]) {
        Serial.println("CRC is not valid!");
        return;
    }
    Serial.println();
   
    // the first ROM byte indicates which chip
    switch (addr[0]) {
      case 0x10:
        Serial.println("  Chip = DS18S20");  // or old DS1820
        temptype = TYPE_DS18S20;
        break;
      case 0x28:
        Serial.println("  Chip = DS18B20");
        temptype = TYPE_DS18B20;
        break;
      case 0x22:
        Serial.println("  Chip = DS1822");
        temptype = TYPE_DS18S22;
        break;
        // ADDED SUPPORT FOR MAX31850!
      case 0x3B:
        Serial.println("  Chip = MAX31850");
        temptype = TYPE_MAX31850;
        break;
      default:
        Serial.println("Device is not a DS18x20 family device.");
        return;
    } 
  
    ds.reset();
    ds.select(addr);
    ds.write(0x44, 1);        // start conversion, with parasite power on at the end
    
    delay(100);     // maybe 750ms is enough, maybe not
    // we might do a ds.depower() here, but the reset will take care of it.
    
    present = ds.reset();
    ds.select(addr);    
    ds.write(0xBE);         // Read Scratchpad
  
    Serial.print("  Data = ");
    Serial.print(present, HEX);
    Serial.print(" ");
    for ( i = 0; i < 9; i++) {           // we need 9 bytes
      data[i] = ds.read();
      Serial.print(data[i], HEX);
      Serial.print(" ");
    }
    Serial.print(" CRC=");
    Serial.print(OneWire::crc8(data, 8), HEX);
    Serial.println();
  
    Serial.print("  Address = 0x"); Serial.println(data[4] & 0xF, HEX);
  
    // Convert the data to actual temperature
    // because the result is a 16 bit signed integer, it should
    // be stored to an "int16_t" type, which is always 16 bits
    // even when compiled on a 32 bit processor.
    int16_t raw = (data[1] << 8) | data[0];
    if (temptype == TYPE_DS18S20) {
      raw = raw << 3; // 9 bit resolution default
      if (data[7] == 0x10) {
        // "count remain" gives full 12 bit resolution
        raw = (raw & 0xFFF0) + 12 - data[6];
      }
    } else if (temptype == TYPE_MAX31850) {
      //Serial.println(raw, HEX);
      if (raw & 0x01) {
        Serial.println("**FAULT!**");
        return;
      }
    } else {
      byte cfg = (data[4] & 0x60);
      // at lower res, the low bits are undefined, so let's zero them
      if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
      else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
      else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
      //// default is 12 bit resolution, 750 ms conversion time
    }
    celsius = (float)raw / 16.0;
    fahrenheit = celsius * 1.8 + 32.0;
    Serial.print("  Temperature = ");
    Serial.print(celsius);
    Serial.print(" Celsius, ");
    Serial.print(fahrenheit);
    Serial.println(" Fahrenheit");
  };


  Serial.println();
  Serial.println("No more addresses.");
  Serial.println();
  ds.reset_search();
  return;
 
  };


void init_shift_reg(int dso, int sckshift){
  int i;
  pinMode(dso, OUTPUT);
  pinMode(sckshift, OUTPUT);

  digitalWriteFast(dso, HIGH);

  for( i = 0; i < 16; i++){
    digitalWriteFast(sckshift, HIGH);
    digitalWriteFast(sckshift, LOW);
  };
  
};


void step_shift_reg(int dso, int sckshfit){
  digitalWriteFast(sckshift, HIGH);
  digitalWriteFast(sckshfit(LOW);

  digitalWriteFast(dso, HIGH);
  
};






  
