# Monitoramento IoT com ESP8266 e MQTT

## Descrição do Projeto
Este projeto implementa um sistema de monitoramento IoT utilizando um ESP8266 para capturar dados de sensores e enviá-los via MQTT para um servidor remoto. Os dados também são exibidos em um display LCD 16x2 e salvos em um cartão SD em caso de falha na conexão.

## Componentes Utilizados
- ESP8266
- Sensor de Temperatura (NTC10K)
- Sensor Ultrassônico (HC-SR04)
- Sensor de TDS (Total Dissolved Solids)
- Módulo SD Card
- Display LCD 16x2 com interface I2C
- LEDs para indicação de status
- Fonte de alimentação compatível

## Bibliotecas Necessárias
As seguintes bibliotecas devem ser instaladas na IDE Arduino:
- `ESP8266WiFi.h` (para conexão Wi-Fi)
- `WiFiManager.h` (para gerenciamento de conexão Wi-Fi)
- `PubSubClient.h` (para comunicação MQTT)
- `Wire.h` (para comunicação I2C)
- `LiquidCrystal_I2C.h` (para controle do LCD)
- `SD.h` (para manipulação do cartão SD)

## Estrutura do Projeto
```
/
|-- src/
|   |-- main.ino       # Código principal do ESP8266
|   |-- leds.cpp       # Implementação dos LEDs
|   |-- leds.h         # Declaração das funções dos LEDs
|-- README.md         # Documentação do projeto
```

## Configuração MQTT
O código está configurado para se conectar a um broker MQTT remoto:
- **Servidor:** `0.tcp.sa.ngrok.io`
- **Porta:** `18623`
- **Tópico de dados:** `sensor/dados`
- **Tópico de logs:** `falhas`
- **Usuário:** `eder`
- **Senha:** `310104`

### Como Alterar as Configurações
Caso precise modificar os detalhes do servidor MQTT, altere as seguintes linhas no `main.ino`:
```cpp
const char* mqtt_server = "SEU_SERVIDOR";
const int mqtt_port = SUA_PORTA;
const char* mqtt_topic = "SEU_TOPICO";
const char* mqtt_user = "SEU_USUARIO";
const char* mqtt_password = "SUA_SENHA";
```

## Como Utilizar
### 1. Configuração Inicial
1. Instale as bibliotecas necessárias na IDE Arduino.
2. Conecte os sensores e periféricos conforme o esquema do projeto.
3. Compile e carregue o código no ESP8266.
4. Conecte-se à rede Wi-Fi gerada pelo ESP8266 para configurar a conexão com a internet.

### 2. Funcionamento
- O ESP8266 coleta leituras de temperatura, distância e TDS.
- Os dados são enviados via MQTT para um broker remoto.
- Caso não haja conexão MQTT, os dados são salvos no cartão SD.
- O display LCD exibe a temperatura em tempo real.
- Os LEDs indicam variações nos valores dos sensores.

## LED Indicators
A configuração e lógica dos LEDs estão em `leds.cpp` e `leds.h`. Eles indicam os valores dos sensores:
- Barra 1: Temperatura
- Barra 2: Distância
- Barra 3: TDS

## Melhorias Futuras
- Implementar criptografia na comunicação MQTT.
- Criar um painel de monitoramento em tempo real.
- Adicionar outros sensores para medições adicionais.

## Licença
Este projeto está licenciado sob a MIT License - veja o arquivo [LICENSE](LICENSE) para mais detalhes.

