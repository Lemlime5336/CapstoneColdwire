#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT22
#define MQ135_PIN 34

// WiFi credentials
const char* ssid = "YOUR_WIFI"; //make sure to connect to 4G, ESP32 cannot connect to 5G
const char* password = "YOUR_WIFI_PASSWORD";

// HiveMQ Cloud credentials
const char* mqtt_server = "YOUR_HIVEMQ_URL"; //simply paste the url for the cluster here as it is shown in the site
const int mqtt_port = 8883;
const char* mqtt_user = "esp32_user";
const char* mqtt_pass = "YOUR_HIVEMQ_PASSWORD"; //paste the password entered for the credential as is
const char* topic = "esp32/sensors/data";

WiFiClientSecure secureClient;
PubSubClient client(secureClient);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  secureClient.setInsecure(); 
  client.setServer(mqtt_server, mqtt_port);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to HiveMQ...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_pass)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int air = analogRead(MQ135_PIN);

  if (isnan(temp) || isnan(hum)) {
    Serial.println("DHT read failed");
    delay(2000);
    return;
  }

  String payload = "{";
  payload += "\"temperature\":" + String(temp) + ",";
  payload += "\"humidity\":" + String(hum) + ",";
  payload += "\"air_quality\":" + String(air);
  payload += "}";

  client.publish(topic, payload.c_str());
  Serial.println(payload);

  delay(5000);
}
