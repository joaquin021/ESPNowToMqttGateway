#ifndef __RESPONSE_UTILS_HPP
#define __RESPONSE_UTILS_HPP

#include <Arduino.h>
#include <string.h>

#include "Commons.h"
#include "UartHandler.hpp"
#include "messages.pb.h"
#include "pb_decode.h"
#include "pb_encode.h"

void printMacAndLenPacketResponse(const uint8_t *mac, int len);

void printResponse(response *deserializedResponse, int len);

response createResponse(request *request);

void buildResponse(response_OpResponse *opResponse, response_Result result_code, const char *payload);

bool deserializeResponse(response *deserializedResponse, const uint8_t *incomingData, int len);

int serializeResponse(uint8_t *buffer, response *response);

uint8_t sendResponseViaUart(response *response);

#endif