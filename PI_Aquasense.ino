#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include "leds.h"


const char* mqtt_server = "seu broker";
const int mqtt_port = "porta";
const char* mqtt_topic = "collection";
const char* mqtt_log_topic = "falhas";
const char* mqtt_user = "seu usuario";
const char* mqtt_password = "sua senha";


WiFiClient espClient;
PubSubClient client(espClient);


#define SD_CS_PIN 4  // Pino CS do cartão SD
#define TEMP_PIN 34
#define TRIG_PIN 13
#define ECHO_PIN 12
#define TDS_PIN 32

// sensor NTC10
double Vs = 3.3, R1 = 10000, Beta = 3950, To = 298.15, Ro = 10000;
double adcMax = 4095.0;

// Calibração do sensor TDS
const float TDS_FACTOR = 0.5;


LiquidCrystal_I2C lcd(0x27, 16, 2);

// Controle de tempo
unsigned long ultimaAtualizacaoLCD = 0, ultimaAtualizacaoLEDs = 0, ultimaAtualizacaoMQTT = 0;
const long intervaloLCD = 3000, intervaloLEDs = 200, intervaloMQTT = 10000;


bool conectadoWiFi = false, conectadoMQTT = false;
unsigned long tempoDesconexao = 0;

// salvando dados no SD
void salvarNoSD(const String &dados) {
    File file = SD.open("/log.txt", FILE_WRITE);
    if (file) {
        file.println(dados);
        file.close();
    } else {
        Serial.println("Falha ao escrever no SD");
    }
}


void reconnectMQTT() {
    int tentativas = 0;
    while (!client.connected() && tentativas < 2) {
        Serial.print("Conectando ao MQTT...");
        if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
            Serial.println("Conectado");
            conectadoMQTT = true;
        } else {
            Serial.print("Falha, rc=");
            Serial.print(client.state());
            Serial.println(" Tentando novamente...");
            tentativas++;
            delay(5000);
        }
    }
    if (!client.connected()) {
        Serial.println("Falha ao conectar ao MQTT.");
        conectadoMQTT = false;
        tempoDesconexao = millis();
    }
}

// sensor de temperatura (NTC10K
float lerTemperatura() {
    int leitura = analogRead(TEMP_PIN);
    double Vout = leitura * Vs / adcMax;
    double Rt = R1 * Vout / (Vs - Vout);
    double T = 1 / (1 / To + log(Rt / Ro) / Beta);
    return T - 273.15;
}

// sensor ultrassônico
float lerDistancia() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long duracao = pulseIn(ECHO_PIN, HIGH);
    return duracao * 0.034 / 2;
}

// sensor TDS
int lerTDS() {
    int leitura = analogRead(TDS_PIN);
    float tensao = leitura * (3.3 / 4095.0);
    float tds = (tensao * TDS_FACTOR) * 1000;
    return (int)tds;
}

// Formatação JSON dos dados
String montarJSON(float temperatura, float distancia, int tds) {
    char buffer[256];
    sprintf(buffer, "{\"temperatura\":%.2f,\"distancia\":%.2f,\"tds\":%d}", temperatura, distancia, tds);
    return String(buffer);
}

// Envio de dados via MQTT ou salvamento no SD
void enviarMQTT(float temperatura, float distancia, int tds) {
    String dadosJSON = montarJSON(temperatura, distancia, tds);

    if (client.connected()) {
        client.publish(mqtt_topic, dadosJSON.c_str());
        Serial.println("Dados enviados: " + dadosJSON);
        conectadoMQTT = true;
    } else {
        Serial.println("MQTT desconectado! Salvando no SD...");
        salvarNoSD(dadosJSON);

        if (tempoDesconexao == 0) {
            tempoDesconexao = millis();
        }
    }
}

void setup() {
    Serial.begin(115200);

    
    lcd.init();
    lcd.backlight();

    
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(SD_CS_PIN, OUTPUT);
    
    
    if (!SD.begin(SD_CS_PIN)) {
        Serial.println("Erro ao inicializar o cartão SD. Tentando novamente...");
        delay(2000);
        if (!SD.begin(SD_CS_PIN)) {
            Serial.println("Falha crítica no SD. Reiniciando...");
            ESP.restart();
        }
    }
    
    WiFiManager wifiManager;
    wifiManager.autoConnect("ESP32_AP");
    Serial.println("WiFi conectado.");
    conectadoWiFi = true;

    
    client.setServer(mqtt_server, mqtt_port);

    
    enviarLogMQTT();
    
    
    inicializarLEDs();
    Serial.println("LEDs inicializados.");
}

void enviarLogMQTT() {
    if (!SD.exists("/log.txt")) {
        Serial.println("Arquivo de log não encontrado.");
        return;
    }

    File file = SD.open("/log.txt", FILE_READ);
    if (!file) {
        Serial.println("Erro ao abrir o arquivo de log para leitura.");
        return;
    }

    String dadosLog = "";
    while (file.available()) {
        dadosLog += file.readStringUntil('\n'); 
        dadosLog += "\\n"; 
    }
    file.close();

    if (client.connected()) {
        client.publish(mqtt_log_topic, dadosLog.c_str());
        Serial.println("Log enviado para o MQTT.");
    } else {
        Serial.println("MQTT desconectado. Não foi possível enviar o log.");
    }
}


void loop() {
    unsigned long agora = millis();

    
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi desconectado! Salvando log...");
        conectadoWiFi = false;
        if (tempoDesconexao == 0) tempoDesconexao = millis();
        salvarNoSD("WiFi caiu em: " + String(millis() / 1000) + "s");
    } else {
        conectadoWiFi = true;
    }

    
    if (!client.connected()) {
        reconnectMQTT();
    }
    client.loop();

    
    if (agora - ultimaAtualizacaoLEDs >= intervaloLEDs) {
        ultimaAtualizacaoLEDs = agora;

        float temperatura = lerTemperatura();
        float distancia = lerDistancia();
        int tds = lerTDS();

        int valorTemp = constrain((int)temperatura, 0, 40);
        int valorDist = constrain((int)distancia, 0, 200);
        int valorTds = map((int)tds, 0, 1000, 0, 100);

        atualizarLEDs(barra1, valorTemp);
        atualizarLEDs(barra2, valorDist);
        atualizarLEDs(barra3, valorTds);
    }

    
    if (agora - ultimaAtualizacaoLCD >= intervaloLCD) {
        ultimaAtualizacaoLCD = agora;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Temp: ");
        lcd.print(lerTemperatura(), 1);
        lcd.print(" C");
    }

    
    if (agora - ultimaAtualizacaoMQTT >= intervaloMQTT) {
        ultimaAtualizacaoMQTT = agora;
        enviarMQTT(lerTemperatura(), lerDistancia(), lerTDS());
    }
}  
