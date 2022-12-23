#include <Arduino.h>

#include "Commons.hpp"
#include "EspNowToSerialManager.hpp"
#include "MqttConfig.hpp"
#include "Secrets.h"
#include "SerialToMqttManager.hpp"
#include "WifiConfig.hpp"

#define RX_PIN 16
#define TX_PIN 17

MqttConfig mqttConfig(MQTT_SERVER, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD);
WiFiConfig wiFiConfig(staticIP, gateway, subnet, SSID, WIFI_PASSWORD);

SerialToMqttManager serialToMqttManager(wiFiConfig, mqttConfig);

void setup() {
    Serial.begin(BAUD_RATE);
    Serial2.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);

    serialToMqttManager.setup();
}

void loop() {
    serialToMqttManager.loop();
    delay(500);
}