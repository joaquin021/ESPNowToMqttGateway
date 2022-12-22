#include "RequestUtils.hpp"

response RequestUtils::manage(const uint8_t *incomingData, int len,
                              request_handler_t request_handler, send_op_handler_t send_op_handler, subscribe_op_handler_t subscribe_op_handler, ping_op_handler_t ping_op_handler) {
    request deserializedRequest = request_init_default;
    response response = response_init_zero;
    bool deserialized = deserialize(&deserializedRequest, incomingData, len);

    if (deserialized) {
        printMacAndLenPacket(deserializedRequest.client_mac, len, "Packet received from: ");
        response = manage(&deserializedRequest, send_op_handler, subscribe_op_handler, ping_op_handler);
        request_handler(&deserializedRequest, incomingData, len, &response);
    } else {
        debugln("Error deserializing request.");
    }
    return response;
}

response RequestUtils::manage(request *request,
                              send_op_handler_t send_op_handler, subscribe_op_handler_t subscribe_op_handler, ping_op_handler_t ping_op_handler) {
    printRequestData(request);
    response response = ResponseUtils::getInstance().createResponse(request);
    for (int count = 0; count < request->operations_count; count++) {
        pb_size_t which_op = request->operations[count].which_op;
        response_OpResponse *opResponse = &response.opResponses[count];

        switch (which_op) {
            case request_Operation_send_tag:
                printSendOperation(&request->operations[count].op.send);
                send_op_handler(request, &request->operations[count].op.send, opResponse);
                break;
            case request_Operation_qRequest_tag:
                printSubscribeOperation(&request->operations[count].op.qRequest);
                subscribe_op_handler(request, &request->operations[count].op.qRequest, opResponse);
                break;
            case request_Operation_ping_tag:
                printPingOperation(&request->operations[count].op.ping);
                ping_op_handler(request, &request->operations[count].op.ping, opResponse);
                break;
            default:
                printUnknownOperation(which_op);
                break;
        }
    }
    return response;
}

bool RequestUtils::deserialize(request *deserializedRequest, const uint8_t *incomingData, int len) {
    pb_istream_t iStream = pb_istream_from_buffer(incomingData, len);
    return pb_decode(&iStream, request_fields, deserializedRequest);
}

void RequestUtils::printRequestData(request *request) {
    debug("Client: ");
    debugln(request->client_id);
    debug("Operations: ");
    debugln(request->operations_count);
    debug("MessageType: ");
    debugln(request->message_type);
}

void RequestUtils::printSendOperation(request_Send *send) {
    debugln("> Send operation");
    debug(">>> Queue: ");
    debugln(send->queue);
    debug(">>> Payload: ");
    debugln(send->payload);
    debug(">>> Persist: ");
    debugln(send->persist);
}

void RequestUtils::printSubscribeOperation(request_Subscribe *subscribe) {
    debugln("> Subscribe operation");
    debug(">>> Queue: ");
    debugln(subscribe->queue);
    debug(">>> Clear: ");
    debugln(subscribe->clear);
}

void RequestUtils::printPingOperation(request_Ping *ping) {
    debugln("> Ping operation");
    debug(">>> Num: ");
    debugln(ping->num);
}

void RequestUtils::printUnknownOperation(pb_size_t which_op) {
    debug("> Unknown: ");
    debugln(which_op);
}

void requestHandlerDummy(request *deserializedRequest, const uint8_t *serializedRequest, int len, response *response) {}
void sendOpHandlerDummy(request *request, request_Send *sendOp, response_OpResponse *opResponse) {}
void subscribeOpHandlerDummy(request *request, request_Subscribe *subscribeOp, response_OpResponse *opResponse) {}
void pingOpHandlerDummy(request *request, request_Ping *pingOp, response_OpResponse *opResponse) {}