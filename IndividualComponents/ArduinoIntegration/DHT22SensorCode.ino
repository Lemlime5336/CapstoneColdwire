#include <DHT.h>

#define DHTPIN 2       // Digital pin connection

DHT dht(DHTPIN, DHT22);

unsigned long lastRead = 0;
const unsigned long interval = 2000; // 2 seconds

void setup() {
  Serial.begin(9600);
  dht.begin();
  Serial.println("DHT22 Sensor Ready");
}

void loop() {
  if (millis() - lastRead >= interval) {
    lastRead = millis();

    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature(); // Celsius

    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT22!");
      return;
    }

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C  |  Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  }
}
