import logging
import os
import time

import trio as trio

from manager import SerialToMqttManager

manager = SerialToMqttManager()


async def loop():
    while True:
        logging.info("Starting main loop")
        await manager.run_mqtt_and_serial_threads()
        time.sleep(5)
        logging.warning("Restarting main loop")


if __name__ == '__main__':
    logging.basicConfig(level=os.environ.get("LOG_LEVEL", "WARNING"))
    trio.run(loop)
