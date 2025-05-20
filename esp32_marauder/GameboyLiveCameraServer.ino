#include "GameboyServer.h"
#include "Gameboy.h"
#include "configs.h"
#include "GameboyLiveCameraServer.h"
#include "gameboy_live_stream_server_assets.h"
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"
#include <base64.h>


unsigned char raw_buffer[FRAME_SIZE]; // max( 16*8*14*8, 16*14*16 ) sensor pixels , tile bytes
GameboyLiveCameraServer gameboy_live_camera_server;
HardwareSerial Serial3(3);
AsyncEventSource events("/events");

// Variables para control de exposición
int32_t final_exposure = 0x0500; // Tiempo de exposición inicial


GameboyLiveCameraServer::GameboyLiveCameraServer()
{
  this->runGameboyLiveCameraServer = false;
}
void GameboyLiveCameraServer::begin()
{
  this->setup();
}
void GameboyLiveCameraServer::start()
{
  //  Initial values
  this->trigger = 0x03;
  this->unk1 = 0xE8;
  this->exposure_time = final_exposure; // 0x0500;
  this->unk2 = 0x24;
  this->unk3 = 0xBF;
  this->dithering = true;
  this->rotate90 = false;
  this->take_a_picture = false;

  pinMode(GAMEBOY_RST, OUTPUT);
  pinMode(GAMEBOY_CLK, OUTPUT);
  pinMode(GAMEBOY_WR, OUTPUT);
  pinMode(GAMEBOY_RD, OUTPUT);
  pinMode(GAMEBOY_CS, OUTPUT);

  // Set Control Pins to Output RST(PH0) CLK(PH1) CS(PH3) WR(PH5) RD(PH6)
  for (uint32_t i = 0; i < sizeof(ADDRESS_GB_GBC_PINS) / sizeof(ADDRESS_GB_GBC_PINS[0]); i++)
  {
    pinMode(ADDRESS_GB_GBC_PINS[i], OUTPUT);
  }
  // Set Data Pins (D0-D7) to Input
  for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
  {
    pinMode(DATA_GB_GBC_PINS[i], INPUT);
  }
  this->setAddress(0x0000);

  digitalWrite(GAMEBOY_CLK, LOW);
  digitalWrite(GAMEBOY_RST, LOW);
  digitalWrite(GAMEBOY_WR, HIGH);
  digitalWrite(GAMEBOY_RD, HIGH);
  digitalWrite(GAMEBOY_CS, HIGH);

  delay(400);
  digitalWrite(GAMEBOY_RST, HIGH);
  this->runGameboyLiveCameraServer = true;
}
void GameboyLiveCameraServer::stop()
{
  //  Initial values
  this->trigger = 0x03;
  this->unk1 = 0xE8;
  this->exposure_time = final_exposure; //0x0500;
  this->unk2 = 0x24;
  this->unk3 = 0xBF;
  this->runGameboyLiveCameraServer = false;
}
void GameboyLiveCameraServer::dataBusAsOutput()
{
  for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
  {
    pinMode(DATA_GB_GBC_PINS[i], OUTPUT);
  }
}
void GameboyLiveCameraServer::dataBusAsInput()
{
  for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
  {
    pinMode(DATA_GB_GBC_PINS[i], INPUT);
  }
}

// Set Gameboy mode
void GameboyLiveCameraServer::gb_mode(void)
{
  // Address pins as outputs
  for (uint32_t i = 0; i < sizeof(ADDRESS_GB_GBC_PINS) / sizeof(ADDRESS_GB_GBC_PINS[0]); i++)
  {
    pinMode(ADDRESS_GB_GBC_PINS[i], OUTPUT);
  }
  // Set Data Pins (D0-D7) to Input
  this->dataBusAsInput();
}
bool GameboyLiveCameraServer::isRunning() {
  return this->runGameboyLiveCameraServer;
}
void GameboyLiveCameraServer::setAddress(unsigned int addr)
{
  // Write each of the bits into the address pins
  for (uint32_t i = 0; i < sizeof(ADDRESS_GB_GBC_PINS) / sizeof(ADDRESS_GB_GBC_PINS[0]); i++)
  {
    digitalWrite(ADDRESS_GB_GBC_PINS[i], addr & (1 << i) ? HIGH : LOW);
  }
}
void GameboyLiveCameraServer::setWriteMode(int is_rom)
{
  if (is_rom)
    digitalWrite(GAMEBOY_CS, HIGH);
  else
    digitalWrite(GAMEBOY_CS, LOW);

  // Set Data Pins (D0-D7) to Output
  for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
  {
    pinMode(DATA_GB_GBC_PINS[i], OUTPUT);
  }
}
int GameboyLiveCameraServer::readCartByte(unsigned int address)
{
  this->setAddress(address);

  this->setReadMode(address < 0x8000);

  unsigned int value = this->getData();
  this->setWaitMode();

  return value;
}
void GameboyLiveCameraServer::setData(unsigned int value)
{
  for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
  {
    digitalWrite(DATA_GB_GBC_PINS[i], value & (1 << i) ? HIGH : LOW);
  }
}
unsigned int GameboyLiveCameraServer::getData(void)
{
  int i;
  unsigned int value = 0;
  for (i = 0; i < 8; i++)
    if (digitalRead(DATA_GB_GBC_PINS[i]) == HIGH)
      value |= (1 << i);
  return value;
}
unsigned int GameboyLiveCameraServer::getDataBit0(void)
{
  return (digitalRead(DATA_GB_GBC_PINS[0]) == HIGH);
}
void GameboyLiveCameraServer::performWrite(void)
{
  digitalWrite(GAMEBOY_WR, LOW);
  digitalWrite(GAMEBOY_RD, HIGH);
}
void GameboyLiveCameraServer::setWaitMode(void)
{
  digitalWrite(GAMEBOY_CS, HIGH);
  digitalWrite(GAMEBOY_WR, HIGH);
  digitalWrite(GAMEBOY_RD, HIGH);

  // Set Data Pins (D0-D7) to Input
  this->dataBusAsInput();
}
void GameboyLiveCameraServer::setReadMode(int is_rom)
{
  // Set Data Pins (D0-D7) to Input
  this->dataBusAsInput();

  digitalWrite(GAMEBOY_WR, HIGH);
  digitalWrite(GAMEBOY_RD, LOW);

  if (is_rom)
    digitalWrite(GAMEBOY_CS, HIGH);
  else
    digitalWrite(GAMEBOY_CS, LOW);
}
void GameboyLiveCameraServer::writeCartByte(unsigned int address, unsigned int value)
{
  this->setAddress(address);

  this->setWriteMode(address < 0x8000);

  this->setData(value);

  this->performWrite();

  if (address >= 0x8000)
  {
    // PHI clock signal
    digitalWrite(GAMEBOY_CLK, LOW);
    // wait(120);
    asm volatile("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");
    digitalWrite(GAMEBOY_CLK, HIGH);
    asm volatile("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");
    // wait(120);
    digitalWrite(GAMEBOY_CLK, LOW);
  }

  this->setWaitMode();
}

void GameboyLiveCameraServer::readPicture(bool is_thumbnail)
{
  this->ramEnable();  // Enable RAM
  this->setRegisterMode();  // Set register mode
  this->writeCartByte(0x0000, 0x0A); // Enable RAM
  this->writeCartByte(0x4000, 0x00); // Set RAM mode, bank 0

  unsigned int addr = 0xA100;
  unsigned int _size = 16 * 14 * 16;
  this->setReadMode(0xA100 < 0x8000);

  int inc = 0;
  uint32_t totalLuminance = 0;

  while (_size--)
  {
    this->setAddress(addr++);
    int value = this->getData();
    raw_buffer[inc] = value;

    // Sumar el valor de luminancia para ajustar la exposición
    totalLuminance += value;
    inc++;
  }
  String colorIndices = base64::encode(raw_buffer, sizeof(raw_buffer));
  events.send(colorIndices.c_str(), "frame", millis());
  delayMicroseconds(10);
  this->setWaitMode();
}
void GameboyLiveCameraServer::ramEnable()
{
  this->writeCartByte(0x0000, 0x0A);
}
void GameboyLiveCameraServer::setRegisterMode(void)
{
  this->writeCartByte(0x4000, 0x10);
}
void GameboyLiveCameraServer::ramDisable()
{
  this->writeCartByte(0x0000, 0x00);
}

void GameboyLiveCameraServer::updateMatrixRegisters()
{
  unsigned char c1 = 0x40, c2 = 0x80, c3 = 0xC0;

  // const unsigned char matrix[] = // high light
  //{
  //     0x89, 0x92, 0xA2, 0x8F, 0x9E, 0xC6, 0x8A, 0x95, 0xAB, 0x91, 0xA1, 0xCF,
  //     0x8D, 0x9A, 0xBA, 0x8B, 0x96, 0xAE, 0x8F, 0x9D, 0xC3, 0x8C, 0x99, 0xB7,
  //     0x8A, 0x94, 0xA8, 0x90, 0xA0, 0xCC, 0x89, 0x93, 0xA5, 0x90, 0x9F, 0xC9,
  //     0x8E, 0x9C, 0xC0, 0x8C, 0x98, 0xB4, 0x8E, 0x9B, 0xBD, 0x8B, 0x97, 0xB1
  // };

  const unsigned char matrix[48] = // low light
  {
    0x8C, 0x98, 0xAC, 0x95, 0xA7, 0xDB, 0x8E, 0x9B, 0xB7, 0x97, 0xAA, 0xE7,
    0x92, 0xA2, 0xCB, 0x8F, 0x9D, 0xBB, 0x94, 0xA5, 0xD7, 0x91, 0xA0, 0xC7,
    0x8D, 0x9A, 0xB3, 0x96, 0xA9, 0xE3, 0x8C, 0x99, 0xAF, 0x95, 0xA8, 0xDF,
    0x93, 0xA4, 0xD3, 0x90, 0x9F, 0xC3, 0x92, 0xA3, 0xCF, 0x8F, 0x9E, 0xBF
  };

  int i;
  for (i = 0; i < 48; i++)
  {
    if (this->dithering)
    {
      this->writeCartByte(0xA006 + i, matrix[i]);
    }
    else
    {
      switch (i % 3)
      {
        case 0:
          this->writeCartByte(0xA006 + i, c1);
          break;
        case 1:
          this->writeCartByte(0xA006 + i, c2);
          break;
        case 2:
          this->writeCartByte(0xA006 + i, c3);
          break;
      }
    }
  }
}
unsigned int GameboyLiveCameraServer::waitPictureReady(void)
{
  this->setRegisterMode();
  this->writeCartByte(0x4000, 0x10); // Set register mode
  unsigned long int clocks = 0;
  this->setAddress(0xA000);
  this->setReadMode(0xA000 < 0x8000);
  while (1)
  {
    if (this->getDataBit0() == 0)
      break;
    digitalWrite(GAMEBOY_CLK, LOW); // clock
    asm volatile("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");
    digitalWrite(GAMEBOY_CLK, HIGH);
    asm volatile("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");
    clocks++;
  }
  digitalWrite(GAMEBOY_CLK, LOW);
  asm volatile("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");
  this->setWaitMode();
  return clocks;
}

void GameboyLiveCameraServer::setup()
{
  //  Initial values
  this->trigger = 0x03;
  this->unk1 = 0xE8;
  this->exposure_time = 0x0500;
  this->unk2 = 0x24;
  this->unk3 = 0xBF;
  this->previous_luminance = 128;
  delay(400);
}
void GameboyLiveCameraServer::enableDithering() {
  this->dithering = true;
}
void GameboyLiveCameraServer::disableDithering() {
  this->dithering = false;
}
void GameboyLiveCameraServer::increaseExposure() {
  // this->exposure_time += 0x80;
  final_exposure += 0x80;
  this->exposure_time = final_exposure;
}
void GameboyLiveCameraServer::decreaseExposure() {
  // this->exposure_time -= 0x80;
  final_exposure -= 0x80;
  this->exposure_time = final_exposure;
}
void GameboyLiveCameraServer::setExposure(uint16_t value) {
  this->exposure_time = value;
}


LinkedList<String> GameboyLiveCameraServer::parseCommand(String input, char* delim) {
  LinkedList<String> cmd_args;

  bool inQuote = false;
  bool inApostrophe = false;
  String buffer = "";

  for (int i = 0; i < input.length(); i++) {
    char c = input.charAt(i);

    if (c == '"') {
      // Check if the quote is within an apostrophe
      if (inApostrophe) {
        buffer += c;
      } else {
        inQuote = !inQuote;
      }
    } else if (c == '\'') {
      // Check if the apostrophe is within a quote
      if (inQuote) {
        buffer += c;
      } else {
        inApostrophe = !inApostrophe;
      }
    } else if (!inQuote && !inApostrophe && strchr(delim, c) != NULL) {
      cmd_args.add(buffer);
      buffer = "";
    } else {
      buffer += c;
    }
  }

  // Add the last argument
  if (!buffer.isEmpty()) {
    cmd_args.add(buffer);
  }

  return cmd_args;
}
String GameboyLiveCameraServer::getSerialInput() {
  String input = "";

  if (Serial.available() > 0)
    input = Serial.readStringUntil('\n');

  input.trim();
  return input;
}
void GameboyLiveCameraServer::main()
{
  if (this->runGameboyLiveCameraServer)
  {
    // Adjust exposure automatically before capturing the image
    //  Capture Picture

    this->ramEnable();                 // Enable RAM
    this->setRegisterMode();           // Set register mode
    this->writeCartByte(0xA000, 0x00); // Set RAM mode, bank 0
    this->writeCartByte(0xA001, this->unk1);
    this->writeCartByte(0xA002, (this->exposure_time >> 8) & 0xFF);
    this->writeCartByte(0xA003, this->exposure_time & 0xFF);
    this->writeCartByte(0xA004, this->unk2);
    this->writeCartByte(0xA005, this->unk3);
    this->updateMatrixRegisters();
    this->writeCartByte(0xA000, this->trigger);
    unsigned int clks = 0;
    while (1)
    {
      clks += this->waitPictureReady();
      int a = this->readCartByte(0xA000);
      if ((a & 1) == 0)
        break;
    }
    this->ramDisable();

    //  Read Picture
    this->readPicture(0);

  }
}


void gb_camera_live_setup() {

  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  disableCore0WDT();
  strncpy(index_html, reinterpret_cast<const char*>(live_stream_index_html), sizeof(live_stream_index_html));
  gbStartAP("[MALVEKE] Live Cam", "12345678");

  // setup ......
  events.onConnect([](AsyncEventSourceClient * client) {
    if (client->lastId()) {
      Serial.printf("Client reconnected! Last message ID that it gat is: %u\n", client->lastId());
    }
    //send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 1000);
  });
  _server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request){
      AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", live_stream_index_js_gz, live_stream_index_js_gz_len);
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
  });
  _server.on("/index.css", HTTP_GET, [](AsyncWebServerRequest *request){
      AsyncWebServerResponse *response = request->beginResponse_P(200, "text/css", live_stream_index_css_gz, live_stream_index_css_gz_len);
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
  });
  _server.on("/Button.js", HTTP_GET, [](AsyncWebServerRequest *request){
      AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", live_stream_Button_js_gz, live_stream_Button_js_gz_len);
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
  });
  _server.on("/Select.js", HTTP_GET, [](AsyncWebServerRequest *request){
      AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", live_stream_Select_js_gz, live_stream_Select_js_gz_len);
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
  });
  _server.on("/gbcamera-logo.png", HTTP_GET, [](AsyncWebServerRequest *request){
      AsyncWebServerResponse *response = request->beginResponse_P(200, "image/png", live_stream_gbcamera_logo_png_gz, live_stream_gbcamera_logo_png_gz_len);
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
  });
  _server.on("/start_gb_camera", HTTP_GET, [gameboy_live_camera_server](AsyncWebServerRequest * request) {
    gameboy_live_camera_server.begin();
    gameboy_live_camera_server.start();
    String json = "{\"status\": true }";
    request->send(200, "application/json", json);
  });
  _server.on("/stop_gb_camera", HTTP_GET, [gameboy_live_camera_server](AsyncWebServerRequest * request) {
    gameboy_live_camera_server.begin();
    gameboy_live_camera_server.stop();
    String json = "{\"status\": true }";
    request->send(200, "application/json", json);
  });
  _server.on("/decrease_exposure", HTTP_GET, [gameboy_live_camera_server](AsyncWebServerRequest * request) {
    gameboy_live_camera_server.decreaseExposure();
    String json = "{\"status\": true }";
    request->send(200, "application/json", json);
  });
  _server.on("/increase_exposure", HTTP_GET, [gameboy_live_camera_server](AsyncWebServerRequest * request) {
    gameboy_live_camera_server.increaseExposure();
    String json = "{\"status\": true }";
    request->send(200, "application/json", json);
  });
  _server.on("/set_exposure", HTTP_GET, [gameboy_live_camera_server](AsyncWebServerRequest * request) {
    const AsyncWebParameter* p = request->getParam("value");
    uint16_t exposure_value = static_cast<uint16_t>(strtoul(p->value().c_str(), nullptr, 10));

    gameboy_live_camera_server.setExposure(exposure_value);
    String json = "{\"status\": true }";
    request->send(200, "application/json", json);
  });
  _server.on("/enable_dithering", HTTP_GET, [gameboy_live_camera_server](AsyncWebServerRequest * request) {
    gameboy_live_camera_server.enableDithering();
    String json = "{\"status\": true }";
    request->send(200, "application/json", json);
  });
  _server.on("/disable_dithering", HTTP_GET, [gameboy_live_camera_server](AsyncWebServerRequest * request) {
    gameboy_live_camera_server.disableDithering();
    String json = "{\"status\": true }";
    request->send(200, "application/json", json);
  });
  _server.on("/", HTTP_GET, [live_stream_index_html, gameboy_live_camera_server](AsyncWebServerRequest * request) {
    // request->send_P(200, "text/html", reinterpret_cast<const char*>(html.c_str()));
    request->send_P(200, "text/html", reinterpret_cast<const char*>(live_stream_index_html));
  });
  _server.addHandler(&events);
  _server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);

  _server.begin();
}

void gb_camera_live_loop() {
  gameboy_live_camera_server.main();
}
