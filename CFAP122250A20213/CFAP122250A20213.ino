//=============================================================================
// "Arduino" example program for Crystalfontz ePaper.
//
// This project is for the CFAP122250A2-0213 :
//
//   https://www.crystalfontz.com/product/CFAP122250A2-0213
//
// It was written against a Seeduino v4.2 @3.3v. An Arduino UNO modified to
// operate at 3.3v should also work.
//-----------------------------------------------------------------------------
// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to <http://unlicense.org/>
//=============================================================================
// Connecting the Arduino to the display
//
// ARDUINO |Adapter |Wire Color |Function
// --------+--------+-----------+--------------------
// D3      |17      |Green      |Busy Line
// D4      |18      |Brown      |Reset Line
// D5      |15      |Purple     |Data/Command Line
// D10     |16      |Blue       |Chip Select Line
// D11     |14      |White      |MOSI
// D13     |13      |Orange     |Clock
// 3.3V    |5       |Red        |Power
// GND     |3       |Black      |Ground
//
// Short the following pins on the adapter board:
// BS2  -> GND
// RESE -> 2.2ohms
//=============================================================================
//Connecting the Arduino to the SD card
//
// ARDUINO  |Wire Color |Function
// ---------+-----------+--------------------
// D8       |Blue       |CS
// D11      |Green      |MOSI
// D12      |Purple     |MISO
// D13      |Brown      |CLK
//
//
//=============================================================================
// Creating image data arrays
//
// Bmp_to_epaper is code that will aid in creating bitmaps necessary from .bmp files.
// The code can be downloaded from the Crystalfontz website: https://www.Crystalfontz.com
// or it can be downloaded from github: https://github.com/crystalfontz/bmp_to_epaper
//=============================================================================

// The display is SPI, include the library header.
#include <SPI.h>
#include <SD.h>
#include <avr/io.h>

// Include the images. These images were prepared with "bmp_to_epaper" which
// is available on the Crystalfontz site.
#include "Images_for_CFAP122250A20213.h"

#define ePaper_RST_0 (digitalWrite(EPD_RESET, LOW))
#define ePaper_RST_1 (digitalWrite(EPD_RESET, HIGH))
#define ePaper_CS_0 (digitalWrite(EPD_CS, LOW))
#define ePaper_CS_1 (digitalWrite(EPD_CS, HIGH))
#define ePaper_DC_0 (digitalWrite(EPD_DC, LOW))
#define ePaper_DC_1 (digitalWrite(EPD_DC, HIGH))

#define EPD_BUSY 3
#define EPD_RESET 4
#define EPD_DC 5
#define EPD_CS 10
#define SD_CS 8

#define VRES 300
#define HRES 400

#define WAIT_BUSY while (0 != digitalRead(EPD_BUSY))

//=============================================================================
//this function will take in a byte and send it to the display with the
//command bit low for command transmission
void writeCMD(uint8_t command)
{
  ePaper_DC_0;
  ePaper_CS_0;
  SPI.transfer(command);
  ePaper_CS_1;
}

//this function will take in a byte and send it to the display with the
//command bit high for data transmission
void writeData(uint8_t data)
{
  ePaper_DC_1;
  ePaper_CS_0;
  SPI.transfer(data);
  ePaper_CS_1;
}

//===========================================================================
void fullUpdate(void)
{
  //
  writeCMD(0x22);
  writeData(0xf7);
  writeCMD(0x20);
}

void partialUpdate(void)
{
  //
  writeCMD(0x22);
  writeData(0xff);
  writeCMD(0x20);
}

//===========================================================================
void setup(void)
{
  //Debug port / Arduino Serial Monitor (optional)
  Serial.begin(115200);
  Serial.println("setup started");
  // Configure the pin directions
  pinMode(EPD_CS, OUTPUT);
  pinMode(EPD_RESET, OUTPUT);
  pinMode(EPD_DC, OUTPUT);
  pinMode(EPD_BUSY, INPUT);
  pinMode(SD_CS, OUTPUT);

  digitalWrite(SD_CS, LOW);

  if (!SD.begin(SD_CS))
  {
    Serial.println("SD could not initialize");
  }
  //Set up SPI interface
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
  SPI.begin();

  initEPD();
  Serial.println("setup complete");
}

//================================================================================
void initEPD()
{

  //-----------------------------------------------------------------------------
  //more detail on the following commands and additional commands not used here
  //can be found on the CFAP176264A0-0270 datasheet on the Crystalfontz website
  //-----------------------------------------------------------------------------

  //reset driver
  ePaper_RST_0;
  delay(40);
  ePaper_RST_1;
  delay(40);

  //Software Reset
  WAIT_BUSY;
  writeCMD(0x12);
  WAIT_BUSY;

  //Get out of deep sleep if we're there already
  writeCMD(0x10);
  writeData(0x00);

  //Driver Output Control
  writeCMD(0x01);
  writeData(0xf9);
  writeData(0x00);
  writeData(0x00);

  //Data Entry mode setting
  writeCMD(0x11);
  writeData(0x01);

  //Set RAM X - address Start / End position
  writeCMD(0x44);
  writeData(0x00);
  writeData(0x0F);

  //Set RAM Y - address Start / End position
  writeCMD(0x45);
  writeData(0xF9);
  writeData(0x00);
  writeData(0x00);
  writeData(0x00);

  //Border Waveform Control
  writeCMD(0x3C);
  writeData(0x05);

  //Display Update Control 1
  writeCMD(0x21);
  writeData(0x00);
  writeData(0x80);

  //Temperature Sensor Control
  writeCMD(0x18);
  writeData(0x80);

  // Set RAM X and RAM Y address counter
  writeCMD(0x4E);
  writeData(0x00);
  writeCMD(0x4F);
  writeData(0xF9);
  writeData(0x00);
  WAIT_BUSY;
}

//================================================================================
void partialUpdateSolid(uint8_t x1, uint8_t y1, uint16_t x2, uint16_t y2, uint8_t color)
{
  //Please note, this is demonstration code on how to use the Partial Updating on the controller but this display itself
  //  does not support partial updating. Using this code may cause ghosting issues over time.

  //Border Waveform Control
  writeCMD(0x3C);
  writeData(0x80);

  //Set RAM X - address Start / End position
  writeCMD(0x44);
  writeData(x1);
  writeData(x2);

  //Set RAM Y - address Start / End position
  writeCMD(0x45);
  writeData(y1 & 0xff);
  writeData(y1 >> 8);
  writeData(y2 & 0xff);
  writeData(y2 >> 8);

  // Set RAM X and RAM Y address counter
  writeCMD(0x4E);
  writeData(x1);
  writeCMD(0x4F);
  writeData(y1 & 0xff);
  writeData(y1 >> 8);

  writeCMD(0x24);
  int i;
  int h;
  for (h = 0; h < y2 - y1; h++)
  {
    for (i = 0; i < (x2 - x1) / 8; i++)
    {
      writeData(color);
    }
  }

  //partial refresh of the same area as the partial update
  partialUpdate();
  WAIT_BUSY;
  initEPD();
}

//================================================================================
void setBaseMap(const uint8_t *holder)
{
  writeCMD(0x24); //Write Black and White image to RAM
  for (int i = 0; i < MONO_ARRAY_SIZE; i++)
  {
    writeData(pgm_read_byte(&holder[i]));
  }

  writeCMD(0x26); //Write Black and White image to RAM
  for (int i = 0; i < MONO_ARRAY_SIZE; i++)
  {
    writeData(pgm_read_byte(&holder[i]));
  }
  fullUpdate();
}

//================================================================================
void partialUpdateCharacters(const uint8_t *holder)
{

  //x1 must be divisible by 8
  uint16_t x1 = 0;
  uint16_t y1 = 100;
  uint16_t x2 = x1 + (64 / 8) - 1;
  uint16_t y2 = y1 + (58) - 1;

  //reset driver
  ePaper_RST_0;
  delay(40);
  ePaper_RST_1;
  delay(40);

  //Border Waveform Control
  writeCMD(0x3C);
  writeData(0x80);

  //Set RAM X - address Start / End position
  writeCMD(0x44);
  writeData(x1);
  writeData(x2);

  //Set RAM Y - address Start / End position
  writeCMD(0x45);
  writeData(y1 & 0xff);
  writeData(y1 >> 8);
  writeData(y2 & 0xff);
  writeData(y2 >> 8);

  // Set RAM X and RAM Y address counter
  writeCMD(0x4E);
  writeData(x1);
  writeCMD(0x4F);
  writeData(y1 & 0xff);
  writeData(y1 >> 8);

  writeCMD(0x24);
  int i;
  int h;
  for (h = 0; h <= y2 - y1; h++)
  {
    for (i = 0; i < (x2 - x1 + 1); i++)
    {
      writeData(~pgm_read_byte(&holder[i + h * 8]));
    }
  }

  //partial refresh of the same area as the partial update
  partialUpdate();
  WAIT_BUSY;
}

//================================================================================
void show_BMPs_in_root(void)
{
  File
      root_dir;
  root_dir = SD.open("/");
  if (0 == root_dir)
  {
    Serial.println("show_BMPs_in_root: Can't open \"root\"");
    return;
  }

  File
      bmp_file;

  while (1)
  {
    bmp_file = root_dir.openNextFile();
    Serial.println(bmp_file);
    if (0 == bmp_file)
    {
      // no more files, break out of while()
      // root_dir will be closed below.
      break;
    }
    //Skip directories (what about volume name?)
    if (0 == bmp_file.isDirectory())
    {
      //The file name must include ".BMP"
      if (0 != strstr(bmp_file.name(), ".BMP"))
      {
        Serial.println(bmp_file.name());
        //The BMP must be exactly 36918 long
        //(this is correct for182x96, 24-bit)
        uint32_t size = bmp_file.size();

        Serial.println(size);
        if ((uint32_t)HRES * VRES * 3 + 50 <= size <= (uint32_t)HRES * VRES * 3 + 60)
        {
          Serial.println("in final loop");

          //Make sure the display is not busy before starting a new command.
          WAIT_BUSY;

          //Select the controller

          writeCMD(0x13);
          //Jump over BMP header
          bmp_file.seek(54);
          //grab one row of pixels from the SD card at a time
          static uint8_t one_line[HRES / 2 * 3];
          for (int line = 0; line < VRES * 2; line++)
          {

            //Set the LCD to the left of this line. BMPs store data
            //to have the image drawn from the other end, uncomment the line below

            //read a line from the SD card
            bmp_file.read(one_line, HRES / 2 * 3);

            //send the line to the display
            send_pixels_BW(HRES / 2 * 3, one_line);
          }

          Serial.println("refreshing......");
          //Write the command: Display Refresh (DRF)
          fullUpdate();
          //Make sure the display is not busy before starting a new command.
          WAIT_BUSY;

          Serial.print(" complete");
          //Give a bit to let them see it
          delay(3000);
        }
      }
    }
    //Release the BMP file handle
    bmp_file.close();
  }
  //Release the root directory file handle
  root_dir.close();
}

//================================================================================
void Load_Flash_Image_To_Display_RAM_RLE(uint16_t width_pixels,
                                         uint16_t height_pixels,
                                         const uint8_t *BW_image)
{
  //Index into *image, that works with pgm_read_byte()
  uint8_t count = 0;

  //Get width_bytes from width_pixel, rounding up
  uint8_t
      width_bytes;
  width_bytes = (width_pixels + 7) >> 3;

  //Make sure the display is not busy before starting a new command.
  WAIT_BUSY;

  //Select the controller
  ePaper_CS_0;

  //Aim at the command register
  ePaper_DC_0;

  // Write RAM (Black White) / RAM 0x24
  // After this command, data entries will be
  // written into the BW RAM until another
  // command is written. Address pointers will
  // advance accordingly
  SPI.transfer(0x24);
  //Pump out the data.
  ePaper_DC_1;
  count = 0;
  for (int i = 0; i < MONO_ARRAY_SIZE; i = i + 2)
  {
    count = pgm_read_byte(&BW_image[i]);
    for (uint8_t j = 0; j < count; j++)
      SPI.transfer(pgm_read_byte(&BW_image[i + 1]));
  }

  fullUpdate();

  //Deslect the controller
  ePaper_CS_1;
}

//================================================================================
void Load_Flash_Image_To_Display_RAM(uint16_t width_pixels,
                                     uint16_t height_pixels,
                                     const uint8_t *BW_image)
{
  //Index into *image, that works with pgm_read_byte()
  uint8_t count = 0;

  //Get width_bytes from width_pixel, rounding up
  uint8_t
      width_bytes;
  width_bytes = (width_pixels + 7) >> 3;

  //Make sure the display is not busy before starting a new command.
  WAIT_BUSY;

  //Select the controller
  ePaper_CS_0;

  //Aim at the command register
  ePaper_DC_0;
  // Write RAM (Black White) / RAM 0x24
  // After this command, data entries will be
  // written into the BW RAM until another
  // command is written. Address pointers will
  // advance accordingly
  SPI.transfer(0x24);
  //Pump out the data.
  ePaper_DC_1;
  count = 0;
  for (int i = 0; i < MONO_ARRAY_SIZE; i++)
  {
    SPI.transfer(pgm_read_byte(&BW_image[i]));
  }

  fullUpdate();

  //Deslect the controller
  ePaper_CS_1;
}

//================================================================================
void send_pixels_BW(uint16_t byteCount, uint8_t *dataPtr)
{
  uint8_t data;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  while (byteCount != 0)
  {
    uint8_t data = 0;
    red = *dataPtr;
    dataPtr++;
    byteCount--;
    green = *dataPtr;
    dataPtr++;
    byteCount--;
    blue = *dataPtr;
    dataPtr++;
    byteCount--;

    if (150 > ((red * .21) + (green * .72) + (blue * .07)))
    {
      data = data | 0x01;
    }

    for (uint8_t i = 0; i < 7; i++)
    {
      red = *dataPtr;
      dataPtr++;
      byteCount--;
      green = *dataPtr;
      dataPtr++;
      byteCount--;
      blue = *dataPtr;
      dataPtr++;
      byteCount--;
      data = data << 1;
      if (127 > ((red * .21) + (green * .72) + (blue * .07)))
      {
        data = data | 0x01;
      }
      else
      {
        data = data & 0xFE;
      }
    }
    writeData(data);
  }
}

void outDeepSleep()
{
  //Software Reset
  WAIT_BUSY;

  writeCMD(0x12);
  WAIT_BUSY;

  writeCMD(0x10);
  writeData(0x00);
}

void deepSleep()
{
  writeCMD(0x10);
  writeData(0x01);
}

//=============================================================================
#define SHUTDOWN_BETWEEN_UPDATES (0)
#define waittime 5000
#define splashscreen 1
#define partialUpdate 1
#define showBMPs 0
#define black 1
#define white 1
void loop()
{
  Serial.println("top of loop");

#if splashscreen
  initEPD();

  //load an image to the display
  Load_Flash_Image_To_Display_RAM(HRES, VRES, Mono_1BPP);

  Serial.print("refreshing . . . ");
  WAIT_BUSY;

  Serial.println("refresh complete");
  //for maximum power conservation, power off the EPD
  delay(waittime);

#endif
#if partialUpdate

  initEPD();
  setBaseMap(Mono_1BPP);
  WAIT_BUSY;
  delay(100);

  partialUpdateCharacters(Mono_Letter_C);
  partialUpdateCharacters(Mono_Letter_F);
  partialUpdateCharacters(Mono_Letter_A);
  partialUpdateCharacters(Mono_Letter_P);
  delay(waittime);
#endif

#if showBMPs
  //BMPs are inverted
  initEPD();

  Epaper_Write_Command(0x01); //Driver output control
  Epaper_Write_Data(0xF9);
  Epaper_Write_Data(0x00);
  Epaper_Write_Data(0x01);
  show_BMPs_in_root();
  delay(waittime);

  //flip the display back to normal
  Epaper_Write_Command(0x01); //Driver output control
  Epaper_Write_Data(0xF9);
  Epaper_Write_Data(0x00);
  Epaper_Write_Data(0x00);

#endif

#if black
  initEPD();

  //Make sure the display is not busy before starting a new command.
  WAIT_BUSY;

  //Select the controller
  ePaper_CS_0;

  //Aim at the command register
  ePaper_DC_0;
  // Write RAM (Black White) / RAM 0x24
  // After this command, data entries will be
  // written into the BW RAM until another
  // command is written. Address pointers will
  // advance accordingly
  SPI.transfer(0x24);
  //Pump out the data.
  ePaper_DC_1;
  for (int i = 0; i < MONO_ARRAY_SIZE; i++)
  {
    SPI.transfer(0xff);
  }

  fullUpdate();

  //Deslect the controller
  ePaper_CS_1;

  delay(waittime);
#endif

#if white
  initEPD();
  //Make sure the display is not busy before starting a new command.
  WAIT_BUSY;

  //Select the controller
  ePaper_CS_0;

  //Aim at the command register
  ePaper_DC_0;
  // Write RAM (Black White) / RAM 0x24
  // After this command, data entries will be
  // written into the BW RAM until another
  // command is written. Address pointers will
  // advance accordingly
  SPI.transfer(0x24);
  //Pump out the data.
  ePaper_DC_1;
  for (int i = 0; i < MONO_ARRAY_SIZE; i++)
  {
    SPI.transfer(0x00);
  }

  fullUpdate();

  //Deslect the controller
  ePaper_CS_1;

  delay(waittime);
#endif

}
//=============================================================================
