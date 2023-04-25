import logging

import serial
import trio
from trio_serial import SerialStream

SERIAL_PORT = '/dev/ttyACM0'
# SERIAL_PORT = '/dev/ttyUSB1'
BAUD_RATE = 115200


class SerialAsyncHelper:
    def __init__(self, read_from_uart_callback):
        self.serial_port = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.050)
        self.read_from_uart_callback = read_from_uart_callback

    async def handle_serial_messages(self):
        logging.info("Starting serial message handler")
        async with SerialStream(SERIAL_PORT, baudrate=115200) as ser:
            buffer = b""
            while True:
                received_data = await ser.receive_some(200)
                logging.debug(received_data)
                if received_data:
                    buffer += received_data
                    while b"|" in buffer:
                        message, buffer = buffer.split(b"|", 1)
                        self.read_from_uart_callback(message)

    async def serial_loop(self):
        logging.info("Starting serial loop")
        async with trio.open_nursery() as nursery:
            nursery.start_soon(self.handle_serial_messages)

    def send_serial_message(self, message):
        self.serial_port.write(message)
        self.serial_port.write(b'|')
