//#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <EEPROM.h>
//#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <DNSServer.h>
//#include <ESP8266WebServer.h>
#include <WiFiManager.h>
const char* ssid;
const char* password;

byte value;
int address = 0;
int memalloc = 1024;
//flag for saving data
bool shouldSaveConfig = false;

void eepromWrite( String buff)
{
     for (int i = 0; i < memalloc; i++){
      EEPROM.write(i, 0);
   }
  const char* id;
  id= buff.c_str();
   for (int i = 0; i < memalloc; i++){ //clear buffer before write
      EEPROM.write(i, 0);
   }
  size_t i;
  //i+=buffLocation;
  for(i=0; i<=strlen(id); i++) //write value to eeprom
  {
    EEPROM.write(i, (uint8_t) id[i]);
  }
  EEPROM.commit();
}

 String eepromRead(){
  Serial.println("Reading EEPROM");
  String buff;
  for (int i = 0; i < memalloc; ++i)
    {
      buff += char(EEPROM.read(i));
    }
    Serial.println(buff);
    return buff;
}

//callback notifying us of the need to save config
void saveConfigCallback () {
        Serial.println("Should save config");
        shouldSaveConfig = true;
        Serial.println("connected...yeey :)");

        DynamicJsonBuffer jsonBuffer;
        JsonObject& root = jsonBuffer.createObject();
        JsonObject& params = root.createNestedObject("params");
        //JsonObject& params = jsonBuffer.parseObject(eepromRead());

        params["ap_name"] = WiFi.SSID();
        params["ap_password"] = WiFi.psk();
        char buf[1024];
        params.printTo(buf);
        eepromWrite(buf);

}

boolean startAP(){
  
    //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);


    if (!wifiManager.startConfigPortal("NetmediasAP")) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.reset();
      delay(5000);
    }
   // Serial.println("local ip");
}


boolean connectWifi(){
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root= jsonBuffer.parseObject(eepromRead());

   if (root.success()) {
      ssid = root["ap_name"].as<char*>();
      password = root["ap_password"].as<char*>();
   }else{
      return false;
   }
  boolean state = true;
  int i = 0;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //MDNS.begin("garagedoor");
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 10){
      state = false;
      break;
    }
    i++;
  }

  if (state){
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

  }
  else {

    Serial.println("");
    Serial.println("Connection failed.");
    return false;
  }
  return true;

}
