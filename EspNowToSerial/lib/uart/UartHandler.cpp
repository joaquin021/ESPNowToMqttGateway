#include "UartHandler.hpp"

void readFromUart(uart_read_cb_t uart_read_cb) {
    if (Serial2.available()) {
        uint8_t buffer[ESPNOW_BUFFERSIZE];
        int bytesRead = Serial2.readBytesUntil(END_TX_CHAR, buffer, ESPNOW_BUFFERSIZE);
        if (bytesRead > 0) {
            logDebug("Bytes read from Serial2: ");
            logDebugln(bytesRead);
            uart_read_cb(buffer, bytesRead);
        }
    }
}

uint8_t sendResponseViaUart(response *response) {
    uint8_t serializedBuffer[ESPNOW_BUFFERSIZE];
    int messageLength = ResponseUtils::getInstance().serialize(serializedBuffer, response);
    writeToUart(serializedBuffer, messageLength);
    return messageLength;
}

void writeToUart(const uint8_t *buffer, size_t size) {
    Serial2.write(buffer, size);
    Serial2.write(END_TX_CHAR);
}