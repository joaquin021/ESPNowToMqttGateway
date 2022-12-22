#ifndef __WIFI_CONFIG_HPP
#define __WIFI_CONFIG_HPP

#include <Arduino.h>

class WiFiConfig {
   private:
    IPAddress staticIP;
    IPAddress gateway;
    IPAddress subnet;
    const char* ssid;
    const char* password;

   public:
    WiFiConfig();
    WiFiConfig(IPAddress staticIP, IPAddress gateway, IPAddress subnet, const char* ssid, const char* password);
    IPAddress getSaticIp();
    IPAddress getGateway();
    IPAddress getSubnet();
    const char* getSsid();
    const char* getPassword();
};

#endif