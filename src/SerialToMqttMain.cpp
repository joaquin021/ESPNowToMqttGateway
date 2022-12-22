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

RequestUtils requestUtils = RequestUtils::getInstance();
ResponseUtils responseUtils = ResponseUtils::getInstance();
MqttService mqttService(MQTT_SERVER, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD);

void sendOpHandler(request *request, request_Send *send, response_OpResponse *opResponse) {
    if (mqttService.isMqttConnected()) {
        if (mqttService.publishMqtt(request->client_id, send)) {
            responseUtils.buildOpResponse(opResponse, response_Result_OK, NULL);
        } else {
            responseUtils.buildOpResponse(opResponse, response_Result_NOK, "Error publishing the mqtt message");
        }
    } else {
        responseUtils.buildOpResponse(opResponse, response_Result_NOMQTT, mqttService.getMqttStatus().c_str());
    }
}

void subscribeOpHandler(request *request, request_Subscribe *subscribeOp, response_OpResponse *opResponse) {
    if (mqttService.existsSubscription(request->client_id, subscribeOp)) {
        if (mqttService.existsDataInTopic(request->client_id, subscribeOp)) {
            String data = mqttService.getData(request->client_id, subscribeOp);
            responseUtils.buildOpResponse(opResponse, response_Result_OK, data.c_str());
        } else {
            responseUtils.buildOpResponse(opResponse, response_Result_NO_MSG, NULL);
        }
    } else {
        if (mqttService.subscribe(request->client_id, subscribeOp)) {
            responseUtils.buildOpResponse(opResponse, response_Result_NO_MSG, "Subscribe to topic");
        } else {
            responseUtils.buildOpResponse(opResponse, response_Result_NOK, "Can not subscribe");
        }
    }
}

void pingOpHandler(request *request, request_Ping *pingOp, response_OpResponse *opResponse) {
    responseUtils.buildOpResponse(opResponse, response_Result_OK, String(pingOp->num).c_str());
}

void requestHandler(request *deserializedRequest, const uint8_t *serializedRequest, int len, response *response) {
    if (response->opResponses_count > 0) {
        sendResponseViaUart(response);
    } else {
        debugln("Response is empty.");
    }
}

void serialDataHandler(const uint8_t *incomingData, int len) {
    debugln("++++++++++++++++++++++++++++++++++++++++++++++++++");
    requestUtils.manage(incomingData, len, requestHandler, sendOpHandler, subscribeOpHandler, pingOpHandler);
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