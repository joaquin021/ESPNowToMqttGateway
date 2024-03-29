#ifndef __UART_HANDLER_HPP
#define __UART_HANDLER_HPP

#include <Arduino.h>

#include "Commons.hpp"
#include "RequestUtils.hpp"
#include "messages.pb.h"

#define BAUD_RATE 115200
#define END_TX_CHAR '|'

typedef void (*uart_read_cb_t)(const uint8_t *incomingData, int len);

void readFromUart(uart_read_cb_t uart_read_cb);

uint8_t sendResponseViaUart(response *response);

void writeToUart(const uint8_t *buffer, size_t size);

#endif