#include "Commons.hpp"

void printMacAndLenPacket(const uint8_t *mac, int len, const char *debugMessage) {
    char macStr[18 + 1 + 4];
    debug(debugMessage);
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x %db",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], len);
    debugln(macStr);
}