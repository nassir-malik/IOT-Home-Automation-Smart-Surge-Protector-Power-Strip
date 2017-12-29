#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <functional>
#include "switch.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"
#include "wifimanagment.h" 
UpnpBroadcastResponder upnpBroadcastResponder;
//#######################################
//#######################################
//I am using ESP8266 E12
const int lineOneRelayPin = 5;
const int lineTwoRelayPin = 4;
//#######################################
Switch *lineOne = NULL;
Switch *lineTwo = NULL;
//#######################################
//on/off callbacks 
void lineOneOn();
void lineOneOff();

void lineTwoOn();
void lineTwoOff();
//#######################################
//#######################################

void setup()
{
   EEPROM.begin(memalloc);
   Serial.begin(115200);
   pinMode(lineOneRelayPin, OUTPUT);
   pinMode(lineTwoRelayPin, OUTPUT);
  // Initialise wifi connection
  //wifiConnected = connectWifi();
  
  if(connectWifi()){
    upnpBroadcastResponder.beginUdpMulticast();
    
    // Define your switches here. Max 14
    // Format: Alexa invocation name, local port no, on callback, off callback
    lineOne = new Switch("line one", 80, lineOneOn, lineOneOff);
    lineTwo = new Switch("line two", 81, lineTwoOn, lineTwoOff);

    Serial.println("Adding switches upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*lineOne);
    upnpBroadcastResponder.addDevice(*lineTwo);
  }else{
    startAP();
  }
}
 
void loop()
{
	 if(WiFi.status() == WL_CONNECTED){
      upnpBroadcastResponder.serverLoop();
      
      lineOne->serverLoop();
      lineTwo->serverLoop();
	 }else{
      startAP();
	 }
}

void lineOneOn() {
    Serial.print("Line 1 turned on ...");
    digitalWrite(lineOneRelayPin, HIGH);
}

void lineOneOff() {
    Serial.print("Line 1 turned off ...");
    digitalWrite(lineOneRelayPin, LOW);
}

void lineTwoOn() {
    Serial.print("Line 2 turned on ...");
    digitalWrite(lineTwoRelayPin, HIGH);
}

void lineTwoOff() {
  Serial.print("Line 2 turned off ...");
  digitalWrite(lineTwoRelayPin, LOW);
}


