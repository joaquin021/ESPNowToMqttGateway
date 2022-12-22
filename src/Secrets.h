#ifndef __SECRETS_H
#define __SECRETS_H

#include <WiFi.h>
#include <stdint.h>

IPAddress MQTT_SERVER(192, 168, 1, X);
uint16_t MQTT_PORT = 1883;
const char* MQTT_USERNAME = "user";
const char* MQTT_PASSWORD = "pass";

const char* SSID = "SSID_NAME";
const char* WIFI_PASSWORD = "WIFIPASS";
IPAddress staticIP(192, 168, 1, X);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

#endif