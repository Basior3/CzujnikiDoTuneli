/*
Sending Data to Google SpreadSheet by WiFi(through Apps Script [js code below])
author Pepek
based on
https://www.youtube.com/watch?v=3Ukwd-IFZ0Y&t=84s
but there were some errors in apps script project (needed to check if (e && e.parameter))
TODO check if there is limit in sended parameters via URL
*/
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>


#include <string>
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <FS.h>
#define ON_Board_LED 2  // On board LED, indicator when connecting to a wifi router

#define DHTPIN D4    // Pin, do którego podłączony jest czujnik DHT11 (D4 odpowiada GPIO2)
#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Janek";   // Your wifi name
const char* password = "1234567890"; // Your wifi password
float dataToSend{0};

//----------------------------------------Host & httpsPort
const char* host = "script.google.com";
const int httpsPort = 443;
//----------------------------------------

WiFiClientSecure client; // Create a WiFiClientSecure object

// Google spreadsheet script ID
String GAS_ID = "AKfycbwa14JGNeq6CZ01sJAFHboMg6skItS_m0703vg3rZZB9hNjuoLpqwiZyEKhh6v7uS6q";

void setup() {
  Serial.println("wszrdlem");
  Serial.begin(115200);
    if (!SPIFFS.begin()) {
          Serial.println("Failed to mount file system");
          return;
      }
      dht.begin();


  WiFi.begin(ssid, password); // Connect to your WiFi router
  Serial.println("");
    
  pinMode(ON_Board_LED,OUTPUT);     // On board LED port as output
  digitalWrite(ON_Board_LED, HIGH); // Turn off Led on board

  //----------------------------------------Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    //----------------------------------------Make LED flashing when connecting to the wifi router
    digitalWrite(ON_Board_LED, LOW);
    delay(200);
    digitalWrite(ON_Board_LED, HIGH);
    delay(200);
    //----------------------------------------
  }
  //----------------------------------------
  digitalWrite(ON_Board_LED, HIGH); // Turn off the LED when it is connected to the wifi router
  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  //----------------------------------------

  client.setInsecure();
}

// Subroutine for sending data to Google Sheets
void sendData(float tem, float hum, int data1, int data2,int data3,int data4,int data5,int data6,int data7,int data8,int data9, std::vector<int> *data) {
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);
  
  //----------------------------------------Connect to Google host
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  //----------------------------------------

  //----------------------------------------Processing data and sending data
  String string_temperature =  String(tem);
  String string_humidity =  String(hum);
  String string_data1 =  String(data1);
  String string_data2 =  String(data2);
  String string_data3 =  String(data3);
  String string_data4 =  String(data4);
  String string_data5 =  String(data5);
  String string_data6 =  String(data6);
  String string_data7 =  String(data7);
  String string_data8 =  String(data8);


//   String string_data4 =  std::to_string(data4);
//   String string_data5 =  std::to_string(data5);
//   String string_data6 =  std::to_string(data6);
//   String string_data7 =  std::to_string(data7);
//   String string_data8 =  std::to_string(data8);
//  string_data9 =  std::to_string(data9);
  String label{"&data"};
  String endLabel{""};
  String string_pres{"pres"};
  int size {(int)data->size()};
  for(int i=0; i < size;i++)
  {
    endLabel =endLabel+ label + String(i)+"="+String(data->at(i));
  }
  Serial.println(endLabel);
  String url = "/macros/s/" + GAS_ID + "/exec?temperature=" + string_temperature
                                      + "&humidity=" + string_humidity
                                      + "&pressure="+ string_pres
                                      + endLabel
                                      // + "&data1="+ string_data1
                                      // + "&data2="+ string_data2
                                      // + "&data3="+ string_data3
                                      // + "&data4="+ string_data4
                                      // + "&data5="+ string_data5
                                      // + "&data6="+ string_data6
                                      // + "&data7="+ string_data7
                                      // + "&data8="+ string_data8
                                      // + "&data9="+ string_data9 ;
  ;
                                      
  Serial.print("requesting URL: ");
  //Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");

  Serial.println("request sent");
  //----------------------------------------

  //----------------------------------------Checking whether the data was sent successfully or not
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.print("reply was : ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
  //----------------------------------------
}

void loop() {
    float h = dht.readHumidity();
    float t = dht.readTemperature();
  
  std::vector <int> data{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from BMP sensor !");
    delay(500);
    return;
  }
  
  String Temp = "Temperature : " + String(t) + " °C";
  String Humi = "Humidity : " + String(h) + " %";
  Serial.println(Temp);
  Serial.println(Humi);
  dataToSend = dataToSend +1;
  sendData(t, h, dataToSend, data.at(1), data.at(2), data.at(3),data.at(4),data.at(5),data.at(6),data.at(7),data.at(8), &data); // Call the sendData subroutine
  delay(5000);
}

