#include "RequestUtils.hpp"

response manageMessageRequest(const uint8_t *incomingData, int len,
                              request_handler_t request_handler,
                              send_op_handler_t send_op_handler,
                              subscribe_op_handler_t subscribe_op_handler,
                              ping_op_handler_t ping_op_handler) {
    request deserializedRequest = request_init_default;
    bool deserialized = deserializeRequest(&deserializedRequest, incomingData, len);
    response response = createResponse(&deserializedRequest);

    if (deserialized) {
        printMacAndLenPacket(deserializedRequest.client_mac, len, "Packet received from: ");
        request_handler(&deserializedRequest);

        for (int count = 0; count < deserializedRequest.operations_count; count++) {
            pb_size_t which_op = deserializedRequest.operations[count].which_op;
            response_OpResponse *opResponse = &response.opResponses[count];

            switch (which_op) {
                case request_Operation_send_tag:
                    send_op_handler(&deserializedRequest.operations[count].op.send, deserializedRequest.client_id, opResponse);
                    break;
                case request_Operation_qRequest_tag:
                    subscribe_op_handler(&deserializedRequest.operations[count].op.qRequest, deserializedRequest.client_id, opResponse);
                    break;
                case request_Operation_ping_tag:
                    ping_op_handler(&deserializedRequest.operations[count].op.ping, opResponse);
                    break;
                default:
                    printUnknownOperation(which_op);
                    break;
            }
        }
    } else {
        debugln("Error deserializing message.");
    }
    return response;
}

bool deserializeRequest(request *deserializedRequest, const uint8_t *incomingData, int len) {
    pb_istream_t iStream = pb_istream_from_buffer(incomingData, len);
    return pb_decode(&iStream, request_fields, deserializedRequest);
}

void printRequestData(request *request) {
    debug("Client: ");
    debugln(request->client_id);
    debug("Operations: ");
    debugln(request->operations_count);
    debug("MessageType: ");
    debugln(request->message_type);
}

void printSendOperation(request_Send *send) {
    debugln("> Send operation");
    debug(">>> Queue: ");
    debugln(send->queue);
    debug(">>> Payload: ");
    debugln(send->payload);
    debug(">>> Persist: ");
    debugln(send->persist);
}

void printSubscribeOperation(request_Subscribe *subscribe) {
    debugln("> Subscribe operation");
    debug(">>> Queue: ");
    debugln(subscribe->queue);
    debug(">>> Clear: ");
    debugln(subscribe->clear);
}

void printPingOperation(request_Ping *ping) {
    debugln("> Ping operation");
    debug(">>> Num: ");
    debugln(ping->num);
}

void printUnknownOperation(pb_size_t which_op) {
    debug("> Unknown: ");
    debugln(which_op);
}

void requestHandlerDummy(request *request) {}
void sendOpHandlerDummy(request_Send *sendOp, char *clientId, response_OpResponse *opResponse) {}
void subscribeOpHandlerDummy(request_Subscribe *subscribeOp, response_OpResponse *opResponse) {}
void pingOpHandlerDummy(request_Ping *pingOp, response_OpResponse *opResponse) {}
