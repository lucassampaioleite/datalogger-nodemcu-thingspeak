#include <ESP8266WiFi.h>

extern "C"{
  #include "user_interface.h" 
}

//credentials
#define WIFI_SSID "XXXXXXXXXXXXXXXXXXX"
#define WIFI_PASSWORD "XXXXXXXXXXXXXXXXXXX"
#define API_KEY "XXXXXXXXXXXXXXXXXXX"
#define SERVER "api.thingspeak.com"

os_timer_t mTimer;
bool _timeout = false;

WiFiClient client;

void tCallback(void *tCall){
    _timeout = true;
}
 
void usrInit(void) {
    os_timer_setfn(&mTimer, tCallback, NULL); //Indicates the subroutine to the timer.
    os_timer_arm(&mTimer, 10000, true); //Indicates to the Timer the time in ms and whether it will be repeated or just once (loop = true)
}

void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

long mockSensorValue(){
  //mock a number between 30 and 40
  return rand() % 10 + 30;
}
 
void setup() {
  Serial.begin(115200);
  initWiFi();
  usrInit(); //start interrupt 
}
 
void loop() {

  if (_timeout){
    float sensor1 = mockSensorValue();
    float sensor2 = mockSensorValue();
    int sensor3 = (int)mockSensorValue();  
      
    if (client.connect(SERVER,80)) { //Starts a TCP client to send data
      String postStr = API_KEY;
             postStr +="&field1=";
             postStr += String(sensor1);
             postStr +="&field2=";
             postStr += String(sensor2);
             postStr +="&field3=";
             postStr += String(sensor3);
             postStr += "\r\n\r\n";
       client.print("POST /update HTTP/1.1\n");
       client.print("Host: api.thingspeak.com\n");
       client.print("Connection: close\n");
       client.print("X-THINGSPEAKAPIKEY: "+(String)API_KEY+"\n");
       client.print("Content-Type: application/x-www-form-urlencoded\n");
       client.print("Content-Length: ");
       client.print(postStr.length());
       client.print("\n\n");
       client.print(postStr);
          
       Serial.print("Sensor1: ");
       Serial.print(sensor1);
       Serial.print(" Sensor2: ");
       Serial.println(sensor2);
       Serial.print(" Sensor3: ");
       Serial.println(sensor3);
    }
    client.stop();
    _timeout = false;
  }
}
