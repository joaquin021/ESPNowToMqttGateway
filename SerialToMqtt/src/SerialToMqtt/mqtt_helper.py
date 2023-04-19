import socket

import paho.mqtt.client as mqtt
import trio

MQTT_HOST = '192.168.1.XX'
MQTT_PORT = 1883
MQTT_USER = "XXX"
MQTT_PASS = "XXX"


class MqttAsyncHelper:
    def __init__(self):
        self.client = mqtt.Client("SerialToMqttGateway")
        self.sock = None
        self._event_large_write = trio.Event()

        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_mqtt_message
        self.client.on_disconnect = self.on_disconnect
        self.client.on_socket_open = self.on_socket_open
        self.client.on_socket_register_write = self.on_socket_register_write
        self.client.on_socket_unregister_write = self.on_socket_unregister_write

    async def read_loop(self):
        while True:
            await trio.lowlevel.wait_readable(self.sock)
            self.client.loop_read()

    async def write_loop(self):
        while True:
            await self._event_large_write.wait()
            await trio.lowlevel.wait_writable(self.sock)
            self.client.loop_write()

    async def misc_loop(self):
        print("misc_loop started")
        while self.client.loop_misc() == mqtt.MQTT_ERR_SUCCESS:
            await trio.sleep(1)
        print("misc_loop finished")

    async def mqtt_loop(self):
        print("Starting mqtt loop")
        self.client.username_pw_set(MQTT_USER, MQTT_PASS)
        self.client.connect(MQTT_HOST, MQTT_PORT)
        async with trio.open_nursery() as nursery:
            nursery.start_soon(self.read_loop)
            nursery.start_soon(self.write_loop)
            nursery.start_soon(self.misc_loop)

    def on_socket_open(self, client, userdata, sock):
        print("Socket opened")
        self.sock = sock
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_SNDBUF, 2048)

    def on_socket_register_write(self, client, userdata, sock):
        print('large write request')
        self._event_large_write.set()

    def on_socket_unregister_write(self, client, userdata, sock):
        print("finished large write")
        self._event_large_write = trio.Event()

    def on_connect(self, client, userdata, flags, rc):
        if rc == 0:
            print("MQTT connected")
        else:
            print(f"MQTT connection failed with error code {rc}")

    def on_disconnect(self, client, userdata, rc):
        if rc == mqtt.MQTT_ERR_SUCCESS:
            print("MQTT disconnected")
        else:
            print(f"MQTT disconnection failed with error code {rc}")

    def on_mqtt_message(self, client, userdata, msg):
        print("message: " + str(msg.payload))

    def send_mqtt_message(self, client_id, topic, payload, qos=0):
        topic = "ESPNowToMqttGateway/" + client_id + "/" + topic
        self.client.publish(topic, payload, qos=qos)

    def subscribe_mqtt_queue(self, client_id, topic):
        topic = "ESPNowToMqttGateway/" + client_id + "/" + topic
        self.client.subscribe(topic)
