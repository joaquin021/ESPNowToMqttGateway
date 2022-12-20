#ifndef __MQTT_SERVICE_HPP
#define __MQTT_SERVICE_HPPS

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include "Commons.h"
#include "messages.pb.h"

class MqttService {
   private:
    const char *mqttUsername;
    const char *mqttPassword;
    WiFiClient wifiClient;
    PubSubClient mqttClient;
    String buildQueueName(char *clientId, char *name);
    void mqttConnect();
    void resusbcribe();

   public:
    MqttService(IPAddress mqttServer, uint16_t mqttPort, const char *mqttUsername, const char *mqttPassword);
    void setupMqtt();
    void mqttLoop();
    String getMqttStatus();
    bool isMqttConnected();
    bool publishMqtt(request_Send *send, char *clientId);
};

#endif