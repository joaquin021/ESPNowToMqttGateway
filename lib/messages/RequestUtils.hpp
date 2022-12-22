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
    void addRequestOperationToRequest(request *request, request_Operation requestOperation);

   public:
    static RequestUtils &getInstance() {
        static RequestUtils instance;
        return instance;
    }
    request createRequest(const char *clientName, uint8_t macWhereToSendTheResponse[6], int32_t messageType = 0);
    void buildSendOperation(request *request, const char *payload, const char *queue, bool retain = false);
    void buildSubscribeOperation(request *request, const char *queue, bool clear = true);
    void buildPingOperation(request *request, int32_t ping);
    response manage(const uint8_t *incomingData, int len,
                    request_handler_t request_handler = requestHandlerDummy, send_op_handler_t send_op_handler = sendOpHandlerDummy,
                    subscribe_op_handler_t subscribe_op_handler = subscribeOpHandlerDummy, ping_op_handler_t ping_op_handler = pingOpHandlerDummy);
    int serialize(uint8_t *buffer, request *request);
    bool deserialize(request *deserializedRequest, const uint8_t *incomingData, int len);
};

#endif