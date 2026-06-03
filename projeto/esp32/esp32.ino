// 1. INCLUSÃO DE BIBLIOTECAS
#include "DHT.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// 2. DEFINIÇÃO DOS PINOS
#define DHTPIN 15
#define DHTTYPE DHT22
#define LED_EXAUSTOR 2

// 3. CONFIGURAÇÕES DA REDE E BROKER MQTT
const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_user = "cloudtech_user"; 
const char* mqtt_pass = "senhaSegura123";

// Tópicos do projeto
const char* mqtt_topic_monitor = "cloudtech/datacenter/monitor";
const char* mqtt_topic_cmd = "cloudtech/datacenter/cmd"; // NOVO: Tópico de comando

// 4. INICIALIZAÇÃO DOS OBJETOS
DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

// Variável global para controlar se o comando veio da nuvem
bool comandoManualAtivo = false;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando-se a rede: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Wi-Fi conectado!");
}

// NOVO: Função que recebe os comandos vindos do Node-RED
void callback(char* topic, byte* payload, unsigned length) {
  Serial.print("Mensagem recebida no topico [");
  Serial.print(topic);
  Serial.print("]: ");
  
  // Converte o payload recebido em uma string legível
  String msg = "";
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.println(msg);

  // Verifica o comando enviado (Item 3.2 do projeto)
  if (msg == "ON") {
    digitalWrite(LED_EXAUSTOR, HIGH); // Liga o exaustor manualmente via nuvem
    comandoManualAtivo = true;
    Serial.println("➔ Exaustor ligado via comando remoto.");
  } 
  else if (msg == "OFF") {
    digitalWrite(LED_EXAUSTOR, LOW); // Desliga o exaustor manualmente via nuvem
    comandoManualAtivo = false;
    Serial.println("➔ Exaustor desligado via comando remoto.");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexao MQTT...");
    String clientId = "ESP32Client-CloudTech-";
    clientId += String(random(0, 0xffff), HEX);
    
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("conectado ao Broker!");
      
      // REQUISITO 3.2: O ESP32 deve assinar o tópico de comandos assim que conectar
      client.subscribe(mqtt_topic_cmd);
      Serial.println("Inscrito no topico de comandos com sucesso.");
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  pinMode(LED_EXAUSTOR, OUTPUT);
  digitalWrite(LED_EXAUSTOR, LOW);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  
  // NOVO: Avisa a biblioteca MQTT qual função deve tratar as mensagens recebidas
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // Essencial para processar os comandos recebidos!

  // Espera 5 segundos entre as leituras
  delay(5000); 

  float umidade = dht.readHumidity();
  float temperatura = dht.readTemperature();

  if (isnan(umidade) || isnan(temperatura)) {
    Serial.println("Falha ao ler o sensor DHT22!");
    return;
  }

  // --- REGRA DE EDGE COMPUTING ADAPTADA ---
  // A prioridade local automática continua existindo se estiver crítico (> 28°C)
  if (temperatura > 28.0) {
    digitalWrite(LED_EXAUSTOR, HIGH);
    Serial.println("⚠️ ALERTA LOCAL: Temperatura critica! Modo automatico prioritario.");
  } 
  // Se a temperatura estiver segura, respeita o último comando vindo da nuvem
  else {
    if (!comandoManualAtivo) {
      digitalWrite(LED_EXAUSTOR, LOW);
    }
  }

  // --- ENVIANDO DADOS ATUALIZADOS PARA A NUVEM ---
  StaticJsonDocument<64> doc;
  doc["temperatura"] = temperatura;
  doc["umidade"] = umidade;
  doc["exaustor"] = digitalRead(LED_EXAUSTOR) == HIGH ? "ON" : "OFF";

  char jsonBuffer[64];
  serializeJson(doc, jsonBuffer);
  
  client.publish(mqtt_topic_monitor, jsonBuffer);
}