#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

// Pines ESP32
#define PIN_POT 34
#define LED_AMARILLO 25

// MAC del ESP3 CENTRAL
uint8_t macESP3[] = {0xEC, 0x64, 0xC9, 0xC9, 0xBF, 0xE1};

// Estructura compatible con ESP1 y ESP3
typedef struct struct_message {
  char nodo[10];
  float temperatura;
  float humedad;
  int nivel;
  char estado[20];
  char alerta[30];
} struct_message;

struct_message datos;

// Callback corregido para ESP32 core nuevo
void OnDataSent(const wifi_tx_info_t *tx_info, esp_now_send_status_t status) {
  Serial.print("ENVIO: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "OK" : "ERROR");
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_AMARILLO, OUTPUT);
  digitalWrite(LED_AMARILLO, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    Serial.println("ERROR ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, macESP3, 6);
  peerInfo.channel = 1;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("ERROR agregando ESP3");
    return;
  }

  Serial.println("ESP2 ESP32 listo");
}

void loop() {
  int lectura = analogRead(PIN_POT);

  int nivel = map(lectura, 0, 4095, 0, 100);
  nivel = constrain(nivel, 0, 100);

  strcpy(datos.nodo, "ESP2");
  datos.temperatura = -1;
  datos.humedad = -1;
  datos.nivel = nivel;

  if (nivel <= 30) {
    strcpy(datos.estado, "ALERTA");
    strcpy(datos.alerta, "NIVEL_BAJO");
    digitalWrite(LED_AMARILLO, HIGH);
  } else {
    strcpy(datos.estado, "NORMAL");
    strcpy(datos.alerta, "OK");
    digitalWrite(LED_AMARILLO, LOW);
  }

  Serial.print("ESP2;");
  Serial.print(datos.temperatura);
  Serial.print(";");
  Serial.print(datos.humedad);
  Serial.print(";");
  Serial.print(datos.nivel);
  Serial.print(";");
  Serial.print(datos.estado);
  Serial.print(";");
  Serial.println(datos.alerta);

  esp_now_send(macESP3, (uint8_t *)&datos, sizeof(datos));

  delay(2000);
}