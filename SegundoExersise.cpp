
// --- Configuración Blynk ---
#define BLYNK_TEMPLATE_ID "TMPL2fz-2rgW-"
#define BLYNK_TEMPLATE_NAME "ESP32 DTH11"
#define BLYNK_AUTH_TOKEN "5hlSOBUb5HZiaYSdLMSGWy3Q2FllaS-U"

#define BLYNK_PRINT Serial   //  Esto habilita debug de Blynk

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include "DHT.h"

// --- Configuración DHT11 ---
#define DHTPIN 4           // Pin donde conectaste DATA del DHT11
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

char ssid[] = "Name";    // tu red WiFi
char pass[] = "Password"; // tu clave WiFi

BlynkTimer timer;

// --- Función que se llama periódicamente para leer y enviar datos ---
void sendSensor() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠ WiFi no conectado, no se envían datos.");
    return;
  }
  if (!Blynk.connected()) {
    Serial.println("⚠ Blynk no conectado, no se envían datos.");
    return;
  }

  float h = dht.readHumidity();
  float t = dht.readTemperature(); // Celsius

  if (isnan(h) || isnan(t)) {
    Serial.println("❌ Error leyendo del DHT11");
    return;
  }

  Serial.print("🌡 Temp: "); Serial.print(t); Serial.print(" °C ");
  Serial.print("💧 Hum: "); Serial.print(h); Serial.println(" %");

  // Enviar a Blynk
  Blynk.virtualWrite(V0, t);  // Temperatura
  Blynk.virtualWrite(V1, h);  // Humedad
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Conexión WiFi
  WiFi.begin(ssid, pass);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi conectado");
  Serial.print("IP local: ");
  Serial.println(WiFi.localIP());

  // Probar si hay Internet (resolver blynk.cloud)
  IPAddress ip;
  if (WiFi.hostByName("blynk.cloud", ip)) {
    Serial.print("🌍 blynk.cloud resuelto → ");
    Serial.println(ip);
  } else {
    Serial.println("❌ No se pudo resolver blynk.cloud (sin internet)");
  }

  // Configurar conexión con Blynk Cloud
  Blynk.config(BLYNK_AUTH_TOKEN);

  if (Blynk.connect()) {
    Serial.println("✅ Conectado a Blynk Cloud");
  } else {
    Serial.println("❌ No se pudo conectar a Blynk");
  }

  // Intervalo de envío cada 5 segundos (ajústalo a 2min = 120000L en uso real)
  timer.setInterval(5000L, sendSensor);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠ Se perdió conexión WiFi, intentando reconectar...");
    WiFi.begin(ssid, pass);
    delay(2000);
  }

  if (!Blynk.connected()) {
    Serial.println("⚠ Se perdió conexión con Blynk");
    Blynk.connect();
  }

  Blynk.run();
  timer.run();
}