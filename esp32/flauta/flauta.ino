// PFS - Flauta Dulce Educativa (ESP32)
// Modo Serial USB directo + Pantalla de inicio de Joseph Fourier a pantalla completa

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

// Retrato silueta estilizada 48x64 de una Flauta Dulce para pantalla de inicio
const unsigned char logoFlauta48[] PROGMEM = {
  0x00, 0x00, 0x07, 0xe0, 0x00, 0x00,
  0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00,
  0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00,
  0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00,
  0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00,
  0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00,
  0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00,
  0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00,
  0x00, 0x00, 0x38, 0x1c, 0x00, 0x00,
  0x00, 0x00, 0x38, 0x1c, 0x00, 0x00,
  0x00, 0x00, 0x38, 0x1c, 0x00, 0x00,
  0x00, 0x00, 0x38, 0x1c, 0x00, 0x00,
  0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00,
  0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00,
  0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00,
  0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00,
  0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00,
  0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00,
  0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00,
  0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00,
  0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00,
  0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00,
  0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00,
  0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00,
  0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00,
  0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00,
  0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00,
  0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00,
  0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00,
  0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00,
  0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00,
  0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00,
  0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00,
  0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00,
  0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00,
  0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00,
  0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00,
  0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00,
  0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00,
  0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00,
  0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00,
  0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00,
  0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00,
  0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00,
  0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00,
  0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00,
  0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00,
  0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00,
  0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00,
  0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00,
  0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00,
  0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00,
  0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00,
  0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00,
  0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00,
  0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
  0x00, 0x01, 0xff, 0xff, 0x80, 0x00,
  0x00, 0x03, 0xff, 0xff, 0xc0, 0x00,
  0x00, 0x07, 0xff, 0xff, 0xe0, 0x00,
  0x00, 0x0f, 0xff, 0xff, 0xf0, 0x00,
  0x00, 0x0f, 0xc0, 0x03, 0xf0, 0x00,
  0x00, 0x0e, 0x00, 0x00, 0x70, 0x00,
  0x00, 0x0c, 0x00, 0x00, 0x30, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

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

// Muestra la pantalla de bienvenida al inicio con la ilustración de la Flauta Dulce
void mostrarPantallaBienvenida() {
  display.clearDisplay();

  // Dibujar flauta dulce a la izquierda (48x64 en x=8, y=0)
  display.drawBitmap(8, 0, logoFlauta48, 48, 64, SSD1306_WHITE);

  // Textos descriptivos a la derecha
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(66, 8);
  display.println("PFS");

  display.setTextSize(1);
  display.setCursor(64, 30);
  display.println("Flauta Dulce");

  display.setCursor(64, 44);
  display.println("ESP32 DSP");

  display.display();
}

// Muestra la nota tocada en GRANDE a pantalla completa y centrada
void mostrarPantallaNota(const char* nombreNota) {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(32, 4);
  display.println("NOTA ACTUAL");

  int len = strlen(nombreNota);
  if (len <= 3) {
    display.setTextSize(4); // Letra gigante
    display.setCursor(24, 24);
  } else {
    display.setTextSize(3); // Letra grande para notas con agudos
    display.setCursor(12, 28);
  }
  display.println(nombreNota);

  display.display();
}

void alumbrarNota(int h1, int h2, int h3, int h4, int h5, int h6, int h7, int h8) {
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
      mostrarPantallaNota(tablaDigitaciones[i].nota);

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

  // Mostrar pantalla completa de bienvenida con Joseph Fourier por 3.5 segundos
  mostrarPantallaBienvenida();
}

void loop() {
  if (Serial.available()) {
    String nota = Serial.readStringUntil('\n');
    processNote(nota);
  }
}
