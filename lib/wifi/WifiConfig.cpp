#include "WifiConfig.hpp"

WiFiConfig::WiFiConfig(){}

WiFiConfig::WiFiConfig(IPAddress staticIP, IPAddress gateway, IPAddress subnet, const char* ssid, const char* password){
    this->staticIP = staticIP;
    this->gateway = gateway;
    this->subnet = subnet;
    this->ssid = ssid;
    this->password = password;
}

IPAddress WiFiConfig::getSaticIp(){
    return this->staticIP;
}

IPAddress WiFiConfig::getGateway(){
    return this->gateway;
}

IPAddress WiFiConfig::getSubnet(){
    return this->subnet;
}

const char* WiFiConfig::getSsid(){
    return this->ssid;
}

const char* WiFiConfig::getPassword(){
    return this->password;
}