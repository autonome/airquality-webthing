#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "Thing.h"
#include "WebThingAdapter.h"

const char* ssid = "";
const char* password = "";

WebThingAdapter adapter("esp8266");

ThingDevice airQualitySensor("airqualitysensor", "Air quality sensor", "thing");

ThingProperty pm25Property("pm25level", "The level of pm2.5", NUMBER);
ThingProperty pm10Property("pm10level", "The level of pm10", NUMBER);
ThingProperty pm100Property("pm100level", "The level of pm100", NUMBER);

unsigned int pm10 = 0;
unsigned int pm25 = 0;
unsigned int pm100 = 0;

// Wait 5 seconds between checking for and posting new data
unsigned int interval = 5 * 1000;

void setup(void){
  Serial.begin(9600);
  //Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  airQualitySensor.addProperty(&pm25Property);
  airQualitySensor.addProperty(&pm10Property);
  airQualitySensor.addProperty(&pm100Property);
  adapter.addDevice(&airQualitySensor);
  adapter.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  adapter.update();
  
  int index = 0;
  char value;
  char previousValue;

  while (Serial.available()) {
    value = Serial.read();
    
    if ((index == 0 && value != 0x42) || (index == 1 && value != 0x4d)){
      Serial.println("Cannot find the data header.");
      break;
    }

    if (index == 4 || index == 6 || index == 8 || index == 10 || index == 12 || index == 14) {
      previousValue = value;
    }
    else if (index == 5) {
      pm10 = 256 * previousValue + value;
      Serial.print("pm10 ");
      Serial.println(pm10);
    }
    else if (index == 7) {
      pm25 = 256 * previousValue + value;
      Serial.print("pm25 ");
      Serial.println(pm25);
    }
    else if (index == 9) {
      pm100 = 256 * previousValue + value;
      Serial.print("pm100 ");
      Serial.println(pm100);
    } else if (index > 15) {
      break;
    }
    index++;
  }

  ThingPropertyValue pm25Value;
  pm25Value.number = pm25;
  pm25Property.setValue(pm25Value);
  
  ThingPropertyValue pm10Value;
  pm10Value.number = pm10;
  pm10Property.setValue(pm10Value);
  
  ThingPropertyValue pm100Value;
  pm100Value.number = pm100;
  pm100Property.setValue(pm100Value);
  
  delay(interval);
}
