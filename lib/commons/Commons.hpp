#ifndef __COMMONS_HPP
#define __COMMONS_HPP

#include "Arduino.h"

#ifdef DEBUG_FLAG
#define debug Serial.print
#define debugln Serial.println
#define debugf Serial.printf
#else
#define debug
#define debugln
#define debugf
#endif

#define ESPNOW_BUFFERSIZE 200

void printMacAndLenPacket(const uint8_t *mac, int len, const char *debugMessage);

#endif