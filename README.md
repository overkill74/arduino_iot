# Arduino IOT
Ho to use Arduino with IOT

# Hardware
Arduino WeMos D1 mini ESP8266

# Setup Arduino IDE
## Install ESP8266 Board on Arduino ID

[https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/]: Follow this link

1. In your Arduino IDE, go to `File> Preferences`
2. Enter http://arduino.esp8266.com/stable/package_esp8266com_index.json into the “**Additional Boards Manager URLs**” field. Then, click the “**OK**”.
Note: if you already have the ESP32 boards URL, you can separate the URLs with a comma as follows:
```
https://dl.espressif.com/dl/package_esp32_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json
```
3. Open the Boards Manager. `Go to Tools > Board > Boards Manager…`
4. Search for ESP8266 and press install button for the “ESP8266 by ESP8266 Community“.
5. That’s it. It should be installed after a few seconds.

## Select the board
Select the board
`Strumenti->Scheda->ESP8266 Boards->LOLIN(WEMOS) D1 R2 & MINI`

## Install library
`Adafruit MQTT Library` and it's related
`Adafruit SleepyDog Library`
`Adafruit FONA Library`
`WiFi101`


#  Sketch
##  Prova_1
Used "Client Base" Example as base. We include the SSID and PWS information in a header called wifi_net.h. This is made to not publish this information. So you must create an header file called 'wifi_net.h' with this contents:

```
#define STASSID "your-ssid"
#define STAPSK  "your-password"
```




