// Configuration file for 10/20-channel thermocouple data logger

// Serial display settings
#define ENABLESERIAL 0 // Set as 1 to enable serial display of temperatures
#define ENABLEETHERNET 1 // Set as 0 if the Ethernet module has not been installed
#define BAUDRATE 9600

// Local network settings
#define ENABLELOCAL 1 // Set as 1 to enable local server and 0 to disable
#define MAC { 0xBE, 0xAC, 0xEF, 0x00, 0x00, 0x01 }
#define LOCALIP { 128, 8, 195, 50 }
#define PORT 80
#define MAXCONNECTIONS 50 // Maximum number of connection attempts before ethernet client resets (it usually takes around 10-20 connections attempts to successfully connect)

// InfluxDB settings
#define ENABLEINFLUX 0 // Set as 1 to enable influxDB and 0 to disable influxDB, remember to disable the local server before enabling InfluxDB 
#define DATABASEIP { 129, 2, 40, 155 }
#define USERNAME ""
#define PASSWORD ""
#define DATABASENAME ""
#define INFLUXTIMEINTERVAL 20000 // Configures the time interval in ms between when the data is sent to the influxDB server
#define CELSIUS 1 // Configures units for data sent to influxDB, use 1 for Celsius and 0 for Farenheit
#define ENABLEINFLUXWARNING 1 // Set as 1 to enable the LCD error message that displays if the device fails to connect to the influxDB server and 0 to disable
// if the LCD error message is enabled, influxDB can be disabled on the device by holding down the channel selection dial 

// Custom channel names
#define ENABLECUSTOMNAMES 1 // Set at 1 to enable custom channel names and 0 to disable
#define CHANNEL1 "Channel_1" // Channel names have a 16 character limit; there should be no spaces in the channel name
#define CHANNEL2 "Channel_2"
#define CHANNEL3 "Channel_3"
#define CHANNEL4 "Channel_4"
#define CHANNEL5 "Channel_5"
#define CHANNEL6 "Channel_6"
#define CHANNEL7 "Channel_7"
#define CHANNEL8 "Channel_8"
#define CHANNEL9 "Channel_9"
#define CHANNEL10 "Channel_10"
#define CHANNEL11 "Channel_11"
#define CHANNEL12 "Channel_12"
#define CHANNEL13 "Channel_13" 
#define CHANNEL14 "Channel_14"
#define CHANNEL15 "Channel_15" 
#define CHANNEL16 "Channel_16"
#define CHANNEL17 "Channel_17"
#define CHANNEL18 "Channel_18"
#define CHANNEL19 "Channel_19"
#define CHANNEL20 "Channel_20"


//Shift_register_configuration_pins
#define DSO 18
#define SCKSHIFT 14

// Select Teensy pins for each channel
#define PIN1 14
#define PIN2 15
#define PIN3 16
#define PIN4 17
#define PIN5 18
#define PIN6 19
#define PIN7 20
#define PIN8 21
#define PIN9 22
#define PIN10 23
#define PIN11 27
#define PIN12 28
#define PIN13 29
#define PIN14 30
#define PIN15 26
#define PIN16 31
#define PIN17 25
#define PIN18 32
#define PIN19 33
#define PIN20 24
