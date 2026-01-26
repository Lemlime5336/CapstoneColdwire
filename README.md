#1. Introduction
The project focuses on creating a IoT & Blockchain-Based Halal Cold Chain Monitoring System </br>
In essence:
1. ESP32 reads sensors (DHT22 + MQ135)
2. It publishes data via MQTT
  - HiveMQ Cloud is the MQTT broker
3. Node.js subscribes to MQTT and saves data
4. MongoDB Atlas stores the sensor data
