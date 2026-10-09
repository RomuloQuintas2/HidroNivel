/*
  ============================================================
  HidroNível — Sistema de Monitoramento de Nível de Reservatório
  ============================================================
  Etapa R5 — Programação e leitura dos sensores
  Disciplina: Sistemas Embarcados | Prof. Lauro Alves

  Resumo do funcionamento:
  Um sensor ultrassônico HC-SR04 mede a distância até a superfície
  da água dentro do reservatório. O Arduino converte essa distância
  em um percentual de nível (0% a 100%) e:
    - mostra o valor no display LCD 16x2 (I2C)
    - acende o LED correspondente à faixa de nível (verde/amarelo/vermelho)
    - aciona um buzzer quando o nível está crítico (<= 20%)
  ============================================================
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ============================================================
// A. CONFIGURAÇÃO DOS PINOS
// ============================================================
const int TRIG_PIN     = 9;   // HC-SR04 - pino de disparo (saída)
const int ECHO_PIN     = 10;  // HC-SR04 - pino de eco (entrada)
const int LED_VERDE    = 2;   // Nível OK      (> 70%)
const int LED_AMARELO  = 3;   // Nível médio    (21% - 70%)
const int LED_VERMELHO = 4;   // Nível crítico  (<= 20%)
const int BUZZER       = 8;   // Alerta sonoro em nível crítico
// Display LCD 16x2 I2C usa os pinos fixos de comunicação I2C do
// Arduino Uno: SDA -> A4, SCL -> A5 (não precisam de pinMode).


// ============================================================
// B. INICIALIZAÇÃO DOS SENSORES / PERIFÉRICOS
// ============================================================
// Objeto do display LCD: endereço I2C 0x27, 16 colunas, 2 linhas.
// (Se o display não responder, trocar para 0x3F, endereço comum alternativo.)
LiquidCrystal_I2C lcd(0x27, 16, 2);


// ============================================================
// C. VARIÁVEIS UTILIZADAS
// ============================================================
// Calibração do reservatório: distância (cm) do sensor até a água
// quando o reservatório está VAZIO e quando está CHEIO.
// Ajustar esses dois valores conforme a altura real do reservatório.
const float DIST_VAZIO = 20.0;   // distância quando o nível = 0%
const float DIST_CHEIO = 2.0;    // distância quando o nível = 100%

// Limites das faixas de nível (em %)
const int NIVEL_CRITICO = 20;    // nível <= 20%  -> alerta vermelho + buzzer
const int NIVEL_MEDIO   = 70;    // nível <= 70%  -> alerta amarelo

// Variáveis de estado, atualizadas a cada ciclo de leitura
long distanciaCm;   // última distância medida pelo sensor (cm)
int  nivelPercentual; // nível de água convertido em percentual (0-100%)


// ============================================================
// D. COMANDOS RESPONSÁVEIS PELA LEITURA
// ============================================================
// Dispara o pulso ultrassônico e mede o tempo de retorno do eco,
// convertendo para distância em centímetros.
long medirDistanciaCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);   // pulso de disparo de 10us
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // pulseIn mede quanto tempo o pino ECHO ficou em HIGH (ida e volta do som)
  long duracao = pulseIn(ECHO_PIN, HIGH, 30000); // timeout de 30ms
  if (duracao == 0) return -1; // nenhum eco recebido = leitura inválida

  // distância = (tempo * velocidade do som) / 2 (ida e volta)
  long distancia = duracao * 0.0343 / 2.0;
  return distancia;
}

// Converte a distância medida (cm) em percentual de nível de água (0-100%)
int calcularPercentual(long distancia) {
  float pct = (DIST_VAZIO - distancia) * 100.0 / (DIST_VAZIO - DIST_CHEIO);
  if (pct < 0)   pct = 0;    // satura em 0% (reservatório vazio ou sensor fora da faixa)
  if (pct > 100) pct = 100;  // satura em 100% (reservatório cheio)
  return (int)pct;
}


// ============================================================
// E. ESTRUTURAS CONDICIONAIS UTILIZADAS
// ============================================================
// Decide em qual faixa de nível o reservatório está e repassa
// a decisão para a função de acionamento dos atuadores (item F).
void avaliarNivel(int pct) {
  if (pct <= NIVEL_CRITICO) {
    acionarAlertaCritico();
  } else if (pct <= NIVEL_MEDIO) {
    acionarAlertaMedio();
  } else {
    acionarNivelOk();
  }
}


// ============================================================
// F. ACIONAMENTO DE ATUADORES
// ============================================================
// Desliga todos os atuadores antes de acionar o estado atual,
// evitando que mais de um LED fique aceso ao mesmo tempo.
void desligarAtuadores() {
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AMARELO, LOW);
  digitalWrite(LED_VERMELHO, LOW);
  noTone(BUZZER);
}

// Nível crítico: LED vermelho aceso + buzzer soando
void acionarAlertaCritico() {
  desligarAtuadores();
  digitalWrite(LED_VERMELHO, HIGH);
  tone(BUZZER, 1000); // tom de alerta contínuo em 1kHz

  lcd.setCursor(0, 1);
  lcd.print("ALERTA: BAIXO!  ");
}

// Nível médio: LED amarelo aceso
void acionarAlertaMedio() {
  desligarAtuadores();
  digitalWrite(LED_AMARELO, HIGH);

  lcd.setCursor(0, 1);
  lcd.print("Nivel medio     ");
}

// Nível adequado: LED verde aceso
void acionarNivelOk() {
  desligarAtuadores();
  digitalWrite(LED_VERDE, HIGH);

  lcd.setCursor(0, 1);
  lcd.print("Reservatorio OK ");
}


// ============================================================
// SETUP — executado uma vez ao ligar/resetar o Arduino
// ============================================================
void setup() {
  Serial.begin(9600);

  // Configuração dos pinos como entrada/saída (item A)
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  // Inicialização do display LCD (item B)
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("HidroNivel");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");
  delay(1500);
  lcd.clear();
}


// ============================================================
// LOOP — executado repetidamente
// ============================================================
void loop() {
  // --- leitura do sensor (item D) ---
  distanciaCm = medirDistanciaCm();

  if (distanciaCm < 0) {
    // leitura inválida: informa erro e tenta novamente no próximo ciclo
    lcd.setCursor(0, 0);
    lcd.print("Erro no sensor  ");
    lcd.setCursor(0, 1);
    lcd.print("Verifique HC-SR04");
    Serial.println("Leitura invalida do HC-SR04");
    delay(2000);
    return;
  }

  // --- processamento da leitura (item C: variáveis) ---
  nivelPercentual = calcularPercentual(distanciaCm);

  // --- decisão por faixa de nível (item E) + acionamento (item F) ---
  avaliarNivel(nivelPercentual);

  // --- atualização do display com o valor atual ---
  lcd.setCursor(0, 0);
  lcd.print("Nivel: ");
  lcd.print(nivelPercentual);
  lcd.print("%   ");

  // --- log no Monitor Serial, útil para depuração ---
  Serial.print("Distancia: ");
  Serial.print(distanciaCm);
  Serial.print(" cm | Nivel: ");
  Serial.print(nivelPercentual);
  Serial.println(" %");

  delay(2000); // intervalo entre leituras (RNF03 - resposta rápida, a cada 2s)
}
