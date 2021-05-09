#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "Led.h"
#include "wifi_net.h"

#ifndef STASSID
#error "Please define STASSID and STAPSK in wifi_net.h"
#endif

#define DOOR_PIN        LED_BUILTIN

#define WLAN_SSID       STASSID //Il nome della tua connessione wifi
#define WLAN_PASS       STAPSK //La password della tua connessione wifi
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883 //per il protocollo SSL usare la porta 8883
#define AIO_USERNAME    "ovk74" //Username creato su Adafruit
#define AIO_KEY         "aio_tqqV63ABy7gWiRHsGJfjl9sl3eER" //IO_key creata su Adafruit

//--------------------------------------------
static WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
//Adafruit_MQTT_Subscribe openDoorFeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/led-di-prova");
Adafruit_MQTT_Publish g_iot_feed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/led-di-prova");

static Led g_led_r = Led("LED_R", D7, 1023, true, 25, 2500);
static Led g_led_g = Led("LED_G", D6, 1023, true, 25, 2500);
static Led g_led_b = Led("LED_B", D5, 1023, true, 25, 2500);

//--------------------------------------------
static void accendi_il_led() {
  digitalWrite(DOOR_PIN, HIGH);
}

//--------------------------------------------
static void spegni_il_led() {
  digitalWrite(DOOR_PIN, LOW);
}

//--------------------------------------------
static void lampeggia(int n)
{
  int x;
  for (int x = 0; x < n; ++x) {
    accendi_il_led();
    delay(200);
    spegni_il_led();
    delay(300);
  }
}

//--------------------------------------------
static int atox(String& str)
{
  int rv = 0;
  int test = 1;
  int len = str.length() - 1;
  while (len >= 0) {
    char p = str[len];
    int add = 0;
    do {
      if (p >= '0' && p <= '9') {
        add = (int)(p - '0');
        break;
      }
      if (p >= 'A' && p <= 'F') {
        add = (int)(p + 10 - 'A');
        break;
      }
      if (p >= 'a' && p <= 'f') {
        add = (int)(p + 10 - 'a');
        break;
      }
    } while (0);

    rv += add * test;
    test *= 16;
    --len;
  }
  return rv;
}

//--------------------------------------------
static bool MQTT_connect()
{
  int8_t ret;

  if (mqtt.connected()) {
    return true;
  }

  spegni_il_led();
  Serial.print("Connessione al server MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0)
  {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Tentativo di connessione tra 5 secondi...");

    mqtt.disconnect();
    for (int i = 0; i < 20; ++i) {
      accendi_il_led();
      delay(100);
      spegni_il_led();
      delay(150);
    }
    retries--;
    if (retries == 0) {
      return false;
    }
  }

  Serial.println("MQTT connesso!");
  lampeggia(2);

  return true;
}

//--------------------------------------------
void setup()
{
  Serial.begin(115200);
  delay(10);

  pinMode(DOOR_PIN, OUTPUT);
  digitalWrite(DOOR_PIN, 0);

  Serial.println(); Serial.println();
  Serial.print("Connessione alla rete Wi-FI ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    lampeggia(1);
    Serial.print("*");
  }
  lampeggia(3);
  Serial.println();

  Serial.println("Wi-FI Connesso!");
  Serial.println("IP del modulo ESP8266 : ");
  Serial.println(WiFi.localIP());

  //mqtt.subscribe(&openDoorFeed);
}

//--------------------------------------------
static void led_rgb_update(const String& color)
{
  String ar = color.substring(1, 3);
  String ag = color.substring(3, 5);
  String ab = color.substring(5, 7);
  g_led_r.setLuminosity(atox(ar) * 4);
  g_led_g.setLuminosity(atox(ag) * 4);
  g_led_b.setLuminosity(atox(ab) * 4);
}

//--------------------------------------------
void loop()
{
  unsigned long tout = 0;
  static int con_err = 0;
  static const String colors[] =
  {
    "#FF0000",
    "#00FF00",
    "#0000FF",
    "#00FFFF",
    "#FFFF00",
    "#FF00FF",
    "#8000FF",
    "#00FF80",
    "#FF8000",
    "#FFFFFF",
    "FINE"
  };
  static int i = 0;

  MQTT_connect();
  
  if (!g_iot_feed.publish((uint8_t*)colors[i].c_str(), colors[i].length())) {
    Serial.println("Error during upload");
    delay(2000);
  }
  else {
    Serial.println(colors[i]);
    led_rgb_update(colors[i]);
    ++i;
    if (colors[i].equalsIgnoreCase("FINE")) {
      i = 0;
    }
    for (int x=0; x<10; ++x) {
      delay(1000); mqtt.ping();
    }
  }
  
  // Verifica la connessione
  if (mqtt.connected())
  {
    if (mqtt.ping()) {
      con_err = 0;
    }
    else {
      if (++con_err > 3) {
        Serial.println("Missing PING 3 times, disocnnect from MQTT");
        mqtt.disconnect();
      }
    }
  }

}
