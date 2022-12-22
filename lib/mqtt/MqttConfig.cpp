#include "MqttConfig.hpp"

MqttConfig::MqttConfig() {}

MqttConfig::MqttConfig(IPAddress serverIp, uint16_t port, const char *username, const char *password) {
    this->serverIp = serverIp;
    this->port = port;
    this->username = username;
    this->password = password;
}

IPAddress MqttConfig::getServerIp() {
    return this->serverIp;
}

uint16_t MqttConfig::getPort() {
    return this->port;
}

const char *MqttConfig::getUsername() {
    return this->username;
}

const char *MqttConfig::getPassword() {
    return this->password;
}