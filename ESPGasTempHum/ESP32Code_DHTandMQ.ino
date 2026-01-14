#include <MQ135.h>
#include <DHT.h>

// Pins
#define DHTPIN 4
#define DHTTYPE DHT22
#define MQ135_PIN 34

DHT dht(DHTPIN, DHTTYPE);
MQ135 mq135_sensor(MQ135_PIN);

unsigned long lastRead = 0;
const unsigned long interval = 2000;

void setup() {
  Serial.begin(9600);
  dht.begin();

  Serial.println("MQ135 warming up...");
  delay(20000);
  Serial.println("DHT22 & MQ135 Ready");
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastRead >= interval) {
    lastRead = currentMillis;

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    float ppm = mq135_sensor.getPPM();

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("DHT22 Read Error");
      return;
    }

    Serial.print("Temp: ");
    Serial.print(temperature, 2);
    Serial.print(" °C | Humidity: ");
    Serial.print(humidity, 2);
    Serial.print(" % | Air Quality: ");
    Serial.print(ppm, 2);
    Serial.println(" PPM");
  }
}
