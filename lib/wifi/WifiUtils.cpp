#include "WifiUtils.hpp"

void connectToWiFi(WiFiConfig wifiConfig) {
    debugln("Trying to connect WiFi.");
    WiFi.config(wifiConfig.getSaticIp(), wifiConfig.getGateway(), wifiConfig.getSubnet());
    WiFi.begin(wifiConfig.getSsid(), wifiConfig.getPassword());
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        debug(".");
    }
    debugln("");
    debugln("WiFi connected.");
    debug("IP address: ");
    debugln(WiFi.localIP());
    debugln("Mac: " + WiFi.macAddress());
}

void setupWiFiForEspNow() {
    WiFi.mode(WIFI_MODE_STA);
    debugln("Mac: " + WiFi.macAddress());
}