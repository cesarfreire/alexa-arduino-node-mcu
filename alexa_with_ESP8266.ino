#include <Arduino.h>

// Biblioteca WIFI do ESP8266
#include <ESP8266WiFi.h>

// Biblioteca FAUXMOESP
#include "fauxmoESP.h"

// Define os pinos a serem ativados/desativados
#define PINO_LAMP_1 D1
#define PINO_LAMP_2 D2

// Define Rate do serial
#define SERIAL_RATE 9600

// SSID e senha da rede WIFI
#define WIFI_SSID "WIFI SSID"
#define WIFI_PASS "WIFI PASSWD"

// Define um nome para os dispositivos a serem conectados na Alexa
#define LAMPADA_1 "LED 1"
#define LAMPADA_2 "LED 2"

// Cria o objeto da FAUXMOESP
fauxmoESP fauxmo;


// Wi-Fi Conexão
void configura_wifi() {

  // Define o como STA (Station)
  WiFi.mode(WIFI_STA);

  // Conecta
  Serial.printf("[WIFI] Conectado ao %s ", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Espera
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  // Conectado
  Serial.printf("[WIFI] Modo STATION, SSID: %s, endereço IP: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}

void setup() {
  // Inicia a Serial
  Serial.begin(SERIAL_RATE);
  Serial.println();

  // Conexão Wi-Fi
  configura_wifi();

  // Define LEDs como saída e iniciam apagados
  pinMode(PINO_LAMP_1, OUTPUT);
  digitalWrite(PINO_LAMP_1, LOW);

  pinMode(PINO_LAMP_2, OUTPUT);
  digitalWrite(PINO_LAMP_2, LOW);

  // Por padrão, fauxmoESP cria seu próprio servidor web na porta definida
  // A porta TCP deve ser 80 para dispositivos gen3 (o padrão é 1901)
  // Isso deve ser feito antes da chamada enable()
  fauxmo.createServer(true); // não é necessário, este é o valor padrão
  fauxmo.setPort(80); // Isso é necessário para dispositivos gen3

  // Você deve chamar enable(true) assim que tiver uma conexão WiFi
  // Você pode ativar ou desativar a biblioteca a qualquer momento
  // Desativá-lo impedirá que os dispositivos sejam descobertos e trocados
  fauxmo.enable(true);

  // Você pode usar diferentes maneiras de chamar a Alexa para modificar o estado dos dispositivos:
  // "Alexa, ligar LED um"

  // Adiciona os dispositivos
  fauxmo.addDevice(LAMPADA_1);
  fauxmo.addDevice(LAMPADA_2);

  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
    // Retorno de chamada quando um comando da Alexa é recebido.
    // Você pode usar device_id ou device_name para escolher o elemento no qual realizar uma ação (relé, LED, ...)
    // O state é um booleano (ON / OFF) e value um número de 0 a 255 (se você disser "definir a luz da cozinha para 50%", receberá 128 aqui).

    Serial.printf("[MAIN] Dispositivo #%d (%s) estado: %s valor: %d\n", device_id, device_name, state ? "ON" : "OFF", value);
    if ( (strcmp(device_name, LAMPADA_1) == 0) ) {
      Serial.println("Lampada 1 alterada pela Alexa");
      if (state) {
        digitalWrite(PINO_LAMP_1, HIGH);
      } else {
        digitalWrite(PINO_LAMP_1, LOW);
      }
    }
    if ( (strcmp(device_name, LAMPADA_2) == 0) ) {
      Serial.println("Lampada 2 alterada pela Alexa");
      if (state) {
        digitalWrite(PINO_LAMP_2, HIGH);
      } else {
        digitalWrite(PINO_LAMP_2, LOW);
      }
    }
  });

}

void loop() {
  // fauxmoESP usa um servidor TCP assíncrono, mas um servidor UDP sincronizado
  // Portanto, temos que pesquisar manualmente os pacotes UDP
  fauxmo.handle();

  // Imprime o freeheap a casa 5 segundos
  static unsigned long last = millis();
  if (millis() - last > 5000) {
    last = millis();
    Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
  }

}
