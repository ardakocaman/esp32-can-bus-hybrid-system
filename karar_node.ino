#include <Arduino.h>
#include "driver/twai.h"

#define CAN_TX_PIN GPIO_NUM_25
#define CAN_RX_PIN GPIO_NUM_26
#define LED_YESIL   19
#define LED_KIRMIZI 18

#define HIZ_DUSUK          30
#define GAZ_DUSUK          40
#define BATARYA_KRITIK     15
#define BATARYA_DUSUK      30
#define SICAKLIK_MAX       60
#define HIZ_SINIRI         120

#define KARAR_NODE_ID      0x400

int batarya_sicaklik = 0;
int batarya          = 0;
int gaz              = 0;
int hiz              = 0;

#define SADECE_EV   0
#define SADECE_IC   1
#define EV_VE_IC    2

int motorKarari() {
  if (batarya_sicaklik > SICAKLIK_MAX) {
    Serial.println("Batarya cok sicak — sadece icten yanmali motor calisiyor.");
    return SADECE_IC;
  }
  if (batarya < BATARYA_KRITIK) {
    Serial.println("Batarya doluluk orani kritik — sadece icten yanmali motor calisiyor.");
    return SADECE_IC;
  }
  if (hiz > HIZ_SINIRI) {
    Serial.println("Yuksek hiz — sadece icten yanmali motor calisiyor.");
    return SADECE_IC;
  }
  if (hiz < HIZ_DUSUK && gaz < GAZ_DUSUK) {
    Serial.println("Dusuk hiz ve gaz — sadece elektrikli motor calisiyor.");
    return SADECE_EV;
  }
  Serial.println("Normal seyir — elektrikli ve icten yanmali motor ayni anda calisiyor.");
  return EV_VE_IC;
}

void ledleriGuncelle(int karar) {
  if (karar == SADECE_EV) {
    digitalWrite(LED_YESIL,   HIGH);
    digitalWrite(LED_KIRMIZI, LOW);
  } else if (karar == SADECE_IC) {
    digitalWrite(LED_YESIL,   LOW);
    digitalWrite(LED_KIRMIZI, HIGH);
  } else {
    digitalWrite(LED_YESIL,   HIGH);
    digitalWrite(LED_KIRMIZI, HIGH);
  }
}

void mesajlariDinle() {
  twai_message_t mesaj;
  while (twai_receive(&mesaj, 0) == ESP_OK) {
    if (mesaj.identifier == 0x200) {
      batarya_sicaklik = mesaj.data[0];
      batarya          = mesaj.data[1];
      Serial.printf("[BATARYA] Sicaklik: %d°C Batarya doluluk orani: %%%d\n",
                    batarya_sicaklik, batarya);
    }
    if (mesaj.identifier == 0x300) {
      gaz = mesaj.data[0];
      hiz = mesaj.data[1];
      Serial.printf("[SURUS] Gaz: %%%d Hiz: %d km/h\n", gaz, hiz);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_YESIL,   OUTPUT);
  pinMode(LED_KIRMIZI, OUTPUT);
  digitalWrite(LED_YESIL,   LOW);
  digitalWrite(LED_KIRMIZI, LOW);

  twai_general_config_t g = TWAI_GENERAL_CONFIG_DEFAULT(CAN_TX_PIN,
CAN_RX_PIN, TWAI_MODE_NORMAL);
  twai_timing_config_t t = TWAI_TIMING_CONFIG_500KBITS();
  twai_filter_config_t f = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  ESP_ERROR_CHECK(twai_driver_install(&g, &t, &f));
  ESP_ERROR_CHECK(twai_start());
  Serial.println("Karar Node Hazir! ID: 0x400");
}

void loop() {
  mesajlariDinle();
  int karar = motorKarari();
  ledleriGuncelle(karar);
  delay(500);
}
