#include <Arduino.h>

#include "Commons.hpp"
#include "EspNowToSerialManager.hpp"

#define RX_PIN 16
#define TX_PIN 17

EspNowToSerialManager espNowToSerialManager;

void setup() {
    Serial.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);
    Serial2.begin(BAUD_RATE, SERIAL_8N1, RX, TX);
    //Serial.begin(BAUD_RATE);
    //Serial2.begin(BAUD_RATE);
    logDebugln("EspNow to Serial");

    espNowToSerialManager.setup();
}

void loop() {
    espNowToSerialManager.loop();
}
