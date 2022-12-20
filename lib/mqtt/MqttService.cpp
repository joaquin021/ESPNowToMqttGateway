#include "MqttService.hpp"

#define WILL_TOPIC GATEWAY_ID "/will"
#define WILL_QOS 1
#define WILL_RETAIN false
#define WILL_MSG "offline"

String MQTT_STATUS_POSITIVE[] = {"MQTT_CONNECTED", "MQTT_CONNECT_BAD_PROTOCOL", "MQTT_CONNECT_BAD_CLIENT_ID", "MQTT_CONNECT_UNAVAILABLE", "MQTT_CONNECT_BAD_CREDENTIALS", "MQTT_CONNECT_UNAUTHORIZED"};
String MQTT_STATUS_NEGATIVE[] = {"", "MQTT_DISCONNECTED", "MQTT_CONNECT_FAILED", "MQTT_CONNECTION_LOST", "MQTT_CONNECTION_TIMEOUT"};

MqttService::MqttService(IPAddress mqttServer, uint16_t mqttPort, const char *mqttUsername, const char *mqttPassword) : mqttClient(wifiClient) {
    mqttClient.setServer(mqttServer, mqttPort);
    this->mqttUsername = mqttUsername;
    this->mqttPassword = mqttPassword;
}

void MqttService::setupMqtt() {
    mqttConnect();
}

void MqttService::mqttLoop() {
    if (mqttClient.connected()) {
        mqttClient.loop();
        //mqttClient.publish(WILL_TOPIC, "online", WILL_RETAIN);
    } else {
        mqttConnect();
        resusbcribe();
    }
}

String MqttService::getMqttStatus() {
    int statusCode = mqttClient.state();
    return statusCode < 0 ? MQTT_STATUS_NEGATIVE[statusCode * -1] : MQTT_STATUS_POSITIVE[statusCode];
}

bool MqttService::isMqttConnected() {
    return mqttClient.connected();
}

bool MqttService::publishMqtt(request_Send *send, char *clientId) {
    bool sendStatus = mqttClient.publish(buildQueueName(clientId, send->queue).c_str(), send->payload, send->persist);
    if (!sendStatus) {
        debugln("Error publishing mqtt message.");
    }
    return sendStatus;
}

void MqttService::mqttConnect() {
    if (mqttClient.connect(GATEWAY_ID, mqttUsername, mqttPassword, WILL_TOPIC, WILL_QOS, WILL_RETAIN, WILL_MSG)) {
        debugln("Mqtt connected: " + getMqttStatus());
    } else {
        debugln("Cannot connect to mqtt: " + getMqttStatus());
    }
}

void MqttService::resusbcribe() {
}

String MqttService::buildQueueName(char *clientId, char *name) {
    String queue = String(GATEWAY_ID);
    queue.concat("/");
    queue.concat(clientId);
    queue.concat("/");
    queue.concat(name);
    return queue;
}