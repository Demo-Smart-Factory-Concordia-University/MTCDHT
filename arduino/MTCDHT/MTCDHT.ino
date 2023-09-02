// ---------------------------------------------------------------- 
//                                                                  
// MTConnect Adapter for ESP8266
//
// (c) Rolf Wuthrich, 
//     2023 Concordia University
//
// author:  Rolf Wuthrich
// email:   rolf.wuthrich@concordia.ca
//
// This software is copyright under the BSD license
//
// --------------------------------------------------------------- 

// Demonstrates how to setup an MTConnect Adapter which
// reads the temperature and temperature from a DHT11 Sensor
//
// The adapter sends SHDR format to an MTConnect Agent which connected
// to this adapter
//
// The adapter assumes the following configuration 
// in the MTConnect device model:
//
//   <DataItem category="SAMPLE" id="Temp" type="TEMPERATURE" units="CELCIUS"/>
//   <DataItem category="SAMPLE" id="Humi" type="HUMIDITY_RELATIVE" units="PERCENT"/>
//
//
// Required boards / libraries :
//
// - ESP8266 Arduino board
//   https://arduino-esp8266.readthedocs.io/en/3.0.2/
// 
// Available via Sketch > Include Library > Manage Libraries:
//
// - DHT sensor library by Adafruit
//   https://github.com/adafruit/DHT-sensor-library
//   https://learn.adafruit.com/dht


#include <ESP8266WiFi.h>
#include <DHT.h>
#include "secrets.h"


// -----------------------------------------------------
// Configuration for WiFi access
const char *ssid     = SECRET_SSID;     // WIFI ssid
const char *password = SECRET_PASS;     // WIFI password


// -----------------------------------------------------
// Configuration for MTConnect Adapter

// Hostname
String ADAPTER_HOSTNAME = "MTCAdapter";

// Port number
const uint16_t port = 7878;

// PONG (answer to '*PING' request from the MTConnect Agent)
#define PONG "* PONG 60000"


// -----------------------------------------------------
// Configuration for DHT sensor

// Pin for 1-wire connection of DHT11
// Ref for pinout: 
// https://randomnerdtutorials.com/getting-started-with-esp8266-wifi-transceiver-review/
#define DHTPIN 4              // This is D2

// DHT sensor type
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Global variables for sensor data
float humiOld;
float tempOld;


// ------------------------------------------------
// Global variables for adapter

WiFiServer server(port);   // max_clients = 1
WiFiClient client;
bool connected = false;


// -----------------------------------------------------------
// Adapter functions

void sendTempSHDR(float temp)
{
  String shdr = "|Temp|" + String(temp);
  Serial.println(shdr);
  client.println(shdr);
}

void sendHumiSHDR(float humi)
{
  String shdr = "|Humi|" + String(humi);
  Serial.println(shdr);
  client.println(shdr);
}


void setup() {
  
  // Start the Serial Monitor
  Serial.begin(115200);

  // Start DHT sensor
  dht.begin();
  digitalWrite(LED_BUILTIN, 1);
  tempOld = -99999.0;
  humiOld = -99999.0;

  // Configure hostename
  WiFi.setHostname(ADAPTER_HOSTNAME.c_str());

  // Conencting to WiFi
  Serial.print("Connecting to WiFi ...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start TCP server (MTConnect Adapter)
  server.begin();
  Serial.println();
  Serial.println("=======================================");
  Serial.print("Starting MTConnect Adapter on port ");
  Serial.println(port);
  Serial.println("=======================================");
  Serial.println();
  Serial.println("Waiting for connection from MTConnect agent");
}

void loop() {

  if (!connected) {
    client = server.available();
    if (client) {
      if (client.connected()) {
        Serial.print("Connection recieved from ");
        Serial.println(client.remoteIP());
        Serial.println("|avail|AVAILABLE");
        client.println("|avail|AVAILABLE");
        connected = true;
      } else {    
        // the connection was not a TCP connection  
        client.stop();  // close the connection:
      }
    }
  } 
  else {
    if (client.connected()) {
      
      // collect sensor data
      float temp = dht.readTemperature();
      float humi = dht.readHumidity();
      
      // Check if * PING request came
      String currentLine = "";
      while (client.available()) { 
        char c = client.read(); 
        if (c == '\n') {
          if (currentLine.startsWith("* PING")) {
            client.println(PONG);
          }
          Serial.println(currentLine);
          currentLine = "";
        }
        currentLine += c;
      }

      // sends SHDR data
      if (temp!=tempOld) {
        sendTempSHDR(temp);
        }
      tempOld = temp;

      if (humi!=humiOld) {
        sendHumiSHDR(humi);
        }
      humiOld = humi;

      
    }
    else {
      Serial.println("Client has disconnected ");
      client.stop();
      connected = false;
    }
  }

}
