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
// - Signage of CO2 levels Acceptable, Warning  and Dangerous 
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
// 03/12/2020 - Revisió rutina d'espera per poder accedir millor al servidor web
//            - Afegit fitxer Wifi_credentials.h
//            - Afegit fitxer CO2_thershold.h
//            - Afegit fitxer Config.h
// 22/12/2020 - Afegit codi MQTT
//            - Revisat codi anterior
//
// ######################################################################################################################
//
// MH-Z19B
#include <SoftwareSerial.h> // Click here to get the library: http://librarymanager/All#EspSoftwareSerial
#include <MHZ19.h> // library for MH-Z19B: https://github.com/strange-v/MHZ19
#include "CO2_thershold.h" // Límits pels llindars de CO2
#define TXPin 15 // para MH-Z19B
#define RXPin 13 // para MH-Z19B

// Instantiate SoftwareSerial
SoftwareSerial ss(RXPin, TXPin);// RX, TX
// Instantiate the MH-Z19B CO2-Sensor with SoftwareSerial
MHZ19 mhz(&ss); 

float CO2;
float Mtemperature;

// BME280
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)
// Instantiate the BME sensor with I2C
Adafruit_BME280 bme; 
float Btemperature, Bhumidity, Bpressure, Baltitude;

// WebServer
#include <ESP8266WebServer.h>
#include "WiFi_credentials.h" // credenciales del WiFi
// Instantiate WebServer at port 80
ESP8266WebServer server(80);  

// Traffic Light
int green = 2;   //D4
int yellow = 14; //D5
int red = 12;    //D6

//MQTT
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "MQTT.h" // Confifguració MQTT

//Buffer to decode MQTT messages
char message_buff[100];

long lastMsg = 0;   
long lastRecu = 0;
bool debug = false;  //Display log message if True

WiFiClient espClient;
PubSubClient client(espClient);

// Configuracions generals 
#include "Config.h" // Configuracions generals

// Control del tiempo
long timerCount = 0;

void setup_wifi()
{
  delay(10);
  Serial.println("------------------WIFI---------------");
  Serial.println("Connecting to ");
  Serial.println(WIFI_NAME);

  //connect to your local wi-fi network
  WiFi.begin(WIFI_NAME, WIFI_PASS);

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

void setup()
{
  Serial.begin(115200);
  //WIFI connection
  setup_wifi();

  //MQTT
  client.setServer(mqtt_server, 1883);    // Configure MQTT connexion
    
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
        for (int j = 0; j< int(preheating_Time/(3000)) ; j++){          
          digitalWrite(red, LOW);
          digitalWrite(yellow, LOW);
          digitalWrite(green, LOW);
          delay(int(500));
          if (i==0){
            digitalWrite(red, HIGH);
            
          }
          if (i==1){
            digitalWrite(red, LOW);  
            digitalWrite(yellow, HIGH);          
         
          }
          if (i==2){
            digitalWrite(red, LOW);  
            digitalWrite(yellow, LOW);   
            digitalWrite(green, HIGH);       
         
          }
          delay(500);          
        }
      }
      Serial.println("-- Reading MH-Z19B --");
      delay(200);
      Serial.print( "Acuracy:" ); 
      Serial.println(mhz.getAccuracy()? "ON" : "OFF" );
      Serial.print( "Detection Range: " ); 
      Serial.println( 5000 );
  }  
}

//Reconnexion MQTT
void reconnect() {
  // Loop until we're reconnected
  int intents = 0;
  while (!client.connected() && intents < 5) {
    Serial.print("Connecting to MQTT broker ...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("Connected to MQTT Server");
    } else {
      Serial.print("Failed to connect to MQTT server, error : ");
      Serial.print(client.state());
      Serial.println(" Wait 5 secondes before to retry");
      delay(5000);
      intents = intents + 1;
    }
  }
}

void loop()
{
  // Grabbing data from MHZ19B sensor
  MHZ19_RESULT response = mhz.retrieveData();
  if (response == MHZ19_RESULT_OK)
  {
    CO2 = mhz.getCO2();
    Mtemperature = mhz.getTemperature();    
    //Serial.print("MZH19B - ");
    //Serial.print(F("CO2: "));
    //Serial.println(CO2);
    //Serial.print(F("Min CO2: "));
    //Serial.println(mhz.getMinCO2());
    //Serial.print("MZH19B - ");
    //Serial.print(F("Temperature: "));
    //Serial.print(Mtemperature);
    //Serial.print(F("Accuracy: "));
    //Serial.println(mhz.getAccuracy());
  }
  else
  {
    Serial.print(F("MHZ Error, code: "));
    Serial.println(response);
  }
  // Switch CO2
  if (CO2 < CO2_med) {
    digitalWrite(green, HIGH);
    digitalWrite(yellow, LOW);
    digitalWrite(red, LOW);
    
  }
  else{
    if (CO2 > CO2_max) {
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

  //Changin format of data to be sending via Web && MQTT
  String t = String(Btemperature);
  String tM  = String(Mtemperature);
  String h = String(Bhumidity);
  String p = String(Bpressure);
  String a = String(Baltitude);
  String c = String(CO2);
  
  //Serial.print(F("BME280 - "));
  //Serial.println("Temperature: " + t);


  //MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop();    
  delay(500);  

  // Wifi server && MQTT Sending data
  // 1st record the time from the timer
  timerCount = millis();
   while (millis() < timerCount + delay_Time) {
    //Wifi server
    server.handleClient();
    //MQTT
    //Enviam cada minut les messures preses
    if (timerCount - lastMsg > 1000*tMostreig){

      lastMsg = timerCount;

      Serial.print("BME280 - ");
      Serial.println("Humidity: " + h);
      Serial.print("MZH19B - ");
      Serial.println("CO2: " + c);
      Serial.println("Temperature: " + tM);
              
      client.publish(String(temperature_topic).c_str(), String(tM).c_str(), true);   // Publish temperature on temperature_topic
      client.publish(String(humidity_topic).c_str(), String(h).c_str(), true);       // and humidity
      client.publish(String(co2_topic).c_str(), String(c).c_str(), true);            // and CO2      
      
    }

    //He de revisar que fa això
    //if (now - lastRecu > 100 ) {
    //  lastRecu = now;
    //  client.subscribe("homeassistant/switch1");
    //}
   }

}

void handle_OnConnect() {
  Btemperature = bme.readTemperature();
  Bhumidity = bme.readHumidity();
  Bpressure = bme.readPressure() / 100.0F;
  Baltitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  int CO2 = mhz.getCO2();
  float Mtemperature = mhz.getTemperature();
  server.send(200, "text/html", SendHTML(Btemperature,Mtemperature,Bhumidity,Bpressure,Baltitude, CO2)); 
  //String t = String(Btemperature);
  Serial.println("Connection done!");
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float Btemperature,float Mtemperature,float humidity,float pressure,float altitude, int CO2){
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
  ptr +="<p>Temperature (BME): ";
  ptr +=Btemperature;
  ptr +="<p>Temperature (MHZ): ";
  ptr +=Mtemperature;  
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
