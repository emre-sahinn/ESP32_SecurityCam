/*
   Author : Emre Şahin
   Date   : 1 Feb 20
   Email  : arxtechnologies@gmail.com
*/

#include "FS.h"       // SD Card
#include "SD_MMC.h"   // SD Card

#include "esp_camera.h"
#include "Arduino.h"
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"
#include <EEPROM.h>            // read and write from flash memory

// define the number of bytes you want to access
#define EEPROM_SIZE 1

// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

uint8_t pictureNumber = 0;
/*EEPROM ORGANISATION: Count up to 16 million
   First Cell : Picture number
   Second Cell: Memory Index 1
   Third Cell : Memory Index 2
   Fourth Cell: Memory Index 3
*/

void setup() {
  Serial.begin(115200);
  //At first init SD_MMC because otherwise begin() returns false
  if (!SD_MMC.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD_MMC.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD_MMC card attached");
    return;
  }

  Serial.print("SD_MMC Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);

  delay(250);

  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_GRAYSCALE ;//PIXFORMAT_JPEG

  if (psramFound()) {
    config.frame_size = FRAMESIZE_XGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 40;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_XGA;
    config.jpeg_quality = 40;//0-63, lower means higher quality
    config.fb_count = 1;
  }

  // Init Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  EEPROM.begin(EEPROM_SIZE);
  //resetEEPROMMemory();      //Use this function when you need empty EEPROM
}

void writeFile(fs::FS &fs) {
  pictureNumber = EEPROM.read(0) + 1;
  String path = "/Photo - " + String(EEPROM.read(3)) + "-" +
                String(EEPROM.read(2)) + "-" + String(EEPROM.read(1)) + "-" +
                String(pictureNumber) + ".jpg";
  Serial.printf("Writing file: %s\n", path.c_str());

  File file = fs.open(path.c_str(), FILE_WRITE);
  Serial.println("take photo..");
  // Take Picture with Camera
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  file.write(fb->buf, fb->len); // payload (image), payload length

  if (EEPROM.read(0) > 254) {
    pictureNumber = 0;
    EEPROM.write(0, pictureNumber);
    EEPROM.write(1, EEPROM.read(1) + 1);
  }
  if (EEPROM.read(1) > 254) {
    EEPROM.write(1, 0);
    EEPROM.write(2, EEPROM.read(2) + 1);
  }
  if (EEPROM.read(2) > 254) {
    EEPROM.write(2, 0);
    EEPROM.write(3, EEPROM.read(3) + 1);
  }

  if (EEPROM.read(3) > 254) { //Extreme condition
    EEPROM.write(3, 0);
  }

  EEPROM.write(0, pictureNumber);
  EEPROM.commit();
  esp_camera_fb_return(fb);
}

// Function to fill the packet buffer with zeros
void initBuff(char* buff) {
  for (int i = 0; i < 240; i++) {
    buff[i] = 0;
  }
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
  file.close();
}

void resetEEPROMMemory() {
  for (int8_t i; i < 4; i++)
    EEPROM.write(i, 0);
}

int i = 0;
unsigned long previousMillis = 0;
void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;
    writeFile(SD_MMC);
  }
}
