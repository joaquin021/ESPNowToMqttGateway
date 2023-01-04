#include <Arduino.h>

#include "Commons.hpp"
#include "EspNowToSerialManager.hpp"

#define RX_PIN 16
#define TX_PIN 17

EspNowToSerialManager espNowToSerialManager;

void setup() {
    Serial.begin(BAUD_RATE);
    Serial2.begin(BAUD_RATE);

    espNowToSerialManager.setup();
}

void loop() {
    espNowToSerialManager.loop();
    delay(500);
}
