#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// --- Declaración de la función ---
void handleNewMessages(int numNewMessages);

// Configuración WiFi
const char *ssid = "Nombre_de_tu_internet";
const char *password = "Tu_contraseña";

// Configuración de Telegram
const String botToken = "7966384699:AAH_XaBEbY1beiJCd6rHZFLhczEJbQOnRtg";

// Variables
const unsigned long botMTBS = 1000;
unsigned long botLastScan = 0;  

// Pines para LEDs
const int led1Pin = 2;
const int led2Pin = 3;
bool ledstatus1 = false;
bool ledstatus2 = false;

// Inicialización del bot
WiFiClientSecure client;
UniversalTelegramBot bot(botToken, client);

void setup() {
  Serial.begin(115200);

  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);

  // Aseguramos estado inicial
  digitalWrite(led1Pin, LOW);
  digitalWrite(led2Pin, LOW);

  // Conectar al WiFi
  Serial.print("Conectando a WiFi...");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado! Direccion IP:");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() - botLastScan > botMTBS) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("Mensaje recibido!");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    botLastScan = millis();
  }
}

// --- Definición de la función ---
void handleNewMessages(int numNewMessages) {
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);
  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;

    if (from_name == "") from_name = "Guest";

    // Normalizar texto recibido
    text.trim();
    Serial.println("Texto recibido: '" + text + "'");

    // Comandos (uso startsWith para aceptar también /COMANDO@BotName)
    if (text == "/LED1ON") {
      ledstatus1 = true;
      digitalWrite(led1Pin, HIGH);
      bot.sendMessage(chat_id, "Se ha encendido el LED 1.", "");
    }
    if (text == "/LED1OFF") {
      ledstatus1 = false;
      digitalWrite(led1Pin, LOW);
      bot.sendMessage(chat_id, "Se ha apagado el LED 1.", "");
    }
    if (text == "/LED2ON") {
      ledstatus2 = true;
      digitalWrite(led2Pin, HIGH);
      bot.sendMessage(chat_id, "Se ha encendido el LED 2.", "");
    }
    if (text == "/LED2OFF") {
      ledstatus2 = false;
      digitalWrite(led2Pin, LOW);
      bot.sendMessage(chat_id, "Se ha apagado el LED 2.", "");
    }
    if (text == "/ALLON") {
      ledstatus1 = true;
      ledstatus2 = true;
      digitalWrite(led1Pin, HIGH);
      digitalWrite(led2Pin, HIGH);
      bot.sendMessage(chat_id, "Todos los LEDs están encendidos.", "");
    }
    if (text == "/ALLOFF") {
      ledstatus1 = false;
      ledstatus2 = false;
      digitalWrite(led1Pin, LOW);
      digitalWrite(led2Pin, LOW);
      bot.sendMessage(chat_id, "Todos los LEDs están apagados.", "");
    }
    if (text.startsWith("/start")) {
      String welcome = "Bienvenido " + from_name + "!\n";
      welcome += "Comandos disponibles:\n";
      welcome += "/LED1_ON : Enciende el primer LED\n";
      welcome += "/LED1_OFF : Apaga el primer LED\n";
      welcome += "/LED2_ON : Enciende el segundo LED\n";
      welcome += "/LED2_OFF : Apaga el segundo LED\n";
      welcome += "/ALL_ON : Enciende todos los LEDs\n";
      welcome += "/ALL_OFF : Apaga todos los leds\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}