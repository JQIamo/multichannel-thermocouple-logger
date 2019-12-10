#include <SPI.h>
#include "Adafruit_MAX31855.h"
#include "LCDStream.h"
#include "LCD_ST7032.h"
#include "LCD.h"
#include "MenuEncoder.h"
#include <Ethernet.h>
#include "InfluxDB.h"
#include "config.h"
#include "SimpleTimer.h"
#include "OneWire_3Channel.h"
#include "ShiftRegTemperature.h"
#include "AnalogReadings.h"
//#include "digitalWriteFast.h"

//set up OneWire readings
#define TYPE_DS18S20 0
#define TYPE_DS18B20 1
#define TYPE_DS18S22 2
#define TYPE_MAX31850 3

void PrintTemps(OneWire ds);

OneWire ds1(20, 21, 24);
OneWire ds2(17, 19, 25);


// Configure local network settings
byte mac[] = MAC; // Assigns MAC address
IPAddress ip(LOCALIP); // Assigns local IP address
EthernetServer server(PORT); // Creates ethernet server object (default is port 80)
boolean enableLocal = ENABLELOCAL;

int LCD_RST = 6;
int LCD_RS = 7;
int LCD_CS = 5;

// Creates LCD object and assign pin numbers
LCD_ST7032 lcd(LCD_RST, LCD_RS, LCD_CS);

int maxCS;

// Channel number selected by the rotary encoder
int encNumber;

// Configures InfluxDB settings
IPAddress databaseIP(DATABASEIP);
char dbName[] = DATABASENAME;
char dbUser[] = USERNAME;
char dbPass[] = PASSWORD;
char userAgent[] = "test";
int influxTimeInterval = INFLUXTIMEINTERVAL;
boolean enableCustomNames = ENABLECUSTOMNAMES;
boolean enableInfluxDB = ENABLEINFLUX; // Enables connections to influxDB server (warning: enabling influxDB without properly configuring influxDB settings will cause the data logger to be unresponsive)
boolean enableWarning = ENABLEINFLUXWARNING;

// Configure serial display settings
int baudrate = BAUDRATE;
boolean enableSerial = ENABLESERIAL;
boolean enableEthernet = ENABLEETHERNET;
// Configure general settings
boolean dbCelsius = CELSIUS;

// Displays an error message when influxDB is not working
boolean errorActive = false;
int errorNumber;
String errorMessage[] = {
  "InfluxDB error",
};

// Creates encoder object and assigns pin number
EncoderStream enc(LCDENC);

// Configures channel names and Teensy pin numbers
int chanNumber[20] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
int chanPin[20] = {PIN1, PIN2, PIN3, PIN4, PIN5, PIN6, PIN7, PIN8, PIN9, PIN10, PIN11, PIN12, PIN13, PIN14, PIN15, PIN16, PIN17, PIN18, PIN19, PIN20};
String chanName[20] = {CHANNEL1, CHANNEL2, CHANNEL3, CHANNEL4, CHANNEL5, CHANNEL6, CHANNEL7, CHANNEL8, CHANNEL9, CHANNEL10, CHANNEL11, CHANNEL12, CHANNEL13, CHANNEL14, CHANNEL15, CHANNEL16, CHANNEL17, CHANNEL18, CHANNEL19, CHANNEL20 };

// Tracks and sets a maximum number of connection attempts before the ethernet client resets
int localConnectionAttempt;
int influxConnectionAttempt;
int maxConnections = MAXCONNECTIONS;

// Creates timer object
SimpleTimer timer;

void setup() {
  Serial.begin(baudrate);
  //Initializes SPI
  SPI.begin();

  // The MAX31855 needs time to stabilize
  delay(500);

  // Initializes LCD
  lcd.begin();
  lcd.clear();
  lcd.cursor();
  lcd.print("Starting...");
  delay(30000);
  lcd.clear();

  // Initializes encoder
  enc.begin();
  encNumber = 1;

  if (enableEthernet == 1) {
    // Initializes Ethernet connection
    Ethernet.begin(mac);

    // Initializes server
    server.begin();
    Serial.print("Local server now running at ");
    Serial.println(Ethernet.localIP());
  }
  //
  timer.setInterval(influxTimeInterval, influxDBTemp);

  //initialize shift register
  pinMode(maxSO,INPUT);
  pinMode(maxSCK, OUTPUT);
  init_shift_reg(DSO, SCKSHIFT); // initialize shift register
  
  //initialize ADC
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

  // Displays temperatures on serial port monitor from pin 14 to pin 23
  if (enableSerial == 1) {
    serialTemp();
  }

  rotaryEncoder();

  // Displays temperatures on LCD
  LCDTemp();

  // Display temperatures using ethernet
  if ( enableLocal == 1) {
    ethernetTemp();
  }

  // Resets local ethernet client after a predefined number of failed connection attempts
  if (localConnectionAttempt > maxConnections) {
    resetClient();
  }

  if (enableInfluxDB == 1) {
    timer.run();
  }

}

void serialTemp() {};
/*
  int cTemp;
  int fTemp;
  int currentChanNumber;
  int i = 0;

  while (i < 20 ) {
    // Select Teensy pin number
    maxCS = chanPin[i];

    // Create thermocouple-to-digital converter object and assign pin numbers
    Adafruit_MAX31855 kTC(maxSCK, maxCS, maxSO);

    cTemp = kTC.readCelsius();
    fTemp = kTC.readFarenheit();

    currentChanNumber = chanNumber[i];

    if (cTemp != 0 && fTemp != 0) {
      if ( enableCustomNames == 1 ) {
        Serial.print(chanName[i]);
      } else {
        Serial.print("Channel ");
        Serial.print(currentChanNumber);
      }
      Serial.print(" C =");
      Serial.println(cTemp);

      if ( enableCustomNames == 1 ) {
        Serial.print(chanName[i]);
      } else {
        Serial.print("Channel ");
        Serial.print(currentChanNumber);
      }
      Serial.print(" F =");
      Serial.println(fTemp);
    }
    i++;

  }
}
*/

void LCDTemp() {};
/*
  int cTemp;
  int fTemp;
  int i;

  if (errorActive == false || enableWarning == 0) {
    // Since the initial element of the array is numbered 0
    i = encNumber - 1;

    maxCS = chanPin[i];

    lcd.setCursor(0, 0);

    Adafruit_MAX31855 kTC(maxSCK, maxCS, maxSO);

    cTemp = kTC.readCelsius();
    fTemp = kTC.readFarenheit();

    if ( enableCustomNames == 1 ) {
      lcd.print(chanName[i]);

      // Clears the remainder of the first line on the LCD
      int chanNameLength = chanName[i].length();
      int emptySpaceLength = 16 - chanNameLength;
      for (int j = 0; j <= emptySpaceLength; j++) {
        lcd.print(" ");
      }
    } else {
      lcd.print("Channel ");
      lcd.print(encNumber);
      lcd.print("   ");
    }
    lcd.setCursor(0, 1);
    if (cTemp != 0 && fTemp != 0) {
      lcd.print(cTemp);
      lcd.print((char)223);
      lcd.print("C ");
      lcd.print(fTemp);
      lcd.print((char)223);
      lcd.print("F   ");
    } else {
      lcd.print("No data     ");
    }
    delay(100);
  } else {
    // Displays an error message when influxDB is not working
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(errorMessage[errorNumber]);
    lcd.setCursor(0, 1);
    delay(3000);
    lcd.setCursor(0, 0);
    lcd.print("Push to disable");
    lcd.setCursor(0, 1);
    lcd.print("Turn to retry");
    delay(4000);
  }

}
*/
void ethernetTemp() {
  // Listens for incoming clients
  int cTemp;
  int fTemp;
  int currentChanNumber;
  int i = 0;
  int tempData[N_CHANNELS];

  EthernetClient client = server.available();

  //
  if (client) {
    Serial.println("New client established");

    // Resets connection attempts to zero upon succesful connection
    localConnectionAttempt = 0;

    // HTTP request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // A reply is sent after if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {

          // HTTP header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");

          // Closes connection after a sucessful response
          client.println("Connection: close");

          // Refreshes page
          client.println("Refresh: 5");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");

          //Display data

          //read oneWire temps
          client.println("OneWire Data<br>");
          client.println("Channel 1<br>");
          PrintTemps(ds1);
          client.println("Channel 2<br>");
          PrintTemps(ds2);

          //read shift reg temps
          client.println("Shift Register Data<br>");
          
          noInterrupts();
          digitalWriteFast(DSO, LOW);
          step_shift_reg(DSO, SCKSHIFT);
          
          shift_reg_temp(DSO, SCKSHIFT, maxSO, maxSCK, tempData);
          interrupts();

          for (int i = 0; i < N_CHANNELS; i++){
            double result;
            if (readCels(tempData[i], &result) == 0){
              client.print("Channel n. ");
              client.print(i);
              client.print(": ");
              client.print(result);
              client.print(" Celsius, <br>");
              client.print(readFahrenheit(result));
              client.println(" Fahrenheit <br>");
            }
          }

          //read analog data
          client.println("Analog Data<br>");
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
            client.println(i);
            client.println(data[i]);
            client.println("<br>");
          }

          //end of displayed data
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(60);
    // close the connection:
    client.stop();
    Serial.println("Client disconnected");
  } else {
    Serial.print("Local server: Client not available (connection attempt: ");
    Serial.print(localConnectionAttempt);
    Serial.println(") Reconnecting...");
    localConnectionAttempt = localConnectionAttempt + 1;
  }
}

void influxDBTemp() {
  String tempData = "";
  int tempTemp[N_CHANNELS];
  int i = 0;
  int cTemp;
  int fTemp;
  int currentChanNumber;

  EthernetClient client;

  noInterrupts();
  digitalWriteFast(DSO, LOW);
  step_shift_reg(DSO, SCKSHIFT);        
  shift_reg_temp(DSO, SCKSHIFT, maxSO, maxSCK, tempTemp);
  interrupts();

  while (i < N_CHANNELS ) {
    
    currentChanNumber = chanNumber[i];
    String currentChanName = chanName[i];
    // Create thermocouple-to-digital converter object and assign pin numbers
    //Adafruit_MAX31855 kTC(maxSCK, maxCS, maxSO);

    double cTemp;
    readCels(tempTemp[i],&cTemp);
    fTemp = readFahrenheit(cTemp);

    if (cTemp != 0 && fTemp != 0) {
      tempData.concat("thermo,channel=");
      // Add channel number to the string as an InfluxDB tag
      String chanNumberString = String (currentChanNumber);
      if ( enableCustomNames == 1 ) {
        tempData.concat(currentChanName);
      } else {
        tempData.concat(chanNumberString);
      }
      tempData.concat(" value=");

      // Convert int to string
      String cTempString = String(cTemp);
      String fTempString = String(fTemp);

      // Add temperature readings to the string
      if (dbCelsius == true) {
        tempData.concat(cTempString);
      } else {
        tempData.concat(fTempString);
      }
      tempData.concat("\n");
    }

    // Resets tempData
    //tempData = "thermo,channel=";

    i++;
  }

  Serial.print("Writing data: ");
  Serial.println(tempData);
  Serial.print(F("Attempting connection to influxDB server at "));
  Serial.println(databaseIP);

  // Send data to influxDB server
  if (client.connect(databaseIP, 8086)) {
    client.print(F("POST /write?db="));
    client.print(dbName);
    client.print(F("&precision=s&u="));
    client.print(dbUser);
    client.print(F("&p="));
    client.print(dbPass);
    client.println(F(" HTTP/1.1"));
    client.print(F("User-Agent: "));
    client.println(userAgent);
    client.println(F("Connection: close"));
    client.print(F("Content-length: "));
    client.println(tempData.length());
    client.println();
    client.print(tempData);

    // Reset tempData
    tempData = "";
    delay(200);

    // Reset number of failed connection attempts
    influxConnectionAttempt = 0;

    if (client.connected()) {
      Serial.println("Stopping connection");
      client.stop();
    }

    //Refreshes DNS
    Ethernet.maintain();
    client.stop();
  } else {
    Serial.print("Influx DB Server: Error: Could not connect to influxDB server. (Connection attempt:");
    Serial.print(influxConnectionAttempt);
    Serial.println(") Reconnecting...");
    influxConnectionAttempt = influxConnectionAttempt + 1;
    errorActive = true;
    errorNumber = 0;
    client.stop();
  }
}

void resetClient () {
  // Initializes Ethernet connection
  Ethernet.begin(mac, ip);

  // Initializes server
  server.begin();
  Serial.print("Local server now running at ");
  Serial.println(Ethernet.localIP());

  // Resets connection attempts to zero
  localConnectionAttempt = 0;
}

void rotaryEncoder() {
  if (enc.available() > 0) {
    char c = enc.read();
    if ( errorActive == false || enableWarning == 0) {
      if (c == '+' && encNumber < 20) {
        encNumber = encNumber + 1;
      } else if (c == '-' && encNumber > 1) {
        encNumber = encNumber - 1;
      } else if (c == '-' && encNumber == 1) {
        encNumber = 20;
      } else if (c == '+' && encNumber == 20) {
        encNumber = 1;
      } else if (c == 'H' || c == 'P') {
        if (encNumber == 1) {
          encNumber = encNumber + 4;
        } else if (encNumber <= 15) {
          encNumber = encNumber + 5;
        } else if (encNumber > 15 || encNumber == 20) {
          encNumber = 1;
        }
      } else {
        encNumber = encNumber;
      }
    } else {
      // Lets user disable InfluxDB when it stops working
      if (c == 'H' || c == 'P' ) {
        errorActive = false;
        enableInfluxDB = false;
        lcd.clear();
      } else if (c == '-' || c == '+') {
        errorActive = false;
        lcd.clear();
      } else {
        lcd.clear();
      }
    }
  }
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
