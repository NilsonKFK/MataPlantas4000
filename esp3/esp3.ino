#include <Arduino.h>
#include <ESP8266WiFi.h>

extern "C" {
  #include <espnow.h>
}

#define LED_MOTOR D5

typedef struct struct_message {
  char nodo[10];
  float temperatura;
  float humedad;
  int nivel;
  char estado[20];
  char alerta[30];
} struct_message;

struct_message datos;

bool motorEncendido = false;

void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&datos, incomingData, sizeof(datos));

  Serial.print("DATA;");
  Serial.print(datos.nodo);
  Serial.print(";");
  Serial.print(datos.temperatura);
  Serial.print(";");
  Serial.print(datos.humedad);
  Serial.print(";");
  Serial.print(datos.nivel);
  Serial.print(";");
  Serial.print(datos.estado);
  Serial.print(";");
  Serial.print(datos.alerta);
  Serial.print(";");
  Serial.println(motorEncendido ? "ENCENDIDO" : "APAGADO");
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_MOTOR, OUTPUT);
  digitalWrite(LED_MOTOR, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != 0) {
    Serial.println("ERROR ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("ESP3 CENTRAL listo");
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
  if (Serial.available()) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();

    if (comando == "MOTOR_ON") {
      motorEncendido = true;
      digitalWrite(LED_MOTOR, HIGH);
      Serial.println("COMANDO;MOTOR;ENCENDIDO");
    }

    if (comando == "MOTOR_OFF") {
      motorEncendido = false;
      digitalWrite(LED_MOTOR, LOW);
      Serial.println("COMANDO;MOTOR;APAGADO");
    }
  }
}