#ifndef __MQTT_SERVICE_HPP
#define __MQTT_SERVICE_HPPS

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include <map>
#include <set>

#include "Commons.hpp"
#include "messages.pb.h"

class MqttService {
   private:
    static MqttService *instance;
    static MqttService *getInstance() { return instance; }
    const char *mqttUsername;
    const char *mqttPassword;
    WiFiClient wifiClient;
    PubSubClient mqttClient;
    std::map<String, String> dataFromTopics;
    std::set<String> subscriptions;
    String buildQueueName(char *clientId, char *name);
    void mqttConnect();
    void resubscribe();
    friend void mqttCallback(char *topic, uint8_t *payload, unsigned int len);

   public:
    MqttService(IPAddress mqttServer, uint16_t mqttPort, const char *mqttUsername, const char *mqttPassword);
    ~MqttService();
    void setupMqtt();
    void mqttLoop();
    String getMqttStatus();
    bool isMqttConnected();
    bool publishMqtt(char *clientId, request_Send *send);
    bool subscribe(char *clientId, request_Subscribe *subscribeOp);
    String getData(char *clientId, request_Subscribe *subscribeOp);
    bool existsSubscription(char *clientId, request_Subscribe *subscribeOp);
    bool existsDataInTopic(char *clientId, request_Subscribe *subscribeOp);
};

#endif