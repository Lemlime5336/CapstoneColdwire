require('dotenv').config();
const mqtt = require('mqtt');
const { MongoClient } = require('mongodb');

// ===== CONFIGURATION =====
const IMID = "IM001";
const MANU_ID = "M001";

// Current delivery ID (should be created in web app for each new delivery)
const currentDeliveryId = "DEL-0260211-0001";

// MQTT topics
const topicEnvironment = 'coldwire/M001/IM001/environmental_logs';
const topicBDE = 'coldwire/M001/IM001/batch_delivery_events';

// MQTT client
const mqttClient = mqtt.connect(`mqtts://${process.env.MQTT_HOST}`, {
  username: process.env.MQTT_USER,
  password: process.env.MQTT_PASS
});

// MongoDB client
const client = new MongoClient(process.env.MONGO_URL);

// ===== HELPER =====
function parseArduinoTimestamp(ts) {
  // Convert "YYYY-MM-DD HH:MM:SS" -> "YYYY-MM-DDTHH:MM:SSZ" (ISO8601 UTC)
  return new Date(ts.replace(" ", "T") + "Z");
}

// ===== MAIN FUNCTION =====
async function start() {
  try {
    // Connect to MongoDB
    await client.connect();
    console.log("Connected to MongoDB");

    const db = client.db('coldwire');
    const envCollection = db.collection('environmental_logs');
    const bdeCollection = db.collection('batch_delivery_events');

    // MQTT connect
    mqttClient.on('connect', () => {
      console.log("Connected to HiveMQ Cloud");
      mqttClient.subscribe(topicEnvironment, (err) => {
        if (!err) console.log("Subscribed to environmental logs topic");
      });
      mqttClient.subscribe(topicBDE, (err) => {
        if (!err) console.log("Subscribed to batch delivery events topic");
      });
    });

    // MQTT message handler
    mqttClient.on('message', async (topic, message) => {
      try {
        console.log("\nMQTT message received:");
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
            Latitude: payload.latitude ?? null,   // GPS latitude
            Longitude: payload.longitude ?? null, // GPS longitude
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

// Start the backend
start();
