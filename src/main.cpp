#include <Adafruit_GFX.h>  // Core graphics library
#include <OSCMessage.h>
#include <P3RGB64x32MatrixPanel.h>

#include "wifi_connection.h"

// constructor with default pin wiring
P3RGB64x32MatrixPanel matrix;
Networking *net;

// Number of pixels of each INDIVIDUAL panel module.
#define PANEL_RES_X 64
#define PANEL_RES_Y 32
#define PANEL_CHAIN 1  // Total number of panels chained one to another

struct Point {
  uint8_t x;
  uint8_t y;
};

struct Line {
  Point center;
  uint8_t length;
  uint8_t angle;
};

std::vector<Line> lines = {{
    {{10, 10}, 10, 0},
}};

// GLOBAL STATE VARIABLES
// Are we currently connected?
boolean connected = false;
boolean initialized = false;

uint8_t red, green, blue;
float decayLength = 0.6;
float decayFactor = pow(decayLength, 0.016);
uint8_t brightness = 0;

void onBang(OSCMessage &msg);
void onColor(OSCMessage &msg);
void onDecay(OSCMessage &msg);

void onWifiChange(boolean _connected) {
  // was connected, now disconnected
  if (connected && !_connected) {
    connected = _connected;
    Serial.println("Stopping matrix...");
    matrix.stop();
    Serial.println(" [Matrix stopped.]");
    initialized = false;
  } else if (_connected && !initialized) {  // got connected, isn't initialized
    connected = _connected;
    Serial.print("Initializing matrix...");
    matrix.begin();
    matrix.fillRect(0, 0, matrix.width(), matrix.height(),
                    matrix.color444(0, 12, 0));
    initialized = true;
    Serial.println(" [Matrix initialized.]");
  }
}

void setup(void) {
  Serial.begin(115200);
  net = new Networking(&onWifiChange);
}

uint8_t fadeUint(uint8_t val, float factor) {
  return (uint8_t)(floorf(((float)val) * factor));
}

void loop(void) {
  if (!connected) {
    delay(1000);
    return;
  }

  // Render and decay
  brightness = fadeUint(brightness, decayFactor);
  uint16_t matrix_color = matrix.colorHSV(0, 0, brightness);
  matrix.fillScreen(matrix_color);

  uint16_t x0 = (uint16_t)rand() % PANEL_RES_X;
  uint16_t y0 = (uint16_t)rand() % PANEL_RES_Y;
  uint16_t length = (uint16_t)rand();
  uint16_t color = (uint16_t)rand();

  if (rand() % 2) {
    matrix.drawFastHLine(x0, y0, length, color);
  } else {
    matrix.drawFastVLine(x0, y0, length, color);
  }

  // Process UDP packets as OSC messages
  int size = Networking::udp->parsePacket();
  if (size <= 0) return;
  OSCMessage inmsg;
  while (size--) {
    inmsg.fill(Networking::udp->read());
  }

  // Process OSC messages as commands using local callback functions
  if (!inmsg.hasError()) {
    inmsg.dispatch("/bang", onBang);
    inmsg.dispatch("/color", onColor);
    inmsg.dispatch("/decay", onDecay);
  } else {
    Serial.println("Error in OSC message.");
  }

  // Rinse & repeat
  inmsg.empty();
  delay(16);
}

void onBang(OSCMessage &msg) {
  /* Serial.println("onBang"); */
  brightness = UINT8_MAX;
}

void onDecay(OSCMessage &msg) {
  float new_decay = msg.getFloat(0);
  // Serial.printf("new decay: %f\n", new_decay);
  decayFactor = new_decay;
}

void onColor(OSCMessage &msg) {
  if (!msg.isString(0)) Serial.println("isString(0) error");
  if (!msg.isFloat(1)) Serial.println("isFloat(1) error");
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
