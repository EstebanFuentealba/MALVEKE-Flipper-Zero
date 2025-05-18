#include "GameBoyCartridge.h"
#include <FS.h>


GameBoyCartridge::GameBoyCartridge()
{
    this->runGameBoyCartridge = false;
    this->writtingRAM = false;
    this->writtingROM = false;
    this->restoringRAM = false;
    this->lastByte = 0;
    this->cartridgeType = 0;
    this->romSize = 0;
    this->romBanks = 0;
    this->ramSize = 0;
    this->ramBanks = 0;
    this->sramSize = 0;
    this->romEndAddress = 0x7FFF;
    this->sramBanks = 0;
    this->romType = 0;
    this->currentBank = 0;
}
void GameBoyCartridge::begin()
{
    this->setup();
}
void GameBoyCartridge::start()
{
    // Set Data Pins (D0-D7) to Input
    for (uint32_t i = 0; i < sizeof(ADDRESS_GB_GBC_PINS) / sizeof(ADDRESS_GB_GBC_PINS[0]); i++)
    {
        pinMode(ADDRESS_GB_GBC_PINS[i], OUTPUT);
    }

    // Set Control Pins to Output RST(PH0) CLK(PH1) CS(PH3) WR(PH5) RD(PH6)

    pinMode(GAMEBOY_RST, OUTPUT);
    pinMode(GAMEBOY_CLK, OUTPUT);
    pinMode(GAMEBOY_CS, OUTPUT);
    pinMode(GAMEBOY_WR, OUTPUT);
    pinMode(GAMEBOY_RD, OUTPUT);

    // Output a high signal on all pins, pins are active low therefore everything is disabled now
    digitalWrite(GAMEBOY_CS, HIGH);
    digitalWrite(GAMEBOY_WR, HIGH);
    digitalWrite(GAMEBOY_RD, HIGH);

    // Output a low signal on CLK(PH1) to disable writing GB Camera RAM
    // Output a low signal on RST(PH0) to initialize MMC correctly
    digitalWrite(GAMEBOY_CLK, LOW);
    digitalWrite(GAMEBOY_RST, LOW);

    // Set Data Pins (D0-D7) to Input
    for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
    {
        pinMode(DATA_GB_GBC_PINS[i], INPUT);
    }
    // for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
    // {
    //     pinMode(DATA_GB_GBC_PINS[i], HIGH);
    // }
    delay(400);

    // RST to H
    digitalWrite(GAMEBOY_RST, HIGH);

    transferJSON.clear();
    this->lastByte = 0;
    this->runGameBoyCartridge = true;
}
void GameBoyCartridge::stop()
{
    this->runGameBoyCartridge = false;
}

void GameBoyCartridge::dataBusAsOutput()
{
    for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
    {
        pinMode(DATA_GB_GBC_PINS[i], OUTPUT);
    }
}
void GameBoyCartridge::dataBusAsInput()
{
    for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
    {
        pinMode(DATA_GB_GBC_PINS[i], INPUT);
    }
}
// Set Gameboy mode
void GameBoyCartridge::gb_mode(void)
{
    // Address pins as outputs
    for (uint32_t i = 0; i < sizeof(ADDRESS_GB_GBC_PINS) / sizeof(ADDRESS_GB_GBC_PINS[0]); i++)
    {
        pinMode(ADDRESS_GB_GBC_PINS[i], OUTPUT);
    }
    // Set Data Pins (D0-D7) to Input
    for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
    {
        pinMode(DATA_GB_GBC_PINS[i], INPUT);
    }
}
// Set the 16 bit address
void GameBoyCartridge::set_address_GB(uint16_t address)
{   
    // this->dataBusAsOutput();
    // Write each of the bits into the address pins
    for (uint32_t i = 0; i < sizeof(ADDRESS_GB_GBC_PINS) / sizeof(ADDRESS_GB_GBC_PINS[0]); i++)
    {
        digitalWrite(ADDRESS_GB_GBC_PINS[i], address & (1 << i) ? HIGH : LOW);
    }
}
byte GameBoyCartridge::read_byte_GB(uint16_t address)
{
    this->set_address_GB(address);

    __asm__("nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t");
          
    // cs_mreqPin_low;
    rdPin_low;

    __asm__("nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t");
    // asm volatile("nop");
    // asm volatile("nop");

    byte data = 0; // Read data
    for (int i = 0; i < 8; i++)
    {
        int raw_data = digitalRead(DATA_GB_GBC_PINS[i]);
        data |= raw_data << i;
        // asm volatile("nop"); // Delay a little (minimum is 2 nops, using 3 to be sure)
        // asm volatile("nop");
        // asm volatile("nop");
        // asm volatile("nop");
    }
    // Switch and RD to HIGH
    rdPin_high;
    // cs_mreqPin_high;
    __asm__("nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t");
    // asm volatile("nop"); // Delay a little (minimum is 2 nops, using 3 to be sure)
    // asm volatile("nop");
    // asm volatile("nop");
    // asm volatile("nop");
    return data;
}
void GameBoyCartridge::headerROM_GB(bool printInfo = true)
{
    // buffer_obj = Buffer();
    // buffer_obj.openFile("header", NULL, true, false);

    transferJSON.clear();
    this->lastByte = 0;

    transferJSON["type"] = "info";

    this->gb_mode();

    for (uint16_t address = 0; address < 0x0180; address++)
    {
        byte value = this->read_byte_GB(address);
        this->startRomBuffer[address] = value;
    }
    byte myLength = 0;
    // Blank out game title
    for (uint8_t b = 0; b < 16; b++)
    {
        this->gameTitle[b] = 0;
    }
    uint8_t cart_data = this->startRomBuffer[0x0143];
    if (cart_data == 0x80)
    {
        //  Cartridge with color function
        transferJSON["gb_type"] = "gbc";
    }
    else
    {
        //  Cartridge with no color function
        transferJSON["gb_type"] = "gb";
    }
    cart_data = this->startRomBuffer[0x0146];
    if (cart_data == 0x03)
    {
        //  Cartridge with SGB functions
        transferJSON["gb_sgb"] = true;
    }
    else
    {
        //  Cartridge without SGB functions
        transferJSON["gb_sgb"] = false;
    }

    // Read cartridge title and check for non-printable text
    for (uint16_t titleAddress = 0x0134; titleAddress <= 0x143; titleAddress++)
    {
        // Calculate the index in gameTitle array
        uint8_t titleIndex = titleAddress - 0x0134;
        
        // Ensure we don't exceed the gameTitle buffer size
        if (titleIndex >= sizeof(this->gameTitle)) {
            break;
        }
        
        char headerChar = this->startRomBuffer[titleAddress];
        if ((headerChar >= 0x30 && headerChar <= 0x39) || // 0-9
            (headerChar >= 0x41 && headerChar <= 0x5A) || // A-Z
            (headerChar >= 0x61 && headerChar <= 0x7A) || // a-z
            (headerChar >= 0x24 && headerChar <= 0x29) || // #$%&'()
            (headerChar == 0x2D) ||                       // -
            (headerChar == 0x2E) ||                       // .
            (headerChar == 0x5F) ||                       // _
            (headerChar == 0x20))
        { // Space
            this->gameTitle[titleIndex] = headerChar;
            myLength++;
        }
        // Replace with an underscore
        else if (headerChar == 0x3A)
        {
            this->gameTitle[titleIndex] = '_';
            myLength++;
        }
        else
        {
            this->gameTitle[titleIndex] = '\0';
            break;
        }
    }
    // Find Game Serial
    cartID[0] = 0;
    if (this->startRomBuffer[0x143] == 0x80 || this->startRomBuffer[0x143] == 0xC0)
    {
        if ((this->gameTitle[myLength - 4] == 'A' || this->gameTitle[myLength - 4] == 'B' || this->gameTitle[myLength - 4] == 'H' || this->gameTitle[myLength - 4] == 'K' || this->gameTitle[myLength - 4] == 'V') && (this->gameTitle[myLength - 1] == 'A' || this->gameTitle[myLength - 1] == 'B' || this->gameTitle[myLength - 1] == 'D' || this->gameTitle[myLength - 1] == 'E' || this->gameTitle[myLength - 1] == 'F' || this->gameTitle[myLength - 1] == 'I' || this->gameTitle[myLength - 1] == 'J' || this->gameTitle[myLength - 1] == 'K' || this->gameTitle[myLength - 1] == 'P' || this->gameTitle[myLength - 1] == 'S' || this->gameTitle[myLength - 1] == 'U' || this->gameTitle[myLength - 1] == 'X' || this->gameTitle[myLength - 1] == 'Y'))
        {
            cartID[0] = this->gameTitle[myLength - 4];
            cartID[1] = this->gameTitle[myLength - 3];
            cartID[2] = this->gameTitle[myLength - 2];
            cartID[3] = this->gameTitle[myLength - 1];
            myLength -= 4;
            this->gameTitle[myLength] = 0;
        }
    }
    // Nintendo Logo Check
    uint8_t logoCheck = 1;
    uint8_t x = 0;
    uint8_t romLogo[48];

    JsonArray logoArray = transferJSON.createNestedArray("logo");
    for (uint16_t romAddress = 0x0104; romAddress <= 0x133; romAddress++)
    {
        if (nintendoLogo[x] != this->startRomBuffer[romAddress] && logoCheck == 1)
        {
            logoCheck = 0;
        }
        romLogo[x] = this->startRomBuffer[romAddress];
        logoArray.add(romLogo[x]);
        x++;
    }
    transferJSON["logo_correct"] = logoCheck;
    // Calculate header checksum
    byte headerChecksum = 0;
    for (int currByte = 0x134; currByte < 0x14D; currByte++)
    {
        headerChecksum = headerChecksum - this->startRomBuffer[currByte] - 1;
    }

    if (headerChecksum != this->startRomBuffer[0x14D])
    {
        // Read Header into array a second time
        for (int currByte = 0x100; currByte < 0x150; currByte++)
        {
            this->startRomBuffer[currByte] = this->read_byte_GB(currByte);
        }
        // Calculate header checksum a second time
        headerChecksum = 0;
        for (int currByte = 0x134; currByte < 0x14D; currByte++)
        {
            headerChecksum = headerChecksum - this->startRomBuffer[currByte] - 1;
        }
    }

    if (headerChecksum != this->startRomBuffer[0x14D])
    {
        transferJSON["message"] = "HEADER CHECKSUM ERROR";
    }

    this->cartridgeType = this->startRomBuffer[0x0147];
    this->romType = this->cartridgeType;
    this->romSize = this->startRomBuffer[0x0148];
    this->ramSize = this->startRomBuffer[0x0149];
    this->sramSize = this->startRomBuffer[0x149];

    // Get Checksum as string
    sprintf(checksumStr, "%02X%02X", this->startRomBuffer[0x14E], this->startRomBuffer[0x14F]);

    // ROM banks
    switch (this->romSize)
    {
    case 0x00:
        this->romBanks = 2;
        break;
    case 0x01:
        this->romBanks = 4;
        break;
    case 0x02:
        this->romBanks = 8;
        break;
    case 0x03:
        this->romBanks = 16;
        break;
    case 0x04:
        this->romBanks = 32;
        break;
    case 0x05:
        this->romBanks = 64;
        break;
    case 0x06:
        this->romBanks = 128;
        break;
    case 0x07:
        this->romBanks = 256;
        break;
    case 0x08:
        this->romBanks = 512;
        break;
    default:
        this->romBanks = 2;
    }

    // SRAM banks
    this->sramBanks = 0;
    if (this->romType == 6)
    {
        this->sramBanks = 1;
    }

    // SRAM size
    switch (this->sramSize)
    {
    case 2:
        this->sramBanks = 1;
        break;
    case 3:
        this->sramBanks = 4;
        break;
    case 4:
        this->sramBanks = 16;
        break;
    case 5:
        this->sramBanks = 8;
        break;
    }

    // RAM end address
    if (this->cartridgeType == 6)
    {
        this->ramEndAddress = 0xA1FF;
    } // MBC2 512 bytes (nibbles)
    if (this->ramSize == 1)
    {
        this->ramEndAddress = 0xA7FF;
    } // 2K RAM
    if (this->ramSize > 1)
    {
        this->ramEndAddress = 0xBFFF;
    } // 8K RAM

    // M161 banks are double size and start with 0
    if (this->romType == 0x104)
    {
        this->romStartBank = 0;
        this->romBanks >>= 1;
        this->romEndAddress = 0x7FFF;
    }
    // MBC6 banks are half size
    else if (this->romType == 32)
    {
        this->romBanks <<= 1;
        this->romEndAddress = 0x3FFF;
    }
    if (strcmp(checksumStr, "00") != 0)
    {
        transferJSON["success"] = 1;
        transferJSON["message"] = "";
        transferJSON["checksum"] = checksumStr;
        transferJSON["title"] = this->gameTitle;

        // Game Boy Camera (USA, Europe):		"GAMEBOYCAMERA"
        // Game Boy Camera Gold (USA):			"GAMEBOYCAMERA G"
        // Pocket Camera (Japan) (Rev A):		"POCKETCAMERA"
        // Pocket Camera (J) (V1.1) [S] (1):	"POCKETCAMERA"

        transferJSON["isCamera"] = strcmp(gameTitle, "GAMEBOYCAMERA") == 0 ||
            strcmp(gameTitle, "GAMEBOYCAMERA G") == 0 ||
            strcmp(gameTitle, "POCKETCAMERA") == 0;
            
        transferJSON["ramBanks"] = this->sramBanks;
        transferJSON["ramEndAddress"] = this->ramEndAddress;
        transferJSON["romBanks"] = this->romBanks;
        transferJSON["romEndAddress"] = this->romEndAddress;
        if (cartID[0] != 0)
        {
            transferJSON["serial"] = cartID;
        }
        transferJSON["rev"] = romVersion;
        if ((this->romType == 0) || (this->romType == 8) || (this->romType == 9))
        {
            transferJSON["mapper"] = "None";
        }
        else if ((this->romType == 1) || (this->romType == 2) || (this->romType == 3))
        {
            transferJSON["mapper"] = "MBC1";
        }
        else if ((this->romType == 5) || (this->romType == 6))
        {
            transferJSON["mapper"] = "MBC2";
        }
        else if ((this->romType == 11) || (this->romType == 12) || (this->romType == 13))
        {
            transferJSON["mapper"] = "MMM01";
        }
        else if ((this->romType == 15) || (this->romType == 16) || (this->romType == 17) || (this->romType == 18) || (this->romType == 19))
        {
            transferJSON["mapper"] = "MBC3";
        }
        else if ((this->romType == 21) || (this->romType == 22) || (this->romType == 23))
        {
            transferJSON["mapper"] = "MBC4";
        }
        else if ((this->romType == 25) || (this->romType == 26) || (this->romType == 27) || (this->romType == 28) || (this->romType == 29) || (this->romType == 309))
        {
            transferJSON["mapper"] = "MBC5";
        }
        else if (this->romType == 32)
        {
            transferJSON["mapper"] = "MBC6";
        }
        else if (this->romType == 34)
        {
            transferJSON["mapper"] = "MBC7";
        }
        else if (this->romType == 252)
        {
            transferJSON["mapper"] = "Camera";
        }
        else if (this->romType == 253)
        {
            transferJSON["mapper"] = "TAMA5";
        }
        else if (this->romType == 254)
        {
            transferJSON["mapper"] = "HuC-3";
        }
        else if (this->romType == 255)
        {
            transferJSON["mapper"] = "HuC-1";
        }
        else if ((this->romType == 0x101) || (this->romType == 0x103))
        {
            transferJSON["mapper"] = "MBC1M";
        }
        else if (this->romType == 0x104)
        {
            transferJSON["mapper"] = "M161";
        }
        switch (this->romSize)
        {
        case 0:
            transferJSON["ROMSize"] = "32 KB";
            break;

        case 1:
            transferJSON["ROMSize"] = "64 KB";
            break;

        case 2:
            transferJSON["ROMSize"] = "128 KB";
            break;

        case 3:
            transferJSON["ROMSize"] = "256 KB";
            break;

        case 4:
            transferJSON["ROMSize"] = "512 KB";
            break;

        case 5:
            transferJSON["ROMSize"] = "1 MB";
            break;

        case 6:
            transferJSON["ROMSize"] = "2 MB";
            break;

        case 7:
            transferJSON["ROMSize"] = "4 MB";
            break;

        case 8:
            transferJSON["ROMSize"] = "8 MB";
            break;
        }

        switch (this->sramSize)
        {
        case 0:
            if (this->romType == 6)
            {
                transferJSON["RAMSize"] = "512 Byte";
            }
            else if (this->romType == 0x22)
            {
                transferJSON["RAMSize"] = String(this->lastByte) + " Byte";
            }
            else if (this->romType == 0xFD)
            {
                transferJSON["RAMSize"] = "32 Byte";
            }
            else
            {
                transferJSON["RAMSize"] = "None";
            }
            break;
        case 1:
            transferJSON["RAMSize"] = "2 KB";
            break;

        case 2:
            transferJSON["RAMSize"] = "8 KB";
            break;

        case 3:
            if (this->romType == 0x20)
            {
                transferJSON["RAMSize"] = "1.03 MB";
            }
            else
            {
                transferJSON["RAMSize"] = "32 KB";
            }
            break;

        case 4:
            transferJSON["RAMSize"] = "128 KB";
            break;

        case 5:
            transferJSON["RAMSize"] = "64 KB";
            break;

        default:
            transferJSON["RAMSize"] = "None";
        }
        delay(2);

        uint32_t crc32sum = 1;
        boolean renamerom = 0;
        int offset = 0;
        /*
        char crcStr[9];

        // Convert precalculated crc to string
        sprintf(crcStr, "%08lX", ~crc32sum);

        //  TODO: Implepement Rom name from memory file
        MemoryFile myFile(gb, sizeof(gb));

        if (myFile.open())
        {
          char gamename[96];
          char crc_search[9];
          // Loop through file
          while (myFile.available())
          {
            //Read 2 lines (game name and CRC)
            get_line(gamename, &myFile, sizeof(gamename));
            get_line(crc_search, &myFile, sizeof(crc_search));
            skip_line(&myFile);  //Skip every 3rd line
            //if checksum search successful, rename the file and end search
            if (strcmp(crc_search, crcStr) == 0) {
              transferJSON["ROMName"] = gamename;
              // Serial.print("gamename: ");
              // Serial.println(gamename);
            }
          }
        }
        */
    }
    else
    {
        transferJSON["success"] = 0;
        transferJSON["message"] = "GAMEPAK ERROR";
    }
    if (printInfo)
    {
        Serial.print("JSON:");
        serializeJson(transferJSON, Serial);
        Serial.println();

        
        // Serializar los datos JSON en una cadena
        // String jsonString;
        // serializeJson(transferJSON, jsonString);
        // String prefixedJsonString = "JSON:" + jsonString;
        // buffer_obj.write(reinterpret_cast<const uint8_t*>(prefixedJsonString.c_str()), prefixedJsonString.length());
        
    }
}
void GameBoyCartridge::write_byte_GB(int address, byte data)
{
    // Set address
    this->set_address_GB(address);
    // Set data
    // Switch data pins to output
    for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
    {
        pinMode(DATA_GB_GBC_PINS[i], OUTPUT);
    }

    for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
    {
        digitalWrite(DATA_GB_GBC_PINS[i], data & (1 << i) ? HIGH : LOW);
    }

    // Wait till output is stable
    wait(120);
    // __asm__("nop\n\t"
    //         "nop\n\t"
    //         "nop\n\t"
    //         "nop\n\t");
    // Pull WR low
    wrPin_low;
    // Leave WR low for at least 60ns
    wait(120);
    // __asm__("nop\n\t"
    //         "nop\n\t"
    //         "nop\n\t"
    //         "nop\n\t");

    // Pull WR HIGH
    wrPin_high;

    // Leave WR high for at least 50ns
    wait(120);
    // __asm__("nop\n\t"
    //         "nop\n\t"
    //         "nop\n\t"
    //         "nop\n\t");

    // Switch data pins to input
    for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
    {
        pinMode(DATA_GB_GBC_PINS[i], INPUT);
    }
    // Enable pullups
    // for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
    // {
    //     pinMode(DATA_GB_GBC_PINS[i], HIGH);
    // }
}

// Turn RD, WR, CS/MREQ and CS2 to high so they are deselected (reset state)
void GameBoyCartridge::rd_wr_csmreq_cs2_reset(void)
{
    cs2Pin_high;     // CS2 off
    cs_mreqPin_high; // CS/MREQ off
    rdPin_high;      // RD off
    wrPin_high;      // WR off
}
// Turn RD, WR and MREQ to high so they are deselected (reset state)
void GameBoyCartridge::rd_wr_mreq_reset(void)
{
    rdPin_high;      // RD off
    wrPin_high;      // WR off
    cs_mreqPin_high; // MREQ off
}
// Turn RD, WR and MREQ off as no power should be applied to GB Cart
void GameBoyCartridge::rd_wr_mreq_off(void)
{
    rdPin_low;
    wrPin_low;
    cs_mreqPin_low; // CS
}

void GameBoyCartridge::readROM_GB()
{
    // Check for ongoing operations
    if (this->writtingRAM || this->writtingROM || this->restoringRAM) {
        transferJSON.clear();
        transferJSON["type"] = "error";
        transferJSON["message"] = "Another operation is in progress";
        Serial.print("JSON:");
        serializeJson(transferJSON, Serial);
        Serial.println();
        return;
    }

    // Reset state variables
    this->writtingRAM = false;
    this->writtingROM = false;
    this->restoringRAM = false;
    this->lastByte = 0;
    this->cartridgeType = 0;
    this->romSize = 0;
    this->romBanks = 0;
    this->ramSize = 0;
    this->ramBanks = 0;
    this->sramSize = 0;
    this->romEndAddress = 0x7FFF;
    this->sramBanks = 0;
    this->romType = 0;
    this->currentBank = 0;

    // Read cartridge header
    bool headerSuccess = false;
    try {
        this->headerROM_GB(false);
        headerSuccess = true;
    } catch (...) {
        headerSuccess = false;
    }

    if (!headerSuccess) {
        transferJSON.clear();
        transferJSON["type"] = "error";
        transferJSON["message"] = "Failed to read cartridge header";
        Serial.print("JSON:");
        serializeJson(transferJSON, Serial);
        Serial.println();
        return;
    }

    // Setup initial ROM reading parameters
    this->romEndAddress = 0x7FFF; // Default end address
    this->romAddress = 0;
    this->romStartBank = 1;
    this->processedProgressBar = 0;
    
    // Apply cartridge-specific settings
    if (this->romType == 0x104) { // M161 banks are double size and start with 0
        this->romStartBank = 0;
        this->romBanks = (this->romBanks > 0) ? this->romBanks >> 1 : 1;
        this->romEndAddress = 0x7FFF;
    } else if (this->romType == 32) { // MBC6 banks are half size
        this->romBanks = (this->romBanks > 0) ? this->romBanks << 1 : 2;
        this->romEndAddress = 0x3FFF;
    }

    // Validate ROM banks
    if (this->romBanks == 0) {
        transferJSON.clear();
        transferJSON["type"] = "error";
        transferJSON["message"] = "Invalid ROM size or unsupported cartridge type";
        Serial.print("JSON:");
        serializeJson(transferJSON, Serial);
        Serial.println();
        return;
    }

    // Calculate total expected bytes to process
    this->totalProgressBar = (uint32_t)(this->romBanks) * 16384;
    if (this->totalProgressBar == 0) {
        transferJSON.clear();
        transferJSON["type"] = "error";
        transferJSON["message"] = "Error calculating ROM size";
        Serial.print("JSON:");
        serializeJson(transferJSON, Serial);
        Serial.println();
        return;
    }

    // Initial setup is complete, start ROM reading process
    this->currentBank = this->romStartBank;
    
    // Send initial status
    transferJSON.clear();
    transferJSON["type"] = "rom_reading";
    transferJSON["total"] = this->totalProgressBar;
    transferJSON["progress"] = 0;
    transferJSON["romBanks"] = this->romBanks;
    Serial.print("JSON:");
    serializeJson(transferJSON, Serial);
    Serial.println();

    // Set the flag before starting the operation
    this->writtingROM = true;
    
    // Give hardware time to stabilize
    delay(100);

    // Read the first ROM bank synchronously, but defer rest to main() loop
    // This gives feedback to the user immediately
    if (this->romBanks > 0) {
        bool bankReadSuccess = false;
        
        // Setup bank switching if needed
        try {
            // Set ROM bank registers based on cartridge type
            switch (this->romType) {
                case 0x104: // M161
                    cs2Pin_low;
                    delay(10);
                    cs2Pin_high;
                    this->write_byte_GB(0x4000, this->currentBank & 0x7);
                    break;
                    
                case 0x101:
                case 0x103: // MBC1M
                    if (this->currentBank < 10) {
                        this->write_byte_GB(0x4000, this->currentBank >> 4);
                        this->write_byte_GB(0x2000, (this->currentBank & 0x1f));
                    } else {
                        this->write_byte_GB(0x4000, this->currentBank >> 4);
                        this->write_byte_GB(0x2000, 0x10 | (this->currentBank & 0x1f));
                    }
                    break;
                    
                case 32: // MBC6
                    this->write_byte_GB(0x2800, 0);
                    this->write_byte_GB(0x3800, 0);
                    this->write_byte_GB(0x2000, this->currentBank);
                    this->write_byte_GB(0x3000, this->currentBank);
                    break;
                    
                default:
                    if (this->romType >= 5) { // MBC2/3/4/5
                        this->write_byte_GB(0x2100, this->currentBank & 0xFF);
                    } else { // Assume MBC1 or similar
                        this->write_byte_GB(0x6000, 0);
                        this->write_byte_GB(0x2000, this->currentBank & 0x1F);
                    }
                    break;
            }
            
            bankReadSuccess = true;
        } catch (...) {
            bankReadSuccess = false;
        }
        
        if (!bankReadSuccess) {
            this->writtingROM = false;
            transferJSON.clear();
            transferJSON["type"] = "error";
            transferJSON["message"] = "Failed to initialize ROM bank switching";
            Serial.print("JSON:");
            serializeJson(transferJSON, Serial);
            Serial.println();
            return;
        }
        
        // Read first chunk (64 bytes) to verify operation
        try {
            // Read ROM data in small chunks
            const size_t bufferSize = sizeof(this->sdBuffer) / sizeof(this->sdBuffer[0]);
            
            // Ensure we don't exceed buffer bounds
            if (bufferSize >= 64) {
                for (int i = 0; i < 64 && this->romAddress + i <= this->romEndAddress; i++) {
                    this->sdBuffer[i] = this->read_byte_GB(this->romAddress + i);
                }
                
                // Send data to Serial1
                Serial1.write(this->sdBuffer, 64);
                this->romAddress += 64;
                this->processedProgressBar += 64;
            } else {
                throw std::runtime_error("Buffer size too small");
            }
        } catch (...) {
            this->writtingROM = false;
            transferJSON.clear();
            transferJSON["type"] = "error";
            transferJSON["message"] = "Failed to read ROM data";
            Serial.print("JSON:");
            serializeJson(transferJSON, Serial);
            Serial.println();
            return;
        }
    }
    
    // Remaining reading will be handled by the main() loop
    // No need to send a status message here, will be handled by main()
}

byte GameBoyCartridge::readByteSRAM_GB(uint16_t myAddress)
{
    this->set_address_GB(myAddress);

    // Switch data pins to input
    __asm__("nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t");
    // Pull CS, CLK (for FRAM MOD) LOW
    cs_mreqPin_low;

    //      CS  RD  WR
    //    0   0   1
    // Pull RD LOW
    rdPin_low;
    wrPin_high;
    phiPin_low;

    __asm__("nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t");
    // Read
    byte tempByte = 0; // Read data
    for (int i = 0; i < 8; i++)
    {
        int raw_data = digitalRead(DATA_GB_GBC_PINS[i]);
        // Serial.print(raw_data);
        tempByte |= raw_data << i;
    }

    //      CS  RD  WR
    //    1   1   1
    // Pull RD HIGH
    rdPin_high;
    if (this->romType == 252)
    {
        // Pull CS HIGH
        cs_mreqPin_high;
    }
    else
    {
        // Pull CS, CLK (for FRAM MOD) HIGH
        cs_mreqPin_high;
        phiPin_high;
    }
    wrPin_high;
    __asm__("nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t");
    return tempByte;
}
// void GameBoyCartridge::printSRAM_GB(AsyncResponseStream *response) 
// {
//     this->writtingRAM = false;
//     this->writtingROM = false;
//     this->lastByte = 0;
//     this->cartridgeType = 0;
//     this->romSize = 0;
//     this->romBanks = 0;
//     this->ramSize = 0;
//     this->ramBanks = 0;
//     this->sramSize = 0;
//     this->romEndAddress = 0x7FFF;
//     this->sramBanks = 0;
//     this->romType = 0;
//     this->currentBank = 0;


//     this->headerROM_GB(false);

//     this->processedProgressBar = 0;
//     this->totalProgressBar  = (uint32_t)(this->sramBanks) * 8192;

//     delay(200);
//     this->rd_wr_mreq_reset();

//     // MBC2 Fix (unknown why this fixes reading the ram, maybe has to read ROM before RAM?)
//     this->read_byte_GB(0x0134);
//     if(this->ramEndAddress > 0) {
//         if (this->cartridgeType <= 4)
//         {                                   // MBC1
//             this->write_byte_GB(0x6000, 1); // Set RAM Mode
//         }
//         this->dataBusAsOutput();
//         // Initialise MBC
//         this->write_byte_GB(0x0000, 0x0A);
//         delayMicroseconds(50);
//         this->dataBusAsInput();

//         //  loop
//         for(int current = this->currentBank; current < this->sramBanks; current++) {
//             this->write_byte_GB(0x4000, current);
//             // Read RAM
//             for (word ramAddress = 0xA000; ramAddress <= this->ramEndAddress; ramAddress += 64)
//             {
//                 uint8_t readData[64];
//                 for (uint8_t i = 0; i < 64; i++)
//                 {
//                     readData[i] = this->readByteSRAM_GB(ramAddress + i);
//                     response->print(readData[i]);
//                 }
//                 this->processedProgressBar += 64;
//             }
//         }
//         // Disable RAM
//         this->write_byte_GB(0x0000, 0x00);
//         delay(50);
//         this->totalProgressBar = (uint32_t)(this->sramBanks) * 8192;
//         this->rd_wr_mreq_off();
        
//     }
// }
void GameBoyCartridge::readSRAM_GB()
{
    // Clear any existing operations first
    if (this->writtingRAM || this->writtingROM || this->restoringRAM) {
        // Another operation is in progress, can't start a new one
        transferJSON.clear();
        transferJSON["type"] = "error";
        transferJSON["message"] = "Another operation is in progress";
        Serial.print("JSON:");
        serializeJson(transferJSON, Serial);
        Serial.println();
        return;
    }

    // Reset state variables
    this->writtingRAM = false;
    this->writtingROM = false;
    this->lastByte = 0;
    this->cartridgeType = 0;
    this->romSize = 0;
    this->romBanks = 0;
    this->ramSize = 0;
    this->ramBanks = 0;
    this->sramSize = 0;
    this->romEndAddress = 0x7FFF;
    this->sramBanks = 0;
    this->romType = 0;
    this->currentBank = 0;

    // Get cartridge information
    bool headerSuccess = false;
    
    try {
        this->headerROM_GB(false);
        headerSuccess = true;
    } catch (...) {
        // Handle any exception during header reading
        headerSuccess = false;
    }

    if (!headerSuccess) {
        transferJSON.clear();
        transferJSON["type"] = "error";
        transferJSON["message"] = "Failed to read cartridge header";
        Serial.print("JSON:");
        serializeJson(transferJSON, Serial);
        Serial.println();
        return;
    }

    // Validate we can read RAM from this cartridge
    if (this->ramEndAddress <= 0 || this->sramBanks == 0) {
        transferJSON.clear();
        transferJSON["type"] = "error";
        transferJSON["message"] = "Cartridge has no RAM or unsupported RAM type";
        Serial.print("JSON:");
        serializeJson(transferJSON, Serial);
        Serial.println();
        return;
    }

    // Initialize progress tracking
    this->processedProgressBar = 0;
    this->totalProgressBar = (uint32_t)(this->sramBanks) * 8192;
    
    if (this->totalProgressBar == 0) {
        transferJSON.clear();
        transferJSON["type"] = "error";
        transferJSON["message"] = "Invalid RAM size calculation";
        Serial.print("JSON:");
        serializeJson(transferJSON, Serial);
        Serial.println();
        return;
    }

    // Send initial status
    transferJSON.clear();
    transferJSON["type"] = "ram";
    transferJSON["total"] = this->totalProgressBar;
    transferJSON["progress"] = 0;
    transferJSON["ramBanks"] = this->sramBanks;
    transferJSON["lastByte"] = this->ramEndAddress;
    
    Serial.print("JSON:");
    serializeJson(transferJSON, Serial);
    Serial.println();

    // Reset control lines
    this->rd_wr_mreq_reset();

    // MBC2 Fix (unknown why this fixes reading the ram, maybe has to read ROM before RAM?)
    bool readSuccess = false;
    
    // Add timeout for hardware access
    unsigned long startTime = millis();
    while (!readSuccess && millis() - startTime < 1000) { // 1 second timeout
        try {
            this->read_byte_GB(0x0134);
            readSuccess = true;
        } catch (...) {
            // Retry on failure
            delay(10);
        }
    }
    
    if (!readSuccess) {
        transferJSON.clear();
        transferJSON["type"] = "error";
        transferJSON["message"] = "Failed to initialize cartridge for RAM reading";
        Serial.print("JSON:");
        serializeJson(transferJSON, Serial);
        Serial.println();
        return;
    }

    // Setup RAM access mode
    if (this->cartridgeType <= 4) { // MBC1
        this->dataBusAsOutput();
        this->write_byte_GB(0x6000, 1); // Set RAM Mode
        this->dataBusAsInput();
    }
    
    // Enable RAM access
    this->dataBusAsOutput();
    this->write_byte_GB(0x0000, 0x0A);
    delayMicroseconds(50);
    this->dataBusAsInput();

    // Start the RAM reading process
    // The main() function will handle the actual reading in chunks
    this->currentBank = 0;
    this->writtingRAM = true;
}
void GameBoyCartridge::ramEnable() {
    this->dataBusAsOutput();
    this->write_byte_GB(0x0000, 0x0A);
    delayMicroseconds(10);
    this->dataBusAsInput();
}
void GameBoyCartridge::ramDisable() {
    this->dataBusAsOutput();
    // Disable SRAM
    this->write_byte_GB(0x0000, 0x00);
    delay(50);
    this->dataBusAsInput();
}
// Timeout threshold for serial_receive() to prevent infinite blocking
static const unsigned long SERIAL_RECEIVE_TIMEOUT_MS = 1000; // milliseconds

// Receive Serial data with timeout
uint8_t GameBoyCartridge::serial_receive() {
    unsigned long start = millis();
    while (Serial.available() <= 0) {
        if (millis() - start >= SERIAL_RECEIVE_TIMEOUT_MS) {
            return 0; // Timeout: no data available
        }
        delay(1);
    }
    return Serial.read();
}
// Transmit Serial data
void GameBoyCartridge::serial_transmit(uint8_t data) {
  while (!(Serial.availableForWrite() > 0)); // Wait for the serial buffer to have space
  Serial.write(data);
}
// Read 1-128 bytes from the Serial 
void GameBoyCartridge::serial_read_bytes(uint8_t count) {
    memset(this->receivedBuffer, 0, sizeof(this->receivedBuffer));
	for (uint8_t x = 0; x < count; x++) {
		this->receivedBuffer[x] = this->serial_receive();
	}
}

uint8_t* GameBoyCartridge::serial_read_64_bytes(uint8_t count) {
    // Use malloc instead of new for consistency with free
    uint8_t* receivedBuffer = (uint8_t*)malloc(count);
    if (!receivedBuffer) {
        // Handle memory allocation failure
        return NULL;
    }
    
    // Read with timeout
    unsigned long startTime = millis();
    for (uint8_t x = 0; x < count; x++) {
        // Add timeout to prevent hanging
        while (Serial.available() <= 0) {
            if (millis() - startTime > SERIAL_RECEIVE_TIMEOUT_MS) {
                // Timeout occurred, clean up and return NULL
                free(receivedBuffer);
                return NULL;
            }
            delay(1);
        }
        receivedBuffer[x] = Serial.read();
    }
    return receivedBuffer;
    
    // IMPORTANT: Caller must free this buffer with free() when done!
}
// Read from Serial until a 0 (string terminator byte) is received
void GameBoyCartridge::serial_read_chars() {
  uint8_t x = 0;
  unsigned long startTime = millis();
  
  // Set maximum buffer size with 1 byte reserved for null terminator
  const size_t maxChars = sizeof(this->receivedBuffer) - 1;
  
  while (x < maxChars) {
    // Check for timeout
    if (millis() - startTime > SERIAL_RECEIVE_TIMEOUT_MS) {
      break; // Prevent infinite loops
    }
    
    // Wait for data with timeout
    if (Serial1.available() <= 0) {
      delay(1);
      continue;
    }
    
    char receivedChar = Serial1.read();
    if (receivedChar == 0) {
      break; // End of string
    }
    
    this->receivedBuffer[x++] = receivedChar;
  }
  
  // Ensure the string is null-terminated
  this->receivedBuffer[x] = 0;
}

// Triggers CS and CLK pin
void GameBoyCartridge::writeByteSRAM_GB(uint16_t address, uint8_t data) {
  // Set address

    this->set_address_GB(address);
                
    for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
    {
        digitalWrite(DATA_GB_GBC_PINS[i], data & (1 << i) ? HIGH : LOW);
    }

    //  CS RD WR (010)

    delayMicroseconds(10);
    wrPin_low;
    rdPin_high;
    cs_mreqPin_low;

    delayMicroseconds(10);

    //  CS RD WR (111)
    wrPin_high;
    rdPin_high;
    cs_mreqPin_high;
    delayMicroseconds(10);
}

String GameBoyCartridge::getSerialInput() {
  String input = "";

  if (Serial.available() > 0)
    input = Serial.readStringUntil('\n');

  input.trim();
  return input;
}
void GameBoyCartridge::read_and_store_chunk(uint32_t offset, uint8_t* buffer_chunk, size_t chunk_size) {
    // Prepare JSON request
    transferJSON.clear();
    transferJSON["type"] = "read";
    transferJSON["offset"] = offset;
    transferJSON["value"] = chunk_size;

    // Serialize JSON request
    Serial.print("JSON:");
    serializeJson(transferJSON, Serial);
    Serial.println();
    // Serial.flush();

    int chunk_index = 0;

    // Read and process JSON response
    while (true) {
        // Check if data is available
        if (Serial.available() >= 0) {
            // Allocate the JSON document
            DynamicJsonDocument doc(512);

            // Read the JSON document from the serial port
            DeserializationError err = deserializeJson(doc, Serial);

            if (err == DeserializationError::Ok) {
                int items[16];
                copyArray(doc["resolutions"].as<JsonArray>(), items);

                // Copy items to the buffer_chunk
                for (int i = 0; i < sizeof(items) && chunk_index < chunk_size; i++) {
                    buffer_chunk[chunk_index] = items[i];
                    chunk_index++;
                }

                // Break out of the loop after processing the JSON response
                break;
            } else {
                // Print error and flush serial buffer
                while (Serial.available() > 0)
                    Serial.read();
            }
        }
    }
}
void GameBoyCartridge::read_and_store_chunks(uint32_t startOffset, uint8_t* buffer, size_t chunkSize, size_t numChunks) {
    for (size_t i = 0; i < numChunks; i++) {
        // Calculate the offset for each chunk
        uint32_t offset = startOffset + i * chunkSize;

        // Call the readAndStoreChunk function for each chunk
        this->read_and_store_chunk(offset, buffer + i * chunkSize, chunkSize);
    }
}
void GameBoyCartridge::read_serial_bytes(uint32_t startOffset, size_t chunkSize, uint8_t* buffer) {
    //  solicita N bytes
    Serial.flush();
    transferJSON.clear();
    transferJSON["type"] = "read";
    transferJSON["offset"] = startOffset; // 0
    transferJSON["value"] = chunkSize; // 64
    Serial.print("JSON:");
    serializeJson(transferJSON, Serial);
    Serial.println();
    // Serial.flush();

    while(true) {
        if (Serial.available() > 0) {
            int rlen = Serial.readBytesUntil('\n', buffer, chunkSize);
            if(rlen == chunkSize)
                break;
        }
    }

}
void GameBoyCartridge::test(int maxBufferSize) {
        // WiFi.mode(WIFI_AP);
        // gbStartAP("Tester", "12345678");
        // Serial.println("OK startAP");
        // _server.on("/image", HTTP_GET, [ serialFile.save_buffer, serialFile.totalReceived ](AsyncWebServerRequest *request) {
        //     AsyncResponseStream *response = request->beginResponseStream("application/octet-stream");
        //     response->write( serialFile.save_buffer, serialFile.totalReceived);
        //     response->addHeader("Content-Disposition", "attachment; filename=ram.sav");
        //     request->send(response);
        // });
        // _server.begin();
        // Procesar los datos recibidos según sea necesario
        // Aquí se puede agregar la lógica para manipular el buffer recibido
    // }

}
void GameBoyCartridge::restoreRAM(int maxBufferSize) {
    // Validate input size
    if (maxBufferSize <= 0 || maxBufferSize > 1048576) { // 1MB max as a reasonable limit
        transferJSON.clear();
        transferJSON["type"] = "error";
        transferJSON["message"] = "Invalid buffer size";
        Serial.print("JSON:");
        serializeJson(transferJSON, Serial);
        Serial.println();
        return;
    }
    
    bool receivingData = true;
    int totalReceived = 0;
    
    // Allocate buffer with error handling
    uint8_t *save_buffer = (uint8_t*)malloc(maxBufferSize * sizeof(uint8_t));
    if (!save_buffer) {
        transferJSON.clear();
        transferJSON["type"] = "error";
        transferJSON["message"] = "Memory allocation failed";
        Serial.print("JSON:");
        serializeJson(transferJSON, Serial);
        Serial.println();
        return;
    }
    
    memset(save_buffer, 0, maxBufferSize); // Clear the buffer
    
    // Flush serial buffer
    while (Serial.available()) {
        Serial.read();
    }
    
    // Add timeout for receiving data
    unsigned long startTime = millis();
    while(receivingData) {
        if (Serial.available() > 0) {
            int bytesRead = Serial.readBytes(save_buffer + totalReceived, maxBufferSize - totalReceived);
            if (bytesRead > 0) {
                totalReceived += bytesRead;
                if (totalReceived >= maxBufferSize) {
                    receivingData = false;
                }
            }
            // Reset timeout when we receive data
            startTime = millis(); 
        }
        
        // Check for timeout
        if (millis() - startTime > 5000) { // 5 second timeout
            transferJSON.clear();
            transferJSON["type"] = "error";
            transferJSON["message"] = "Timeout waiting for data";
            Serial.print("JSON:");
            serializeJson(transferJSON, Serial);
            Serial.println();
            free(save_buffer);
            return;
        }
        
        // Small delay to prevent CPU hogging
        delay(1);
    }

    // Configure GPIO pins
    pinMode(GAMEBOY_RST, OUTPUT);
    pinMode(GAMEBOY_CLK, OUTPUT);
    pinMode(GAMEBOY_WR, OUTPUT);
    pinMode(GAMEBOY_RD, OUTPUT);
    pinMode(GAMEBOY_CS, OUTPUT);

    // Set Control Pins to Output RST(PH0) CLK(PH1) CS(PH3) WR(PH5) RD(PH6)
    for (uint32_t i = 0; i < sizeof(ADDRESS_GB_GBC_PINS)/sizeof(ADDRESS_GB_GBC_PINS[0]); i++)
    {
        pinMode(ADDRESS_GB_GBC_PINS[i], OUTPUT);
    }
    // Set Data Pins (D0-D7) to Input
    for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS)/sizeof(DATA_GB_GBC_PINS[0]); i++)
    {
        pinMode(DATA_GB_GBC_PINS[i], INPUT);
    }

    digitalWrite(GAMEBOY_CLK, LOW);
    digitalWrite(GAMEBOY_RST, LOW);
    digitalWrite(GAMEBOY_WR, HIGH);
    digitalWrite(GAMEBOY_RD, HIGH);
    digitalWrite(GAMEBOY_CS, HIGH);

    delay(400);
    digitalWrite(GAMEBOY_RST, HIGH);

    if(this->ramEndAddress > 0) {
        this->totalRamBytes = (this->sramBanks) * 8192;
        this->currentAddress = 0xA000;

        // MBC2 Fix
        this->read_byte_GB(0x0134);
        
        // Enable SRAM for MBC1
        if (this->romType <= 4 || (this->romType >= 11 && this->romType <= 13)) {
            this->write_byte_GB(0x6000, 1); // Set RAM Mode
        }
        
        // Initialize MBC - Enable RAM
        this->ramEnable();
        this->currentBank = 0;

        // Switch RAM banks
        int x = 0;
        for (uint8_t currBank = 0; currBank < this->sramBanks; currBank++) {
            this->dataBusAsOutput();
            this->write_byte_GB(0x4000, currBank);
            this->dataBusAsInput();
            
            // Write RAM
            this->dataBusAsOutput();

            for (word sramAddress = 0xA000; sramAddress <= this->ramEndAddress; sramAddress++)
            {   
                // Ensure we don't exceed the buffer bounds
                if (x < maxBufferSize) {
                    this->writeByteSRAM_GB(sramAddress, save_buffer[x]);
                    x++;
                } else {
                    // We've reached the end of our buffer
                    break;
                }
            }
            
            // Send progress update
            transferJSON.clear();
            transferJSON["type"] = "progress";
            transferJSON["progress"] = (int)(this->ramEndAddress - 0xA000);
            transferJSON["bank"] = currBank;
            transferJSON["total_banks"] = this->sramBanks;
            Serial.print("JSON:");
            serializeJson(transferJSON, Serial);
            Serial.println();

            this->dataBusAsInput();
        }

        // Disable RAM when done
        this->ramDisable();
        
        // Send success message
        transferJSON.clear();
        transferJSON["type"] = "success";
        transferJSON["bytes_written"] = x;
        Serial.print("JSON:");
        serializeJson(transferJSON, Serial);
        Serial.println();
    } else {
        // No RAM to restore
        transferJSON.clear();
        transferJSON["type"] = "error";
        transferJSON["message"] = "No RAM to restore";
        Serial.print("JSON:");
        serializeJson(transferJSON, Serial);
        Serial.println();
    }
    
    // Always free the buffer, even on error paths
    free(save_buffer);
}
// void GameBoyCartridge::startReadRAM_GB() {

// }
// void GameBoyCartridge::endReadRAM_GB() {

// }
// void GameBoyCartridge::startWriteRAM_GB()
// {
//     //  Load ROM header
//     this->headerROM_GB(false);
//     this->writtingRAM = true;

//     this->rd_wr_mreq_reset();

//     // MBC2 Fix (unknown why this fixes it, maybe has to read ROM before RAM?)
//     this->read_byte_GB(0x0134);

//     // Does cartridge have RAM
//     if (this->ramEndAddress > 0)
//     {
//         if (this->cartridgeType <= 4)
//         {                                   // MBC1
//             this->write_byte_GB(0x6000, 1); // Set RAM Mode
//         }

//         // Initialise MBC
//         this->write_byte_GB(0x0000, 0x0A);

//         // Switch RAM banks
//         for (uint8_t bank = 0; bank < ramBanks; bank++)
//         {
//             this->write_byte_GB(0x4000, bank);

//             // Write RAM
//             for (uint16_t ramAddress = 0xA000; ramAddress <= this->ramEndAddress; ramAddress++)
//             {
//                 // Wait for serial input
//                 while (Serial.available()){
//                 }

//                 // Read input
//                 uint8_t readValue = (uint8_t)Serial.read();

//                 // Write to RAM
//                 cs_mreqPin_low;
//                 this->write_byte_GB(ramAddress, readValue);
//                 asm volatile("nop");
//                 asm volatile("nop");
//                 asm volatile("nop");
//                 cs_mreqPin_high;
//             }
//         }

//         // Disable RAM
//         this->write_byte_GB(0x0000, 0x00);
//         Serial.flush(); // Flush any serial data that wasn't processed
//     }
// }
// void GameBoyCartridge::endWriteRAM_GB()
// {
//     this->writtingRAM = false;
// }
bool GameBoyCartridge::isWrittingROM()
{
    return this->writtingROM;
}
bool GameBoyCartridge::isWrittingRAM()
{
    return this->writtingRAM;
}
bool GameBoyCartridge::isRestoringRAM()
{
    return this->restoringRAM;
}
void GameBoyCartridge::setup()
{
    disableCore0WDT();  
    this->writtingRAM = false;
    this->writtingROM = false;
    this->lastByte = 0;
    this->cartridgeType = 0;
    this->romSize = 0;
    this->romBanks = 0;
    this->ramSize = 0;
    this->ramBanks = 0;
    this->sramSize = 0;
    this->romEndAddress = 0x7FFF;
    this->sramBanks = 0;
    this->romType = 0;
    this->currentBank = 0;
}
void GameBoyCartridge::main()
{
    // Define timeouts and limits
    const unsigned long MAX_OPERATION_TIME = 30000; // 30 seconds max for any single operation
    const int MAX_READ_RETRIES = 3;           // Maximum number of read retries
    
    static unsigned long operationStartTime = 0;
    static bool timeoutCheckEnabled = false;
    
    // Start timing when an operation begins
    if ((this->isWrittingRAM() || this->isWrittingROM() || this->isRestoringRAM()) && !timeoutCheckEnabled) {
        operationStartTime = millis();
        timeoutCheckEnabled = true;
    } else if (!(this->isWrittingRAM() || this->isWrittingROM() || this->isRestoringRAM())) {
        timeoutCheckEnabled = false;
    }
    
    // Check for operation timeout
    if (timeoutCheckEnabled && (millis() - operationStartTime > MAX_OPERATION_TIME)) {
        // Operation has timed out, cancel it and report error
        transferJSON.clear();
        transferJSON["type"] = "error";
        transferJSON["message"] = "Operation timed out";
        Serial.print("JSON:");
        serializeJson(transferJSON, Serial);
        Serial.println();
        
        // Reset operation flags
        this->writtingRAM = false;
        this->writtingROM = false;
        this->restoringRAM = false;
        timeoutCheckEnabled = false;
        
        // Disable RAM access if needed
        this->write_byte_GB(0x0000, 0x00);
        this->rd_wr_mreq_off();
        return;
    }
    
    // Process RAM reading operation
    if (this->isWrittingRAM())
    {
        // Validate current bank is within bounds
        if (this->currentBank >= this->sramBanks || this->sramBanks == 0) {
            // Invalid bank number, abort operation
            transferJSON.clear();
            transferJSON["type"] = "error";
            transferJSON["message"] = "Invalid RAM bank number";
            Serial.print("JSON:");
            serializeJson(transferJSON, Serial);
            Serial.println();
            
            this->writtingRAM = false;
            this->write_byte_GB(0x0000, 0x00); // Disable RAM
            this->rd_wr_mreq_off();
            return;
        }

        // Set the current RAM bank
        bool bankSwitchSuccess = false;
        int retryCount = 0;
        while (!bankSwitchSuccess && retryCount < MAX_READ_RETRIES) {
            try {
                this->write_byte_GB(0x4000, this->currentBank);
                bankSwitchSuccess = true;
            } catch (...) {
                retryCount++;
                delay(10);
            }
        }
        
        if (!bankSwitchSuccess) {
            // Failed to switch banks, abort operation
            transferJSON.clear();
            transferJSON["type"] = "error";
            transferJSON["message"] = "Failed to switch RAM bank";
            Serial.print("JSON:");
            serializeJson(transferJSON, Serial);
            Serial.println();
            
            this->writtingRAM = false;
            this->write_byte_GB(0x0000, 0x00); // Disable RAM
            this->rd_wr_mreq_off();
            return;
        }
        
        // Read RAM in chunks of 64 bytes
        const size_t CHUNK_SIZE = 64;
        uint8_t readData[CHUNK_SIZE];
        bool chunkReadSuccess = true;
        
        // Calculate expected end of RAM for bounds checking
        word maxRamAddress = (this->ramEndAddress > 0xBFFF) ? 0xBFFF : this->ramEndAddress;
        
        // Process one chunk of RAM data
        word ramAddress = 0xA000;
        while (ramAddress <= maxRamAddress && chunkReadSuccess) {
            // Read up to 64 bytes
            for (uint8_t i = 0; i < CHUNK_SIZE && ramAddress + i <= maxRamAddress; i++) {
                try {
                    readData[i] = this->readByteSRAM_GB(ramAddress + i);
                } catch (...) {
                    // If reading fails, set error flag and break
                    chunkReadSuccess = false;
                    break;
                }
            }
            
            if (!chunkReadSuccess) {
                break;  // Exit the loop if reading failed
            }
            
            // Send the chunk to Serial1
            if (Serial1.availableForWrite() >= CHUNK_SIZE) {
                Serial1.write(readData, CHUNK_SIZE);
                // Don't flush here - it can block for too long
                
                // Update progress
                ramAddress += CHUNK_SIZE;
                this->processedProgressBar += CHUNK_SIZE;
                
                // Only process one chunk per main() call to prevent blocking too long
                break;
            } else {
                // If serial buffer is full, wait for next loop iteration
                break;
            }
        }
        
        // Check if we've finished this bank
        if (ramAddress > maxRamAddress || !chunkReadSuccess) {
            // Move to next bank or finish the operation
            this->currentBank++;
            
            // Check if we've processed all banks
            if (this->currentBank >= this->sramBanks || !chunkReadSuccess) {
                // Disable RAM
                this->write_byte_GB(0x0000, 0x00);
                delay(50);
                
                // Calculate totals and send completion message
                this->totalProgressBar = (uint32_t)(this->sramBanks) * 8192;
                
                transferJSON.clear();
                if (chunkReadSuccess) {
                    transferJSON["type"] = "success";
                } else {
                    transferJSON["type"] = "error";
                    transferJSON["message"] = "Failed to read RAM data";
                }
                
                transferJSON["total"] = this->totalProgressBar;
                transferJSON["progress"] = (this->totalProgressBar > 0) ? 
                    (this->processedProgressBar * 100 / this->totalProgressBar) : 0;
                transferJSON["ramBanks"] = this->sramBanks;
                transferJSON["bytesRead"] = this->processedProgressBar;
                
                Serial.print("JSON:");
                serializeJson(transferJSON, Serial);
                Serial.println();
                
                // Operation complete
                this->writtingRAM = false;
                this->rd_wr_mreq_off();
            }
        }
    } 
    else if(this->isRestoringRAM()) 
    {   
        // The restoringRAM operation is not currently implemented in the main loop
        // Disable the flag to prevent hanging
        this->restoringRAM = false;
        
        transferJSON.clear();
        transferJSON["type"] = "error";
        transferJSON["message"] = "RAM restoration via main loop not implemented";
        Serial.print("JSON:");
        serializeJson(transferJSON, Serial);
        Serial.println();
    } 
    else if (this->isWrittingROM())
    {
        // Validate current bank is within bounds
        if (this->currentBank >= this->romBanks || this->romBanks == 0) {
            // Invalid bank number, abort operation
            transferJSON.clear();
            transferJSON["type"] = "error";
            transferJSON["message"] = "Invalid ROM bank number";
            Serial.print("JSON:");
            serializeJson(transferJSON, Serial);
            Serial.println();
            
            this->writtingROM = false;
            return;
        }

        // Set up bank and address registers based on the current bank
        bool bankSwitchSuccess = false;
        int retryCount = 0;
        
        // Handle bank > 1 address adjustment
        if (this->currentBank > 1) {
            this->romAddress = 0x4000;
            // MBC6 banks are half size
            if (this->romType == 32) {
                this->romEndAddress = 0x5FFF;
            }
        }
        
        // Attempt bank switching with retry
        while (!bankSwitchSuccess && retryCount < MAX_READ_RETRIES) {
            try {
                // Switch banks based on cartridge type
                switch (this->romType) {
                    case 0x104: // M161
                        cs2Pin_low;
                        delay(10);
                        cs2Pin_high;
                        this->write_byte_GB(0x4000, this->currentBank & 0x7);
                        break;
                        
                    case 0x101:
                    case 0x103: // MBC1M
                        if (this->currentBank < 10) {
                            this->write_byte_GB(0x4000, this->currentBank >> 4);
                            this->write_byte_GB(0x2000, (this->currentBank & 0x1f));
                        } else {
                            this->write_byte_GB(0x4000, this->currentBank >> 4);
                            this->write_byte_GB(0x2000, 0x10 | (this->currentBank & 0x1f));
                        }
                        break;
                        
                    case 32: // MBC6
                        this->write_byte_GB(0x2800, 0);
                        this->write_byte_GB(0x3800, 0);
                        this->write_byte_GB(0x2000, this->currentBank);
                        this->write_byte_GB(0x3000, this->currentBank);
                        break;
                        
                    default:
                        if (this->romType >= 5) { // MBC2/3/4/5
                            if (this->romType >= 11 && this->romType <= 13) {
                                if ((this->currentBank & 0x1f) == 0) {
                                    // reset MMM01
                                    cs2Pin_low;
                                    delay(10);
                                    cs2Pin_high;
                                    
                                    // remap to higher 4Mbits ROM
                                    this->write_byte_GB(0x3fff, 0x20);
                                    this->write_byte_GB(0x5fff, 0x40);
                                    this->write_byte_GB(0x7fff, 0x01);
                                    this->write_byte_GB(0x1fff, 0x3a);
                                    this->write_byte_GB(0x1fff, 0x7a);
                                    
                                    // for every 4Mbits ROM, restart from 0x0000
                                    this->romAddress = 0x0000;
                                    this->currentBank++;
                                } else {
                                    this->write_byte_GB(0x6000, 0);
                                    this->write_byte_GB(0x2000, (this->currentBank & 0x1f));
                                }
                            } else {
                                if ((this->romType >= 0x19 && this->romType <= 0x1E) && 
                                    (this->currentBank == 0 || this->currentBank == 256)) {
                                    this->write_byte_GB(0x3000, (this->currentBank >> 8) & 0xFF);
                                }
                                this->write_byte_GB(0x2100, this->currentBank & 0xFF);
                            }
                        } else { // Assume MBC1 or similar
                            this->write_byte_GB(0x6000, 0);
                            this->write_byte_GB(0x4000, this->currentBank >> 5);
                            this->write_byte_GB(0x2000, this->currentBank & 0x1F);
                        }
                        break;
                }
                bankSwitchSuccess = true;
            } catch (...) {
                retryCount++;
                delay(10);
            }
        }
        
        if (!bankSwitchSuccess) {
            // Failed to switch banks, abort operation
            transferJSON.clear();
            transferJSON["type"] = "error";
            transferJSON["message"] = "Failed to switch ROM bank";
            Serial.print("JSON:");
            serializeJson(transferJSON, Serial);
            Serial.println();
            
            this->writtingROM = false;
            return;
        }
        
        // Read ROM data in smaller chunks to prevent long blocking
        // Read a single chunk of 512 bytes or less per main() call
        const size_t CHUNK_SIZE = 64; // Read 64 bytes at a time
        const size_t MAX_CHUNKS_PER_LOOP = 8; // Process up to 512 bytes per main() call
        
        // Validate ROM address is within valid range
        word maxRomAddress = this->romEndAddress;
        if (maxRomAddress > 0x7FFF) maxRomAddress = 0x7FFF; // Safety limit
        
        // Process ROM data in chunks
        if (this->romAddress <= maxRomAddress) {
            bool allChunksReadSuccessfully = true;
            
            for (size_t chunk = 0; chunk < MAX_CHUNKS_PER_LOOP; chunk++) {
                // Check if we've reached the end of this bank
                if (this->romAddress > maxRomAddress) {
                    break;
                }
                
                // Calculate size of this chunk (might be smaller at end of bank)
                size_t bytesToRead = CHUNK_SIZE;
                if (this->romAddress + bytesToRead > maxRomAddress + 1) {
                    bytesToRead = maxRomAddress + 1 - this->romAddress;
                }
                
                if (bytesToRead == 0) break;
                
                // Read chunk with retry
                bool chunkReadSuccess = false;
                retryCount = 0;
                
                while (!chunkReadSuccess && retryCount < MAX_READ_RETRIES) {
                    try {
                        // Read ROM data into buffer
                        for (size_t i = 0; i < bytesToRead; i++) {
                            this->sdBuffer[i] = this->read_byte_GB(this->romAddress + i);
                        }
                        chunkReadSuccess = true;
                    } catch (...) {
                        retryCount++;
                        delay(10);
                    }
                }
                
                if (!chunkReadSuccess) {
                    allChunksReadSuccessfully = false;
                    break;
                }
                
                // Send data to serial port
                if (Serial.availableForWrite() >= bytesToRead) {
                    Serial.write(this->sdBuffer, bytesToRead);
                    
                    // Update progress
                    this->romAddress += bytesToRead;
                    this->processedProgressBar += bytesToRead;
                } else {
                    // If serial buffer is full, wait for next loop iteration
                    break;
                }
            }
            
            // If we finished this bank or encountered an error, move to next bank
            if (this->romAddress > maxRomAddress || !allChunksReadSuccessfully) {
                // If there was an error reading, report it
                if (!allChunksReadSuccessfully) {
                    transferJSON.clear();
                    transferJSON["type"] = "warning";
                    transferJSON["message"] = "Error reading ROM data in bank " + String(this->currentBank);
                    transferJSON["bank"] = this->currentBank;
                    Serial.print("JSON:");
                    serializeJson(transferJSON, Serial);
                    Serial.println();
                }
                
                // Move to next bank
                this->currentBank++;
                
                // Send progress update every bank
                transferJSON.clear();
                transferJSON["type"] = "progress";
                transferJSON["bank"] = this->currentBank - 1; // Just completed this bank
                transferJSON["total"] = this->totalProgressBar;
                transferJSON["transferred"] = this->processedProgressBar;
                transferJSON["progress"] = (this->totalProgressBar > 0) ? 
                    (this->processedProgressBar * 100 / this->totalProgressBar) : 0;
                Serial.print("JSON:");
                serializeJson(transferJSON, Serial);
                Serial.println();
            }
        }
        
        // Check if we've processed all banks
        if (this->currentBank >= this->romBanks) {
            // Operation complete
            transferJSON.clear();
            transferJSON["type"] = "success";
            transferJSON["total"] = this->totalProgressBar;
            transferJSON["transferred"] = this->processedProgressBar;
            transferJSON["progress"] = 100;
            transferJSON["romBanks"] = this->romBanks;
            transferJSON["bytesRead"] = this->processedProgressBar;
            
            Serial.print("JSON:");
            serializeJson(transferJSON, Serial);
            Serial.println();
            
            this->writtingROM = false;
        }
    }
}
