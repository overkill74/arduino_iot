# Arduino IOT
Ho to use Arduino with IOT

# 1. Hardware
Arduino WeMos D1 mini ESP8266

# 2. Sketch
## 2.1. Prova_1
Used "Client Base" Example as base. We include the SSID and PWS information in a header called wifi_net.h. This is made to not publish this information. So you must create an header file called 'wifi_net.h' with this contents:

```
#define STASSID "your-ssid"
#define STAPSK  "your-password"
```




