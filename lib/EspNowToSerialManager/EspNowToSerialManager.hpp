#ifndef __ESP_NOW_TO_SERIAL_MANAGER_HPP
#define __ESP_NOW_TO_SERIAL_MANAGER_HPP

#include "Commons.hpp"
#include "EspNowService.hpp"
#include "UartHandler.hpp"
#include "WifiUtils.hpp"

class EspNowToSerialManager {
   private:
    static EspNowToSerialManager *instance;
    static EspNowToSerialManager *getInstance() { return instance; }
    EspNowService espNowService;
    friend void serialDataHandler(const uint8_t *incomingData, int len);
    friend void responseHandler(response *deserializedResponse, const uint8_t *serializedResponse, int len);
    friend void espNowRecvCallBack(const uint8_t *mac, const uint8_t *incomingData, int len);

   public:
    EspNowToSerialManager();
    void setup();
    void loop();
};

#endif