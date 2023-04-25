import logging
import os
import traceback

import trio

from messages_pb2 import request, response
from mqtt_helper import MqttAsyncHelper, MqttConfig
from serial_helper import SerialAsyncHelper


class SerialToMqttManager:
    def __init__(self):
        self.serial_helper = SerialAsyncHelper(self.read_from_uart_callback)
        mqtt_config = MqttConfig.Builder(os.environ['GATEWAY_ID'], os.environ['GATEWAY_MAC']) \
            .with_host(os.environ['MQTT_HOST']) \
            .with_port(int(os.environ['MQTT_PORT'])) \
            .with_user(os.environ['MQTT_USER']) \
            .with_password(os.environ['MQTT_PASSWORD']) \
            .build()
        self.mqtt_helper = MqttAsyncHelper(mqtt_config, self.serial_helper)

    async def run_mqtt_and_serial_threads(self):
        try:
            logging.info("Starting mqtt and serial threads")
            async with trio.open_nursery() as nursery:
                nursery.start_soon(self.mqtt_helper.mqtt_loop)
                nursery.start_soon(self.serial_helper.serial_loop)
        except:
            logging.error("Error in trio main process")
            traceback.print_exc()

    def read_from_uart_callback(self, payload):
        logging.debug("++++++++++++++++++++++++++++++++++++++++++++++++++")
        logging.debug(payload)
        request_message = request()
        # request_message.ParseFromString(payload[:-1])
        request_message.ParseFromString(payload)
        logging.debug(request_message)
        response_message = response()
        response_message.from_mac = request_message.to_mac
        response_message.to_mac = request_message.from_mac
        response_message.message_type = request_message.message_type

        for operation in request_message.operations:
            if operation.WhichOneof('op') == "send":
                self.mqtt_helper.publish(request_message, operation.send)
                op_response = response.OpResponse()
                op_response.operation_type = operation.send.operation_type
                op_response.result_code = response.Result.OK
                op_response.payload = ""
                response_message.opResponses.extend([op_response])

            if operation.WhichOneof('op') == "subscribe":
                self.mqtt_helper.subscribe(request_message, operation.subscribe)
                op_response = response.OpResponse()
                op_response.operation_type = operation.subscribe.operation_type
                op_response.result_code = response.Result.SUBSCRIBED_OK
                op_response.payload = ""
                response_message.opResponses.extend([op_response])

            if operation.WhichOneof('op') == "ping":
                op_response = response.OpResponse()
                op_response.operation_type = operation.ping.operation_type
                op_response.result_code = response.Result.OK
                op_response.payload = str(operation.ping.num)
                response_message.opResponses.extend([op_response])
        logging.debug(response_message)
        self.serial_helper.send_serial_message(response_message.SerializeToString())
        logging.debug("--------------------------------------------------")
