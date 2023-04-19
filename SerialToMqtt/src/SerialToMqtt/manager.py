import traceback

import trio

from messages_pb2 import request, response
from mqtt_helper import MqttAsyncHelper
from serial_helper import SerialAsyncHelper


class SerialToMqttManager:
    def __init__(self):
        self.mqtt_helper = MqttAsyncHelper()
        self.serial_helper = SerialAsyncHelper(self.read_from_uart_callback)

    async def run_mqtt_and_serial_threads(self):
        try:
            print("Starting mqtt and serial threads")
            async with trio.open_nursery() as nursery:
                nursery.start_soon(self.mqtt_helper.mqtt_loop)
                nursery.start_soon(self.serial_helper.serial_loop)
        except:
            print("Error in trio main process")
            traceback.print_exc()

    def read_from_uart_callback(self, payload):
        print("++++++++++++++++++++++++++++++++++++++++++++++++++")
        print(payload)
        request_message = request()
        # request_message.ParseFromString(payload[:-1])
        request_message.ParseFromString(payload)
        print(request_message)
        response_message = response()
        response_message.from_mac = request_message.to_mac
        response_message.to_mac = request_message.from_mac
        response_message.message_type = request_message.message_type

        for operation in request_message.operations:
            if operation.WhichOneof('op') == "send":
                # print(operation.send)
                self.mqtt_helper.send_mqtt_message(request_message.client_id, operation.send.queue,
                                                   operation.send.payload)
                op_response = response.OpResponse()
                op_response.operation_type = operation.send.operation_type
                op_response.result_code = response.Result.OK
                op_response.payload = ""
                response_message.opResponses.extend([op_response])

            if operation.WhichOneof('op') == "subscribe":
                # print(operation.subscribe)
                self.mqtt_helper.subscribe_mqtt_queue(request_message.client_id, operation.subscribe.queue)
                op_response = response.OpResponse()
                op_response.operation_type = operation.subscribe.operation_type
                op_response.result_code = response.Result.SUBSCRIBED_OK
                op_response.payload = ""
                response_message.opResponses.extend([op_response])

            if operation.WhichOneof('op') == "ping":
                # print(operation.ping)
                op_response = response.OpResponse()
                op_response.operation_type = operation.ping.operation_type
                op_response.result_code = response.Result.OK
                op_response.payload = str(operation.ping.num)
                response_message.opResponses.extend([op_response])
        print(response_message)
        self.serial_helper.send_serial_message(response_message.SerializeToString())
        print("--------------------------------------------------")
