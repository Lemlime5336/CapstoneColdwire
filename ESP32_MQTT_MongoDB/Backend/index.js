require('dotenv').config();
const mqtt = require('mqtt');
const { MongoClient } = require('mongodb');

// ===== CONFIGURATION =====
const IMID = "IM001";
const MANU_ID = "M001";

// Current delivery ID hardcoded
const currentDeliveryId = "DEL-0260211-0001";

// MQTT
const topicEnvironment = 'coldwire/M001/IM001/sensors';
const topicBDE = 'coldwire/M001/IM001/batch_delivery_events';

const mqttClient = mqtt.connect(`mqtts://${process.env.MQTT_HOST}`, {
  username: process.env.MQTT_USER,
  password: process.env.MQTT_PASS
});

// MongoDB
const client = new MongoClient(process.env.MONGO_URL);

// ===== HELPER to convert timestamps =====
function parseArduinoTimestamp(ts) {
  // Convert "YYYY-MM-DD HH:MM:SS" -> "YYYY-MM-DDTHH:MM:SSZ" (ISO8601 UTC)
  return new Date(ts.replace(" ", "T") + "Z");
}

// ===== MAIN FUNCTION =====
async function start() {
  try {
    await client.connect();
    console.log("Connected to MongoDB");

    const db = client.db('coldwire');
    const envCollection = db.collection('environmental_logs');
    const bdeCollection = db.collection('batch_delivery_events');

    mqttClient.on('connect', () => {
      console.log("Connected to HiveMQ Cloud");
      mqttClient.subscribe(topicEnvironment);
      mqttClient.subscribe(topicBDE);
      console.log("Subscribed to environmental logs topic");
      console.log("Subscribed to batch delivery events topic");
    });

    mqttClient.on('message', async (topic, message) => {
      try {
        console.log("MQTT message received:");
        console.log("Topic:", topic);
        console.log("Payload:", message.toString());

        const payload = JSON.parse(message.toString());

        // ----- ENVIRONMENTAL LOG -----
        if (topic === topicEnvironment) {
          const envLog = {
            DeliveryID: currentDeliveryId,
            EIMID: IMID,
            ManuID: MANU_ID,
            Temperature: payload.temperature,
            Humidity: payload.humidity,
            Gas: payload.air_quality,
            Timestamp: parseArduinoTimestamp(payload.timestamp)
          };

          await envCollection.insertOne(envLog);
          console.log("Saved environmental log:", envLog);
        }

        // ----- BATCH DELIVERY EVENT -----
        if (topic === topicBDE) {
          const bdeLog = {
            DeliveryID: currentDeliveryId,
            ManuID: MANU_ID,
            RFIDTag: payload.rfid_tag,
            BatchId: payload.batch_id,
            Status: payload.status,
            Timestamp: parseArduinoTimestamp(payload.timestamp)
          };

          await bdeCollection.insertOne(bdeLog);
          console.log("Saved batch delivery event:", bdeLog);
        }

      } catch (err) {
        console.error("Message processing error:", err);
      }
    });

  } catch (err) {
    console.error("Startup error:", err);
  }
}

start();
