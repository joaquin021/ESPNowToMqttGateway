#include "EspNowToSerialManager.hpp"

EspNowToSerialManager *EspNowToSerialManager::instance = nullptr;
ResponseUtils responseUtils = ResponseUtils::getInstance();

void responseHandler(response *deserializedResponse, const uint8_t *serializedResponse, int len) {
    EspNowToSerialManager *instance = EspNowToSerialManager::getInstance();
    instance->espNowService.send(deserializedResponse->client_mac, serializedResponse, len);
}

void serialDataHandler(const uint8_t *incomingData, int len) {
    debugln("++++++++++++++++++++++++++++++++++++++++++++++++++");
    responseUtils.manage(incomingData, len, responseHandler, opResponseHandlerDummy);
    debugln("--------------------------------------------------");
}

void espNowRecvCallBack(const uint8_t *mac, const uint8_t *incomingData, int len) {
    debugln("++++++++++++++++++++++++++++++++++++++++++++++++++");
    printMacAndLenPacket(mac, len, "Packet received from: ");
    writeToUart(incomingData, len);
    debugln("--------------------------------------------------");
}

EspNowToSerialManager::EspNowToSerialManager() {
    instance = this;
}

void EspNowToSerialManager::setup() {
    setupWiFiForEspNow();
    espNowService.setup(espNowSendCallBackDummy, espNowRecvCallBack);
}

void EspNowToSerialManager::loop() {
    readFromUart(serialDataHandler);
}