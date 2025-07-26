/* ESP32 MQTT + ThingsBoard Smart Automatic Farm Kit 8051-ESP32 */
#include <WiFi.h>
#include <PubSubClient.h>  // Missing include
#include <ArduinoJson.h>
#include <HardwareSerial.h>

// ========== WiFi & MQTT Config ==========
#define WIFI_SSID          "Tan Tu"
#define WIFI_PASSWORD      "tusmeqc1210"
#define TOKEN              "QuYiIb4aKH0LEo9c6YXW"
#define THINGSBOARD_SERVER "thingsboard.cloud"
#define MQTT_PORT          1883

// ==================== CONFIGURATION ====================
WiFiClient wifiClient;
PubSubClient client(wifiClient);
HardwareSerial SerialUART(2); // UART2: GPIO16=RX, GPIO17=TX

// ========== GPIO for controlling Pump & Fan ==========
#define PUMP_PIN 4
#define FAN_PIN  5

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
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n=== ESP32 ThingsBoard Smart Farm System ===");
    Serial.println("Initializing...");

    // Initialize UART2 for 8051 communication
    SerialUART.begin(9600, SERIAL_8N1, 16, 17);
    Serial.println("✓ UART2 initialized (9600 baud, GPIO16/17)");

    // Setup GPIO output
    pinMode(PUMP_PIN, OUTPUT);
    pinMode(FAN_PIN, OUTPUT);
    digitalWrite(PUMP_PIN, LOW); // OFF by default
    digitalWrite(FAN_PIN, LOW);  // OFF by default

    // Initialize WiFi
    InitWiFi();

    client.setServer(THINGSBOARD_SERVER, MQTT_PORT);
    client.setCallback(callback);

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
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        Serial.print("Signal strength: ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
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
    
    // Update pump and fan status based on GPIO states
    sensorData.pump_status = digitalRead(PUMP_PIN);
    sensorData.fan_status = digitalRead(FAN_PIN);
    
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
    telemetry["wifiRSSI"] = WiFi.RSSI();
    telemetry["freeHeap"] = ESP.getFreeHeap();
    
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
    Serial.println("  🚰 Water Level: " + String(sensorData.water_level));
    Serial.println("  🚰 Pump: " + String(sensorData.pump_status ? "ON" : "OFF"));
    Serial.println("  🌀 Fan: " + String(sensorData.fan_status ? "ON" : "OFF"));
    Serial.println("==================");
}

// ========== Handle Incoming RPC ==========
void callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("RPC received [");
    Serial.print(topic);
    Serial.print("]: ");

    payload[length] = '\0'; // Ensure string end
    Serial.println((char *)payload);

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
        Serial.print("JSON parse error: ");
        Serial.println(error.c_str());
        return;
    }

    const char *method = doc["method"];
    bool param = doc["params"];

    if (strcmp(method, "setPump") == 0) {
        digitalWrite(PUMP_PIN, param ? HIGH : LOW);
        sensorData.pump_status = param;
        Serial.print("Pump turned ");
        Serial.println(param ? "ON" : "OFF");
        
        // Send confirmation back to ThingsBoard
        sendRPCResponse(topic, method, param);
    } else if (strcmp(method, "setFan") == 0) {
        digitalWrite(FAN_PIN, param ? HIGH : LOW);
        sensorData.fan_status = param;
        Serial.print("Fan turned ");
        Serial.println(param ? "ON" : "OFF");
        
        // Send confirmation back to ThingsBoard
        sendRPCResponse(topic, method, param);
    } else {
        Serial.println("Unknown RPC method: " + String(method));
    }
}

void sendRPCResponse(char* topic, const char* method, bool status)
{
    // Extract request ID from topic
    String topicStr = String(topic);
    int requestIdStart = topicStr.lastIndexOf('/') + 1;
    String requestId = topicStr.substring(requestIdStart);
    
    // Create response topic
    String responseTopic = "v1/devices/me/rpc/response/" + requestId;
    
    // Create response payload
    StaticJsonDocument<100> response;
    response["method"] = method;
    response["status"] = status ? "success" : "failed";
    
    char buffer[100];
    serializeJson(response, buffer);
    
    client.publish(responseTopic.c_str(), buffer);
}


