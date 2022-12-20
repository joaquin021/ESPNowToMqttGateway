#include "ResponseUtils.hpp"

void printMacAndLenPacketResponse(const uint8_t *mac, int len) {
    char macStr[18 + 1 + 4];  // 18 mac + 1 space + 3 len
    debug("Packet to: ");
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x %db",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], len);
    debugln(macStr);
}

void printResponse(response *deserializedResponse, int len) {
    printMacAndLenPacketResponse(deserializedResponse->client_mac, len);
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

uint8_t sendResponseViaUart(response *response) {
    uint8_t serializedBuffer[ESPNOW_BUFFERSIZE];
    int messageLength = serializeResponse(serializedBuffer, response);
    Serial2.write(serializedBuffer, messageLength);
    Serial2.write(END_TX_CHAR);
    return messageLength;
}