// =============================================================
// Flauta Dulce - Dispositivo de enseñanza de flauta soprano
// Firmware para ESP32
// =============================================================
// Este firmware conecta el ESP32 a un servidor WebSocket en
// Railway. Al recibir el nombre de una nota (ej: "Do", "Re#",
// "Sol''"), busca la digitación correspondiente y la muestra
// en la pantalla OLED y en los LEDs indicadores.
// =============================================================

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WebSocketsClient_Generic.h>

// =============================================================
// ⚙️  CONFIGURACIÓN DEL USUARIO - ¡MODIFICA ESTOS VALORES!
// =============================================================

// --- Credenciales WiFi ---
// Cambia estos valores por el nombre y contraseña de tu red WiFi
const char* wifi_ssid     = "consultorio";
const char* wifi_password = "Consultorio2024";

// --- Servidor WebSocket (Railway) ---
// Cambia estos valores por la URL de tu servidor en Railway
const char* ws_host = "flauta-dulce-esp32-production.up.railway.app";
const int   ws_port = 443;
const char* ws_path = "/ws";

// =============================================================
// Definición de pines - LEDs indicadores de digitación
// =============================================================
// Cada pin corresponde a un orificio de la flauta dulce
// Los pines con sufijo 'r' son LEDs de color rojo (medio tapar)
const int o1  = 15;   // Orificio 1 (pulgar trasero)
const int o2  = 2;    // Orificio 2
const int o3  = 4;    // Orificio 3
const int o4  = 16;   // Orificio 4
const int o5  = 17;   // Orificio 5
const int o6  = 5;    // Orificio 6 (tapar completo)
const int o6r = 18;   // Orificio 6 (medio tapar - rojo)
const int o7  = 19;   // Orificio 7 (tapar completo)
const int o7r = 21;   // Orificio 7 (medio tapar - rojo)
const int o8  = 22;   // Orificio 8 (tapar completo)
const int o8r = 23;   // Orificio 8 (medio tapar - rojo)

// =============================================================
// Configuración de la pantalla OLED
// =============================================================
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_SDA      26
#define OLED_SCK      25
#define OLED_ADDRESS  0x3C

// Crear instancia de la pantalla OLED (I2C, sin pin de reset)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// =============================================================
// Cliente WebSocket
// =============================================================
WebSocketsClient webSocket;

// Variable para almacenar la IP local (para mostrar en pantalla)
String ipLocal = "";

// =============================================================
// Estructura para la tabla de digitaciones
// =============================================================
// Valores de digitación:
//   0 = orificio abierto (LED apagado)
//   1 = orificio tapado completamente (LED verde encendido)
//   2 = orificio medio tapado (LED rojo encendido)
struct Digitacion {
  const char* nota;  // Nombre de la nota (ej: "Do", "Re#", "Sol''")
  int h1;            // Orificio 1 (pulgar trasero)
  int h2;            // Orificio 2
  int h3;            // Orificio 3
  int h4;            // Orificio 4
  int h5;            // Orificio 5
  int h6;            // Orificio 6
  int h7;            // Orificio 7
  int h8;            // Orificio 8
};

// =============================================================
// Tabla de digitaciones - Flauta dulce soprano (Do4 a Do6)
// =============================================================
// Registro grave: Do - Si
// Registro agudo: Do'' - Do'''
const Digitacion tablaDigitaciones[] = {
  // --- Registro grave ---
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
  // --- Registro agudo ---
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

// Número total de notas en la tabla
const int NUM_NOTAS = sizeof(tablaDigitaciones) / sizeof(tablaDigitaciones[0]);

// =============================================================
// Función: alumbrarNota
// =============================================================
// Recibe los 8 valores de digitación y enciende los LEDs
// correspondientes. También dibuja la digitación en la OLED.
//
// Valores:
//   0 = abierto  -> LED apagado, círculo vacío en OLED
//   1 = tapado   -> LED verde encendido, círculo lleno en OLED
//   2 = medio    -> LED rojo encendido, círculo medio en OLED
void alumbrarNota(int h1, int h2, int h3, int h4, int h5, int h6, int h7, int h8) {
  // --- Lógica de la pantalla OLED ---
  display.clearDisplay();

  // Orificio 1 (pulgar trasero) - se dibuja a la izquierda
  if (h1 == 1) {
    display.fillCircle(20, 10, 8, SSD1306_WHITE);
  } else if (h1 == 2) {
    display.fillCircle(20, 10, 8, SSD1306_WHITE);
    display.fillCircle(20, 10, 4, SSD1306_BLACK);
  } else {
    display.drawCircle(20, 10, 8, SSD1306_WHITE);
  }

  // Orificios 2-5 (delanteros superiores)
  int orificiosDelanteros[] = {h2, h3, h4, h5};
  for (int i = 0; i < 4; i++) {
    int y = 10 + i * 14;
    if (orificiosDelanteros[i] == 1) {
      display.fillCircle(80, y, 8, SSD1306_WHITE);
    } else if (orificiosDelanteros[i] == 2) {
      display.fillCircle(80, y, 8, SSD1306_WHITE);
      display.fillCircle(80, y, 4, SSD1306_BLACK);
    } else {
      display.drawCircle(80, y, 8, SSD1306_WHITE);
    }
  }

  // Orificios 6, 7, 8 (delanteros inferiores - dobles)
  int orificiosDobles[] = {h6, h7, h8};
  for (int i = 0; i < 3; i++) {
    int y = 10 + (4 + i) * 14;
    // Solo dibujar si cabe en pantalla
    if (y + 8 <= SCREEN_HEIGHT) {
      if (orificiosDobles[i] == 1) {
        display.fillCircle(80, y, 6, SSD1306_WHITE);
      } else if (orificiosDobles[i] == 2) {
        display.fillCircle(80, y, 6, SSD1306_WHITE);
        display.fillCircle(80, y, 3, SSD1306_BLACK);
      } else {
        display.drawCircle(80, y, 6, SSD1306_WHITE);
      }
    }
  }

  display.display();

  // --- Lógica de los LEDs (GPIOs) ---

  // Orificio 1: solo tiene LED verde
  digitalWrite(o1, (h1 >= 1) ? HIGH : LOW);

  // Orificio 2: solo tiene LED verde
  digitalWrite(o2, (h2 >= 1) ? HIGH : LOW);

  // Orificio 3: solo tiene LED verde
  digitalWrite(o3, (h3 >= 1) ? HIGH : LOW);

  // Orificio 4: solo tiene LED verde
  digitalWrite(o4, (h4 >= 1) ? HIGH : LOW);

  // Orificio 5: solo tiene LED verde
  digitalWrite(o5, (h5 >= 1) ? HIGH : LOW);

  // Orificio 6: tiene LED verde (o6) y LED rojo (o6r)
  if (h6 == 1) {
    digitalWrite(o6, HIGH);
    digitalWrite(o6r, LOW);
  } else if (h6 == 2) {
    digitalWrite(o6, LOW);
    digitalWrite(o6r, HIGH);
  } else {
    digitalWrite(o6, LOW);
    digitalWrite(o6r, LOW);
  }

  // Orificio 7: tiene LED verde (o7) y LED rojo (o7r)
  if (h7 == 1) {
    digitalWrite(o7, HIGH);
    digitalWrite(o7r, LOW);
  } else if (h7 == 2) {
    digitalWrite(o7, LOW);
    digitalWrite(o7r, HIGH);
  } else {
    digitalWrite(o7, LOW);
    digitalWrite(o7r, LOW);
  }

  // Orificio 8: tiene LED verde (o8) y LED rojo (o8r)
  if (h8 == 1) {
    digitalWrite(o8, HIGH);
    digitalWrite(o8r, LOW);
  } else if (h8 == 2) {
    digitalWrite(o8, LOW);
    digitalWrite(o8r, HIGH);
  } else {
    digitalWrite(o8, LOW);
    digitalWrite(o8r, LOW);
  }
}

// =============================================================
// Función: mostrarTextoOLED
// =============================================================
// Muestra un texto centrado en la pantalla OLED
void mostrarTextoOLED(const char* linea1, const char* linea2 = "") {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Línea 1 - centrada verticalmente si no hay línea 2
  display.setCursor(0, 20);
  display.println(linea1);

  // Línea 2 (opcional)
  if (strlen(linea2) > 0) {
    display.setCursor(0, 40);
    display.println(linea2);
  }

  display.display();
}

// =============================================================
// Función: processNote (procesarNota)
// =============================================================
// Busca la nota recibida en la tabla de digitaciones.
// Si la encuentra, llama a alumbrarNota() con los valores
// correspondientes. Si no la encuentra, muestra un error.
void processNote(String nota) {
  // Eliminar espacios en blanco al inicio y final
  nota.trim();

  Serial.print("Nota recibida: ");
  Serial.println(nota);

  // Buscar la nota en la tabla de digitaciones
  for (int i = 0; i < NUM_NOTAS; i++) {
    if (nota.equals(tablaDigitaciones[i].nota)) {
      Serial.print("Digitación encontrada para: ");
      Serial.println(tablaDigitaciones[i].nota);

      // Mostrar el nombre de la nota en la parte superior de la OLED
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.print("Nota: ");
      display.println(tablaDigitaciones[i].nota);
      display.display();

      // Encender los LEDs según la digitación
      alumbrarNota(
        tablaDigitaciones[i].h1,
        tablaDigitaciones[i].h2,
        tablaDigitaciones[i].h3,
        tablaDigitaciones[i].h4,
        tablaDigitaciones[i].h5,
        tablaDigitaciones[i].h6,
        tablaDigitaciones[i].h7,
        tablaDigitaciones[i].h8
      );
      return;
    }
  }

  // Si la nota no se encontró en la tabla
  Serial.print("Nota NO encontrada: ");
  Serial.println(nota);
  mostrarTextoOLED("Nota no encontrada:", nota.c_str());
}

// =============================================================
// Función: webSocketEvent
// =============================================================
// Manejador de eventos del WebSocket.
// Se llama automáticamente cuando hay cambios en la conexión
// o cuando se reciben mensajes del servidor.
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {

    case WStype_DISCONNECTED:
      Serial.println("[WS] Desconectado del servidor");
      mostrarTextoOLED("Desconectado", "Reconectando...");
      break;

    case WStype_CONNECTED:
      Serial.print("[WS] Conectado a: ");
      Serial.println((char*)payload);
      mostrarTextoOLED("Conectado!", ipLocal.c_str());
      // Pequeña pausa para que el usuario vea el mensaje
      delay(1000);
      // Mostrar pantalla de espera
      mostrarTextoOLED("Listo", ipLocal.c_str());
      break;

    case WStype_TEXT:
      Serial.print("[WS] Mensaje recibido: ");
      Serial.println((char*)payload);
      // Procesar la nota recibida
      processNote(String((char*)payload));
      break;

    case WStype_ERROR:
      Serial.println("[WS] Error en WebSocket");
      mostrarTextoOLED("Error WS", "Reintentando...");
      break;

    case WStype_PING:
      Serial.println("[WS] Ping recibido");
      break;

    case WStype_PONG:
      Serial.println("[WS] Pong recibido");
      break;

    default:
      break;
  }
}

// =============================================================
// Función: apagarTodosLosLeds
// =============================================================
// Apaga todos los LEDs indicadores
void apagarTodosLosLeds() {
  digitalWrite(o1, LOW);
  digitalWrite(o2, LOW);
  digitalWrite(o3, LOW);
  digitalWrite(o4, LOW);
  digitalWrite(o5, LOW);
  digitalWrite(o6, LOW);
  digitalWrite(o6r, LOW);
  digitalWrite(o7, LOW);
  digitalWrite(o7r, LOW);
  digitalWrite(o8, LOW);
  digitalWrite(o8r, LOW);
}

// =============================================================
// setup() - Inicialización del sistema
// =============================================================
void setup() {
  // --- Inicializar puerto serial para depuración ---
  Serial.begin(115200);
  Serial.println();
  Serial.println("=================================");
  Serial.println("  Flauta Dulce - ESP32");
  Serial.println("  Dispositivo de enseñanza");
  Serial.println("=================================");

  // --- Configurar pines como salida ---
  pinMode(o1, OUTPUT);
  pinMode(o2, OUTPUT);
  pinMode(o3, OUTPUT);
  pinMode(o4, OUTPUT);
  pinMode(o5, OUTPUT);
  pinMode(o6, OUTPUT);
  pinMode(o6r, OUTPUT);
  pinMode(o7, OUTPUT);
  pinMode(o7r, OUTPUT);
  pinMode(o8, OUTPUT);
  pinMode(o8r, OUTPUT);

  // Apagar todos los LEDs al inicio
  apagarTodosLosLeds();

  // --- Inicializar pantalla OLED ---
  Wire.begin(OLED_SDA, OLED_SCK);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("Error: No se pudo inicializar la pantalla OLED");
    // Parpadear LED integrado para indicar error
    while (true) {
      delay(500);
    }
  }
  display.clearDisplay();
  display.display();
  Serial.println("Pantalla OLED inicializada correctamente");

  // --- Conectar a WiFi ---
  mostrarTextoOLED("Conectando WiFi...", wifi_ssid);
  Serial.print("Conectando a WiFi: ");
  Serial.println(wifi_ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);

  // Esperar conexión WiFi con indicador visual
  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    intentos++;

    // Mostrar progreso en OLED cada 2 segundos
    if (intentos % 4 == 0) {
      String msg = "Intento " + String(intentos / 2) + "s...";
      mostrarTextoOLED("Conectando WiFi...", msg.c_str());
    }

    // Si lleva más de 30 segundos, reiniciar ESP32
    if (intentos > 60) {
      Serial.println("\nError: No se pudo conectar al WiFi. Reiniciando...");
      mostrarTextoOLED("Error WiFi", "Reiniciando...");
      delay(2000);
      ESP.restart();
    }
  }

  // Conexión WiFi exitosa
  ipLocal = WiFi.localIP().toString();
  Serial.println();
  Serial.print("WiFi conectado. IP: ");
  Serial.println(ipLocal);

  mostrarTextoOLED("WiFi conectado!", ipLocal.c_str());
  delay(2000);

  // --- Inicializar cliente WebSocket ---
  Serial.print("Conectando WebSocket a: ");
  Serial.print(ws_host);
  Serial.print(":");
  Serial.print(ws_port);
  Serial.println(ws_path);

  mostrarTextoOLED("Conectando servidor...", ws_host);

  // Conectar usando WSS (SSL) ya que Railway usa HTTPS
  webSocket.beginSSL(ws_host, ws_port, ws_path);

  // Registrar el manejador de eventos
  webSocket.onEvent(webSocketEvent);

  // Configurar intervalo de reconexión automática (5 segundos)
  webSocket.setReconnectInterval(5000);

  // Habilitar heartbeat para mantener la conexión viva
  // Ping cada 15 segundos, timeout de 3 segundos, desconectar tras 2 fallos
  webSocket.enableHeartbeat(15000, 3000, 2);

  Serial.println("Cliente WebSocket inicializado");
  Serial.println("Esperando conexión al servidor...");
}

// =============================================================
// loop() - Bucle principal
// =============================================================
void loop() {
  // El cliente WebSocket necesita que se llame a loop()
  // continuamente para procesar mensajes entrantes,
  // manejar reconexiones y enviar heartbeats
  webSocket.loop();
}
