#ifndef __RESPONSE_UTILS_HPP
#define __RESPONSE_UTILS_HPP

#include <Arduino.h>
#include <string.h>

#include "Commons.hpp"
#include "messages.pb.h"
#include "pb_decode.h"
#include "pb_encode.h"

typedef void (*response_handler_t)(response *deserializedResponse, const uint8_t *serializedResponse, int len);
typedef void (*op_responser_handler_t)(response *response, response_OpResponse *opResponse, int operationNumber);

void responseHandlerDummy(response *deserializedResponse, const uint8_t *serializedResponse, int len);
void opResponseHandlerDummy(response *response, response_OpResponse *opResponse, int operationNumber);

class ResponseUtils {
   private:
    void manage(response *response, op_responser_handler_t op_responser_handler);
    void printResponseData(response *deserializedResponse);
    void printOpResponse(response_OpResponse *opResponse, int operationNumber);

   public:
    static ResponseUtils &getInstance() {
        static ResponseUtils instance;
        return instance;
    }
    response createResponse(request *request);
    void buildOpResponse(response_OpResponse *opResponse, response_Result result_code, const char *payload);
    void manage(const uint8_t *incomingData, int len, response_handler_t response_handler = responseHandlerDummy, op_responser_handler_t op_responser_handler = opResponseHandlerDummy);
    int serializeResponse(uint8_t *buffer, response *response);
    bool deserializeResponse(response *deserializedResponse, const uint8_t *incomingData, int len);
};

#endif