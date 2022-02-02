#ifndef __wifi_h_
#define __wifi_h_

#include <WiFi.h>
#include <WiFiUdp.h>

class Networking {
private:
  static const char *ssid;
  static const char *pass;
  static const int udpPort;
  static bool connecting;
  static void (*onWifiChange)(boolean);

  static void connect();
public:
  Networking(void (*onWifiChange)(boolean));
  static void WiFiEvent(WiFiEvent_t event);
  static WiFiUDP *udp;
};

#endif
