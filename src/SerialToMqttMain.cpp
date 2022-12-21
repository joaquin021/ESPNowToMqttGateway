#include <Arduino.h>
#include <WiFi.h>

#include "Commons.hpp"
#include "MqttService.hpp"
#include "RequestUtils.hpp"
#include "ResponseUtils.hpp"
#include "Secrets.h"
#include "UartHandler.hpp"
#include "messages.pb.h"

#define RX_PIN 16
#define TX_PIN 17

MqttService mqttService(MQTT_SERVER, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD);

void sendOpHandler(request_Send *send, char *clientId, response_OpResponse *opResponse) {
    printSendOperation(send);
    if (mqttService.isMqttConnected()) {
        if (mqttService.publishMqtt(clientId, send)) {
            buildResponse(opResponse, response_Result_OK, NULL);
        } else {
            buildResponse(opResponse, response_Result_NOK, "Error publishing the mqtt message");
        }
    } else {
        buildResponse(opResponse, response_Result_NOMQTT, mqttService.getMqttStatus().c_str());
    }
}

void subscribeOpHandler(request_Subscribe *subscribeOp, char *clientId, response_OpResponse *opResponse) {
    printSubscribeOperation(subscribeOp);
    if (mqttService.existsSubscription(clientId, subscribeOp)) {
        if (mqttService.existsDataInTopic(clientId, subscribeOp)) {
            String data = mqttService.getData(clientId, subscribeOp);
            buildResponse(opResponse, response_Result_OK, data.c_str());
        } else {
            buildResponse(opResponse, response_Result_NO_MSG, NULL);
        }
    } else {
        if (mqttService.subscribe(clientId, subscribeOp)) {
            buildResponse(opResponse, response_Result_NO_MSG, "Subscribe to topic");
        } else {
            buildResponse(opResponse, response_Result_NOK, "Can not subscribe");
        }
    }
}

void pingOpHandler(request_Ping *pingOp, response_OpResponse *opResponse) {
    printPingOperation(pingOp);
    buildResponse(opResponse, response_Result_OK, String(pingOp->num).c_str());
}

void serialDataHandler(const uint8_t *incomingData, int len) {
    debugln("++++++++++++++++++++++++++++++++++++++++++++++++++");
    response response = manageMessageRequest(incomingData, len, printRequestData, sendOpHandler, subscribeOpHandler, pingOpHandler);
    if (response.opResponses_count > 0) {
        sendResponseViaUart(&response);
    } else {
        debugln("Response is empty.");
    }
    debugln("--------------------------------------------------");
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
    Serial.begin(BAUD_RATE);
    Serial2.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);

    setupWiFi();
    mqttService.setupMqtt();
}

void loop() {
    readFromUart(serialDataHandler);
    mqttService.mqttLoop();
    delay(500);
}