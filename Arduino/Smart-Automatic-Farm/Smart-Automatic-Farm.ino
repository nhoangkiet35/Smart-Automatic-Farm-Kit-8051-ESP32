/* ESP32 MQTT + ThingsBoard Smart Automatic Farm Kit 8051-ESP32 */
#include <WiFi.h>
#include <PubSubClient.h> 
#include <ArduinoJson.h>
#include <HardwareSerial.h>

// ========== WiFi & MQTT Config ==========
#define WIFI_SSID          "Tan Tu"
#define WIFI_PASSWORD      "tusmeqc1210"
#define TOKEN              "QuYiIb4aKH0LEo9c6YXW"
#define THINGSBOARD_SERVER "thingsboard.cloud"
#define MQTT_PORT          1883

// ========== Pin Definitions ==========
#define PUMP_PIN           26  // GPIO26 for water pump
#define FAN_PIN            27  // GPIO27 for fan

// ==================== CONFIGURATION ====================
WiFiClient wifiClient;
PubSubClient client(wifiClient);
HardwareSerial SerialUART(2); // UART2: GPIO16=RX, GPIO17=TX

// ========== Sensor Data Structure ==========
struct SensorData {
    int soil_moisture;
    int temperature;
    int humidity;
    int water_level;
    bool pump_status;
    bool fan_status;
} sensorData = {0, 0, 0, 0, false, false};

// ==================== SETUP ====================
void setup()
{
    // Initialize Serial for debugging
    Serial.begin(9600);
    delay(1000);
    Serial.println("\n=== ESP32 ThingsBoard Smart Farm System ===");
    Serial.println("Initializing...");

    // Initialize UART2 for 8051 communication
    SerialUART.begin(9600, SERIAL_8N1, 16, 17);
    Serial.println("✓ UART2 initialized (9600 baud, GPIO16/17)");

    // Initialize motor pins
    pinMode(PUMP_PIN, OUTPUT);
    pinMode(FAN_PIN, OUTPUT);
    digitalWrite(PUMP_PIN, LOW); // Pump OFF initially
    digitalWrite(FAN_PIN, LOW);  // Fan OFF initially
    Serial.println("✓ Motor pins initialized (Pump: GPIO26, Fan: GPIO27)");

    // Initialize WiFi
    InitWiFi();

    // Set MQTT server and callback
    client.setServer(THINGSBOARD_SERVER, MQTT_PORT);
    client.setCallback(callback); // Set callback for RPC

    // System ready
    Serial.println("=== System Ready ===");
    Serial.println("Waiting for data from 8051...");
}

// ==================== MAIN LOOP ====================
void loop()
{
    if(!client.connected()) reconnect();
    client.loop();

    // Process incoming UART data
    processUARTData();

    // Publish sensor data every 30 seconds
    static unsigned long lastPublish = 0;
    if (millis() - lastPublish > 30000) {
        lastPublish = millis();
        publishSensorData();
    }

    delay(100); // Keep system responsive
}

// ==================== WIFI FUNCTIONS ====================
void InitWiFi()
{
    Serial.print("Connecting to WiFi: ");
    Serial.println(WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n✓ WiFi connected successfully!");
    } else {
        Serial.println("\n❌ WiFi connection failed!");
        Serial.println("Please check SSID and password");
    }
}

// ========== MQTT Reconnect ==========
void reconnect()
{
    while (!client.connected()) {
        Serial.print("Connecting to ThingsBoard...");
        if (client.connect("Smart Farm ESP32", TOKEN, NULL)) {
            Serial.println("[DONE]");
            client.subscribe("v1/devices/me/rpc/request/+"); // Subscribe to RPC
        } else {
            Serial.print("[FAILED], rc=");
            Serial.print(client.state());
            Serial.println(" retry in 5s");
            delay(5000);
        }
    }
}

// ==================== UART DATA PROCESSING ====================
void processUARTData()
{
    if (SerialUART.available()) {
        String receivedData = SerialUART.readString();
        receivedData.trim();
        Serial.println("📡 Received from 8051: " + receivedData);
        if (receivedData.length() > 0) {
            // Parse sensor data: "soil,temp,hum,water"
            if (parseSensorData(receivedData)) {
                printSensorData();
                // Data will be published in main loop
            }
        }
    }
}

bool parseSensorData(String data)
{
    // Expected format: "soil,temp,hum,water"
    int commaIndex1 = data.indexOf(',');
    int commaIndex2 = data.indexOf(',', commaIndex1 + 1);
    int commaIndex3 = data.indexOf(',', commaIndex2 + 1);
    
    if (commaIndex1 == -1 || commaIndex2 == -1 || commaIndex3 == -1) {
        Serial.println("❌ Invalid data format received");
        return false;
    }
    
    sensorData.soil_moisture = data.substring(0, commaIndex1).toInt();
    sensorData.temperature = data.substring(commaIndex1 + 1, commaIndex2).toInt();
    sensorData.humidity = data.substring(commaIndex2 + 1, commaIndex3).toInt();
    sensorData.water_level = data.substring(commaIndex3 + 1).toInt();
    
    return true;
}

void publishSensorData()
{
    if (!client.connected()) return;
    
    // Create JSON payload for ThingsBoard
    StaticJsonDocument<200> telemetry;
    telemetry["soilMoisture"] = sensorData.soil_moisture;
    telemetry["temperature"] = sensorData.temperature;
    telemetry["humidity"] = sensorData.humidity;
    telemetry["waterLevel"] = sensorData.water_level;
    telemetry["pumpStatus"] = sensorData.pump_status;
    telemetry["fanStatus"] = sensorData.fan_status;
    
    char buffer[200];
    serializeJson(telemetry, buffer);
    
    if (client.publish("v1/devices/me/telemetry", buffer)) {
        Serial.println("✓ Sensor data published to ThingsBoard");
    } else {
        Serial.println("❌ Failed to publish sensor data");
    }
}

void printSensorData()
{
    Serial.println("=== SENSOR DATA ===");
    Serial.println("  🌱 Soil Moisture: " + String(sensorData.soil_moisture) + "%");
    Serial.println("  🌡️ Temperature: " + String(sensorData.temperature) + "°C");
    Serial.println("  💧 Humidity: " + String(sensorData.humidity) + "%");
    Serial.println("  🚰 Water Level: " + String(sensorData.water_level)+ "%");
    Serial.println("  🚰 Pump: " + String(sensorData.pump_status ? "ON" : "OFF"));
    Serial.println("  🌀 Fan: " + String(sensorData.fan_status ? "ON" : "OFF"));
    Serial.println("==================");
}

// ========== Handle Incoming RPC ==========
void callback(char* topic, byte* payload, unsigned int length)
{
    Serial.println("📩 RPC Received on topic: " + String(topic));
    
    // Convert payload to String
    String payloadStr;
    for (unsigned int i = 0; i < length; i++) {
        payloadStr += (char)payload[i];
    }

    // Parse JSON payload
    DynamicJsonDocument doc(256);
    DeserializationError error = deserializeJson(doc, payloadStr);
    if (error) {
        Serial.println("❌ Failed to parse RPC payload");
        return;
    }

    // Extract the "params" value and "method"
    bool state = doc["params"];
    String method = doc["method"];

    if (method == "setPump") {
        sensorData.pump_status = state;
        digitalWrite(PUMP_PIN, state ? HIGH : LOW);
        Serial.println("💧 Pump set to: " + String(state ? "ON" : "OFF"));
        
        // Send command to 8051 via UART
        SerialUART.println("PUMP:" + String(state ? "ON" : "OFF"));
        
        // Publish updated status to ThingsBoard
        if (digitalRead(PUMP_PIN) == HIGH) {
            client.publish("v1/devices/me/attributes", "{\"pumpStatus\": \"true\"}");
        } else {
            client.publish("v1/devices/me/attributes", "{\"pumpStatus\": \"false\"}");
        }
    }
    else if (method == "setFan") {
        sensorData.fan_status = state;
        digitalWrite(FAN_PIN, state ? HIGH : LOW);
        Serial.println("🌀 Fan set to: " + String(state ? "ON" : "OFF"));
        
        // Send command to 8051 via UART
        SerialUART.println("FAN:" + String(state ? "ON" : "OFF"));
        
        // Publish updated status to ThingsBoard
        if (digitalRead(FAN_PIN) == HIGH) {
            client.publish("v1/devices/me/attributes", "{\"fanStatus\": \"true\"}");
        } else {
            client.publish("v1/devices/me/attributes", "{\"fanStatus\": \"false\"}");
        }
    }
}