// Wemos MH-Z19B BME28
// Meassure CO2 and environmental data with Wemos D1 mini, MH-Z19B i BME280
//
// Martí Vich, 01/12/2020
//
// based on / credits to:
// https://github.com/AndreasIBZ/ESP32/tree/main/CCS811_BME280_influxDB_Makespace_Mallorca
// https://github.com/miguelangelcasanova/codos
// https://github.com/Makespace-Mallorca/codos

//
// ##### TODO #####
// - Introducción de los datos en InfluxDB y su visualización mediante grafana
// - Revisión si se puede optimizar el código
// - Revisión del Deep Sleep
// - Mover a libreria parte del código que sea modulable


// ################
//
// Code description
// - Measurement of CO2 level and temperature using the MH-Z19B
// - Measurement of the environment (temperature, pressure, humidity) using the BME280
// - Signage of CO2 levels Acceptable (<499 ppm), Warning (500 <CO2 <799ppm) and Dangerous (> 799ppm)
//
// Wiring MH-Z19B -> Wemos D1 mini
// Vin - 5V (VIN)
// RX - GPIO 13 (Software Serial TX)
// TX - GPIO 15 (Software Serial RX)
// GND - GND
//

//
// Wiring BME280 -> Wemos D1 mini
// SCL - D1
// SDA - D2
// VCC - 3V3
// GND - GND

//
// Traffic Light
// GREEN  - D4
// ORANGE - D5
// RED    - D6

// Changelog
// 01/12/2020 - Primera versió 
// 02/12/2020 - Afegim el semàfor i el sensor BME280 
//
// ######################################################################################################################
//
// MH-Z19B
#include <SoftwareSerial.h> // Click here to get the library: http://librarymanager/All#EspSoftwareSerial
#include <MHZ19.h> // library for MH-Z19B: https://github.com/strange-v/MHZ19
#define TXPin 15 // para MH-Z19B
#define RXPin 13 // para MH-Z19B

// Instantiate SoftwareSerial
SoftwareSerial ss(RXPin, TXPin);// RX, TX
// Instantiate the MH-Z19B CO2-Sensor with SoftwareSerial
MHZ19 mhz(&ss); 

//BME280
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)
// Instantiate the BME sensor with I2C
Adafruit_BME280 bme; 
float Btemperature, Bhumidity, Bpressure, Baltitude;

//WebServer
#include <ESP8266WebServer.h>
/*Put your SSID & Password*/
const char* ssid = "SSID";  // Enter SSID here
const char* password = "PASSWORD";  //Enter Password here
// Instantiate WebServer at port 80
ESP8266WebServer server(80);  

//Traffic Light
int green = 2;   //D4
int yellow = 14; //D5
int red = 12;    //D6-12

void setup()
{
  Serial.begin(115200);
  //Initialize traffic light
  pinMode(green, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(red, OUTPUT);

  //BME280
  Serial.println("-- Initializing BME280...");
  bme.begin(0x76); 

  // MH-Z19B: Initialize Software Serial
  Serial.println("-- Initializing SoftwareSerial (MH-Z19B)...");
  { ss.begin(9600); 
      delay(100);
      mhz.setAutoCalibration( false );  // default settings - off autocalibration
      Serial.println("Preheating...");
      //Preheating of the sensor (usually 3 minutes)
      for (int i = 0; i<3; i++){
        for (int j = 0; j< 30; j++){          
          digitalWrite(red, LOW);
          digitalWrite(yellow, LOW);
          digitalWrite(green, LOW);
          delay(1000);
          if (i==0){
            digitalWrite(red, HIGH);
          }
          if (i==1){
            digitalWrite(red, HIGH);  
            digitalWrite(yellow, HIGH);          
          }
          if (i==2){
            digitalWrite(red, HIGH);  
            digitalWrite(yellow, HIGH);   
            digitalWrite(green, HIGH);       
          }
          delay(1000);          
        }
      }
      Serial.println("-- Reading MH-Z19B --");
      delay(200);
      Serial.print( "Acuracy:" ); Serial.println(mhz.getAccuracy()? "ON" : "OFF" );
      Serial.print( "Detection Range: " ); Serial.println( 5000 );
  }  

  //Wifi
  Serial.println("------------------WIFI---------------");
  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
  // Grabbing data from MHZ19B sensor
  MHZ19_RESULT response = mhz.retrieveData();
  if (response == MHZ19_RESULT_OK)
  {
    Serial.println("MZH19B");
    Serial.print(F("CO2: "));
    Serial.println(mhz.getCO2());
    //Serial.print(F("Min CO2: "));
    //Serial.println(mhz.getMinCO2());
    //Serial.print(F("Temperature: "));
    //Serial.println(mhz.getTemperature());
    //Serial.print(F("Accuracy: "));
    //Serial.println(mhz.getAccuracy());
  }
  else
  {
    Serial.print(F("Error, code: "));
    Serial.println(response);
  }
  // Switch CO2
  if (mhz.getCO2() < 499) {
    digitalWrite(green, HIGH);
    digitalWrite(yellow, LOW);
    digitalWrite(red, LOW);
    
  }
  else{
    if (mhz.getCO2() > 799) {
       digitalWrite(red, HIGH);
       digitalWrite(yellow, LOW);
       digitalWrite(green, LOW);
    }
    else{
       digitalWrite(yellow, HIGH);
       digitalWrite(green, LOW);
       digitalWrite(red, LOW);
    }    
  }

  // Grabbing data from BME280
  Btemperature = bme.readTemperature();
  Bhumidity = bme.readHumidity();
  Bpressure = bme.readPressure() / 100.0F;
  Baltitude = bme.readAltitude(SEALEVELPRESSURE_HPA);

  String t = String(Btemperature);
  String h = String(Bhumidity);
  String p = String(Bpressure);
  String a = String(Baltitude);
  
  Serial.println("BME280");
  Serial.println("Temperature: " + t);

  //Wifi server
  server.handleClient();
  // dealy 15s
  delay(15000);
}

void handle_OnConnect() {
  Btemperature = bme.readTemperature();
  Bhumidity = bme.readHumidity();
  Bpressure = bme.readPressure() / 100.0F;
  Baltitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  int CO2 = mhz.getCO2();
  server.send(200, "text/html", SendHTML(Btemperature,Bhumidity,Bpressure,Baltitude, CO2)); 
  //String t = String(Btemperature);
  Serial.println("Connection done!");
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float temperature,float humidity,float pressure,float altitude, int CO2){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>ESP8266 Weather Station</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>ESP8266 Weather Station</h1>\n";
  ptr +="<p>Temperature: ";
  ptr +=temperature;
  ptr +="&deg;C</p>";
  ptr +="<p>Humidity: ";
  ptr +=humidity;
  ptr +="%</p>";
  ptr +="<p>Pressure: ";
  ptr +=pressure;
  ptr +="hPa</p>";
  ptr +="<p>Altitude: ";
  ptr +=altitude;
  ptr +="m</p>";
  ptr +="<p>CO2: ";
  ptr +=CO2;
  ptr +="ppm</p>";
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
