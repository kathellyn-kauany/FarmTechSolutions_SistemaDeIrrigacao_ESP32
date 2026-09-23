#include <DHT.h>

#define PIN_N 12
#define PIN_P 14
#define PIN_K 27
#define PIN_LDR 34
#define PIN_DHT 15
#define PIN_RELE 22

#define DHTTYPE DHT22
DHT dht(PIN_DHT, DHTTYPE);

// true ou false, copiado manualmente do resultado do Python
bool chuvaPrevista = false;  

void setup() {
  Serial.begin(115200);

  pinMode(PIN_N, INPUT_PULLUP);
  pinMode(PIN_P, INPUT_PULLUP);
  pinMode(PIN_K, INPUT_PULLUP);

  pinMode(PIN_RELE, OUTPUT);
  digitalWrite(PIN_RELE, LOW);

  dht.begin();

  Serial.println("===============================================");
  Serial.println("  FarmTech Solutions - Sistema de Irrigação    ");
  Serial.println("===============================================");
}
void loop() {
  bool temN = (digitalRead(PIN_N) == LOW);
  bool temP = (digitalRead(PIN_P) == LOW);
  bool temK = (digitalRead(PIN_K) == LOW);
  //Controles via Teclado no Monitor Serial
  //Para alternar clique na parte vazia do Wokwi e pressione a tecla correspondente:
  //[1] Alterna Nitrogenio (N)
  //[2] Alterna Fosforo (P)
  //[3] Alterna Potassio (K)


  int valorLDR = analogRead(PIN_LDR);
  Serial.print("Lux->ADC: "); Serial.println(valorLDR);
  float phSolo = map(valorLDR, 0, 4095, 0, 140) / 10.0;

  float umidadeSolo = dht.readHumidity();

  Serial.print("[NUTRIENTES] N: "); Serial.print(temN ? "OK" : "FALTA");
  Serial.print(" | P: "); Serial.print(temP ? "OK" : "FALTA");
  Serial.print(" | K: "); Serial.println(temK ? "OK" : "FALTA");

  Serial.print("[PARAMETROS] pH Solo: "); Serial.print(phSolo);
  Serial.print(" | Umidade Solo: "); Serial.print(umidadeSolo); Serial.println("%");
  Serial.print("[API] Previsao climática: "); Serial.println(chuvaPrevista ? "Chuva prevista" : "Sem previsão de chuva");

  if (isnan(umidadeSolo)) {
    Serial.println(">>> SENSOR: dado de umidade inválido. Irrigação desligada.");
    digitalWrite(PIN_RELE, LOW);
    Serial.println("-----------------------------------------------");
    delay(2000);
    return;
  }


//PARAMETROS NECESSÁRIOS PARA CULTURA DE CAFÉ: Umidade do solo >= 60% e pH entre 5.5 e 6.5
  bool soloSeco = (umidadeSolo < 60.0);
  bool phIdeal = (phSolo >= 5.5 && phSolo <= 6.5);

// LÓGICA DE DECISÃO DE IRRIGAÇÃO E FERTIRRIGAÇÃO - CULTURA DO CAFÉ
  if (chuvaPrevista) {
    digitalWrite(PIN_RELE, LOW);
    Serial.println(">>> DECISÃO: [IRRIGAÇÃO DESLIGADA] Chuva prevista para a região do cafezal.");

  } else if (!soloSeco) {
    digitalWrite(PIN_RELE, LOW);
    Serial.print(">>> DECISÃO: [IRRIGAÇÃO DESLIGADA] Umidade do solo em ");
    Serial.print(umidadeSolo);
    Serial.println("% já é suficiente para a lavoura de café (>= 60%).");

  } else if (!phIdeal) {
    digitalWrite(PIN_RELE, LOW);
    Serial.print(">>> DECISÃO: [IRRIGAÇÃO BLOQUEADA] pH atual (");
    Serial.print(phSolo);
    Serial.println(") fora da faixa ideal para o café (5.5 a 6.5). Faça a correção do solo!");

  } else {
    // Solo seco (< 60%), sem chuva prevista e pH ideal para o café (5.5 a 6.5)
    digitalWrite(PIN_RELE, HIGH);

    // Diagnóstico dos nutrientes NPK
    if (!temN || !temP || !temK) {
      Serial.print(">>> DECISÃO: [IRRIGAÇÃO + FERTIRRIGAÇÃO LIGADA] Solo do cafezal necessita de reposição de: ");
      if (!temN) Serial.print("Nitrogênio (N) ");
      if (!temP) Serial.print("Fósforo (P) ");
      if (!temK) Serial.print("Potássio (K) ");
      Serial.println();
    } else {
      Serial.println(">>> DECISÃO: [IRRIGAÇÃO SIMPLES LIGADA] Solo do cafezal seco, sem chuva e com NPK em níveis adequados.");
    }
  }

  Serial.println("-----------------------------------------------");
  delay(2000);
}
