#include "SerialToMqttManager.hpp"

SerialToMqttManager *SerialToMqttManager::instance = nullptr;
RequestUtils requestUtils = RequestUtils::getInstance();
ResponseUtils responseUtils = ResponseUtils::getInstance();

void requestHandler(request *deserializedRequest, const uint8_t *serializedRequest, int len, response *response) {
    SerialToMqttManager *instance = SerialToMqttManager::getInstance();
    if (response->opResponses_count > 0) {
        instance->sendResponseViaUart(response);
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
            responseUtils.buildOpResponse(opResponse, response_Result_NOK, "Error publishing the mqtt message");
        }
    } else {
        responseUtils.buildOpResponse(opResponse, response_Result_NOMQTT, instance->mqttService.getMqttStatus().c_str());
    }
}

void subscribeOpHandler(request *request, request_Subscribe *subscribeOp, response_OpResponse *opResponse) {
    SerialToMqttManager *instance = SerialToMqttManager::getInstance();
    if (instance->mqttService.existsSubscription(request->client_id, subscribeOp)) {
        if (instance->mqttService.existsDataInTopic(request->client_id, subscribeOp)) {
            String data = instance->mqttService.getData(request->client_id, subscribeOp);
            responseUtils.buildOpResponse(opResponse, response_Result_OK, data.c_str());
        } else {
            responseUtils.buildOpResponse(opResponse, response_Result_NO_MSG, NULL);
        }
    } else {
        if (instance->mqttService.subscribe(request->client_id, subscribeOp)) {
            responseUtils.buildOpResponse(opResponse, response_Result_NO_MSG, "Subscribe to topic");
        } else {
            responseUtils.buildOpResponse(opResponse, response_Result_NOK, "Can not subscribe");
        }
    }
}

void pingOpHandler(request *request, request_Ping *pingOp, response_OpResponse *opResponse) {
    responseUtils.buildOpResponse(opResponse, response_Result_OK, String(pingOp->num).c_str());
}

void serialDataHandler(const uint8_t *incomingData, int len) {
    logDebugln("++++++++++++++++++++++++++++++++++++++++++++++++++");
    requestUtils.manage(incomingData, len, requestHandler, sendOpHandler, subscribeOpHandler, pingOpHandler);
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

uint8_t SerialToMqttManager::sendResponseViaUart(response *response) {
    uint8_t serializedBuffer[ESPNOW_BUFFERSIZE];
    int messageLength = ResponseUtils::getInstance().serialize(serializedBuffer, response);
    writeToUart(serializedBuffer, messageLength);
    return messageLength;
}