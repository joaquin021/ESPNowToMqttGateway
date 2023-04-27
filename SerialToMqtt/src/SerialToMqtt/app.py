import logging
import os
import time

import trio as trio

from manager import SerialToMqttManager

LOGGER = logging.getLogger("Main")


async def loop():
    while True:
        LOGGER.info("Starting main loop")
        manager = SerialToMqttManager()
        await manager.run_mqtt_and_serial_threads()
        del manager
        time.sleep(5)
        LOGGER.warning("Restarting main loop")


if __name__ == '__main__':
    logging.basicConfig(level=os.environ.get("LOG_LEVEL", "DEBUG"),
                        format='%(asctime)s :: %(levelname)s :: %(name)s :: %(message)s')
    trio.run(loop)
