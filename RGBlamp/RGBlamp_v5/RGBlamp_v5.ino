//***************************************************
// Externí LED WB2812B přepínané tlačítkem a přes Blynk
// Ošetřeno: manuální ovládání i když není spojení přes wi-fi
// Efekt Rainbow
//***************************************************
/*Zajímavé odkazy
  Původní program
  je příklad z https://docs.m5stack.com/en/unit/neopixel
  resp. https://github.com/m5stack/M5Stack/blob/master/examples/Unit/RGB_LED_SK6812/display_rainbow/display_rainbow.ino
  Knihovna a obecné informace
  https://docs.m5stack.com/en/unit/neopixel
  https://github.com/FastLED/FastLED/wiki/Overview
  https://github.com/FastLED/FastLED/wiki/Pixel-reference#chsv
  https://cs.wikipedia.org/wiki/HSV
  Příklady
  https://wokwi.com/arduino/libraries/FastLED/Blink
  BLYNK IoT
  https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk
  Příklad pro tlačítko https://examples.blynk.cc/?board=ESP32&shield=ESP32%20WiFi&example=GettingStarted%2FVirtualPinRead&auth=_l5IbMrtHpyQaAeHqG1wCQXiu--KYDOA&dev=Quickstart%20Device&tmpl=TMPLIXw3EzCx
  Příklad pro LED https://examples.blynk.cc/?board=ESP32&shield=ESP32%20WiFi&example=GettingStarted%2FVirtualPinWrite&auth=_l5IbMrtHpyQaAeHqG1wCQXiu--KYDOA&dev=Quickstart%20Device&tmpl=TMPLIXw3EzCx
  Příklady pro wi-fi https://randomnerdtutorials.com/esp32-useful-wi-fi-functions-arduino/
*/
#include "M5Atom.h"
#include "FastLED.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

//Údaje Blynk
// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID "TMPLQwu5D1qI"
#define BLYNK_DEVICE_NAME "HEX light"
#define BLYNK_AUTH_TOKEN "n2AK1jAdi-jdgCZ0U4V1FFFHo2ysTxrg"
// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial
char auth[] = BLYNK_AUTH_TOKEN;
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "SSID";
char pass[] = "hesloWifi";
char server[] = "blynk.cloud";
int port = 80;
BlynkTimer timer; // Creating a timer object
bool BlynkConnected;

//LED pasek/modul WS2812B
#define Neopixel_PIN 26
#define NUM_LEDS 24
CRGB leds[NUM_LEDS];
uint8_t gHue = 0;
int LED_brightness = 5;

//Integrovaná LED v M5Atom
#define Neopixel_PIN2 27
#define NUM_LEDS2 1
CRGB leds2[NUM_LEDS2];
int LED2_brightness = 20;

//Tlačítko
#define BTN_PIN 25
bool stateB = 1; //Pomocná proměnná pro přepínání stavu LED
int BTN_last = 0; //Proměnná pro uložení předešlého stavu tlačítka
int BTN_cur = 0; //Proměnná pro uložení současného stavu tlačítka
int BTN_SW_last = 0; //Proměnná v Blynku pro uložení předešlého stavu tlačítka
int BTN_SW_cur = 0; //Proměnná v Blynku pro uložení současného stavu tlačítka

void setup()
{
  M5.begin();
  Serial.begin(115200);

  pinMode(BTN_PIN, INPUT_PULLUP); //Nastavení tlačítka - vnitřní Pull-up rezistor
  // Neopixel initialization
  FastLED.addLeds<WS2812B, Neopixel_PIN2, GRB>(leds2, NUM_LEDS2).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(LED2_brightness);
  FastLED.addLeds<WS2812B, Neopixel_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(LED_brightness);
  //Na začátku se LED zhasnou
  fill_solid(leds, NUM_LEDS, CRGB(0, 0, 0)); //LED pásek zhasne
  fill_solid(leds2, NUM_LEDS2, CRGB(0, 0, 255)); //LED se rozsvítí modře a měla by po 50 ms změnit barvu
  FastLED.show();

  // Blynk.begin(auth, ssid, pass);
  //  You can also specify server:
  // Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  // Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
  // WIFI_STA viz https://randomnerdtutorials.com/esp32-useful-wi-fi-functions-arduino/
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi...");
  for (byte i = 0; (i < 10) && (WiFi.status() != WL_CONNECTED); i++)
  {
    Serial.print('.');
    delay(1000);
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println(WiFi.localIP());
    Blynk.config(auth, server, port);
  }
  else 
  {
    fill_solid(leds2, NUM_LEDS2, CRGB(255, 255, 0)); // LED se rozsvítí modře a měla by po 50 ms změnit barvu
    FastLED.show();
    WiFi.disconnect();
  }
  // Blynk.connected();

  if (WiFi.status() == WL_CONNECTED)
  {
      timer.setInterval(500L, myTimerEvent); // Staring a timer
  }
  delay(50);
}

void myTimerEvent() // This loop defines what happens when timer is triggered
{
  Blynk.virtualWrite(V1, stateB);
  Serial.print("Hodnota posuvníku V1 je: ");
  Serial.println(stateB);
}

// Tato funkce bude volána při každém posuvníku Widget
// v aplikaci Blynk zapíše hodnoty do virtuálního pinu 0
BLYNK_WRITE(V0)
{
  int pinValue = param.asInt(); // přiřazení příchozí hodnoty z pinu V1 do proměnné
  BTN_SW_cur = pinValue;
  // Můžete také použít:
  // Řetězec i = param.asStr();
  // double d = param.asDouble();
  Serial.print("Hodnota posuvníku V0 je: ");
  Serial.println(pinValue);
}

void loop() {
  BTN_cur = digitalRead(BTN_PIN); //Čtení stavu tlačítka

  if (WiFi.status() == WL_CONNECTED)
  {
    Blynk.run();
    timer.run(); // runs the timer in the loop
  }
  

  //Pokud došlo k uvolnění hw tlačítka (was released),
  //nebo k uvolnění sw tlačítka, změní se stateB
  if (BTN_cur && !BTN_last || BTN_SW_cur && !BTN_SW_last) {
    stateB = !stateB;
  }
  if (stateB) {
    //Funkce je popsána v https://fastled.io/docs/3.1/group___colorutils.html
    fill_rainbow(leds, NUM_LEDS, gHue, 7);  // rainbow effect
    fill_solid(leds2, NUM_LEDS2, CRGB(0, 255, 0));
    FastLED.show();  // must be executed for neopixel becoming effective
    EVERY_N_MILLISECONDS(20) {
      gHue++;
    }
  }
  else {
    //Funkce je popsána v https://fastled.io/docs/3.1/group___colorutils.html
    fill_solid(leds, NUM_LEDS, CRGB(0, 0, 0));
    fill_solid(leds2, NUM_LEDS2, CRGB(255, 0, 0));
    FastLED.show();
  }
  BTN_last = BTN_cur;
  BTN_SW_last = BTN_SW_cur;
  M5.update();
}

