require('dotenv').config();

// index.js
const mqtt = require('mqtt');
const { MongoClient } = require('mongodb');

// Configuration
const IMID = "IM001";      // IoT Module ID
const MANU_ID = "M001";    // Manufacturer ID

// HiveMQ Cloud
const mqttClient = mqtt.connect(process.env.MQTT_HOST, {
  username: process.env.MQTT_USER,
  password: process.env.MQTT_PASS
});

const topic = 'coldwire/M001/IM001/environment';

// MongoDB
const client = new MongoClient(process.env.MONGO_URL);

async function start() {
  try {
    await client.connect();
    console.log('Connected to MongoDB');

    const db = client.db('coldwire');
    const collection = db.collection('environmental_logs');

    mqttClient.on('connect', () => {
      console.log('Connected to HiveMQ Cloud');
      mqttClient.subscribe(topic);
    });

    mqttClient.on('message', async (topic, message) => {
      const payload = JSON.parse(message.toString());

      const log = {
        imId: IMID,
        manuId: MANU_ID,
        temperature: payload.temperature,
        humidity: payload.humidity,
        gas: payload.air_quality,
        timestamp: new Date()
      };

      await collection.insertOne(log);
      console.log('Saved log:', log);
    });

  } catch (err) {
    console.error('Startup error:', err);
  }
}

start();
