#include "SerialToMqttManager.hpp"

SerialToMqttManager *SerialToMqttManager::instance = nullptr;
RequestUtils requestUtils = RequestUtils::getInstance();
ResponseUtils responseUtils = ResponseUtils::getInstance();

void requestHandler(request *deserializedRequest, const uint8_t *serializedRequest, int len, response *response) {
    if (response->opResponses_count > 0) {
        sendResponseViaUart(response);
    } else {
        logDebugln("Response is empty.");
    }
}

void sendOpHandler(request *request, request_Send *send, response_OpResponse *opResponse) {
    SerialToMqttManager *instance = SerialToMqttManager::getInstance();
    if (instance->mqttService.isMqttConnected()) {
        if (instance->mqttService.publishMqtt(request->client_id, send)) {
            responseUtils.buildOpResponse(opResponse, response_Result_OK, NULL);
        } else {
            responseUtils.buildOpResponse(opResponse, response_Result_ERROR, "Error publishing the mqtt message");
        }
    } else {
        responseUtils.buildOpResponse(opResponse, response_Result_MQTT_ERROR, instance->mqttService.getMqttStatus().c_str());
    }
}

void subscribeOpHandler(request *request, request_Subscribe *subscribeOp, response_OpResponse *opResponse) {
    SerialToMqttManager *instance = SerialToMqttManager::getInstance();
    if (instance->mqttService.subscribe(request, subscribeOp)) {
        responseUtils.buildOpResponse(opResponse, response_Result_SUBSCRIBED_OK, NULL);
    } else {
        responseUtils.buildOpResponse(opResponse, response_Result_ERROR, "Can not subscribe");
    }
}

void pingOpHandler(request *request, request_Ping *pingOp, response_OpResponse *opResponse) {
    responseUtils.buildOpResponse(opResponse, response_Result_OK, String(pingOp->num).c_str());
}

void serialDataHandler(const uint8_t *incomingData, int len) {
    logDebugln("++++++++++++++++++++++++++++++++++++++++++++++++++");
    requestUtils.manage(incomingData, len, requestHandler, sendOpHandler, subscribeOpHandler, unSubscribeOpHandlerDummy, pingOpHandler);
    logDebugln("--------------------------------------------------");
}

SerialToMqttManager::SerialToMqttManager(WiFiConfig wifiConfig, MqttConfig mqttConfig) : mqttService(mqttConfig) {
    instance = this;
    this->wifiConfig = wifiConfig;
}

void SerialToMqttManager::setup() {
    connectToWiFi(wifiConfig);
    mqttService.setupMqtt();
}

void SerialToMqttManager::loop() {
    mqttService.mqttLoop();
    readFromUart(serialDataHandler);
}