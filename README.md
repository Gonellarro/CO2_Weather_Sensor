![top](imgs/cover.png)

# README - SENSOR DE CO2

Este documento describe como realizar un dispositivo de bajo costo para registrar la cantidad de CO2 que hay en el aire, indicando mediante un semáforo en que situación nos encontramos. Se incluyen dos versiones de programa en función de si necesitamos solo un avisador de CO2 máximo o si queremos llevar un control de aire, temperatura y humedad más sofisticado y centralizado mediante la consola de Grafana.

----------

TIEMPO ESTIMADO: **120/240 min (básico/avanzado)**

-------

**APRENDERÁS:**

```
IDE ARDUINO, I2C, COMUNICACIÓN SERIE, INCLUSIÓN DE LIBRERÍAS, INCORPORACIÓN DE PLACAS, WIFI, DOCKER, INFLUXDB, TELEGRAPH, GRAFANA, MQTT, SENSORES
```

------

## Visión general

El año 2020 nos sorprendió a todos con la llegada de un virus devastador que ha infectado a todo el mundo sin distinción de raza, sexo o credo. Las fronteras políticas y geográficas han sido meras entelequias humanas que este virus ha despreciado y no le han parado en su expansión. 

La humanidad se ha visto amenazada de forma inesperada y con una rapidez no vista en mucho tiempo. Solo los confinamientos masivos por parte de las naciones han conseguido contener o mitigar el avance del mismo, siendo las otras alternativas, mucho menos efectivas. La dualidad entre el mantenimiento de la economía y la preservación de la salud de la población es el tema que más preocupa a los dirigentes del mundo, ya que este se debate entre salvaguardar nuestra salud o la economía. Muchas de las voces autorizadas del mundo, comparan la reciente situación como mucho peor a la reciente segunda guerra mundial y poder mantener el tejido productivo en marcha sorteando los contagios es el reto que nos tiene en marcha durante el final del 2020 principios del 2021. Hasta que el mundo entero no se inmunice mediante el uso de vacunas o la reversión del virus, tenemos que ser capaces de buscar formas o soluciones que nos ayuden a protegernos y mantener como se pueda la actividad económica. 

Es por eso, que científicos de todo el mundo han considerado la relación entre la ventilación y  la transmisión del virus como una parte fundamental en la expansión del mismo. Un espacio mal o poco ventilado con infectados por el virus es la causa más habitual de contagio. Los aerosoles han aparecido como vectores muy eficaces en la transmisión y es por ello, que para poder controlar estos aerosoles en el aire se apuesta por sensores de CO2 que nos indican, no si existen aerosoles, sino cuan viciado está el aire. La relación entre cantidad de CO2 y dichos aerosoles es directa, con lo cual no es necesario medir niveles de COVID o aerosoles, sino de CO2 y nos aseguraremos un aire limpio sin costes elevados.

Este proyecto trata de obtener lecturas de varios sensores (de CO2, temperatura y humedad) para indicar la calidad del aire en función del CO2. El controlador que se encarga de ello se trata de un Wemos D1 Mini que es un microcontrolador pequeño, barato y con Wifi, que nos permitirá recoger los datos necesarios y enseñarlos mediante un semáforo de colores e incluso, si disponemos de red Wifi, mediante una pequeña página web. El sensor que utilizamos para registrar las ppm (partes por millón) de CO2 es el MHZ19b que hemos comprobado que tiene una fiabilidad y precisión bastante elevada junto con un precio acceptable. El sensor de humedad es el BME280 que también da lecturas de temperatura, pero estas son menos rigurosas. Se tomarán estas últimas de forma cuantitativa y no cualitativa.

El proyecto tiene una segunda parte, más elaborada y compleja, que pretende realizar un sistema de control centralizado de los datos que toma el microcontrolador, usando bases de datos y el sistema gestor de gráficas y dashboards, llamado Grafana. Todo este sistema, se puede instalar sobre un PC, una máquina virtual, una raspberry, etc... pero para hacerlo mucho más modular y portable, se ha optado por hacerlo mediante docker-compose que permite replicarlo con muy poco esfuerzo.



> **Nota:** Sea cual sea el nivel de integración que se quiera llevar a cabo (básico o avanzado), el proyecto se ha pensado para que se pueda llevar a cabo sin cambios. Esto significa que tanto los dispositivos, montaje como código, son los mismos en el nivel básico y el avanzado. Desde un principio se ha pensado en poderlo llevar a cabo sin más pretensión que el nivel básico y no penaliza a aquellos que no quieran complicarse la vida o no necesiten tanto nivel de monitorización



## Dispositivo básico

El dispositivo básico registrará los datos de CO2, temperatura y humedad de los sensores que incorpora.  Una vez montado el circuito, cargado el programa al Wemos y dado corriente, este empezará con un proceso de precalentado (3 minutos en los que los 3 leds parpadearán de forma secuencial) y se encenderá el led que corresponda al nivel de CO2.

### Componentes necesarios

-  1 x Wemos D1 Mini
-  3 x Resistencias 220 Ohm
-  3 x Led (Rojo, Naranja y Verde)
-  1 x MHZ19B
-  1 x BME 280
-  1 x Protoboard/PCB
-  1 x Cable USB
-  1 x Fuente alimentación 5V/1A

### Diagrama esquemático

![Esquema](imgs/Diagrama_esquematico.png)



![Fritzing](imgs/Diagrama_bb.png)

### Construir el circuito

A la hora de construir el circuito debemos de tener en cuenta todo el patillaje y como está conectado entre ellos. Es importante tener una descripción detallada de cada componente, así como su DataSheet si es necesario. Veamos ahora el patillaje de cada componente:

- *Wemos D1 mini pinout*

![wemos_d1_mini_pinout](imgs/wemos_d1_mini_pinout.jpg)



- *MHZ19 pinout*

![mhz19b_pinout](imgs/mhz19b_pinout.jpg)

- *BME280 pinout* 

![BME280](imgs/BME280_pinout.jpg)

- *LEDs pinout (recordar que son diodos y por tanto, tienen una pata + y la otra -)*

 ![led_pinout](imgs/led_pinout.jpg)



Procederemos al montaje del circuito en una protoboard, tal y como se muestra en el diagrama esquemático y respetando los pines del diagrama. Hay que conectar el Wemos al ordenador mediante un cable USB, que será el que nos permitirá darle corriente por una parte y cargar el programa por otra.



> **Nota**: Deberemos comprobar en el IDE de Arduino en la sección que viene, que la placa ha sido conectada en un puerto del PC mediante la revisión del puerto. Si no es así, hay que revisar las conexiones con el Wemos, el cable o cambiar de conexión USB del PC, teniendo en cuenta que cada conexión corresponde a un COM diferente.

### Creación del código: Detectar niveles de CO2 con LEDS

1. **Importación de la placa Wemos D1 mini**

Lo primero que debemos hacer al abrir el Arduino IDE es importar los microcontroladores que queremos que sea capaz de compilar. Para ello, añadiremos la linea

```
http://arduino.esp8266.com/stable/package_esp8266com_index.json
```

a las preferencias del IDE/Gestión de URLs adicionales de tarjetas:

![Code01](imgs/Code01.jpg)

Podemos ver que lo hemos importado bien si encontramos la placa LOLIN(WEMOS) D1 R2 & mini en el menú Herramientas\Gestor de tarjetas\ESP8266 Boards (2.7.4) .

Si hemos conectado correctamente el Wemos, lo veremos conecado en el menú Herramientas\Puerto

![wemos2](imgs/wemos_02.jpg)



2. **Importación de las librerías del sensor de CO2**

Debemos bajarnos el ZIP de la librería que está en https://github.com/strange-v/MHZ19 e importarlo como librería. 

![libraryMHZ19B_01](imgs/libraryMHZ19B_01.jpg)



Vamos al menú Programa\Incluir Librería\Añadir biblioteca .ZIP

![libraryMHZ19B_02](imgs/libraryMHZ19B_02.jpg)



Podemos comprobar que está bien si encontramos la librería en el menú Programa\Incluir Librería\MHZ19

![image-20210120191643404](imgs/libraryMHZ19B_03.jpg)



3. **Importación de las librerías del sensor de temperatura**

En este caso es más sencillo, ya que tenemos que añadir solo las librerías de Adafruit y que ya están listadas por defecto. Vamos al menú Programa\Incluir Librería\Administrar bibliotecas y buscamos por BME. Elegimos la librería de Adafruit

![libraryBME280](imgs/libraryBME280.jpg)



Igualmente comprobaremos que lo hemos cargado correctamente si aparece dentro de las librerías disponibles.

![image-20210120192342501](imgs/libraryBME280_02.jpg)

4. **El código**

Una vez ya tenemos todo el sistema preparado, podemos cargar el código. Para tener una estructura más clara, legible y escalable, se ha dividido en 5 ficheros diferentes:

- MHZ19B_wemos_BM280.1.3.ino
- Config.h
- Wifi_credentials.h
- CO2_thershold.h
- MQTT.h



El fichero principal, que controla todo el proceso (MHZ19B_wemos_BM280.1.3.ino)

```python
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
// - Revisión si se puede optimizar el código
// - Revisión del Deep Sleep
// - Mover a libreria parte del código que sea modulable


// #####################################################################################
//
// Code description
// - Measurement of CO2 level and temperature using the MH-Z19B
// - Measurement of the environment (temperature, pressure, humidity) using the BME280
// - Signage of CO2 levels Acceptable, Warning  and Dangerous 
//
// Wiring MH-Z19B -> Wemos D1 mini
// Vin - 5V (VIN)
// RX - GPIO 13 (Software Serial TX) - D8
// TX - GPIO 15 (Software Serial RX) - D7
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
// #########################################################################################
//
// MH-Z19B
#include <SoftwareSerial.h> // Click here to get the library: http://librarymanager/All#EspSoftwareSerial
#include <MHZ19.h> // library for MH-Z19B: https://github.com/strange-v/MHZ19
#include "CO2_thershold.h" // Límits pels llindars de CO2
#define TXPin 15 // D8 - para MH-Z19B
#define RXPin 13 // D7 - para MH-Z19B

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
```

Explicación del código paso a paso:

Inicialización del sensor de CO2 (MHZ19B): Este sensor permite la captura de datos mediante transmisión serie. Creamos un objeto de la clase *SoftwareSerial* y lo llamamos *ss*. Le asignamos lis pines 15 y 13 del Wemos para la transmisión y recepción de los datos del sensor. Ver datasheet para más detalles. Creamos un objeto de tipo *MHZ19* llamado *mhz* al cual le asignamos como canal de transmisión serie el objeto que hemos creado *ss*. 

Finalmente, creamos las variables *CO2* y *Mtemperature* que nos guardaran estos valores del sensor.



```python
// MH-Z19B
#include <SoftwareSerial.h> // Click here to get the library: http://librarymanager/All#EspSoftwareSerial
#include <MHZ19.h> // library for MH-Z19B: https://github.com/strange-v/MHZ19
#include "CO2_thershold.h" // Límits pels llindars de CO2
#define TXPin 15 // D8 - para MH-Z19B
#define RXPin 13 // D7 - para MH-Z19B

// Instantiate SoftwareSerial
SoftwareSerial ss(RXPin, TXPin);// RX, TX
// Instantiate the MH-Z19B CO2-Sensor with SoftwareSerial
MHZ19 mhz(&ss); 

float CO2;
float Mtemperature;
```

Veamos ahora el sensor de presión, el BME280. Cargamos las librerías correspondientes para poder interactuar con él y ya lo instanciamos para poder trabajar mediente el protocolo [I2C](https://www.i2c-bus.org/):

```python
// BME280
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)
// Instantiate the BME sensor with I2C
Adafruit_BME280 bme; 
float Btemperature, Bhumidity, Bpressure, Baltitude;


```

Seguimos con la creación del servidor web. Para ello cargamos las librerías necesarias, cargamos las credenciales del fichero externo *Wifi_credentials.h* y creamos el objeto *server* en el puerto 80 de tipo *ESP8266Webserver*.

```
// WebServer
#include <ESP8266WebServer.h>
#include "WiFi_credentials.h" // credenciales del WiFi
// Instantiate WebServer at port 80
ESP8266WebServer server(80);  
```

Asignamos los pines 2, 12 y 14 para los leds del semáforo.

```python
// Traffic Light
int green = 2;   //D4
int yellow = 14; //D5
int red = 12;    //D6
```

Finalmente cargamos el módulo MQTT para enviar los mensajes al servidor MQTT. Se crea un cliente de wifi para poder interactuar con el servidor MQTT (espClient) y el objeto client de tipo PubSubClient para publicar las medidas mediante el cliente wifi espClient.

```python
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
```

Creamos la función de setup de la Wifi. Mostramos por el monitor serie cual esla  IP que le ha dado el servidor de DHCP. Si queremos que use una IP estática hay que cambiar esta parte del código.

También arranca el servidor web cuando ya tiene IP.

```python
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
```

Analizamos el setup más despacio para revisar que hace cada parte:

1. Indicamos la velocidad de conexión, arrancamos la wifi, configuramos el cliente de MQTT con la dirección del servidor y el puerto, así como indicamos que los pines verde, naranja y rojo son de salida

```python
  Serial.begin(115200);
  //WIFI connection
  setup_wifi();

  //MQTT
  client.setServer(mqtt_server, 1883);    // Configure MQTT connexion
    
  //Initialize traffic light
  pinMode(green, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(red, OUTPUT);
```

2. Arrancamos la comunicación I2C con el BME280. Su dirección es la 0x76 (consultar datasheet)
3. Arrancamos la comunicación serie con el MHZ19B. Establecemos la velocidad de comunicación a 9600 baudios, realizamos el preheating como indican en el datasheet indicando mediante el parpadeo de los leds, que estamos precalentando el sensor. Van parpadeando uno a uno, durante un minuto, hasta que pasan los 3 minutos.

```python
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
```

Finalmente analizamos el loop, como parte más interesante de la práctica. 

1. Recogemos los datos de MHZ19B

```python
  // Grabbing data from MHZ19B sensor
  MHZ19_RESULT response = mhz.retrieveData();
  if (response == MHZ19_RESULT_OK)
  {
    CO2 = mhz.getCO2();
    Mtemperature = mhz.getTemperature();    
  }
  else
  {
    Serial.print(F("MHZ Error, code: "));
    Serial.println(response);
  }

```

2. Encendemos el led que toca en función del valor de CO2 obtenido. En este caso hemos optado por usar una secuencia de ifs anidados, pero se puede programar mediante la sentencia [switch...case](https://www.arduino.cc/reference/en/language/structure/control-structure/switchcase/)

 

```python
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

```

3. Recogemos los datos del BME280

```python
  // Grabbing data from BME280
  Btemperature = bme.readTemperature();
  Bhumidity = bme.readHumidity();
  Bpressure = bme.readPressure() / 100.0F;
  Baltitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
```

4. Enviamos los datos por MQTT y lo publicamos al servidor web del Wemos

   Cada delay_time revisamos los datos

   ```python
     // Wifi server && MQTT Sending data
     // 1st record the time from the timer
     timerCount = millis();
      while (millis() < timerCount + delay_Time) {
   ```

   Atendemos el servicio web

   ```python
   //Wifi server
   server.handleClient();
   ```

   Cada minuto enviamos los datos al servicio de MQTT
   
   ```python
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
   ```

El resto del código no es tan imprescindible para entender el funcionamiento del sensor y materializa lo que le hemos programado en lo explicado hasta aquí.

Revisamos el resto de ficheros:

Disponemos de un fichero de configuración general (config.h), que controla:

- Tiempo de preheating
- Tiempo de recarga de sensores
- Tiempo de muestreo de MQTT

```python
#ifndef Config_h

// General configuration parameters

const int delay_Time = 15000; // 15 seconds
const int preheating_Time = 180000; // 3 minutes
const int tMostreig = 15; // 60 seconds to send every meassurement to MQTT server



#endif  
```

Disponemos de un fichero (Wifi_credentials.h) que configura los parámetros de la wifi:

```python
#ifndef WiFi_credentials_h

// WiFi
const char WIFI_NAME[] = "your_ISD";
const char WIFI_PASS[] = "your_pass";

#endif
```

Disponemos de un fichero (CO2_thershold.h) que configura los umbrales de encendido de los leds:

```python
#ifndef CO2_thershold_h

// WiFi
const int CO2_med = 599;
const int CO2_max = 799;

#endif
```

Finalmente, disponemos de un fichero (MQTT.h) que configura los parámetros de los meassurements/topics que se envían mediante MQTT a la base de datos InfluxDB para visualizarlos en Grafana. Sin duda este es el fichero más extraño y no es necesario rellenar si no se van a usar los datos en un sistema centralizado.

```python
const char mqtt_server[] = "";   // MQTT Server IP address
const char mqtt_user[] = "";                // if exist
const char mqtt_password[] = "";            // idem

String device_name = "";

String temperature_topic = "sensors/" + device_name + "/temperature";  //Topic temperature
String humidity_topic = "sensors/" + device_name + "/humidity";        //Topic humidity
String co2_topic = "sensors/" + device_name + "/co2";                  //Topic CO2
```



### Pruebas y funcionamiento

Pasaremos ahora a cargar el código al Wemos. Para ello, una vez conectado el Wemos y confirmado que el IDE de Arduino lo detecta en algún puerto del PC, le daremos a compilar y subir.



![compilar](imgs/compilar.png)



Si todo ha ido bien, el programa se habrá compilado y se habrá subido al Wemos. Entonces veremos la secuencia de LEDs:

1. Led verde encendido: Búsqueda de Wifi y asignación de IP
2. Ciclo de encendido/apagado de cada uno de los leds durante un minuto, para precalentar el sensor de CO2 (primero el rojo, luego el naranja y finalmente el verde)
3. Finalemente se encenderá el Led correspondiente al nivel que le hayamos programado en el fichero de configuración.
4. Podemos acceder al dispositivo mediante Wifi a la dirección IP del mismo

//IMATGE

### BONUS: Creación de la placa PCB

Si queremos hacer el proyecto redondo, ya solo nos queda pasar todo el montaje a una placa. Podemos optar por las placas perforadas y cablear todos los componentes, o diseñar una placa desde cero y enviarla a fabricar. 

En este caso ya hemos realizado la placa, la hemos diseñado con [Kicad](https://kicad.org/) y enviado a fabricar a un proveedor de Internet. Dejamos los ficheros necesarios (JLCPCB.zip) en la carpeta  *Dessign/Kicad* para que se puedan enviar al proveedor que se desee y poder acabar el dispositivo de forma que se pueda usar en la vida real.

![front](imgs/front.jpg)

![back](imgs/back.jpg)

El dispositivo montado

![final](imgs/final.jpg)



## Dispositivo avanzado

- Instalación en máquina virtual
- Instalación de docker
- Creación de contenedores de:
  - Servicio de grafana
  - Servicio de InfluxDB
  - Servicio de MQTT

//CONTINUAR

*Imagen de portada creada por <a href='https://www.freepik.es/vectores/humo'>Vector de Humo creado por macrovector - www.freepik.es</a>
