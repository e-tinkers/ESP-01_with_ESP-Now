// esp8266-espnow-receiver.ino
// This code works for ESP8266 only.
// ESP-NOW for ESP32 has different API from the ESP8266 implementation

#include <ESP8266WiFi.h>
#include <espnow.h>

#define RETRY_INTERVAL 5000

// the following 2 settings must match transmitter's settings
uint8_t mac[] = {0x82, 0x88, 0x88, 0x88, 0x88, 0x88};
const uint8_t channel = 14;
struct __attribute__((packed)) DataStruct {
  float temperature;
  float humidity;
};

DataStruct myData;

void initVariant() {

}

void receiveCallBackFunction(uint8_t *senderMac, uint8_t *incomingData, uint8_t len) {
  memcpy(&myData, incomingData, len);
  Serial.printf("Transmitter MacAddr: %02x:%02x:%02x:%02x:%02x:%02x, ", senderMac[0], senderMac[1], senderMac[2], senderMac[3], senderMac[4], senderMac[5]);
  Serial.printf("Temperature: %.2f, ", myData.temperature);
  Serial.printf("Humidity: %.2f, ", myData.humidity);
}

void setup() {
  WiFi.mode(WIFI_AP);
  wifi_set_macaddr(SOFTAP_IF, &mac[0]);
  WiFi.disconnect();
  
  Serial.begin(74880);
  Serial.println();
  Serial.println("ESP-Now Receiver");
  Serial.printf("Transmitter mac: %s\n", WiFi.macAddress().c_str());
  Serial.printf("Receiver mac: %s\n", WiFi.softAPmacAddress().c_str());
  if (esp_now_init() != 0) {
    Serial.println("ESP_Now init failed...");
    delay(RETRY_INTERVAL);
    ESP.restart();
  }
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(receiveCallBackFunction);
  Serial.println("Slave ready. Waiting for messages...");
}

void loop() {

}
