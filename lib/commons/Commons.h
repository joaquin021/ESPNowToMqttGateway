#ifndef __COMMONS_H
#define __COMMONS_H

#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)

#define BAUD_RATE 115200
#define ESPNOW_BUFFERSIZE 200
#define END_TX_CHAR '|'
#define GATEWAY_ID "ESPNowToMqttGateway"

#endif