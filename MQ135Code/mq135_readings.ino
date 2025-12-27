#include <MQ135.h>

#define MQ135_PIN A0

MQ135 mq135_sensor(MQ135_PIN);

unsigned long lastRead = 0;
const unsigned long interval = 2000; // 2 seconds

void setup() {
  Serial.begin(9600);
  Serial.println("MQ135 warming up...");
  delay(20000); // sensor warm-up 
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastRead >= interval) {
    lastRead = currentMillis;

    float ppm = mq135_sensor.getPPM();

    Serial.print("Air Quality (PPM): ");
    Serial.println(ppm);
  }
}
