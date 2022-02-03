#include "wifi_connection.h"

const char *Networking::ssid = "REDACTED";
const char *Networking::pass = "REDACTED";
const int Networking::udpPort = 3333;
bool Networking::connecting = false;
void (*Networking::onWifiChange)(boolean) = NULL;
WiFiUDP *Networking::udp = new WiFiUDP();

Networking::Networking(void (*_onWifiChange)(boolean)) {
  Networking::onWifiChange = _onWifiChange;

  Serial.printf("Register WiFi event handler...");
  WiFi.onEvent(Networking::WiFiEvent);
  delay(100);

  Serial.printf("Set WiFi mode to STA...");
  WiFi.mode(WIFI_STA);
  delay(100);

  Networking::connect();
};

void Networking::connect() {
  Serial.printf("Deleting old WiFi config...");
  WiFi.disconnect(true);
  delay(500);

  Serial.printf(" Connecting to WiFi network: '%s'...", ssid);
  WiFi.begin(Networking::ssid, Networking::pass);
  Serial.println("Started the WiFi connection handshake...");
  delay(500);
}

void Networking::WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.println("WiFi connected! IP address: " +
                     WiFi.localIP().toString());
      Networking::onWifiChange(true);
      // initializes the UDP state
      // This initializes the transfer buffer
      Networking::udp->begin(Networking::udpPort);
      Networking::connecting = false;
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.print("WiFi lost connection... ");
      Networking::udp->stop();
      Serial.println("Stopped UDP.");
      Networking::onWifiChange(false);
      if (!Networking::connecting) {
        Serial.println("Will start reconnection...");
        Networking::connecting = true;
        Networking::connect();
      }
      break;
  }
}
