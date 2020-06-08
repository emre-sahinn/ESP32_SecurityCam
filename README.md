# ESP32 Security Camera

<img src="https://user-images.githubusercontent.com/30238276/83972942-cc80de80-a8eb-11ea-9541-ea2e76cdec73.png" width="20%" height="20%"> <img src="https://user-images.githubusercontent.com/30238276/83972947-cee33880-a8eb-11ea-815a-e5a11b83577a.png" width="30%" height="20%"> 

ESP32 is capable of functioning reliably in industrial environments, with operating temperatures ranging from –40°C to +125°C. Also, it achieves ultra-low power consumption which is good for our purpose, a wireless security camera! As a development board, I chose ESP32CAM which is a tiny module based on ESP32 chip and OV2640. The reason why I chose ESP32CAM is that it is cheap and very versatile. So to reduce the power consumption, I disconnect the onboard flash and disabled WiFi, Bluetooth modules. The only thing that I added is an 8GB SD card for camera images. I also lowered the image quality to QVGA, without RGB colors so only black and white color range. By doing that I reduced the image size to 7KB which is great for our purpose! If my calculations are correct we can fit approximately 1 billion photos. For indexing and file name, I used EEPROM bit operations to increment the current photo index, so clients can search through photo indexes to find what they are looking for. In the end, I 3D printed chassis for the ESP32CAM module to make it water and heat resistant.  

## Goal

You can identify thieves or make a video about your newly sprouted seed's growth!

## Reliability

I tested ESP32CAM to take a photo of my parent's car every second for security reasons and over 20 thousand photos, there are no errors!

<img src="https://user-images.githubusercontent.com/30238276/84045100-1089e700-a9b1-11ea-9295-c160fedb9dcf.png" width="60%" height="60%">

## Getting Started

These instructions will get you a copy of the project up and running on your development board: <br>
Step 1) Download Arduino IDE<br>
Step 2) Download ESP32 Core for IDE<br>
Step 6) Compile the given code<br>
Step 7) Connect the camera module to ESP32<br>
Step 8) Plug 5V adaptor or 3.3V LiPo Battery to ESP32<br>
<br>
Now you are ready to go! If you use stock values, ESP32 will take photo of every 1 seconds.<br>
<br>

### Prerequisites

1 x ESP32CAM module with OV2640<br>
1 x 5V 2A adaptor or 3.3V LiPo battery<br>
1 x USB Cable (to upload the code)<br>
2 x Heatsink (Optional)<br>

## Built With

* [Arduino](https://www.arduino.cc/) - Arduino IDE
* [ESP Core](https://github.com/espressif/arduino-esp32) - Arduino ESP32 Core

