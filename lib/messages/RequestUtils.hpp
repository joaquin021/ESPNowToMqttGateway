#ifndef __REQUEST_UTILS_HPP
#define __REQUEST_UTILS_HPP

#include <Arduino.h>

#include "Commons.hpp"
#include "ResponseUtils.hpp"
#include "messages.pb.h"
#include "pb_decode.h"
#include "pb_encode.h"

typedef void (*request_handler_t)(request *deserializedRequest, const uint8_t *serializedRequest, int len, response *response);
typedef void (*send_op_handler_t)(request *request, request_Send *sendOp, response_OpResponse *opResponse);
typedef void (*subscribe_op_handler_t)(request *request, request_Subscribe *subscribeOp, response_OpResponse *opResponse);
typedef void (*ping_op_handler_t)(request *request, request_Ping *pingOp, response_OpResponse *opResponse);

void requestHandlerDummy(request *deserializedRequest, const uint8_t *serializedRequest, int len, response *response);
void sendOpHandlerDummy(request *request, request_Send *sendOp, response_OpResponse *opResponse);
void subscribeOpHandlerDummy(request *request, request_Subscribe *subscribeOp, response_OpResponse *opResponse);
void pingOpHandlerDummy(request *request, request_Ping *pingOp, response_OpResponse *opResponse);

class RequestUtils {
   private:
    response manage(request *request, send_op_handler_t send_op_handler, subscribe_op_handler_t subscribe_op_handler, ping_op_handler_t ping_op_handler);
    void printRequestData(request *request);
    void printSendOperation(request_Send *send);
    void printSubscribeOperation(request_Subscribe *subscribe);
    void printPingOperation(request_Ping *ping);
    void printUnknownOperation(pb_size_t which_op);

   public:
    static RequestUtils &getInstance() {
        static RequestUtils instance;
        return instance;
    }
    response manage(const uint8_t *incomingData, int len,
                    request_handler_t request_handler = requestHandlerDummy, send_op_handler_t send_op_handler = sendOpHandlerDummy,
                    subscribe_op_handler_t subscribe_op_handler = subscribeOpHandlerDummy, ping_op_handler_t ping_op_handler = pingOpHandlerDummy);
    bool deserialize(request *deserializedRequest, const uint8_t *incomingData, int len);
};

#endif