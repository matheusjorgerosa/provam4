#include <WiFi.h>
#include <HTTPClient.h>

#define led_azul 9 // Pino utilizado para controle do led azul
#define led_verde 2 // Pino utilizado para controle do led verde
#define led_vermelho 40 // Pino utilizado para controle do led vermelho
#define led_amarelo 9 // Pino utilizado para controle do led azul

const int pinoBotao = 18;  // Pino de leitura do input vindo do botão
int estadoBotao = 0;  // Variável para salvar o estado do botão

const int pinoLDR = 4;  // Pino de leitura do sensor LDR
int limite=1000; // Limite de luminosidade para considerar o ambiente claro

bool flag = 0;

void setup() {

  // Configuração inicial dos pinos para controle dos leds como OUTPUTs (saídas) do ESP32
  pinMode(led_azul,OUTPUT);
  pinMode(led_verde,OUTPUT);
  pinMode(led_vermelho,OUTPUT);

  // Inicialização das entradas
  pinMode(pinoBotao, INPUT); // Inicializa o pino do botão como input

  digitalWrite(led_azul, LOW);
  digitalWrite(led_verde, LOW);
  digitalWrite(led_vermelho, LOW);

  Serial.begin(9600); // Configuração para debug por interface serial entre ESP e computador com baud rate de 9600

  WiFi.begin("Wokwi-GUEST", ""); // Conexão à rede WiFi aberta com SSID Wokwi-GUEST

  while (WiFi.status() != WL_CONNECT_FAILED) {
    delay(100);
  }
  Serial.println("Conectado ao WiFi com sucesso!"); // Considerando que saiu do loop acima, o ESP32 agora está conectado ao WiFi (outra opção é colocar este comando dentro do if abaixo)

  if(WiFi.status() == WL_CONNECTED){ // Se o ESP32 estiver conectado à Internet
    HTTPClient http;

    String serverPath = "http://www.google.com.br/"; // Endpoint da requisição HTTP

    http.begin(serverPath.c_str());

    int httpResponseCode = http.GET(); // Código do Resultado da Requisição HTTP

    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
      }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      }
      http.end();
    }

  else {
    Serial.println("WiFi Disconnected");
  }
}

void botaoPressionado(){
  estadoBotao = digitalRead(pinoBotao);

  digitalWrite(led_verde, LOW);
  digitalWrite(led_amarelo, LOW);
  digitalWrite(led_vermelho, HIGH);
  if(estadoBotao == 1 && flag == 1){
    Serial.println("Botão pressionado!");
    delay(1000);
    digitalWrite(led_vermelho, LOW);
    digitalWrite(led_amarelo, LOW);
    digitalWrite(led_verde, HIGH);
  }
  delay(5000);
}


void semaforo(){
  // Essas constantes devem corresponder aos atributos "gama" e "rl10" do fotoresistor
  const float GAMMA = 0.7;
  const float RL10 = 50;

  int analogValue = analogRead(pinoLDR);
  float voltage = analogValue / 1024. * 5;
  float resistance = 2000 * voltage / (1 - voltage / 5);
  float lux = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1 / GAMMA));

  if(lux<=limite){
    digitalWrite(led_verde, LOW);
    digitalWrite(led_amarelo, LOW);
    digitalWrite(led_vermelho, LOW);

    Serial.println("Escuro!");
    digitalWrite(led_amarelo, HIGH);
    delay(1000);
    digitalWrite(led_amarelo, LOW);
    delay(1000);
  }else{
    flag = 0;
    Serial.println("Claro!");
    digitalWrite(led_verde, HIGH);
    digitalWrite(led_amarelo, LOW);
    digitalWrite(led_vermelho, LOW);
    delay(3000);
    digitalWrite(led_verde, LOW);
    digitalWrite(led_amarelo, HIGH);
    digitalWrite(led_vermelho, LOW);
    delay(2000);
    flag = 1;
    botaoPressionado();
  }
}

void loop() {
  semaforo();
}