These steps work as of 15/01/2025.
# 1. Steps to run:
1. Ensure HiveMQ cluster is running
2. Ensure MongoDB cluster is active
3. Connect ESP32 with all components properly integrated to the power source
   - verify the esp32 is connected to wifi using serial monitor
   - also verify all sensors are properly integrated(by checking whether readings are shown in the serial monitor)
4. In terminal of choice, run the command: node index.js



# 2. Arduino Components and Integration

# 3. HiveMQ Setup
1. Create an account
2. Create a "Serverless Cluster" (the free option)
3. Click on "Manage Cluster"
4. Go to "Access Management"
5. Click on "Add Credentials"
  - Enter a username of choice
  - Enter a password of choice (save this somewhere as it will be required later).
  - Confirm password
  - Set permission to "Publish and Subscribe"
  - Make sure to save the credentials 
6. Click on "Web Client" and enter the previously saved username and password. (this is to verify that HiveMQ is recieving sensor data)
  - Click on "Connect"
  - Click on "Subscribe to all messages"
7. All necessary information for the arduino code can be found in the "Overview" tab, including:
  - mqtt_server  (paste the url as is, within "")
  - mqtt_port
  - mqtt_user (the username set in step 5)
  - mqtt_pass (the password set in step 5)


# 4. MongoDB Setup
1. Create an account
2. Create a project (this should be the name of the database)
3. Do not add members for the time being
4. Create a cluster (in this case project+cluster)
  - Select the free clsyter
  - Make NO other changes (use default provider and region settings)
  - Click on "Create Deployment"
5. You should see the "Connect to {clustename}" box now
  - Click on "Allow access from anywhere" -for testing purposes.
  - Click on "Add your current IP address" - so you can connect to MongoDB yourself.
  - Enter a username and password (save it somewhere as it will be required later)
  - Click on "Create Database User"
  - Click on "Choose a Connection Method"
  - Click on "Drivers" under "Connect to your application"
    - follow the installation instructions, if not already installed (in your terminal enter: npm install mongodb)
    - make sure to save the connection string (will be needed later, save securely)
  - Save settings
6. Go to "Database and Network Access"
7. Click on "Add new Database user" (this is to craete a user wihtout admin priveleges to read and write the iot data)
  - Enter a username of choice
  - Enter a password of choice
  - Under "Built-in Role", select "Read and write to any database"
  - Click on "Add User"
