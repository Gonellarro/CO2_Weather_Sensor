# CO2_Wheather_Sensor
Building a CO2 and wheather station with Wemos D1 mini micro controller, MH Z19B and BM280 sensors

The aim of this project is to build a CO2 and weather station who grab meassures, send them through MQTT protocol to a MQTT server, and display them by means of a grafana dashboard. To reach this objective we have planned to build a device with Wemos D1 mini microcontroller attached to a MHZ19B CO2 sensor and BME280 temperature, preassure and humidity meassures. 

In the way to achieve the full project we dispose:

1. One VM (Core i3 7300 + 2GB of RAM, 50GB of SSD) with Ubuntu Server 20.10
2. Installation of docker as part of the installation of the OS (via snap)
3. Registration of a public domain from duckdns.org
4. Deployment of the following docker-compose instances:
  4.1. Reverse-proxy-server implemented with nginx + Public certificate from Letsencrypt
  4.2. Grafana-InfluxDB-Telegraf (TIG) and Mosquitto (Eclipse)

  
