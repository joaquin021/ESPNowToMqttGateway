#include <Arduino.h>

#include "Commons.hpp"
#include "EspNowToSerialManager.hpp"
#include "SerialToMqttManager.hpp"

#define RX_PIN 2
#define TX_PIN 15

EspNowToSerialManager espNowToSerialManager;

void setup() {
    Serial.begin(BAUD_RATE);
    Serial2.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);

    espNowToSerialManager.setup();
}

void loop() {
    espNowToSerialManager.loop();
    delay(500);
}
