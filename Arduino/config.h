// Configuration file for 10/20-channel thermocouple data logger

// Local network settings
#define ENABLELOCAL // Use "true" to disable the local web server
#define MAC { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }
#define LOCALIP { 192, 168, 0, 6 }
#define PORT 80
#define MAXCONNECTIONS 50 // Maximum number of connection attempts before ethernet client resets (it usually takes around 10-20 connections attempts to successfully connect)

// InfluxDB settings
#define ENABLEINFLUX 0 // Set as 1 to enable influxDB and 0 to disable influxDB 
#define DATABASEIP { 129, 2, 40, 155 }
#define USERNAME ""
#define PASSWORD ""
#define DATABASENAME ""
#define CELSIUS 1 // Configures units for data sent to influxDB, use 1 for Celsius and 0 for Farenheit

// Custom channel names
#define CHANNEL1 "" // Channel names have a 16 character limit; the channel name will display as "Channel X" if left blank
#define CHANNEL2 "" 
#define CHANNEL3 "" 
#define CHANNEL4 "" 
#define CHANNEL5 "" 
#define CHANNEL6 "" 
#define CHANNEL7 "" 
#define CHANNEL8 "" 
#define CHANNEL9 "" 
#define CHANNEL10 "" 
#define CHANNEL11 "" 
#define CHANNEL12 "" 
#define CHANNEL13 "" 
#define CHANNEL14 "" 
#define CHANNEL15 "" 
#define CHANNEL16 ""
#define CHANNEL17 ""
#define CHANNEL18 ""
#define CHANNEL19 ""
#define CHANNEL20 "" 


