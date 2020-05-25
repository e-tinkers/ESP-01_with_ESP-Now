// esp8266-espnow-transmitter.ino
// This code works for ESP8266 only.
// ESP-NOW for ESP32 has different API from the ESP8266 implementation

#include <ESP8266WiFi.h>
#include <espnow.h>

#define RETRY_INTERVAL 5000
#define SEND_INTERVAL 1000 

// Use the following pattern to create a Locally Administered MAC Address
//   x2-xx-xx-xx-xx-xx
//   x6-xx-xx-xx-xx-xx
//   xA-xx-xx-xx-xx-xx
//   xE-xx-xx-xx-xx-xx
// replace x with any hex value

// the following three settings must match the slave settings
uint8_t remoteMac[] = {0x82, 0x88, 0x88, 0x88, 0x88, 0x88};
const uint8_t channel = 14;
struct __attribute__((packed)) DataStruct {
  float temperature;
  float humidity;
};

DataStruct myData;

unsigned long sentStartTime;
unsigned long lastSentTime;

void sendData() {
  uint8_t bs[sizeof(myData)];
  memcpy(bs, &myData, sizeof(myData));
  
  sentStartTime = micros();
  esp_now_send(NULL, bs, sizeof(myData)); // NULL means send to all peers
}

void sendCallBackFunction(uint8_t* mac, uint8_t sendStatus) {
  unsigned long sentEndTime = micros();
  Serial.printf("Send To: %02x:%02x:%02x:%02x:%02x:%02x ", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.printf("Temperature: %.2f ", myData.temperature);
  Serial.printf("Humidity: %.2f ", myData.humidity);
  Serial.printf("Trip micros: %4lu, ", sentEndTime - sentStartTime);
  Serial.printf("Status: %s\n", (sendStatus == 0 ? "Success" : "Failed"));
}

void setup() {
  WiFi.mode(WIFI_STA); // Station mode for esp-now controller
  WiFi.disconnect();

  Serial.begin(74880);
  Serial.println();
  Serial.println("ESP-Now Transmitter");
  Serial.printf("Transmitter mac: %s \n", WiFi.macAddress().c_str());
  Serial.printf("Receiver mac: %02x:%02x:%02x:%02x:%02x:%02x\n", remoteMac[0], remoteMac[1], remoteMac[2], remoteMac[3], remoteMac[4], remoteMac[5]);
  Serial.printf("WiFi Channel: %i\n", channel);

  if (esp_now_init() != 0) {
    Serial.println("ESP_Now init failed...");
    delay(RETRY_INTERVAL);
    ESP.restart();
  }
    
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_add_peer(remoteMac, ESP_NOW_ROLE_SLAVE, channel, NULL, 0);
  esp_now_register_send_cb(sendCallBackFunction);
}

void loop() {
  if (millis() - lastSentTime >= SEND_INTERVAL) {
    lastSentTime += SEND_INTERVAL;
    myData.temperature = 32.3;   // replace this with your actual sensor reading code
    myData.humidity = 70.8;      // replace this with your actual sensor reading code
    sendData();  
  }
}
