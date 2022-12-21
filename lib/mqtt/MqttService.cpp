#include "MqttService.hpp"

#define WILL_TOPIC GATEWAY_ID "/will"
#define WILL_QOS 1
#define WILL_RETAIN false
#define WILL_MSG "offline"

String MQTT_STATUS_POSITIVE[] = {"MQTT_CONNECTED", "MQTT_CONNECT_BAD_PROTOCOL", "MQTT_CONNECT_BAD_CLIENT_ID", "MQTT_CONNECT_UNAVAILABLE", "MQTT_CONNECT_BAD_CREDENTIALS", "MQTT_CONNECT_UNAUTHORIZED"};
String MQTT_STATUS_NEGATIVE[] = {"", "MQTT_DISCONNECTED", "MQTT_CONNECT_FAILED", "MQTT_CONNECTION_LOST", "MQTT_CONNECTION_TIMEOUT"};

MqttService *MqttService::instance = nullptr;

void mqttCallback(char *topic, uint8_t *payload, unsigned int len) {
    debugln("++++++++++++++++++++++++++++++++++++++++++++++++++");
    debugf("> Packet received via Mqtt.\n>>> Topic: %s\n>>> Len: %d\n", topic, len);
    MqttService *instance = MqttService::getInstance();
    if (len == 0) {
        instance->dataFromTopics.erase(topic);
    } else {
        char charPayload[len + 1];
        memcpy(charPayload, payload, len);
        charPayload[len] = 0;
        String strPayload(charPayload);
        instance->dataFromTopics[String(topic)] = strPayload;
    }
    debugln("--------------------------------------------------");
}

MqttService::MqttService(IPAddress mqttServer, uint16_t mqttPort, const char *mqttUsername, const char *mqttPassword) : mqttClient(wifiClient) {
    instance = this;
    mqttClient.setServer(mqttServer, mqttPort);
    mqttClient.setCallback(mqttCallback);
    this->mqttUsername = mqttUsername;
    this->mqttPassword = mqttPassword;
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
    if (mqttClient.connect(GATEWAY_ID, mqttUsername, mqttPassword, WILL_TOPIC, WILL_QOS, WILL_RETAIN, WILL_MSG)) {
        debugln("Mqtt connected: " + getMqttStatus());
    } else {
        debugln("Cannot connect to mqtt: " + getMqttStatus());
    }
}

void MqttService::resubscribe() {
    for (String subscription : this->subscriptions) {
        mqttClient.subscribe(subscription.c_str());
    }
    debugf("Resubscribed to %d topics\n", this->subscriptions.size());
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
        debugln("Error publishing mqtt message.");
    }
    return sendStatus;
}

bool MqttService::subscribe(char *clientId, request_Subscribe *subscribeOp) {
    String queue = buildQueueName(clientId, subscribeOp->queue);
    bool result = mqttClient.subscribe(queue.c_str());
    if (result) {
        subscriptions.insert(queue);
    }
    return result;
}

String MqttService::getData(char *clientId, request_Subscribe *subscribeOp) {
    String queue = buildQueueName(clientId, subscribeOp->queue);
    std::map<String, String>::iterator dataPair = dataFromTopics.find(queue);
    String data = dataPair->second;
    if (subscribeOp->clear) {
        dataFromTopics.erase(dataPair);
    }
    return data;
}

bool MqttService::existsSubscription(char *clientId, request_Subscribe *subscribeOp) {
    String queue = buildQueueName(clientId, subscribeOp->queue);
    return subscriptions.find(queue) != subscriptions.end();
}

bool MqttService::existsDataInTopic(char *clientId, request_Subscribe *subscribeOp) {
    String queue = buildQueueName(clientId, subscribeOp->queue);
    return dataFromTopics.find(queue) != dataFromTopics.end();
}

String MqttService::buildQueueName(char *clientId, char *name) {
    String queue = String(GATEWAY_ID);
    queue.concat("/");
    queue.concat(clientId);
    queue.concat("/");
    queue.concat(name);
    return queue;
}