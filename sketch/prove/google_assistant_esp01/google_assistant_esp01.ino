#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
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
#define AIO_KEY         "aio_JQwi29eMqfo6RJWOoKPi7e3XIdMy" //IO_key creata su Adafruit

#define LED_R           D7
#define LED_G           D6
#define LED_B           D5



WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Subscribe openDoorFeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/led-di-prova");

void MQTT_connect();



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
static void dimmer_led_hex(int val) {
  if (val >= 128) {
    digitalWrite(DOOR_PIN, LOW);
  }
  else {
    digitalWrite(DOOR_PIN, HIGH);
  }
  //analogWrite(DOOR_PIN, val);
}

//--------------------------------------------
static void dimmer_led_perc(int perc) {
  if (perc < 0) {
    perc = 0;
  }
  if (perc > 100) {
    perc = 100;
  }
  dimmer_led_hex((perc * 255) / 100);
}

//--------------------------------------------
static void accendi_il_led() {
  dimmer_led_hex(255);
}

//--------------------------------------------
static void spegni_il_led() {
  dimmer_led_hex(0);
}

//--------------------------------------------
void MQTT_connect()
{
  int8_t ret;

  if (mqtt.connected()) {
    return;
  }

  spegni_il_led();
  Serial.print("Connessione al server MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Tentativo di connessione tra 5 secondi...");
    mqtt.disconnect();
    //delay(5000);
    for (int i = 0; i < 20; ++i) {
      accendi_il_led();
      delay(100);
      spegni_il_led();
      delay(150);
    }
    retries--;
    if (retries == 0) {
      while (1);
    }
  }
  Serial.println("MQTT connesso!");
  lampeggia(2);
}


//--------------------------------------------
void setup()
{

  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_B, HIGH);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  Serial.begin(115200);
  delay(10);

  pinMode(DOOR_PIN, OUTPUT);
  digitalWrite(DOOR_PIN, 0);

#if 0
  for (int i = 0; i < 255; i++) {
    analogWrite(LED_B, i);
    Serial.println(i);
    delay(5);
  }
  for (int i = 255; i > 0; i--) {
    analogWrite(LED_B, i);
    Serial.println(i);
    delay(5);
  }
#endif
  //digitalWrite(DOOR_PIN, LOW);
  //analogWrite(DOOR_PIN, 0);

  Serial.println(); Serial.println();
  Serial.print("Connessione alla rete Wi-FI ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    //delay(500);
    lampeggia(1);
    Serial.print("*");
  }
  lampeggia(3);
  Serial.println();

  Serial.println("Wi-FI Connesso!");
  Serial.println("IP del modulo ESP8266 : ");
  Serial.println(WiFi.localIP());


  mqtt.subscribe(&openDoorFeed);
}

static void init_gpio()
{
  static bool done = false;
  if (done) return;

  done = true;

  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_B, HIGH);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  digitalWrite(LED_R, 1); digitalWrite(LED_G, 1); digitalWrite(LED_B, 1); delay(500);

  digitalWrite(LED_R, 1); digitalWrite(LED_G, 1); digitalWrite(LED_B, 0); delay(500);
  digitalWrite(LED_R, 1); digitalWrite(LED_G, 0); digitalWrite(LED_B, 1); delay(500);
  digitalWrite(LED_R, 0); digitalWrite(LED_G, 1); digitalWrite(LED_B, 1); delay(500);

  digitalWrite(LED_R, 1); digitalWrite(LED_G, 0); digitalWrite(LED_B, 0); delay(500);
  digitalWrite(LED_R, 0); digitalWrite(LED_G, 1); digitalWrite(LED_B, 0); delay(500);
  digitalWrite(LED_R, 0); digitalWrite(LED_G, 0); digitalWrite(LED_B, 1); delay(500);
  digitalWrite(LED_R, 0); digitalWrite(LED_G, 0); digitalWrite(LED_B, 0); delay(500);
  digitalWrite(LED_R, 1); digitalWrite(LED_G, 1); digitalWrite(LED_B, 1); delay(500);
}

//--------------------------------------------


void loop()
{
  MQTT_connect();

  init_gpio();

  Adafruit_MQTT_Subscribe *subscription;


  
  while ((subscription = mqtt.readSubscription(5000)))
  {
    if (subscription == &openDoorFeed)
    {
      char* rx = (char*)openDoorFeed.lastread;
      int rxlen = strlen(rx);

      Serial.print(F("Dati ricevuti ("));
      Serial.print(rxlen);
      Serial.print(F(") : "));
      Serial.println((char *)openDoorFeed.lastread);
      //      digitalWrite(DOOR_PIN, (int)openDoorFeed.lastread);

      do
      {
        if (!strcmp(rx, "on"))  {
          accendi_il_led();
          break;
        }
        if (!strcmp(rx, "off")) {
          spegni_il_led();
          break;
        }
        if (!strcmp(rx, "ON"))  {
          accendi_il_led();
          break;
        }
        if (!strcmp(rx, "OFF")) {
          spegni_il_led();
          break;
        }
        if (rxlen == 1) {
          char n = rx[0];
          if (n >= '0' && n <= '9') {
            lampeggia(n - '0');
            //dimmer_led_perc((n - '0') * 10);
          }
          break;
        }

      } while (0);
    }
  }

  if (! mqtt.ping()) {
    mqtt.disconnect();
  }
}
