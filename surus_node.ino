#include <Arduino.h> 
#include "driver/twai.h" 
 
#define CAN_TX_PIN  GPIO_NUM_25 
#define CAN_RX_PIN  GPIO_NUM_26 
#define POT_GAZ         33 
#define POT_HIZ         35 
#define SURUS_NODE_ID   0x300 
 
void setup() { 
  Serial.begin(115200); 
  analogReadResolution(12); 
 
  twai_general_config_t g = TWAI_GENERAL_CONFIG_DEFAULT(CAN_TX_PIN, 
CAN_RX_PIN, TWAI_MODE_NO_ACK); 
  twai_timing_config_t  t = TWAI_TIMING_CONFIG_500KBITS(); 
  twai_filter_config_t  f = TWAI_FILTER_CONFIG_ACCEPT_ALL(); 
 
  if (twai_driver_install(&g, &t, &f) != ESP_OK) { 
    Serial.println("CAN kurulum hatasi!"); 
    return; 
  } 
  if (twai_start() != ESP_OK) { 
    Serial.println("CAN baslatilamadi!"); 
    return; 
  } 
 
  Serial.println("Surus Node Hazir!"); 
} 
 
void loop() { 
  int gaz = map(analogRead(POT_GAZ), 0, 4095, 0, 100); 
  int hiz = map(analogRead(POT_HIZ), 0, 4095, 0, 150); 
 
  twai_message_t mesaj = {}; 
  mesaj.identifier       = SURUS_NODE_ID; 
  mesaj.data_length_code = 2; 
  mesaj.data[0]          = gaz; 
  mesaj.data[1]          = hiz; 
 
  if (twai_transmit(&mesaj, pdMS_TO_TICKS(10)) == ESP_OK) { 
    Serial.printf("TX BASARILI — Hiz: %d km/h  Gaz: %%%d\n", hiz, gaz); 
  } else { 
    Serial.println("TX HATA!"); 
  } 
 
  delay(500); 
}