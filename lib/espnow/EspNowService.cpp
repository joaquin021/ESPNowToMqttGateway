#include "EspNowService.hpp"

void espNowSendCallBackDummy(const uint8_t *mac_addr, esp_now_send_status_t status) {}
void espNowRecvCallBackDummy(const uint8_t *mac_addr, const uint8_t *data, int data_len) {}

void EspNowService::setup(esp_now_send_cb_t esp_now_send_cb, esp_now_recv_cb_t esp_now_recv_cb) {
    if (esp_now_init() != ESP_OK) {
        debugln("There was an error initializing ESP-NOW");
        return;
    }
    esp_now_register_send_cb(esp_now_send_cb);
    esp_now_register_recv_cb(esp_now_recv_cb);
}

void EspNowService::send(const uint8_t *mac, const uint8_t *outputData, int len) {
    pair(mac);
    esp_err_t result = esp_now_send(mac, outputData, len);
    if (result == ESP_OK) {
        debugln("The message was sent sucessfully via ESP-NOW.");
    } else {
        debugln("There was an error sending via ESP-NOW the response.");
    }
}

void EspNowService::pair(const uint8_t *mac) {
    bool existPeer = esp_now_is_peer_exist(mac);
    if (!existPeer) {
        esp_now_peer_info_t peerInfo;
        memcpy(peerInfo.peer_addr, mac, 6);
        peerInfo.channel = 0;
        peerInfo.encrypt = false;
        if (esp_now_add_peer(&peerInfo) != ESP_OK) {
            debugln("Failed to add peer");
        }
    }
}