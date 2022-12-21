#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

#include "Commons.h"
#include "RequestUtils.hpp"
#include "UartHandler.hpp"
#include "messages.pb.h"

#define RX_PIN 2
#define TX_PIN 15

void EspNowPair(const uint8_t *mac) {
    bool existPeer = esp_now_is_peer_exist(mac);
    if (!existPeer) {
        esp_now_peer_info_t peerInfo;
        memcpy(peerInfo.peer_addr, mac, 6);
        peerInfo.channel = 0;
        peerInfo.encrypt = false;
        if (esp_now_add_peer(&peerInfo) != ESP_OK) {
            debugln("Failed to add peer");
        }
    }
}

void sendResponseViaEspNow(const uint8_t *mac, const uint8_t *outputData, int len) {
    EspNowPair(mac);
    esp_err_t result = esp_now_send(mac, outputData, len);
    if (result == ESP_OK) {
        debugln("The message was sent sucessfully.");
    } else {
        debugln("There was an error sending the message.");
    }
}

void OnRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    debugln("++++++++++++++++++++++++++++++++++++++++++++++++++");
    printMacAndLenPacketReceived(mac, len);
    writeToUart(incomingData, len);
    debugln("--------------------------------------------------");
}

void onSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    debugln("Send success via esp-now.");
}

void serialDataHandler(const uint8_t *incomingData, int len) {
    debugln("++++++++++++++++++++++++++++++++++++++++++++++++++");
    response deserializedResponse = response_init_zero;
    deserializeResponse(&deserializedResponse, incomingData, len);
    printResponse(&deserializedResponse, len);
    sendResponseViaEspNow(deserializedResponse.client_mac, incomingData, len);
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
    esp_now_register_send_cb(onSent);
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
