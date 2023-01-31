#ifndef __MQTT_CONFIG_HPP
#define __MQTT_CONFIG_HPP

#include <Arduino.h>

class MqttConfig {
   private:
    IPAddress serverIp;
    uint16_t port;
    const char *username;
    const char *password;

   public:
    MqttConfig();
    MqttConfig(IPAddress serverIp, uint16_t port, const char *username, const char *password);
    IPAddress getServerIp();
    uint16_t getPort();
    const char *getUsername();
    const char *getPassword();
};

#endif