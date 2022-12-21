#ifndef __REQUEST_UTILS_HPP
#define __REQUEST_UTILS_HPP

#include <Arduino.h>

#include "Commons.hpp"
#include "ResponseUtils.hpp"
#include "messages.pb.h"
#include "pb_decode.h"
#include "pb_encode.h"

typedef void (*request_handler_t)(request *request);
typedef void (*send_op_handler_t)(request_Send *sendOp, char *clientId, response_OpResponse *opResponse);
typedef void (*subscribe_op_handler_t)(request_Subscribe *subscribeOp, char *clientId, response_OpResponse *opResponse);
typedef void (*ping_op_handler_t)(request_Ping *pingOp, response_OpResponse *opResponse);

void requestHandlerDummy(request *request);
void sendOpHandlerDummy(request_Send *sendOp, char *clientId, response_OpResponse *opResponse);
void subscribeOpHandlerDummy(request_Subscribe *subscribeOp, char *clientId, response_OpResponse *opResponse);
void pingOpHandlerDummy(request_Ping *pingOp, response_OpResponse *opResponse);

response manageMessageRequest(const uint8_t *incomingData, int len,
                              request_handler_t request_handler = requestHandlerDummy,
                              send_op_handler_t send_op_handler = sendOpHandlerDummy,
                              subscribe_op_handler_t subscribe_op_handler = subscribeOpHandlerDummy,
                              ping_op_handler_t ping_op_handler = pingOpHandlerDummy);

bool deserializeRequest(request *deserializedRequest, const uint8_t *incomingData, int len);

void printRequestData(request *request);

void printSendOperation(request_Send *send);

void printSubscribeOperation(request_Subscribe *subscribe);

void printPingOperation(request_Ping *ping);

void printUnknownOperation(pb_size_t which_op);

#endif