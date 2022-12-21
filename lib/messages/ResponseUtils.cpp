#include "ResponseUtils.hpp"

void printResponse(response *deserializedResponse, int len) {
    printMacAndLenPacket(deserializedResponse->client_mac, len, "Packet to: ");
    debug("Operations: ");
    debugln(deserializedResponse->opResponses_count);
    debug("MessageType: ");
    debugln(deserializedResponse->message_type);
    for (int count = 0; count < deserializedResponse->opResponses_count; count++) {
        response_OpResponse opResponse = deserializedResponse->opResponses[count];
        debugln("> Operation " + String(count));
        debug(">>> Result Code: ");
        debugln(opResponse.result_code);
        debug(">>> Payload: ");
        debugln(opResponse.payload);
    }
}

response createResponse(request *request) {
    response response = response_init_zero;
    response.opResponses_count = request->operations_count;
    memcpy(response.client_mac, request->client_mac, sizeof(request->client_mac));
    response.message_type = request->message_type;
    return response;
}

void buildResponse(response_OpResponse *opResponse, response_Result result_code, const char *payload) {
    opResponse->result_code = result_code;
    if (payload) {
        strlcpy(opResponse->payload, payload, sizeof(opResponse->payload));
    }
}

bool deserializeResponse(response *deserializedResponse, const uint8_t *incomingData, int len) {
    pb_istream_t iStream = pb_istream_from_buffer(incomingData, len);
    return pb_decode(&iStream, response_fields, deserializedResponse);
}

int serializeResponse(uint8_t *buffer, response *response) {
    pb_ostream_t myStream = pb_ostream_from_buffer(buffer, ESPNOW_BUFFERSIZE);
    pb_encode(&myStream, response_fields, response);
    return myStream.bytes_written;
}