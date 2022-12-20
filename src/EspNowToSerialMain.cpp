#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

#include "Commons.h"
#include "RequestUtils.hpp"
#include "UartHandler.hpp"
#include "messages.pb.h"

#define RX_PIN 2
#define TX_PIN 15

void OnRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    debugln("++++++++++++++++++++++++++++++++++++++++++++++++++");
    printMacAndLenPacketReceived(mac, len);
    writeToUart(incomingData, len);
    debugln("--------------------------------------------------");
}

void serialDataHandler(const uint8_t *incomingData, int len) {
    debugln("++++++++++++++++++++++++++++++++++++++++++++++++++");
    response deserializedResponse = response_init_zero;
    deserializeResponse(&deserializedResponse, incomingData, len);
    printResponse(&deserializedResponse, len);
    debugln("--------------------------------------------------");
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
    Serial.begin(BAUD_RATE);
    Serial2.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);

    setupWiFi();
    setupEspNow();
}

void loop() {
    readFromUart(serialDataHandler);
    delay(500);
}
