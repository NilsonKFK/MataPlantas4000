#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DHT.h>

extern "C" {
  #include <espnow.h>
}

#define DHTPIN D4
#define DHTTYPE DHT11

#define LED_VERDE D5
#define LED_ROJO D6

DHT dht(DHTPIN, DHTTYPE);

uint8_t macESP3[] = {0xEC, 0x64, 0xC9, 0xC9, 0xBF, 0xE1};

typedef struct struct_message {
  char nodo[10];
  float temperatura;
  float humedad;
  int nivel;
  char estado[20];
  char alerta[30];
} struct_message;

struct_message datos;

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("ENVIO: ");
  Serial.println(sendStatus == 0 ? "OK" : "ERROR");
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);

  dht.begin();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != 0) {
    Serial.println("ERROR ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(macESP3, ESP_NOW_ROLE_COMBO, 1, NULL, 0);

  Serial.println("ESP1 listo");
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  strcpy(datos.nodo, "ESP1");
  datos.nivel = -1;

  if (isnan(h) || isnan(t)) {
    datos.temperatura = 0;
    datos.humedad = 0;
    strcpy(datos.estado, "ERROR");
    strcpy(datos.alerta, "FALLA_DHT11");

    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_ROJO, HIGH);
  } else {
    datos.temperatura = t;
    datos.humedad = h;

    if (t > 30) {
      strcpy(datos.estado, "ALERTA");
      strcpy(datos.alerta, "TEMP_ALTA");
      digitalWrite(LED_VERDE, LOW);
      digitalWrite(LED_ROJO, HIGH);
    } else if (h < 40) {
      strcpy(datos.estado, "ALERTA");
      strcpy(datos.alerta, "HUMEDAD_BAJA");
      digitalWrite(LED_VERDE, LOW);
      digitalWrite(LED_ROJO, HIGH);
    } else {
      strcpy(datos.estado, "NORMAL");
      strcpy(datos.alerta, "OK");
      digitalWrite(LED_VERDE, HIGH);
      digitalWrite(LED_ROJO, LOW);
    }
  }

  Serial.print("ESP1;");
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