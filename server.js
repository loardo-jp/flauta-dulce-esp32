// server.js — Relay WebSocket para flauta dulce educativa
const express = require('express');
const http = require('http');
const { WebSocketServer } = require('ws');
const path = require('path');

const PORT = process.env.PORT || 3000;

// Express sirve archivos estáticos desde public/
const app = express();
app.use(express.static(path.join(__dirname, 'public')));

const server = http.createServer(app);

// WebSocket server — sin path propio, manejaremos upgrade manualmente
const wss = new WebSocketServer({ noServer: true });

// Conjuntos de clientes registrados
const browsers = new Set();
const esp32s = new Set();

server.on('upgrade', (req, socket, head) => {
  if (req.url === '/ws') {
    wss.handleUpgrade(req, socket, head, (ws) => {
      wss.emit('connection', ws, req);
    });
  } else {
    socket.destroy();
  }
});

wss.on('connection', (ws) => {
  console.log('[+] Nueva conexión WebSocket');

  ws.deviceType = null;

  ws.on('message', (data) => {
    const msg = data.toString();

    // Intentar parsear como JSON (registro)
    try {
      const json = JSON.parse(msg);
      if (json.type === 'register') {
        ws.deviceType = json.device;
        if (json.device === 'browser') browsers.add(ws);
        else if (json.device === 'esp32') esp32s.add(ws);
        console.log(`[✓] Registrado: ${json.device} (browsers=${browsers.size}, esp32s=${esp32s.size})`);
        return;
      }
    } catch (_) {
      // No es JSON — es una nota
    }

    // Si es un browser, reenviar la nota a todos los ESP32
    if (ws.deviceType === 'browser') {
      for (const client of esp32s) {
        if (client.readyState === 1) client.send(msg);
      }
      console.log(`[♪] Nota: ${msg} → ${esp32s.size} ESP32(s)`);
    }
  });

  ws.on('close', () => {
    browsers.delete(ws);
    esp32s.delete(ws);
    console.log(`[-] Desconexión (browsers=${browsers.size}, esp32s=${esp32s.size})`);
  });
});

server.listen(PORT, () => {
  console.log(`Servidor escuchando en puerto ${PORT}`);
});
