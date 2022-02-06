#include <Adafruit_GFX.h>  // Core graphics library
#include <OSCMessage.h>
#include <P3RGB64x32MatrixPanel.h>

#define DEBUG_GFX 0
#define DEBUG_OSC 0

#include <list>

#include "geometry/component.h"
#include "geometry/line.h"
#include "geometry/point.h"
#include "geometry/screen.h"
#include "geometry/util.h"
#include "wifi/wifi_connection.h"

// constructor with default pin wiring
P3RGB64x32MatrixPanel matrix(true);
Networking *net;

std::list<Component *> components;
Screen *screen = new Screen(&matrix);

// GLOBAL STATE VARIABLES
// Are we currently connected?
boolean connected = false;
boolean initialized = false;

void onBang(OSCMessage &msg);
void onLine(OSCMessage &msg);
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
    matrix.swapBuffer();
    components.push_back(screen);
    initialized = true;
    Serial.println(" [Matrix initialized.]");
  }
}

void setup(void) {
  Serial.begin(115200);
  net = new Networking(&onWifiChange);
}

void loop(void) {
  if (!connected) {
    delay(1000);
    Serial.print(".");
    return;
  }

  // Tick and render all components
  std::list<Component *>::iterator i = components.begin();
  while (i != components.end()) {
    Component *component = (*i);
    bool componentAlive = component->tick();
    if (!componentAlive) {
      i = components.erase(i);
      /* delete component; */
    } else {
      component->render();
      ++i;
    }
  }
  matrix.swapBuffer();

  // Process UDP packets as OSC messages
  while (true) {
    int size = Networking::udp->parsePacket();
    if (size <= 0) break;
    OSCMessage inmsg;
    while (size--) {
      inmsg.fill(Networking::udp->read());
    }

    // Process OSC messages as commands using local callback functions
    if (!inmsg.hasError()) {
      inmsg.dispatch("/bang", onBang);
      inmsg.dispatch("/line", onLine);
      inmsg.dispatch("/color", onColor);
      inmsg.dispatch("/decay", onDecay);
    } else {
      Serial.println("Error in OSC message.");
    }

    // Rinse & repeat
    inmsg.empty();
  }
  delay(16);
}

void onBang(OSCMessage &msg) {
#if DEBUG_OSC
  Serial.printf("[DEBUG] OSC.onBang\n");
#endif
  screen->setBrightness((uint8_t)0x1f);
}

void onDecay(OSCMessage &msg) {
  float new_decay = msg.getFloat(0);
#if DEBUG_OSC
  Serial.printf("[DEBUG] OSC.onDecay: %f\n", new_decay);
#endif
  screen->setDecayLength(new_decay);
}

void onLine(OSCMessage &msg) {
  uint8_t type = (uint8_t)msg.getInt(0);
#if DEBUG_OSC
  Serial.printf("[DEBUG] OSC.onLine: %u\n", type);
#endif
  // coordinates within 64x32, at least (8,4) or lower (x,y)
  Point center = {(uint8_t)(esp_random() & 0x3f | 0x8),
                  (uint8_t)(esp_random() & 0x1f | 0x4)};
  // can't be zero bc tick() when zero can make it 255
  uint8_t radius = (uint8_t)(esp_random() & 0b000011110) | 0b01;

  switch (type) {
    case 1:
      components.push_back(
          new LineThatRotates(&matrix, center, radius, esp_random() % 90));
      break;
    case 2:
      components.push_back(
          new LineThatShakes(&matrix, center, radius, esp_random() % 90));
      break;
    case 0:
    default:
      components.push_back(
          new Line(&matrix, center, radius, esp_random() % 90));
      break;
  }
}

void onColor(OSCMessage &msg) {
  if (!msg.isInt(0)) Serial.println("isFloat(0) error");
  uint8_t val32 = msg.getInt(0) & 0x1f;
#if DEBUG_OSC
  Serial.printf("[DEBUG] OSC.onColor: %u\n", val32);
#endif
  screen->setHue(val32);
}
