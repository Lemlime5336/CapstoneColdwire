//Step 1: INCLUDE LIBRARIES
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>

//STEP 2: PIN DEFINITIONS
#define DHTPIN 4
#define DHTTYPE DHT22
#define MQ135_PIN 34

//STEP 3: WI-FI CREDENTIALS
const char* ssid = "XX"; //// ESP32 supports 2.4 GHz Wi-Fi only (not 5 GHz)
const char* password = "XX";

//STEP 4: HiveMQ CREDENTIALS
const char* mqtt_server = "XX.s1.eu.hivemq.cloud"; //simply paste the url for the cluster here as it is shown in the site
const int mqtt_port = XX;
const char* mqtt_user = "XX";
const char* mqtt_pass = "XX";//paste the password entered for the credential as is
const char* topic = "XX";

//STEP 5: COMMUNICATION SETUP
WiFiClientSecure secureClient;
PubSubClient client(secureClient);
DHT dht(DHTPIN, DHTTYPE);

//STEP 6: SETUP
void setup() {
  //STEP 6.1: START SERIAL MONITOR AND DHT22
  Serial.begin(115200);
  dht.begin();

  //STEP 6.2: WI-FI CONNECTION LOOP
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  //STEP 6.3 MQTT TLS + BROKER SETUP
  secureClient.setInsecure(); // OK for now
  client.setServer(mqtt_server, mqtt_port);
}

//STEP 7: PERSISTENT MQTT CONNECTION
void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to HiveMQ...");
    if (client.connect("M001-IM001", mqtt_user, mqtt_pass)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

//STEP 8: MAIN LOOP
void loop() {
  //STEP 8.1: CHECK MQTT CONNECTION (AND RECONNECT IF NOT)
  if (!client.connected()) reconnect();
  client.loop();

  //STEP 8.2: READING SENSORS
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int air = analogRead(MQ135_PIN);

  //STEP 8.3: ERROR HANDLING
  if (isnan(temp) || isnan(hum)) {
    Serial.println("DHT read failed");
    delay(2000);
    return;
  }
  if (air < 50 || air > 4000) {
    Serial.println("MQ135 sensor abnormal");
  }

  //STEP 8.4: JSON PAYLOAD
  String payload = "{";
  payload += "\"temperature\":" + String(temp) + ",";
  payload += "\"humidity\":" + String(hum) + ",";
  payload += "\"air_quality\":" + String(air);
  payload += "}";

  //STEP 8.5: PUBLISHING TO MQTT
  client.publish(topic, payload.c_str());
  Serial.println(payload);

  delay(5000);
}
