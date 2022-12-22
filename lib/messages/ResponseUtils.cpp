#include "ResponseUtils.hpp"

void ResponseUtils::manage(const uint8_t *incomingData, int len, response_handler_t response_handler, op_responser_handler_t op_responser_handler) {
    response deserializedResponse = response_init_zero;
    bool deserialized = deserialize(&deserializedResponse, incomingData, len);
    if (deserialized) {
        printMacAndLenPacket(deserializedResponse.client_mac, len, "Packet to: ");
        manage(&deserializedResponse, op_responser_handler);
        response_handler(&deserializedResponse, incomingData, len);
    } else {
        debugln("Error deserializing response.");
    }
}

void ResponseUtils::manage(response *response, op_responser_handler_t op_responser_handler) {
    printResponseData(response);
    for (int count = 0; count < response->opResponses_count; count++) {
        response_OpResponse opResponse = response->opResponses[count];
        printOpResponse(&opResponse, count);
        op_responser_handler(response, &opResponse, count);
    }
}

response ResponseUtils::createResponse(request *request) {
    response response = response_init_zero;
    response.opResponses_count = request->operations_count;
    memcpy(response.client_mac, request->client_mac, sizeof(request->client_mac));
    response.message_type = request->message_type;
    return response;
}

void ResponseUtils::buildOpResponse(response_OpResponse *opResponse, response_Result result_code, const char *payload) {
    opResponse->result_code = result_code;
    if (payload) {
        strlcpy(opResponse->payload, payload, sizeof(opResponse->payload));
    }
}

int ResponseUtils::serialize(uint8_t *buffer, response *response) {
    pb_ostream_t myStream = pb_ostream_from_buffer(buffer, ESPNOW_BUFFERSIZE);
    pb_encode(&myStream, response_fields, response);
    return myStream.bytes_written;
}

bool ResponseUtils::deserialize(response *deserializedResponse, const uint8_t *incomingData, int len) {
    pb_istream_t iStream = pb_istream_from_buffer(incomingData, len);
    return pb_decode(&iStream, response_fields, deserializedResponse);
}

void ResponseUtils::printResponseData(response *deserializedResponse) {
    debug("Operations: ");
    debugln(deserializedResponse->opResponses_count);
    debug("MessageType: ");
    debugln(deserializedResponse->message_type);
}

void ResponseUtils::printOpResponse(response_OpResponse *opResponse, int operationNumber) {
    debugln("> Operation " + String(operationNumber));
    debug(">>> Result Code: ");
    debugln(opResponse->result_code);
    debug(">>> Payload: ");
    debugln(opResponse->payload);
}

void responseHandlerDummy(response *deserializedResponse, const uint8_t *serializedResponse, int len) {}
void opResponseHandlerDummy(response *response, response_OpResponse *opResponse, int operationNumber) {}