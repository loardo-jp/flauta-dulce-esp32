// PFS - Flauta Dulce Educativa (ESP32)
// Recibe el nombre de la nota por Serial (USB) y enciende los LEDs

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// Pines LEDs
const int o1  = 15;
const int o2  = 2;
const int o3  = 4;
const int o4  = 16;
const int o5  = 17;
const int o6  = 5;
const int o6r = 18;
const int o7  = 19;
const int o7r = 21;
const int o8  = 22;
const int o8r = 23;

// Pantalla OLED
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_SDA      26
#define OLED_SCK      25
#define OLED_ADDRESS  0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Estructura de digitación
struct Digitacion {
  const char* nota;
  int h1, h2, h3, h4, h5, h6, h7, h8;
};

const Digitacion tablaDigitaciones[] = {
  { "Do",      1, 1, 1, 1, 1, 1, 1, 1 },
  { "Do#",     1, 1, 1, 1, 1, 1, 2, 1 },
  { "Re",      1, 1, 1, 1, 1, 1, 0, 1 },
  { "Re#",     1, 1, 1, 1, 1, 2, 0, 1 },
  { "Mi",      1, 1, 1, 1, 1, 0, 0, 1 },
  { "Fa",      1, 1, 1, 1, 0, 0, 0, 1 },
  { "Fa#",     1, 1, 1, 0, 1, 1, 1, 1 },
  { "Sol",     1, 1, 1, 0, 0, 0, 0, 1 },
  { "Sol#",    1, 1, 0, 1, 1, 0, 0, 1 },
  { "La",      1, 1, 0, 0, 0, 0, 0, 1 },
  { "La#",     1, 0, 1, 0, 0, 0, 0, 1 },
  { "Si",      1, 0, 0, 0, 0, 0, 0, 1 },
  { "Do''",    0, 1, 0, 0, 0, 0, 0, 1 },
  { "Do#''",   1, 1, 0, 0, 0, 0, 0, 0 },
  { "Re''",    0, 1, 0, 0, 0, 0, 0, 0 },
  { "Re#''",   1, 1, 1, 1, 1, 2, 0, 2 },
  { "Mi''",    1, 1, 1, 1, 1, 1, 1, 1 },
  { "Fa''",    1, 1, 1, 1, 0, 0, 0, 2 },
  { "Fa#''",   1, 1, 1, 0, 1, 0, 0, 2 },
  { "Sol''",   1, 1, 1, 0, 0, 0, 0, 2 },
  { "Sol#''",  1, 1, 0, 1, 0, 0, 0, 2 },
  { "La''",    1, 1, 0, 0, 0, 0, 0, 2 },
  { "La#''",   1, 1, 0, 1, 1, 1, 0, 2 },
  { "Si''",    1, 1, 0, 1, 1, 0, 0, 2 },
  { "Do'''",   1, 0, 0, 1, 0, 0, 0, 2 },
};

const int NUM_NOTAS = sizeof(tablaDigitaciones) / sizeof(tablaDigitaciones[0]);

void alumbrarNota(int h1, int h2, int h3, int h4, int h5, int h6, int h7, int h8) {
  display.clearDisplay();

  if (h1 == 1) display.fillCircle(20, 10, 8, SSD1306_WHITE);
  else if (h1 == 2) { display.fillCircle(20, 10, 8, SSD1306_WHITE); display.fillCircle(20, 10, 4, SSD1306_BLACK); }
  else display.drawCircle(20, 10, 8, SSD1306_WHITE);

  int fd[] = {h2, h3, h4, h5};
  for (int i = 0; i < 4; i++) {
    int y = 10 + i * 14;
    if (fd[i] == 1) display.fillCircle(80, y, 8, SSD1306_WHITE);
    else if (fd[i] == 2) { display.fillCircle(80, y, 8, SSD1306_WHITE); display.fillCircle(80, y, 4, SSD1306_BLACK); }
    else display.drawCircle(80, y, 8, SSD1306_WHITE);
  }

  int db[] = {h6, h7, h8};
  for (int i = 0; i < 3; i++) {
    int y = 10 + (4 + i) * 14;
    if (y + 8 <= SCREEN_HEIGHT) {
      if (db[i] == 1) display.fillCircle(80, y, 6, SSD1306_WHITE);
      else if (db[i] == 2) { display.fillCircle(80, y, 6, SSD1306_WHITE); display.fillCircle(80, y, 3, SSD1306_BLACK); }
      else display.drawCircle(80, y, 6, SSD1306_WHITE);
    }
  }

  display.display();

  digitalWrite(o1, (h1 >= 1) ? HIGH : LOW);
  digitalWrite(o2, (h2 >= 1) ? HIGH : LOW);
  digitalWrite(o3, (h3 >= 1) ? HIGH : LOW);
  digitalWrite(o4, (h4 >= 1) ? HIGH : LOW);
  digitalWrite(o5, (h5 >= 1) ? HIGH : LOW);

  if (h6 == 1) { digitalWrite(o6, HIGH); digitalWrite(o6r, LOW); }
  else if (h6 == 2) { digitalWrite(o6, LOW); digitalWrite(o6r, HIGH); }
  else { digitalWrite(o6, LOW); digitalWrite(o6r, LOW); }

  if (h7 == 1) { digitalWrite(o7, HIGH); digitalWrite(o7r, LOW); }
  else if (h7 == 2) { digitalWrite(o7, LOW); digitalWrite(o7r, HIGH); }
  else { digitalWrite(o7, LOW); digitalWrite(o7r, LOW); }

  if (h8 == 1) { digitalWrite(o8, HIGH); digitalWrite(o8r, LOW); }
  else if (h8 == 2) { digitalWrite(o8, LOW); digitalWrite(o8r, HIGH); }
  else { digitalWrite(o8, LOW); digitalWrite(o8r, LOW); }
}

void mostrarTextoOLED(const char* linea1, const char* linea2 = "") {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.println(linea1);
  if (strlen(linea2) > 0) {
    display.setCursor(0, 40);
    display.println(linea2);
  }
  display.display();
}

void apagarTodosLosLeds() {
  digitalWrite(o1, LOW); digitalWrite(o2, LOW); digitalWrite(o3, LOW);
  digitalWrite(o4, LOW); digitalWrite(o5, LOW);
  digitalWrite(o6, LOW); digitalWrite(o6r, LOW);
  digitalWrite(o7, LOW); digitalWrite(o7r, LOW);
  digitalWrite(o8, LOW); digitalWrite(o8r, LOW);
}

void processNote(String nota) {
  nota.trim();
  if (nota.length() == 0) return;

  Serial.print("Nota: ");
  Serial.println(nota);

  for (int i = 0; i < NUM_NOTAS; i++) {
    if (nota.equals(tablaDigitaciones[i].nota)) {
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println(tablaDigitaciones[i].nota);
      display.display();

      alumbrarNota(
        tablaDigitaciones[i].h1, tablaDigitaciones[i].h2,
        tablaDigitaciones[i].h3, tablaDigitaciones[i].h4,
        tablaDigitaciones[i].h5, tablaDigitaciones[i].h6,
        tablaDigitaciones[i].h7, tablaDigitaciones[i].h8
      );
      return;
    }
  }
  Serial.print("Nota no encontrada: ");
  Serial.println(nota);
}

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  Serial.begin(115200);
  Serial.println("PFS - Flauta Dulce ESP32");
  Serial.println("Modo: Serial USB");
  Serial.println("Esperando notas...");

  pinMode(o1, OUTPUT); pinMode(o2, OUTPUT); pinMode(o3, OUTPUT);
  pinMode(o4, OUTPUT); pinMode(o5, OUTPUT);
  pinMode(o6, OUTPUT); pinMode(o6r, OUTPUT);
  pinMode(o7, OUTPUT); pinMode(o7r, OUTPUT);
  pinMode(o8, OUTPUT); pinMode(o8r, OUTPUT);

  apagarTodosLosLeds();

  Wire.begin(OLED_SDA, OLED_SCK);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("Error OLED");
    while (true) delay(500);
  }

  mostrarTextoOLED("PFS Listo", "Esperando nota...");
}

void loop() {
  if (Serial.available()) {
    String nota = Serial.readStringUntil('\n');
    processNote(nota);
  }
}
