#include "GameboyServer.h"
#include "Gameboy.h"
#include "configs.h"
#include "GameboyLiveCameraServer.h"

#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"
#include <base64.h>


unsigned char raw_buffer[FRAME_SIZE]; // max( 16*8*14*8, 16*14*16 ) sensor pixels , tile bytes
// static unsigned char GBCAM_BUFFER[14336];
// uint8_t linear_buffer_a[128 * 112 / 4];
// uint32_t luma_acc = 0;
// uint32_t luma_idx = 0;
GameboyLiveCameraServer gameboy_live_camera_server;
HardwareSerial Serial3(3);
AsyncEventSource events("/events");

// Variables para control de exposición
// uint32_t luma_hist[5] = {0};   // Histograma de luminancia
// uint32_t luma_mean = 0;        // Luminancia media
// int32_t ae_error = 0;          // Error de autoexposición
// int32_t ae_rate = 0;           // Tasa de cambio del error
// int32_t ae_last_error = 0;     // Último error registrado
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
/*
  // Set the 16 bit address
  void GameboyLiveCameraServer::set_address_GB(uint16_t address)
  {
    // Write each of the bits into the address pins
    for (uint32_t i = 0; i < sizeof(ADDRESS_GB_GBC_PINS) / sizeof(ADDRESS_GB_GBC_PINS[0]); i++)
    {
        digitalWrite(ADDRESS_GB_GBC_PINS[i], address & (1 << i) ? HIGH : LOW);
    }
  }*/
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
// void GameboyLiveCameraServer::adjustExposure()
// {
//     // Reset luma accumulator and index
//     luma_acc = 0;
//     luma_idx = 0;

//     // Capture an image first to measure the average luminance
//     this->ramEnable();                 // Enable RAM
//     this->setRegisterMode();           // Set register mode
//     this->writeCartByte(0x4000, 0x10); // Set register mode
//     this->writeCartByte(0xA000, 0x02); // Trigger the capture

//     // Wait for the picture to be ready
//     unsigned long clocks = this->waitPictureReady();
//     this->readPicture(false); // Read the full resolution image into raw_buffer

//     // Calculate average luminance
//     for (int i = 0; i < sizeof(raw_buffer); i++)
//     {
//         luma_acc += raw_buffer[i];
//         luma_idx++;
//     }

//     uint32_t avg_luma = luma_acc / luma_idx;

//     // Adjust exposure based on the average luminance
//     if (avg_luma > 0x80) // Example threshold, can be adjusted
//     {
//         this->decreaseExposure();
//     }
//     else if (avg_luma < 0x50) // Example threshold, can be adjusted
//     {
//         this->increaseExposure();
//     }
// }

// void GameboyLiveCameraServer::readPicture(bool is_thumbnail)
// {
//     this->writeCartByte(0x0000, 0x0A); // Enable RAM
//     this->writeCartByte(0x4000, 0x00); // Set RAM mode, bank 0

//     unsigned int addr = 0xA100;
//     unsigned int _size = 16 * 14 * 16;
//     this->setReadMode(0xA100 < 0x8000);
//     int inc = 0;
//     while (_size--)
//     {
//         this->setAddress(addr++);
//         int value = this->getData();
//         raw_buffer[inc] = value;
//         inc++;
//     }
//     String colorIndices = base64::encode(raw_buffer, sizeof(raw_buffer));
//     events.send(colorIndices.c_str(), "frame", millis());
//     this->setWaitMode();
// }
// void GameboyLiveCameraServer::readPicture(bool is_thumbnail)
// {
//     this->ramEnable();  // Enable RAM
//     this->setRegisterMode();  // Set register mode
//     this->writeCartByte(0x0000, 0x0A); // Enable RAM
//     this->writeCartByte(0x4000, 0x00); // Set RAM mode, bank 0

//     unsigned int addr = 0xA100;
//     unsigned int _size = 16 * 14 * 16;
//     this->setReadMode(0xA100 < 0x8000);

//     int inc = 0;
//     uint32_t totalLuminance = 0;

//     while (_size--)
//     {
//         this->setAddress(addr++);
//         int value = this->getData();
//         raw_buffer[inc] = value;

//         // Sumar el valor de luminancia para ajustar la exposición
//         totalLuminance += value;
//         inc++;
//     }

//     uint32_t averageLuminance = totalLuminance / (16 * 14 * 16);
//     averageLuminance = (averageLuminance + previous_luminance) / 2;
//     previous_luminance = averageLuminance;

//     // Establecer una luminancia deseada
//     uint32_t desiredLuminance = 128;

//     // Calcular la diferencia entre la luminancia media y la deseada
//     int luminanceDifference = desiredLuminance - averageLuminance;

//     // Ajuste gradual del tiempo de exposición solo si la diferencia es significativa
//     const int exposureAdjustmentThreshold = 2;  // Reducido para más sensibilidad
//     const int exposureAdjustmentStep = 0x100;   // Aumentado para un ajuste más grande

//     if (abs(luminanceDifference) > exposureAdjustmentThreshold)
//     {
//         if (luminanceDifference > 0)
//         {
//             if (this->exposure_time < 0x0FFF) {  // Limitar el máximo de exposure_time
//                 this->exposure_time += exposureAdjustmentStep;
//             }
//         }
//         else
//         {
//             if (this->exposure_time > 0x0100) {  // Limitar el mínimo de exposure_time
//                 this->exposure_time -= exposureAdjustmentStep;
//             }
//         }
//     }

//     String colorIndices = base64::encode(raw_buffer, sizeof(raw_buffer));
//     events.send(colorIndices.c_str(), "frame", millis());
//     this->setWaitMode();
// }

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
  // // Min image brightness: 128*112*0=0
  // // Max image brightness: 128*112*3=43008

  // // Indoor: Brightness 100, Contrast 10, Gain 10
  // // Outdoors: Brightness 160, Contrast 6, Gain 1
  // // Calcular la luminancia media
  // uint32_t averageLuminance = totalLuminance / (16 * 14 * 16);
  // averageLuminance = (averageLuminance + previous_luminance) / 2;
  // previous_luminance = averageLuminance;
  // // // Ajustar el tiempo de exposición basado en la luminancia media
  // // if (averageLuminance < 100) {
  // //     // Si la imagen es muy oscura, aumentar el tiempo de exposición
  // //     this->increaseExposure();
  // // } else if (averageLuminance > 160) {
  // //     // Si la imagen es muy clara, disminuir el tiempo de exposición
  // //     this->decreaseExposure();
  // // }
  // // Establecer una luminancia deseada
  // // uint32_t desiredLuminance = 128;

  // // // Calcular la diferencia entre la luminancia media y la deseada
  // // int luminanceDifference = desiredLuminance - averageLuminance;

  // // // Ajuste gradual del tiempo de exposición solo si la diferencia es significativa
  // // const int exposureAdjustmentThreshold = 5;  // Reducido para más sensibilidad
  // // const int exposureAdjustmentStep = 0x100;   // Aumentado para un ajuste más grande

  // // if (abs(luminanceDifference) > exposureAdjustmentThreshold)
  // // {
  // //     if (luminanceDifference > 0)
  // //     {
  //         if (averageLuminance < 0x50) {  // Limitar el máximo de exposure_time
  //             this->increaseExposure();
  //         }
  //     // }
  //     // else
  //     // {
  //         if (averageLuminance > 0x80) {  // Limitar el mínimo de exposure_time
  //             // this->exposure_time -= exposureAdjustmentStep;
  //             this->decreaseExposure();
  //         }
  //     // }
  // // }


  String colorIndices = base64::encode(raw_buffer, sizeof(raw_buffer));
  events.send(colorIndices.c_str(), "frame", millis());
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


void gb_camera_server_setup() {

  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  disableCore0WDT();
  String html = "<html><body><h1>:B</h1><script>if (!!window.EventSource) {var source = new EventSource('/events');window.source = source; source.addEventListener('open', function(e) {console.log('Events Connected');}, false); source.addEventListener('error', function(e) {if (e.target.readyState != EventSource.OPEN) {console.log('Events Disconnected');}}, false);source.addEventListener('message', function(e) {console.log('message', e.data);}, false);source.addEventListener('myevent', function(e) {console.log('myevent', e.data);}, false);};</script></body></html>";
  strncpy(index_html, reinterpret_cast<const char*>(html.c_str()), sizeof(html.c_str()));
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
  _server.on("/start_gb_camera", HTTP_GET, [html, gameboy_live_camera_server](AsyncWebServerRequest * request) {
    gameboy_live_camera_server.begin();
    gameboy_live_camera_server.start();
    String json = "{\"status\": true }";
    request->send(200, "application/json", json);
  });
  _server.on("/stop_gb_camera", HTTP_GET, [html, gameboy_live_camera_server](AsyncWebServerRequest * request) {
    gameboy_live_camera_server.begin();
    gameboy_live_camera_server.stop();
    String json = "{\"status\": true }";
    request->send(200, "application/json", json);
  });
  _server.on("/decrease_exposure", HTTP_GET, [html, gameboy_live_camera_server](AsyncWebServerRequest * request) {
    gameboy_live_camera_server.decreaseExposure();
    String json = "{\"status\": true }";
    request->send(200, "application/json", json);
  });
  _server.on("/increase_exposure", HTTP_GET, [html, gameboy_live_camera_server](AsyncWebServerRequest * request) {
    gameboy_live_camera_server.increaseExposure();
    String json = "{\"status\": true }";
    request->send(200, "application/json", json);
  });
  _server.on("/set_exposure", HTTP_GET, [html, gameboy_live_camera_server](AsyncWebServerRequest * request) {
    const AsyncWebParameter* p = request->getParam("value");
    uint16_t exposure_value = static_cast<uint16_t>(strtoul(p->value().c_str(), nullptr, 10));

    gameboy_live_camera_server.setExposure(exposure_value);
    String json = "{\"status\": true }";
    request->send(200, "application/json", json);
  });
  _server.on("/enable_dithering", HTTP_GET, [html, gameboy_live_camera_server](AsyncWebServerRequest * request) {
    gameboy_live_camera_server.enableDithering();
    String json = "{\"status\": true }";
    request->send(200, "application/json", json);
  });
  _server.on("/disable_dithering", HTTP_GET, [html, gameboy_live_camera_server](AsyncWebServerRequest * request) {
    gameboy_live_camera_server.disableDithering();
    String json = "{\"status\": true }";
    request->send(200, "application/json", json);
  });
  _server.on("/", HTTP_GET, [html, gameboy_live_camera_server](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", reinterpret_cast<const char*>(html.c_str()));
  });
  _server.addHandler(&events);
  _server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);

  _server.begin();
}

void gb_camera_server_loop() {
  // if(eventTriggered){ // your logic here
  //send event "myevent"
  // events.send("my event content","myevent",millis());
  // delay(5000);
  // }
  gameboy_live_camera_server.main();
}
