import time

import trio as trio

from manager import SerialToMqttManager

manager = SerialToMqttManager()


async def loop():
    while True:
        print("Starting main loop")
        await manager.run_mqtt_and_serial_threads()
        time.sleep(5)
        print("Restarting main loop")


if __name__ == '__main__':
    trio.run(loop)
