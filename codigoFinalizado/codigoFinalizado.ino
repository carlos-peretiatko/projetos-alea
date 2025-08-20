/*
  Projeto: Estação de Controle de Posição com VarSpeedServo + Potenciômetro + OLED
  Placa: Arduino UNO (ajustável para ESP32 mudando pinos do I2C/ADC)
  Bibliotecas: VarSpeedServo.h, Adafruit_SSD1306, Adafruit_GFX, Wire
  Autor: Carlos E Barreto, Carlos E Peretiatko, Magno Freitas, Rafael Kuhnne
  Data: 20/08/2025

  Descrição:
  - Lê um potenciômetro no pino A0 (0–1023).
  - Mapeia para 0–180 graus.
  - Lê um potenciômetro no pino A1 (0-1023).
  - Mapeia para 0-100% da velocidade.
  - Move um servo suavemente usando VarSpeedServo (pino 9).
  - Exibe no OLED o ângulo atual, status ("Movendo"/"Parado") e velocidade (0-100%).
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <VarSpeedServo.h>   // biblioteca para movimentos suaves

// ======= Definições de hardware =======
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

#define POT_PIN2 A1     // potenciômetro da velocidade (0-1023)
#define POT_PIN A0      // potenciômetro do ângulo (0–1023)
#define SERVO_PIN 9     // sinal do servo

// ======= Objetos =======
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
VarSpeedServo myServo;

// ======= Variáveis de controle =======

int anguloAnterior = 90;   // última posição registrada
bool emMovimento = false;  // verificação se servo está em movimento
//ambos acima para que não ocorra problemas na inicialização do servo

void setup() {
  Serial.begin(9600);

  // Inicializa servo
  myServo.attach(SERVO_PIN);
  myServo.write(90, 30, true); // começa em 90° suavemente (velocidade 30)

  // Inicializa display OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("Falha ao inicializar OLED!"));
    for (;;); // trava se der erro
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.println("Sistema Iniciado");
  display.display();
  delay(1500);
}

void loop() {
  // ======= Leitura do potenciômetro =======
  int valorPot = analogRead(POT_PIN);           // lê valor (0–1023)
  int angulo = map(valorPot, 0, 1023, 0, 180);  // mapeia para graus (0–180)
  int valorVel = analogRead(POT_PIN2);          // lê valor (0-1023)
  int velocidade = map(valorVel, 0, 1023, 0, 100); //mapeia para porcentagem (0-100%)

  myServo.write(angulo, velocidade, false);  // movimento do servo, definido pelos potênciometros lidos acima.

  // ======= Status de movimento =======
  if (myServo.isMoving()) {
    emMovimento = true;
  } else {
    emMovimento = false;
  }

  // ======= Debug serial =======
  Serial.print("Pot: ");
  Serial.print(valorPot);
  Serial.print(" -> Angulo: ");
  Serial.println(angulo);
  Serial.print("Velocidade: ");
  serial.println(velocidade);

  // ======= Atualiza OLED =======
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 10);
  display.print("Angulo: ");
  display.print(angulo);
  display.println(" deg");

  display.setCursor(0, 30);
  if (emMovimento) {
    display.println("Status: Movendo");
  } else {
    display.println("Status: Parado");
  }

  display.setCursor(0,50);
  display.print("Velocidade: ");
  display.print(velocidade);
  display.println("%");
  display.display();

  delay(50); // pequena pausa p/ suavizar atualização
}
