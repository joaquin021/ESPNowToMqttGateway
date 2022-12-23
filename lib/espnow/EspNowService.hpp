#ifndef __ESP_NOW_SERVICE_HPP
#define __ESP_NOW_SERVICE_HPP

#include <esp_now.h>

#include "Commons.hpp"

void espNowSendCallBackDummy(const uint8_t *mac_addr, esp_now_send_status_t status);
void espNowRecvCallBackDummy(const uint8_t *mac_addr, const uint8_t *data, int data_len);

class EspNowService {
   private:
    void pair(const uint8_t *mac);

   public:
    EspNowService() {}
    void setup(esp_now_send_cb_t esp_now_send_cb = espNowSendCallBackDummy, esp_now_recv_cb_t esp_now_recv_cb = espNowRecvCallBackDummy);
    void send(const uint8_t *mac, const uint8_t *outputData, int len);
};

#endif