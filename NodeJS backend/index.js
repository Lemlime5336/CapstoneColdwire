// index.js
const mqtt = require('mqtt');
const { MongoClient } = require('mongodb');

// Configuration
const IMID = "IM001";      // IoT Module ID
const MANU_ID = "M001";    // Manufacturer ID

// HiveMQ Cloud
const mqttClient = mqtt.connect('mqtts://befc55ef7cb24ed58794c4fbb11bfcb8.s1.eu.hivemq.cloud:8883', {
  username: 'esp32_user',
  password: 'xuszyv-3wuzka-kUppes'
});

const topic = 'esp32/sensors/data';

// MongoDB
const mongoUrl = 'mongodb+srv://ColdwireUser:dAbcyd-9bitwu-memvar@coldwirecluster.vwopuvw.mongodb.net/?appName=ColdwireCluster';
const client = new MongoClient(mongoUrl);

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
