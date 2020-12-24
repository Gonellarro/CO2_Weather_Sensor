
const char mqtt_server[] = "192.168.1.5";   // MQTT Server IP address
const char mqtt_user[] = "";                // if exist
const char mqtt_password[] = "";            // idem

String device_name = "habitacio";

String temperature_topic = "sensors/" + device_name + "/temperature";  //Topic temperature
String humidity_topic = "sensors/" + device_name + "/humidity";        //Topic humidity
String co2_topic = "sensors/" + device_name + "/co2";                  //Topic CO2
