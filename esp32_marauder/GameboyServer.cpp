#include "GameboyServer.h"

void gbStartAP(char* ssid, char* password) {
  const IPAddress _IP(172, 0, 0, 1);
  // WiFi.softAP(ssid, password);
  // _IP = WiFi.softAPIP();
  WiFi.softAPConfig(_IP, _IP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, password);
  _json["type"] = "connected";
  _json["ip"] = _IP;
  _json["ssid"] = ssid;
  _json["password"] = password;
  
  Serial.print("JSON:");
  serializeJson(_json, Serial);
  Serial.println();

  _dnsServer.start(53, "*", _IP);
}
