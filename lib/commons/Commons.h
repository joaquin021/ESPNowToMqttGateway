#ifndef __COMMONS_H
#define __COMMONS_H

#define debug(x) Serial.print(x)
#define debugf(x, y) Serial.printf(x, y)
#define debugln(x) Serial.println(x)

#define ESPNOW_BUFFERSIZE 200
#define END_TX_CHAR '|'
#define GATEWAY_ID "ESPNowToMqttGateway"

#endif