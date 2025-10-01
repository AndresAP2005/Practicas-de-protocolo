// Definiciones requeridas para Blynk
#define BLYNK_TEMPLATE_ID "TMPL2HpVaa0xg"
#define BLYNK_TEMPLATE_NAME "VerTemp"
#define BLYNK_AUTH_TOKEN "Lb-wUOdfNP0Q9GfmxGtDD5u0kgrUFM6b"

#include <Arduino.h>
#include "BluetoothSerial.h"
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include "DHT.h"

// --- CONFIGURACIÓN ---
#define DHTPIN 26     // Pin del DHT11
#define DHTTYPE DHT11
#define LED1 4        // LED permiso concedido
#define LED2 22         // LED permiso denegado

char ssid[] = "Name";    // nombre del wifi
char pass[] = "Password";     // Password WiFi

// --- Objetos ---
BluetoothSerial SerialBT;
DHT dht(DHTPIN, DHTTYPE);

String comando = "";
int permiso = 0;   // Se actualiza desde Blynk
void procesarComando(String cmd);
// --- Función Blynk ---
BLYNK_WRITE(V1) {
  permiso = param.asInt(); // 1 = permitido, 0 = negado
  Serial.print("[BLYNK] Valor recibido en V1 = ");
  Serial.println(permiso);

  if (permiso) {
    Serial.println("[BLYNK] Permiso concedido");
    // LEDs
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, LOW);    
  } else {
    Serial.println("[BLYNK] Permiso denegado");
    // LEDs
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, HIGH);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // --- LEDs ---
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);

  // --- Iniciar Bluetooth ---
  if (!SerialBT.begin("ESP32_BT_TEMP")) {
    Serial.println("[ERROR] No se pudo iniciar Bluetooth");
    while (true);
  }
  Serial.println("[OK] Bluetooth iniciado como ESP32_BT_TEMP");

  // --- Iniciar WiFi + Blynk ---
  Serial.print("[...] Conectando WiFi");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[OK] WiFi conectado");

  // ✅ En IoT se usa el token directo
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // --- Iniciar DHT ---
  dht.begin();
  Serial.println("[OK] DHT11 listo");
}

void loop() {
  Blynk.run();

  // Revisión de comandos Bluetooth
 if (SerialBT.available()) {
  String cmd = SerialBT.readString();  // Lee todo lo que haya en el buffer
  procesarComando(cmd);
}
}

void procesarComando(String cmd) {
  cmd.trim();
  Serial.print("[CMD] Recibido: ");
  Serial.println(cmd);

  if (cmd == "TEMP") {
    if (permiso == 1) {
      float t = dht.readTemperature();
      if (isnan(t)) {
        SerialBT.println("Error leyendo DHT11");
        return;
      }
      // Enviar temperatura
      SerialBT.print("Temperatura: ");
      SerialBT.print(t);
      SerialBT.println(" *C");
      Serial.println("[OK] Enviada temperatura por Bluetooth");

    } else {
      SerialBT.println("Permiso DENEGADO desde Blynk");
      Serial.println("[BLOQUEADO] No se envió temperatura");
    }
  }
  else {
    Serial.println("[WARN] Comando no reconocido");
  }
}