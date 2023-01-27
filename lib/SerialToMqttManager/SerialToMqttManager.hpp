#ifndef __SERIAL_TO_MQTT_MANAGER_HPP
#define __SERIAL_TO_MQTT_MANAGER_HPP

#include "Commons.hpp"
#include "MqttService.hpp"
#include "RequestUtils.hpp"
#include "ResponseUtils.hpp"
#include "UartHandler.hpp"
#include "WifiConfig.hpp"
#include "WifiUtils.hpp"
#include "messages.pb.h"

class SerialToMqttManager {
   private:
    static SerialToMqttManager *instance;
    static SerialToMqttManager *getInstance() { return instance; }
    WiFiConfig wifiConfig;
    MqttService mqttService;
    friend void serialDataHandler(const uint8_t *incomingData, int len);
    friend void requestHandler(request *deserializedRequest, const uint8_t *serializedRequest, int len, response *response);
    friend void sendOpHandler(request *request, request_Send *send, response_OpResponse *opResponse);
    friend void subscribeOpHandler(request *request, request_Subscribe *subscribeOp, response_OpResponse *opResponse);
    friend void pingOpHandler(request *request, request_Ping *pingOp, response_OpResponse *opResponse);

   public:
    SerialToMqttManager(WiFiConfig wifiConfig, MqttConfig mqttConfig);
    void setup();
    void loop();
};

#endif