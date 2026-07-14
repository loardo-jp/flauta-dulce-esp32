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

// Mapa de bits PROGMEM 48x64 - Retrato de Joseph Fourier en blanco y negro
const unsigned char logoFourier[] PROGMEM = {
  0x00, 0x03, 0xf8, 0x00, 0x00, 0x00,
  0x00, 0x1f, 0xfe, 0x00, 0x00, 0x00,
  0x00, 0x7f, 0xff, 0x80, 0x00, 0x00,
  0x01, 0xf8, 0x1f, 0xe0, 0x00, 0x00,
  0x03, 0xe0, 0x07, 0xf0, 0x00, 0x00,
  0x07, 0x87, 0xe1, 0xf8, 0x00, 0x00,
  0x0f, 0x1f, 0xf8, 0xfc, 0x00, 0x00,
  0x1e, 0x3f, 0xfc, 0x7e, 0x00, 0x00,
  0x1c, 0x7f, 0xfe, 0x3e, 0x00, 0x00,
  0x38, 0xff, 0xff, 0x3f, 0x00, 0x00,
  0x31, 0xff, 0xff, 0x9f, 0x00, 0x00,
  0x73, 0xff, 0xff, 0xcf, 0x80, 0x00,
  0x63, 0xff, 0xff, 0xe7, 0x80, 0x00,
  0x67, 0xff, 0xff, 0xf7, 0xc0, 0x00,
  0xc7, 0xff, 0xff, 0xf3, 0xc0, 0x00,
  0xcf, 0xff, 0xff, 0xf9, 0xe0, 0x00,
  0xcc, 0x0f, 0xf8, 0x19, 0xe0, 0x00,
  0xcc, 0x07, 0xf0, 0x08, 0xe0, 0x00,
  0xcd, 0xb3, 0x66, 0xdb, 0xe0, 0x00,
  0xcd, 0xb1, 0x66, 0xdb, 0xe0, 0x00,
  0x8c, 0x01, 0xe0, 0x03, 0xc0, 0x00,
  0x8e, 0x01, 0xe0, 0x03, 0xc0, 0x00,
  0x8e, 0x01, 0xe0, 0x07, 0xc0, 0x00,
  0x8f, 0x00, 0xc0, 0x07, 0xc0, 0x00,
  0x87, 0x80, 0x00, 0x0f, 0x80, 0x00,
  0x87, 0x86, 0x18, 0x0f, 0x80, 0x00,
  0xc3, 0xc6, 0x18, 0x1f, 0x00, 0x00,
  0xc1, 0xe0, 0x00, 0x1e, 0x00, 0x00,
  0x60, 0xf0, 0x00, 0x3c, 0x00, 0x00,
  0x60, 0x78, 0x00, 0x78, 0x00, 0x00,
  0x30, 0x3e, 0x01, 0xf0, 0x00, 0x00,
  0x38, 0x1f, 0xff, 0xe0, 0x00, 0x00,
  0x1c, 0x0f, 0xff, 0xc0, 0x00, 0x00,
  0x0e, 0x03, 0xff, 0x80, 0x00, 0x00,
  0x0f, 0x00, 0xfe, 0x00, 0x00, 0x00,
  0x1f, 0x80, 0x3c, 0x00, 0x00, 0x00,
  0x3f, 0xc0, 0x18, 0x01, 0x80, 0x00,
  0x7f, 0xe0, 0x18, 0x03, 0xc0, 0x00,
  0xff, 0xf0, 0x18, 0x07, 0xe0, 0x00,
  0xff, 0xf8, 0x18, 0x0f, 0xf0, 0x00,
  0xff, 0xfc, 0x18, 0x1f, 0xf8, 0x00,
  0xff, 0xfe, 0x18, 0x3f, 0xfc, 0x00,
  0xef, 0xff, 0x18, 0x7f, 0xfe, 0x00,
  0xc7, 0xff, 0x98, 0xff, 0x9f, 0x00,
  0xc3, 0xff, 0x99, 0xff, 0x0f, 0x80,
  0xc1, 0xff, 0xd9, 0xfe, 0x07, 0xc0,
  0xc0, 0xff, 0xd9, 0xfc, 0x03, 0xe0,
  0xc0, 0x7f, 0xdb, 0xf8, 0x01, 0xf0,
  0xc0, 0x3f, 0xdb, 0xf0, 0x01, 0xf0,
  0xc0, 0x1f, 0xff, 0xe0, 0x00, 0xf8,
  0xc0, 0x0f, 0xff, 0xc0, 0x00, 0xfc,
  0xc0, 0x07, 0xff, 0x80, 0x00, 0x7e,
  0xc0, 0x03, 0xff, 0x00, 0x00, 0x3f,
  0xc0, 0x01, 0xfe, 0x00, 0x00, 0x3f,
  0xc0, 0x00, 0xfc, 0x00, 0x00, 0x1f,
  0xc0, 0x00, 0xf8, 0x00, 0x00, 0x1f,
  0xc0, 0x00, 0xf0, 0x00, 0x00, 0x0f,
  0xc0, 0x00, 0xe0, 0x00, 0x00, 0x0f,
  0xc0, 0x00, 0xc0, 0x00, 0x00, 0x07,
  0xc0, 0x00, 0x80, 0x00, 0x00, 0x07,
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x07,
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x03,
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x03,
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x03
};

void mostrarPantallaNotaYFourier(const char* nombreNota) {
  display.clearDisplay();

  // Dibujar imagen de Joseph Fourier a la derecha (48x64 píxeles en x=78, y=0)
  display.drawBitmap(78, 0, logoFourier, 48, 64, SSD1306_WHITE);

  // Título superior izquierdo
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 4);
  display.println("NOTA:");

  // Nombre de la nota en grande en la mitad izquierda
  int len = strlen(nombreNota);
  if (len <= 3) {
    display.setTextSize(3); // Letra muy grande si es corta (ej. Do, Re#)
    display.setCursor(0, 22);
  } else {
    display.setTextSize(2); // Letra mediana si es larga (ej. Sol#'')
    display.setCursor(0, 26);
  }
  display.println(nombreNota);

  display.display();
}

void alumbrarNota(int h1, int h2, int h3, int h4, int h5, int h6, int h7, int h8) {
  // Control de LEDs físicos de la flauta
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
  display.drawBitmap(78, 0, logoFourier, 48, 64, SSD1306_WHITE);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 18);
  display.println(linea1);
  if (strlen(linea2) > 0) {
    display.setCursor(0, 36);
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
      mostrarPantallaNotaYFourier(tablaDigitaciones[i].nota);

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

  mostrarTextoOLED("PFS Listo", "Conecta USB");
}

void loop() {
  if (Serial.available()) {
    String nota = Serial.readStringUntil('\n');
    processNote(nota);
  }
}
