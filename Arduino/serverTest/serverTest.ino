#include <SPI.h>
#include <Ethernet.h>
#include "influxDB.h"
#include "config.h"
#include "SimpleTimer.h"

// Configure local network settings
byte mac[] = MAC; // Assigns MAC address
IPAddress ip(LOCALIP); // Assigns local IP address
EthernetServer server(PORT); // Creates ethernet server object (default is port 80)
boolean enableLocal = ENABLELOCAL;



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

// Displays an error message when influxDB is not working
boolean errorActive = false;
int errorNumber;
String errorMessage[] = {
  "InfluxDB error",
};


// Tracks and sets a maximum number of connection attempts before the ethernet client resets
int localConnectionAttempt;
int influxConnectionAttempt;
int maxConnections = MAXCONNECTIONS;

// Creates timer object
SimpleTimer timer;

void setup() {
  // Initializes server
  server.begin();
  Serial.print("Local server now running at ");
  Serial.println(Ethernet.localIP());
  //
}

void loop() {

  // Display temperatures using ethernet
  if ( enableLocal == 1) {
    ethernetTemp();
  }

  if (enableInfluxDB == 1) {
    timer.run();
  }

}

void ethernetTemp() {
  // Listens for incoming clients
  int cTemp;
  int fTemp;
  int currentChanNumber;
  int i = 0;

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
          }
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
  int i = 0;
  int cTemp;
  int fTemp;
  int currentChanNumber;
  
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
