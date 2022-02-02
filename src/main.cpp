#include <OSCMessage.h>
/* #include <ESP32-HUB75-MatrixPanel-I2S-DMA.h> */

#include <Adafruit_GFX.h> // Core graphics library
#include <P3RGB64x32MatrixPanel.h>

// constructor with default pin wiring
P3RGB64x32MatrixPanel matrix;

#define PANEL_RES_X 64 // Number of pixels wide of each INDIVIDUAL panel module.
#define PANEL_RES_Y 32 // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN 1  // Total number of panels chained one to another

// Are we currently connected?
boolean connected = false;
boolean initialized = false;

uint8_t red, green, blue;
float decayLength = 0.6;
float decayFactor = pow(decayLength, 0.016);

#include "wifi.h"

void onBang(OSCMessage &msg);
void onColor(OSCMessage &msg);

void onWifiChange(boolean _connected) {
  // was connected, now disconnected
  if (connected && !_connected) {
    connected = _connected;
    Serial.println("Stopping matrix...");
    matrix.stop();
    Serial.println(" [Matrix stopped.]");
    initialized = false;
  } else 
    // got connected, isn't initialized
  if (_connected && !initialized) {
    connected = _connected;
    Serial.print("Initializing matrix...");
    matrix.begin();
    matrix.fillRect(0, 0, matrix.width(), matrix.height(),
                    matrix.color444(0, 12, 0));
    initialized = true;
    Serial.println(" [Matrix initialized.]");
  }
}

Networking *net;

void setup(void) {
  Serial.begin(115200);
  net = new Networking(&onWifiChange);
}

void loop(void) {
  if (!connected) {
    delay(1000);
    return;
  }

  // Render and decay
  matrix.fillRect(0, 0, matrix.width(), matrix.height(),
                  matrix.color555(red, green, blue));
  red *= decayFactor;
  green *= decayFactor;
  blue *= decayFactor;

  // Process UDP packets as OSC messages
  int size = Networking::udp->parsePacket();
  if (size <= 0)
    return;
  OSCMessage inmsg;
  while (size--) {
    inmsg.fill(Networking::udp->read());
  }

  // Process OSC messages as commands using local callback functions
  if (!inmsg.hasError()) {
    inmsg.dispatch("/bang", onBang);
    inmsg.dispatch("/color", onColor);
  } else {
    Serial.println("Error in OSC message.");
  }

  // Rinse & repeat
  inmsg.empty();
  delay(16);
}

void onBang(OSCMessage &msg) {
  /* Serial.println("onBang"); */
  uint32_t rnd = esp_random();
  red = 0x1 | (rnd & 0xf);
  green = 0x1 | ((rnd >> 4) & 0xf);
  blue = 0x1 | ((rnd >> 8) & 0xf);
}

void onColor(OSCMessage &msg) {
  if (!msg.isString(0))
    Serial.println("isString(0) error");
  if (!msg.isFloat(1))
    Serial.println("isFloat(1) error");
  char color[8];
  msg.getString(0, color);
  uint8_t val32 = msg.getInt(1) & 0x1f;
  /* Serial.printf("onColor: %s(%u)\n", color, val32); */
  if (strcmp("red", color) == 0) {
    red = val32;
    return;
  }
  if (strcmp("green", color) == 0) {
    green = val32;
    return;
  }
  if (strcmp("blue", color) == 0) {
    blue = val32;
    return;
  }
}
