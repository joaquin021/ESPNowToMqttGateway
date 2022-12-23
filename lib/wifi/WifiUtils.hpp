#ifndef __WIFI_UTILS_HPP
#define __WIFI_UTILS_HPP

#include <WiFi.h>

#include "Commons.hpp"
#include "WifiConfig.hpp"

void connectToWiFi(WiFiConfig wifiConfig);
void setupWiFiForEspNow();

#endif