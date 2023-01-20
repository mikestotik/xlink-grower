#include <Arduino.h>
#include <WiFi.h>
#include <AsyncMqttClient.h>
#include <iostream>
#include <vector>
#include "Grower.h"
#include "sensors/SystemSensorHandler.h"
#include "sensors/LightSensorHandler.h"
#include "sensors/AirSensorHandler.h"
#include "sensors/SoilSensorHandler.h"
#include "sensors/Sensor.h"
#include "controls/Control.h"


#define GPIO_CONTROL_SOCKET_1 4
#define GPIO_CONTROL_SOCKET_2 16
#define GPIO_CONTROL_SOCKET_3 17
#define GPIO_CONTROL_SOCKET_4 5
#define GPIO_CONTROL_SOCKET_5 18
#define GPIO_CONTROL_SOCKET_6 19

#define GPIO_SENSOR_SOIL_HUMIDITY_1 36

#define MAIN_TOPIC_SYSTEM "system"
#define SYSTEM_TOPIC_RESTART "restart"
#define SYSTEM_TOPIC_INFO "info"
#define SYSTEM_TOPIC_MONITOR "monitor"

#define MAIN_TOPIC_SENSOR "sensor"

#define MAIN_TOPIC_CONTROL "control"
// for sync control state
#define CONTROL_TOPIC_STATE "state"


enum class Topic {
    Info,
    Sensor,
    Control,
};

enum class InfoType {
    System,
    Controls,
    Sensors,
};

enum class SystemInfoType {
    Setup,
};


enum class SystemSensorID {
    State,
};

const char *wifiSsid = "MW45V_5171";
const char *wifiPassword = "00648602";

const char *mqttHost = "192.168.1.164";
const int mqttPort = 1883;
const char *mqttUser = "xlink";
const char *mqttPass = "xlink";

const long sensorInterval = 1000;
unsigned long previousMillis = 0;

AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;


Grower *device;


void connectToWifi() {
    Serial.println("Connecting to Wi-Fi...");
    WiFi.begin(wifiSsid, wifiPassword);
}


void connectToMqtt() {
    Serial.println("Connecting to MQTT...");
    mqttClient.connect();
}


void WiFiEvent(WiFiEvent_t event) {
    Serial.printf("[WiFi-event] event: %d\n", event);

    switch (event) {
        case SYSTEM_EVENT_STA_GOT_IP:
            Serial.println("WiFi connected");
            Serial.println("IP address: ");
            Serial.println(WiFi.localIP());
            connectToMqtt();
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            Serial.println("WiFi lost connection");
            xTimerStop(mqttReconnectTimer, 0);
            xTimerStart(wifiReconnectTimer, 0);
            break;
    }
}


void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
    Serial.println("Disconnected from MQTT.");
    Serial.print("Reason: ");
    Serial.println((uint8_t) reason);

    if (WiFi.isConnected()) {
        xTimerStart(mqttReconnectTimer, 0);
    }
}


void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
//    Serial.println("Subscribe acknowledged.");
//    Serial.print("  packetId: ");
//    Serial.println(packetId);
//    Serial.print("  qos: ");
//    Serial.println(qos);
}


void onMqttUnsubscribe(uint16_t packetId) {
    Serial.println("Unsubscribe acknowledged.");
    Serial.print("  packetId: ");
    Serial.println(packetId);
}


void onMqttPublish(uint16_t packetId) {
//    Serial.println("Publish acknowledged.");
//    Serial.print("  packetId: ");
//    Serial.println(packetId);
}


String buildTopic(Topic topic, short type, short id) {
    return (device->getId() + "/" + (short) topic + "/" + type + "/" + id);
}


void onMqttConnect(bool sessionPresent) {
    Serial.println("Connected to MQTT.");

    Serial.print("Session present: ");
    Serial.println(sessionPresent);

    // Subscribe to restart
    String controlSystemRestartTopic =
        buildTopic(Topic::Control, (short) ControlType::System, (short) SystemControlType::Restart);
    mqttClient.subscribe(controlSystemRestartTopic.c_str(), 0);

    // Subscribe to Controls
    for (Control control: device->getControls()) {
        String controlTopic = buildTopic(Topic::Control, (short) control.getType(), control.getId());
        mqttClient.subscribe(controlTopic.c_str(), 0);
    }

    // Publish system setup
    String systemSetupInfoTopic =
        buildTopic(Topic::Info, (short) InfoType::System, (short) SystemInfoType::Setup);
    mqttClient.publish(systemSetupInfoTopic.c_str(), 0, false, Grower::getSystemInfo().c_str());
}


int len(std::string str) {
    int length = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        length++;
    }
    return length;
}


std::vector<std::string> split(std::string topic, char sep) {
    std::vector<std::string> list;

    int currIndex = 0, startIndex = 0, endIndex, i = 0;

    while (i <= len(topic)) {
        if (topic[i] == sep || i == len(topic)) {
            endIndex = i;
            std::string subStr;
            subStr.append(topic, startIndex, endIndex - startIndex);
            list.push_back(subStr);
            currIndex += 1;
            startIndex = endIndex + 1;
        }
        i++;
    }
    return list;
}


void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index,
                   size_t total) {
    String message;
    for (int i = 0; i < len; i++) {
        message += (char) payload[i];
    }
    Serial.print(message);

    Serial.print("topic: ");
    Serial.println(topic);
    Serial.print("  message: ");
    Serial.println(message);

    std::vector<std::string> sTopic = split(topic, '/');

    if ((Topic) std::stoi(sTopic[1]) == Topic::Control) {
        auto controlType = (ControlType) std::stoi(sTopic[2]);
        int controlId = std::stoi(sTopic[3]);
        int controlValue = std::stoi(message.c_str());

        Control control = device->getControl(controlType, controlId);
        control.setValue(controlValue);
    }
}


void setup() {
    Serial.begin(115200);


    device = new Grower(
        String(ESP.getEfuseMac()),
        {
            *new Control(0, ControlType::Socket, GPIO_CONTROL_SOCKET_1),
            *new Control(1, ControlType::Socket, GPIO_CONTROL_SOCKET_2),
            *new Control(2, ControlType::Socket, GPIO_CONTROL_SOCKET_3),
            *new Control(3, ControlType::Socket, GPIO_CONTROL_SOCKET_4),
            *new Control(4, ControlType::Socket, GPIO_CONTROL_SOCKET_5),
            *new Control(5, ControlType::Socket, GPIO_CONTROL_SOCKET_6)
        },
        {
            *new Sensor(0, SensorType::System, new SystemSensorHandler()),
            *new Sensor(0, SensorType::Light, new LightSensorHandler()),
            *new Sensor(0, SensorType::Air, new AirSensorHandler()),
            *new Sensor(0, SensorType::Soil, new SoilSensorHandler(GPIO_SENSOR_SOIL_HUMIDITY_1))
        }
    );

    mqttReconnectTimer = xTimerCreate(
        "mqttTimer",
        pdMS_TO_TICKS(2000),
        pdFALSE,
        (void *) nullptr,
        reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt)
    );
    wifiReconnectTimer = xTimerCreate(
        "wifiTimer",
        pdMS_TO_TICKS(2000),
        pdFALSE,
        (void *) nullptr,
        reinterpret_cast<TimerCallbackFunction_t>(connectToWifi)
    );

    WiFi.onEvent(WiFiEvent);

    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onSubscribe(onMqttSubscribe);
    mqttClient.onUnsubscribe(onMqttUnsubscribe);
    mqttClient.onMessage(onMqttMessage);
    mqttClient.onPublish(onMqttPublish);

    mqttClient.setClientId(device->getId().c_str());
    mqttClient.setCredentials(mqttUser, mqttPass);
    mqttClient.setServer(mqttHost, mqttPort);

    connectToWifi();
}


void loop() {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= sensorInterval) {
        previousMillis = currentMillis;

        for (Sensor sensor: device->getSensors()) {
            if (sensor.getStatus()) {
                String topic = buildTopic(Topic::Sensor, (short) sensor.getType(), (short) sensor.getId());
                String payload = sensor.getData();
                mqttClient.publish(topic.c_str(), 0, false, payload.c_str());
            }
        }
    }
}
