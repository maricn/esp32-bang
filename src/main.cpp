#include <Adafruit_GFX.h>  // Core graphics library
#include <OSCMessage.h>
#include <P3RGB64x32MatrixPanel.h>
#include <list>

#include "geometry/util.h"
#include "geometry/component.h"
#include "geometry/line.h"
#include "geometry/point.h"
#include "geometry/screen.h"
#include "wifi_connection.h"

// constructor with default pin wiring
P3RGB64x32MatrixPanel matrix;
Networking *net;


std::list<Component *> components;
Screen *screen = new Screen(&matrix);

// GLOBAL STATE VARIABLES
// Are we currently connected?
boolean connected = false;
boolean initialized = false;

uint8_t red, green, blue;

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
    return;
  }

  // Tick and render all components
  for (Component *component : components) {
    bool componentAlive = component->tick();
    if (!componentAlive) {
      components.remove(component);
      delete component;
      continue;
    }

    component->render();
  }
  /* brightness = fadeUint(brightness, decayFactor); */
  /* uint16_t matrix_color = matrix.colorHSV(0, 0, brightness); */
  /* matrix.fillScreen(matrix_color); */

  /* uint16_t x0 = (uint16_t)rand() % PANEL_RES_X; */
  /* uint16_t y0 = (uint16_t)rand() % PANEL_RES_Y; */
  /* uint16_t length = (uint16_t)rand(); */
  /* uint16_t color = (uint16_t)rand(); */

  /* if (rand() % 2) { */
  /*   matrix.drawFastHLine(x0, y0, length, color); */
  /* } else { */
  /*   matrix.drawFastVLine(x0, y0, length, color); */
  /* } */

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
    inmsg.dispatch("/line", onLine);
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
  screen->setBrightness(0x1f);
}

void onDecay(OSCMessage &msg) {
  float new_decay = msg.getFloat(0);
  // Serial.printf("new decay: %f\n", new_decay);
  screen->setDecayFactor(new_decay);
}

void onLine(OSCMessage &msg) {
  Line *line = new Line(&matrix,
                        new Point((uint8_t)(esp_random() % PANEL_RES_X),
                                  (uint8_t)(esp_random() % PANEL_RES_Y)),
                        esp_random() % 20, (float) ((int8_t) (esp_random() & 0x7f) << 1) / (float) INT8_MAX);
  components.push_back(line);
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
