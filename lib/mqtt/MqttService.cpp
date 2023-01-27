#include "MqttService.hpp"

#define GATEWAY_ID "ESPNowToMqttGateway"
#define WILL_TOPIC GATEWAY_ID "/will"
#define WILL_QOS 1
#define WILL_RETAIN false
#define WILL_MSG "offline"

String MQTT_STATUS_POSITIVE[] = {"MQTT_CONNECTED", "MQTT_CONNECT_BAD_PROTOCOL", "MQTT_CONNECT_BAD_CLIENT_ID", "MQTT_CONNECT_UNAVAILABLE", "MQTT_CONNECT_BAD_CREDENTIALS", "MQTT_CONNECT_UNAUTHORIZED"};
String MQTT_STATUS_NEGATIVE[] = {"", "MQTT_DISCONNECTED", "MQTT_CONNECT_FAILED", "MQTT_CONNECTION_LOST", "MQTT_CONNECTION_TIMEOUT"};

MqttService *MqttService::instance = nullptr;

uint8_t gatewayAddress[6] = {0x90, 0x38, 0x0C, 0xED, 0x87, 0x40};

response createResponseFromMqtt(mac_address mac, subscription_client_data clientData, uint8_t *payload, unsigned int len) {
    response response = response_init_zero;
    response.opResponses_count = 1;
    memcpy(response.from_mac, gatewayAddress, sizeof(gatewayAddress));
    memcpy(response.to_mac, mac.address, sizeof(mac.address));
    response.message_type = clientData.message_type;
    response.opResponses[0].operation_type = clientData.operation_type;
    memcpy(response.opResponses[0].payload, payload, sizeof(payload));
    response.opResponses[0].payload[len] = 0;
    response.opResponses[0].result_code = response_Result_OK;
    return response;
}

void mqttCallback(char *topic, uint8_t *payload, unsigned int len) {
    logDebugln("++++++++++++++++++++++++++++++++++++++++++++++++++");
    logDebugf("> Packet received via Mqtt.\n>>> Topic: %s\n>>> Len: %d\n", topic, len);
    MqttService *instance = MqttService::getInstance();
    if (len != 0) {
        if (instance->isSubscription(topic)) {
            std::map<mac_address, subscription_client_data> clientsForQueue = instance->subscriptions.find(topic)->second;
            for (auto clientsIterator = clientsForQueue.begin(); clientsIterator != clientsForQueue.end(); clientsIterator++) {
                response response = createResponseFromMqtt(clientsIterator->first, clientsIterator->second, payload, len);
                sendResponseViaUart(&response);
            }
        }
    }
    logDebugln("--------------------------------------------------");
}

MqttService::MqttService(MqttConfig mqttConfig) : mqttClient(wifiClient) {
    instance = this;
    this->mqttConfig = mqttConfig;
    mqttClient.setServer(mqttConfig.getServerIp(), mqttConfig.getPort());
    mqttClient.setCallback(mqttCallback);
}

MqttService::~MqttService() {
    mqttClient.disconnect();
}

void MqttService::setupMqtt() {
    mqttConnect();
}

void MqttService::mqttLoop() {
    if (mqttClient.connected()) {
        mqttClient.loop();
        // mqttClient.publish(WILL_TOPIC, "online", WILL_RETAIN);
    } else {
        mqttConnect();
        resubscribe();
    }
}

void MqttService::mqttConnect() {
    if (mqttClient.connect(GATEWAY_ID, mqttConfig.getUsername(), mqttConfig.getPassword(), WILL_TOPIC, WILL_QOS, WILL_RETAIN, WILL_MSG)) {
        logDebugln("Mqtt connected: " + getMqttStatus());
    } else {
        logDebugln("Cannot connect to mqtt: " + getMqttStatus());
    }
}

void MqttService::resubscribe() {
    for (auto it = subscriptions.begin(); it != subscriptions.end(); it++) {
        mqttClient.subscribe(it->first.c_str());
    }
    logDebugf("Resubscribed to %d topics\n", this->subscriptions.size());
}

String MqttService::getMqttStatus() {
    int statusCode = mqttClient.state();
    return statusCode < 0 ? MQTT_STATUS_NEGATIVE[statusCode * -1] : MQTT_STATUS_POSITIVE[statusCode];
}

bool MqttService::isMqttConnected() {
    return mqttClient.connected();
}

bool MqttService::publishMqtt(char *clientId, request_Send *send) {
    bool sendStatus = mqttClient.publish(buildQueueName(clientId, send->queue).c_str(), send->payload, send->persist);
    if (!sendStatus) {
        logDebugln("Error publishing mqtt message.");
    }
    return sendStatus;
}

bool MqttService::subscribe(request *request, request_Subscribe *subscribeOp) {
    std::string queue = buildQueueName(request->client_id, subscribeOp->queue);
    logDebugf("Subscription request from: %02x:%02x:%02x:%02x:%02x:%02x, topic: %s\n", request->from_mac[0], request->from_mac[1],
              request->from_mac[2], request->from_mac[3], request->from_mac[4], request->from_mac[5], queue.c_str());
    bool existsSubscription = isSubscription(queue);
    if (!existsSubscription) {
        logDebugln("There is not subscription for this topic. Subscribing...");
        existsSubscription = mqttClient.subscribe(queue.c_str());
    }
    if (existsSubscription && !isSubscriptionForClient(queue, request->from_mac)) {
        logDebugln("There is no subscription for this client in this topic. Subscribing...");
        registerSubscription(queue, request, subscribeOp);
    }
    return existsSubscription;
}

bool MqttService::unSubscribe(request *request, request_Subscribe *subscribeOp) {
    std::string queue = buildQueueName(request->client_id, subscribeOp->queue);
    return false;
}

bool MqttService::unSubscribe(std::string queue) {
    return mqttClient.unsubscribe(queue.c_str());
}

void MqttService::registerSubscription(std::string queue, request *request, request_Subscribe *subscribeOp) {
    subscription_client_data clientData = {request->message_type, subscribeOp->operation_type, millis()};
    mac_address clientMac;
    memcpy(clientMac.address, request->from_mac, 6);
    subscriptions[queue][clientMac] = clientData;
}

bool MqttService::isSubscriptionForClient(std::string queue, uint8_t address[6]) {
    mac_address clientMac;
    memcpy(clientMac.address, address, 6);
    if (isSubscription(queue)) {
        logDebugln("Checking if subscription exists for this client.");
        std::map<mac_address, subscription_client_data> clientsForQueue = subscriptions.find(queue)->second;
        return clientsForQueue.find(clientMac) != clientsForQueue.end();
    }
    return false;
}

bool MqttService::isSubscription(std::string queue) {
    logDebugln("Checking if subscription exists.");
    return subscriptions.find(queue) != subscriptions.end();
}

/*
String MqttService::getData(char *clientId, request_Subscribe *subscribeOp) {
    String queue = buildQueueName(clientId, subscribeOp->queue);
    std::map<String, String>::iterator dataPair = dataFromTopics.find(queue);
    String data = dataPair->second;
    dataFromTopics.erase(dataPair);
    return data;
}

bool MqttService::existsDataInTopic(char *clientId, request_Subscribe *subscribeOp) {
    String queue = buildQueueName(clientId, subscribeOp->queue);
    return dataFromTopics.find(queue) != dataFromTopics.end();
}
*/

std::string MqttService::buildQueueName(char *clientId, char *name) {
    return std::string(GATEWAY_ID)
        .append("/")
        .append(clientId)
        .append("/")
        .append(name);
}