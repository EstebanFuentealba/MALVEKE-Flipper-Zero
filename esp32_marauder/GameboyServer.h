#ifndef gameboy_server_h
#define gameboy_server_h

#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <DNSServer.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <cstdint>
#include <cstring>
#include <sstream>
#include <vector>
#include <string>

#define MAX_HTML_SIZE 20000

DNSServer _dnsServer;
IPAddress _IP;
AsyncWebServer _server(80);
DynamicJsonDocument _json(2048);


int _index_html_len;
const uint8_t* _index_html;

void gbStartAP(char* ssid, char* password);

#endif
