#ifndef __COMMONS_H
#define __COMMONS_H

#define DEBUG_FLAG

#ifdef DEBUG_FLAG
#define debug Serial.print
#define debugln Serial.println
#define debugf Serial.printf
#else
#define debug
#define debugln
#define debugf
#endif
#define BAUD_RATE 115200
#define ESPNOW_BUFFERSIZE 200
#define END_TX_CHAR '|'
#define GATEWAY_ID "ESPNowToMqttGateway"

#endif