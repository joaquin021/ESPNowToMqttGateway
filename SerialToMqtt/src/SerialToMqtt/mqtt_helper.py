import socket
import time

import paho.mqtt.client as mqtt
from messages_pb2 import request, response
import trio
from serial_helper import SerialAsyncHelper


class MqttConfig:
    def __init__(self, builder: 'MqttConfig.Builder'):
        self.gateway_id = builder.gateway_id
        self.gateway_mac = builder.gateway_mac
        self.host = builder.host
        self.port = builder.port
        self.user = builder.user
        self.password = builder.password

    class Builder:
        def __init__(self, gateway_id: str, gateway_mac: str):
            self.gateway_id = gateway_id
            self.gateway_mac = gateway_mac
            self.host = None
            self.port = None
            self.user = None
            self.password = None

        def with_host(self, host: str):
            self.host = host
            return self

        def with_port(self, port: int):
            self.port = port
            return self

        def with_user(self, user: str):
            self.user = user
            return self

        def with_password(self, password: str):
            self.password = password
            return self

        def build(self):
            return MqttConfig(self)


class MqttAsyncHelper:
    def __init__(self, mqtt_config: MqttConfig, serial_helper: SerialAsyncHelper):
        self.mqtt_config = mqtt_config
        self.client = mqtt.Client(self.mqtt_config.gateway_id)
        self.subscriptions = {}
        self.sock = None
        self._event_large_write = trio.Event()
        self.serial_helper = serial_helper

        self.client.on_connect = self._on_connect
        self.client.on_message = self._on_mqtt_message
        self.client.on_disconnect = self._on_disconnect
        self.client.on_socket_open = self._on_socket_open
        self.client.on_socket_register_write = self._on_socket_register_write
        self.client.on_socket_unregister_write = self._on_socket_unregister_write

    async def _read_loop(self):
        while True:
            await trio.lowlevel.wait_readable(self.sock)
            self.client.loop_read()

    async def _write_loop(self):
        while True:
            await self._event_large_write.wait()
            await trio.lowlevel.wait_writable(self.sock)
            self.client.loop_write()

    async def _misc_loop(self):
        print("misc_loop started")
        while self.client.loop_misc() == mqtt.MQTT_ERR_SUCCESS:
            await trio.sleep(1)
        print("misc_loop finished")

    async def mqtt_loop(self):
        print("Starting mqtt loop")
        self.client.username_pw_set(self.mqtt_config.user, self.mqtt_config.password)
        self.client.connect(self.mqtt_config.host, self.mqtt_config.port)
        async with trio.open_nursery() as nursery:
            nursery.start_soon(self._read_loop)
            nursery.start_soon(self._write_loop)
            nursery.start_soon(self._misc_loop)

    def _on_socket_open(self, client, userdata, sock):
        print("Socket opened")
        self.sock = sock
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_SNDBUF, 2048)

    def _on_socket_register_write(self, client, userdata, sock):
        print('large write request')
        self._event_large_write.set()

    def _on_socket_unregister_write(self, client, userdata, sock):
        print("finished large write")
        self._event_large_write = trio.Event()

    def _on_connect(self, client, userdata, flags, rc):
        if rc == 0:
            print("MQTT connected")
        else:
            print(f"MQTT connection failed with error code {rc}")

    def _on_disconnect(self, client, userdata, rc):
        if rc == mqtt.MQTT_ERR_SUCCESS:
            print("MQTT disconnected")
        else:
            print(f"MQTT disconnection failed with error code {rc}")

    def _on_mqtt_message(self, client, userdata, message):
        print("Received message '" + str(message.payload) + "' on topic '"
              + message.topic + "' with QoS " + str(message.qos))
        if message.topic in self.subscriptions:
            for mac, subscription_client_data in self.subscriptions[message.topic].items():
                response_message = response()
                response_message.from_mac = bytes.fromhex(self.mqtt_config.gateway_mac)
                response_message.to_mac = bytes.fromhex(mac)
                response_message.message_type = subscription_client_data["message_type"]
                op_response = response.OpResponse()
                op_response.operation_type = subscription_client_data["operation_type"]
                op_response.result_code = response.Result.OK
                op_response.payload = message.payload
                response_message.opResponses.extend([op_response])
                print(response_message)
                self.serial_helper.send_serial_message(response_message.SerializeToString())

    def publish(self, request_message: request, send_operation: request.Send):
        self.client.publish(self.build_topic(request_message.client_id, send_operation.queue), send_operation.payload,
                            qos=1, retain=send_operation.persist)

    def subscribe(self, request_message: request, subscribe_operation: request.Subscribe):
        topic = self.build_topic(request_message.client_id, subscribe_operation.queue)
        if topic not in self.subscriptions:
            self.client.subscribe(self.build_topic(request_message.client_id, subscribe_operation.queue))
            self.subscriptions[topic] = {}
        if request_message.from_mac.hex() not in self.subscriptions[topic]:
            self._register_subscription(topic, request_message, subscribe_operation)

    def _register_subscription(self, topic: str, request_message: request, subscribe_operation: request.Subscribe):
        self.subscriptions[topic][request_message.from_mac.hex()] = {
            "message_type": request_message.message_type,
            "operation_type": subscribe_operation.operation_type,
            "subscribed_at": int(time.time() * 1000.0)
        }
        print(self.subscriptions)

    def build_topic(self, client_id: str, queue: str) -> str:
        return "{}/{}/{}".format(self.mqtt_config.gateway_id, client_id, queue)
