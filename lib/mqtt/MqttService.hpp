#ifndef __MQTT_SERVICE_HPP
#define __MQTT_SERVICE_HPP

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include <map>
#include <set>

#include "Commons.hpp"
#include "MqttConfig.hpp"
#include "UartHandler.hpp"
#include "messages.pb.h"

struct subscription_client_data {
    int32_t message_type;
    int32_t operation_type;
    unsigned long subscribed_since;
};

struct mac_address {
    uint8_t address[6];

    bool operator==(const mac_address &other) const {
        for (int i = 0; i < 6; i++) {
            if (address[i] != other.address[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator<(const mac_address &other) const {
        for (int i = 0; i < 6; i++) {
            if (address[i] < other.address[i]) {
                return true;
            } else if (address[i] > other.address[i]) {
                return false;
            }
        }
        return false;
    }
};

class MqttService {
   private:
    static MqttService *instance;
    static MqttService *getInstance() { return instance; }
    MqttConfig mqttConfig;
    WiFiClient wifiClient;
    PubSubClient mqttClient;
    std::map<std::string, std::map<mac_address, subscription_client_data>> subscriptions;
    std::string buildQueueName(char *clientId, char *name);
    void mqttConnect();
    void resubscribe();
    bool isSubscription(std::string queue);
    bool isSubscriptionForClient(std::string queue, uint8_t address[6]);
    void registerSubscription(std::string queue, request *request, request_Subscribe *subscribeOp);
    friend void mqttCallback(char *topic, uint8_t *payload, unsigned int len);
    friend response createResponseFromMqtt(mac_address mac, subscription_client_data clientData, uint8_t *payload, unsigned int len);

   public:
    MqttService(MqttConfig mqttConfig);
    ~MqttService();
    void setupMqtt();
    void mqttLoop();
    String getMqttStatus();
    bool isMqttConnected();
    bool publishMqtt(char *clientId, request_Send *send);
    bool subscribe(request *request, request_Subscribe *subscribeOp);
    bool unSubscribe(request *request, request_Subscribe *subscribeOp);
    bool unSubscribe(std::string queue);
};

#endif