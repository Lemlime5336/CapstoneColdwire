#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// Pins
#define SS_PIN 5
#define RST_PIN 22

MFRC522 rfid(SS_PIN, RST_PIN);

// WiFi + MQTT setup
const char* ssid = "XX";
const char* password = "XX";

const char* mqtt_server = "XX.s1.eu.hivemq.cloud"; //simply paste the url for the cluster here as it is shown in the site
const int mqtt_port = XX;
const char* mqtt_user = "XX";
const char* mqtt_pass = "XX";//paste the password entered for the credential as is
const char* topic = "coldwire/M001/IM001/delivery";
WiFiClientSecure secureClient;
PubSubClient client(secureClient);

// Batch tracking
struct Batch {
  String rfid_tag;
  String status;       // "loading" or "unloading"
  unsigned long lastScan; // millis() of last scan
};

Batch currentBatches[20];   // max 20 batches at once
int batchCount = 0;
const unsigned long scanTimeout = 30000; // 30-second timeout

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP: "); Serial.println(WiFi.localIP());

  secureClient.setInsecure();
  client.setServer(mqtt_server, mqtt_port);
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32Client", mqtt_user, mqtt_pass)) {
      Serial.println("Connected to HiveMQ");
    } else {
      delay(2000);
    }
  }
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String rfid_tag = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      if (rfid.uid.uidByte[i] < 16) rfid_tag += "0"; // pad single-digit hex
      rfid_tag += String(rfid.uid.uidByte[i], HEX);
    }
    rfid_tag.toUpperCase(); // normalize

    processRFID(rfid_tag);

    rfid.PICC_HaltA();
  }
}

void processRFID(String rfid_tag) {
  unsigned long now = millis();

  // Check if tag already exists
  for (int i = 0; i < batchCount; i++) {
    if (currentBatches[i].rfid_tag == rfid_tag) {
      // If enough time has passed
      if (now - currentBatches[i].lastScan >= scanTimeout) {
        // Toggle status
        currentBatches[i].status = (currentBatches[i].status == "loading") ? "unloading" : "loading";
        currentBatches[i].lastScan = now;
        publishStatus(currentBatches[i].rfid_tag, currentBatches[i].status);
      } else {
        Serial.println("Duplicate scan ignored (timeout)");
      }
      return; // done
    }
  }

  // New tag → loading
  if (batchCount < 10) {
    currentBatches[batchCount].rfid_tag = rfid_tag;
    currentBatches[batchCount].status = "loading";
    currentBatches[batchCount].lastScan = now;
    batchCount++;

    publishStatus(rfid_tag, "loading");
  } else {
    Serial.println("Batch table full!");
  }
}

void publishStatus(String tag, String status) {
  String payload = "{";
  payload += "\"rfid_tag\":\"" + tag + "\",";
  payload += "\"batch_id\":\"BATCH123\",";
  payload += "\"status\":\"" + status + "\"";
  payload += "}";

  client.publish(topic, payload.c_str());
  Serial.println("Payload sent: " + payload);
}
