#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

#include "Commons.h"
#include "RequestUtils.hpp"
#include "messages.pb.h"

#define RX_PIN 2
#define TX_PIN 15

void OnRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    debugln("++++++++++++++++++++++++++++++++++++++++++++++++++");
    printMacAndLenPacketReceived(mac, len);
    Serial2.write(incomingData, len);
    Serial2.write(END_TX_CHAR);
    debugln("--------------------------------------------------");
}

void serialDataHandler(const uint8_t *incomingData, int len) {
    debugln("++++++++++++++++++++++++++++++++++++++++++++++++++");
    response deserializedResponse = response_init_zero;
    deserializeResponse(&deserializedResponse, incomingData, len);
    printResponse(&deserializedResponse, len);
    debugln("--------------------------------------------------");
}

void readFromSerial() {
    if (Serial2.available()) {
        uint8_t buffer[ESPNOW_BUFFERSIZE];
        int bytesRead = Serial2.readBytesUntil(END_TX_CHAR, buffer, ESPNOW_BUFFERSIZE);
        if (bytesRead > 0) {
            debug("Bytes read from Serial2: ");
            debugln(bytesRead);
            serialDataHandler(buffer, bytesRead);
        }
    }
}

void setupWiFi() {
    WiFi.mode(WIFI_MODE_STA);
    debugln("Gateway mac: " + WiFi.macAddress());
}

void setupEspNow() {
    if (esp_now_init() != ESP_OK) {
        debugln("There was an error initializing ESP-NOW");
        return;
    }
    esp_now_register_recv_cb(OnRecv);
}

void setup() {
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);

    setupWiFi();
    setupEspNow();
}

void loop() {
    readFromSerial();
    delay(500);
}
