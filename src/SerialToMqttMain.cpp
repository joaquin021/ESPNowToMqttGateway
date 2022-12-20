#include <Arduino.h>
#include <WiFi.h>

#include "Commons.h"
#include "MqttService.hpp"
#include "RequestUtils.hpp"
#include "ResponseUtils.hpp"
#include "Secrets.h"
#include "messages.pb.h"

MqttService mqttService(MQTT_SERVER, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD);

void sendOpHandler(request_Send *send, char *clientId, response_OpResponse *opResponse) {
    printSendOperation(send);
    if (mqttService.isMqttConnected()) {
        if (mqttService.publishMqtt(send, clientId)) {
            buildResponse(opResponse, response_Result_OK, NULL);
        } else {
            buildResponse(opResponse, response_Result_NOK, "Error publishing the mqtt message");
        }
    } else {
        buildResponse(opResponse, response_Result_NOMQTT, mqttService.getMqttStatus().c_str());
    }
}

void subscribeOpHandler(request_Subscribe *subscribeOp, response_OpResponse *opResponse) {
    printSubscribeOperation(subscribeOp);
    buildResponse(opResponse, response_Result_NOK, "Can not subscribe");
}

void pingOpHandler(request_Ping *pingOp, response_OpResponse *opResponse) {
    printPingOperation(pingOp);
    buildResponse(opResponse, response_Result_OK, String(pingOp->num).c_str());
}

void serialDataHandler(const uint8_t *incomingData, int len) {
    debugln("++++++++++++++++++++++++++++++++++++++++++++++++++");
    response response = manageMessageRequest(incomingData, len, printRequestData, sendOpHandler, subscribeOpHandler, pingOpHandler);
    sendResponseViaUart(&response);
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
    debugln("Trying to connect WiFi.");
    WiFi.config(staticIP, gateway, subnet);
    WiFi.begin(SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        debug(".");
    }
    debugln("");
    debugln("WiFi connected.");
    debug("IP address: ");
    debugln(WiFi.localIP());
}

void setup() {
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, 16, 17);

    setupWiFi();
    mqttService.setupMqtt();
}

void loop() {
    readFromSerial();
    mqttService.mqttLoop();
    delay(500);
}