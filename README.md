# CO2 Wheather Sensor
Building a CO2 and wheather station with Wemos D1 mini micro controller, MH Z19B and BM280 sensors

The purpose of this project is to help teachers know when to ventilate the classroom because it is reaching dangerous CO2 levels. By creating this device, teachers will have a reliable tool for measuring the CO2 level of their classes, cheap and easy to build. 
Using simple signals (traffic lights) anyone can know the state of the air, if it is correct, worrying or dangerous. 
This system helps a lot to detect poor air quality and therefore a need to ventilate the classroom or the room where we are.

The second part of the project is to display meassures via dashboard available everywhere of the school even the Internet. By the use of dahsboards, we can have instant meassures of every single class from anywhere in the school, an historical and much more that could help us to improve the quality of air.

---

# Tech


We divide the project into several parts: on the one hand there is the hardware implementation of the device and on the other, the software that supports the extra functionalities that we want to get out of it.

First, the basis; the device itself:

- 1 x Wemos D1 mini
- 1 x MZ H19B CO2 sensor
- 1 x BME280 weather sensor
- 3 x LED
- 1 x PCB

Second, the extrapower; the uses of a dashboard to monitorize it:

- VM (2GB of RAM, 20GB of SSD) with Ubuntu Server 20.10
- Installation of docker as part of the installation of the OS (via snap)
- Registration of a public domain from duckdns.org
- Deployment of the following docker-compose instances: 
    - Reverse-proxy-server implemented with nginx + Public certificate from Letsencrypt 
    - Grafana-InfluxDB-Telegraf (TIG) and Mosquitto (Eclipse)
