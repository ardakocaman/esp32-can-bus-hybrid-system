#include <Arduino.h> 
#include "driver/twai.h" 
 
#define CAN_TX_PIN    GPIO_NUM_25 
#define CAN_RX_PIN    GPIO_NUM_26 
#define POT_SICAKLIK    33 
#define POT_BATARYA     35 
#define BATARYA_NODE_ID 0x200 
 
void setup() { 
  Serial.begin(115200); 
  analogReadResolution(12); 
 
  twai_general_config_t g = TWAI_GENERAL_CONFIG_DEFAULT(CAN_TX_PIN, 
CAN_RX_PIN, TWAI_MODE_NORMAL); 
  twai_timing_config_t  t = TWAI_TIMING_CONFIG_500KBITS(); 
  twai_filter_config_t  f = TWAI_FILTER_CONFIG_ACCEPT_ALL(); 
 
  ESP_ERROR_CHECK(twai_driver_install(&g, &t, &f)); 
  ESP_ERROR_CHECK(twai_start()); 
 
  Serial.println("Batarya Node Hazir!"); 
} 
 
void loop() { 
  int sicaklik = map(analogRead(POT_SICAKLIK), 0, 4095, 0, 80); 
  int batarya  = map(analogRead(POT_BATARYA),  0, 4095, 0, 100); 
 
  twai_message_t mesaj = {}; 
  mesaj.identifier       = BATARYA_NODE_ID; 
  mesaj.data_length_code = 2; 
  mesaj.data[0]          = sicaklik; 
  mesaj.data[1]          = batarya; 
 
  twai_transmit(&mesaj, pdMS_TO_TICKS(10)); 
 
  Serial.printf("Sicaklik: %d°C  |  Batarya: %%%d\n", sicaklik, batarya); 
 
  delay(500); 
}