#ifndef GameboyLiveCameraServer_h
#define GameboyLiveCameraServer_h

#include "Gameboy.h"
#include "Buffer.h"
#include <LinkedList.h>

extern Buffer buffer_obj; 

class GameboyLiveCameraServer {

  private:

    uint8_t trigger;
    uint8_t unk1;
    bool auto_exposure;
    uint16_t exposure_time;
    uint32_t previous_luminance;
    uint8_t unk2;
    uint8_t unk3;
    bool dithering;
    bool rotate90;


    bool runGameboyLiveCameraServer;
    void setup();
    
    void setAddress(unsigned int addr);
    void gb_mode(void);
    void setWriteMode(int is_rom);
    int readCartByte(unsigned int address);
    void setData(unsigned int value);
    unsigned int getData(void);
    unsigned int getDataBit0(void);
    void performWrite(void);
    void setWaitMode(void);
    void setReadMode(int is_rom);
    void writeCartByte(unsigned int address, unsigned int value);
    // void adjustExposure();
    void readPicture(bool is_thumbnail);
    void ramEnable();
    void setRegisterMode(void);
    void ramDisable();
    void updateMatrixRegisters();
    unsigned int waitPictureReady(void);

    


    // void rd_wr_mreq_off(void);
    // void rd_wr_mreq_reset(void);
    // void rd_wr_csmreq_cs2_reset(void);
    void dataBusAsOutput();
    void dataBusAsInput();

    String getSerialInput();
    LinkedList<String> parseCommand(String input, char* delim);

  public:
    bool take_a_picture;
    GameboyLiveCameraServer();
    void main();
    void begin();
    void start();
    void stop();

    void increaseExposure();
    void decreaseExposure();
    void enableDithering();
    void disableDithering();
    void setExposure(uint16_t value);
    bool isRunning();

};


void gb_camera_live_loop();
void gb_camera_live_setup();
#endif
