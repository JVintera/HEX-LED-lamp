//***************************************************
//Externí LED WB2812B přepínané tlačítkem a přes Blynk
//Efekt Rainbow
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
//char ssid[] = "Zima";
//char pass[] = "Ro2uJjej";
//char ssid[] = "Zima24_guest";
//char pass[] = "Zimikovi812/";
char ssid[] = "SPSELIT-host";
char pass[] = "SPSg21esT";
BlynkTimer timer; // Creating a timer object


//LED pasek/modul WS2812B
#define Neopixel_PIN 26
#define NUM_LEDS 24
CRGB leds[NUM_LEDS];
uint8_t gHue = 0;
int LED_brightness = 100;

//Integrovaná LED v M5Atom
#define Neopixel_PIN2 27
#define NUM_LEDS2 1
CRGB leds2[NUM_LEDS2];

//Tlačítko
#define BTN_PIN 25
bool stateB = 1; //Pomocná proměnná pro přepínání stavu LED
int BTN_last = 0; //Proměnná pro uložení předešlého stavu tlačítka
int BTN_cur = 0; //Proměnná pro uložení současného stavu tlačítka
int BTN_SW_last = 0; //Proměnná v Blynku pro uložení předešlého stavu tlačítka
int BTN_SW_cur = 0; //Proměnná v Blynku pro uložení současného stavu tlačítka


void setup() {
  M5.begin();
  Serial.begin(115200);

  pinMode(BTN_PIN, INPUT_PULLUP); //Nastavení tlačítka - vnitřní Pull-up rezistor
  // Neopixel initialization
  FastLED.addLeds<WS2812B, Neopixel_PIN2, GRB>(leds2, NUM_LEDS2).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2812B, Neopixel_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(LED_brightness);
  //Na začátku se LED zhasnou
  fill_solid(leds, NUM_LEDS, CRGB(0, 0, 0)); //LED pásek zhasne
  fill_solid(leds2, NUM_LEDS2, CRGB(0, 0, 255)); //LED se rozsvítí modře a měla by po 50 ms změnit barvu
  FastLED.show();


  //Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
  timer.setInterval(500L, myTimerEvent); //Staring a timer
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

  Blynk.run();
  timer.run(); // runs the timer in the loop

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



//***************************************************
//Externí LED WB2812B přepínané tlačítkem
//Efekt Rainbow
//***************************************************
///*Zajímavé odkazy
//  Původní program
//  je příklad z https://docs.m5stack.com/en/unit/neopixel
//  resp. https://github.com/m5stack/M5Stack/blob/master/examples/Unit/RGB_LED_SK6812/display_rainbow/display_rainbow.ino
//  Knihovna a obecné informace
//  https://docs.m5stack.com/en/unit/neopixel
//  https://github.com/FastLED/FastLED/wiki/Overview
//  https://github.com/FastLED/FastLED/wiki/Pixel-reference#chsv
//  https://cs.wikipedia.org/wiki/HSV
//  Příklady
//  https://wokwi.com/arduino/libraries/FastLED/Blink
//*/
//#include "M5Atom.h"
//#include "FastLED.h"
//
////LED pasek/modul WS2812B
//#define Neopixel_PIN 26
//#define NUM_LEDS 24
//CRGB leds[NUM_LEDS];
//uint8_t gHue = 0;
//int LED_brightness = 20;
//
////Integrovaná LED v M5Atom
//#define Neopixel_PIN2 27
//#define NUM_LEDS2 1
//CRGB leds2[NUM_LEDS2];
//int LED_brightness2 = 5;
//
////Tlačítko
//#define BTN_PIN 25
//int BTN_last = 0; //Proměnná ro uložení předešlého stavu tlačítka
//int BTN_cur = 0; //Proměnná ro uložení současného stavu tlačítka
//
//void setup() {
//  M5.begin();
//  pinMode(BTN_PIN, INPUT_PULLUP); //Nastavení tlačítka - vnitřní Pull-up rezistor
//  // Neopixel initialization
//  FastLED.addLeds<WS2812B, Neopixel_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
//  FastLED.setBrightness(LED_brightness);
//  FastLED.addLeds<WS2812B, Neopixel_PIN2, GRB>(leds2, NUM_LEDS2).setCorrection(TypicalLEDStrip);
//  FastLED.setBrightness(LED_brightness2);
//  //Na začátku se LED zhasnou
//  fill_solid(leds, NUM_LEDS, CRGB(0, 0, 0)); //LED pásek zhasne
//  fill_solid(leds2, NUM_LEDS2, CRGB(0, 0, 255)); //LED se rozsvítí modře a měla by po 50 ms změnit barvu
//  FastLED.show();
//  delay(50);
//}
//
//void loop() {
//  static bool stateB = 1; //Pomocná proměnná pro přepínání stavu LED
//  BTN_cur = digitalRead(BTN_PIN); //Čtení stavu tlačítka
//
//  //Pokud došlo k uvolnění tlačítka (was released)
//  if (BTN_cur && !BTN_last) {
//    stateB = !stateB;
//  }
//  if (stateB) {
//    //Funkce je popsána v https://fastled.io/docs/3.1/group___colorutils.html
//    fill_rainbow(leds, NUM_LEDS, gHue, 7);  // rainbow effect
//    fill_solid(leds2, NUM_LEDS2, CRGB(0, 255, 0));
//    FastLED.show();  // must be executed for neopixel becoming effective
//    EVERY_N_MILLISECONDS(20) {
//      gHue++;
//    }
//  }
//  else {
//    //Funkce je popsána v https://fastled.io/docs/3.1/group___colorutils.html
//    fill_solid(leds, NUM_LEDS, CRGB(0, 0, 0));
//    fill_solid(leds2, NUM_LEDS2, CRGB(255, 0, 0));
//    FastLED.show();
//  }
//  BTN_last = BTN_cur;
//  M5.update();
//}


//***************************************************
//Stmívání LED jedním tlačítkem
//***************************************************
/*Funkce pro práci s LED a tlačítkem jsou popsány v
  https://docs.m5stack.com/en/api/atom/system
  https://docs.m5stack.com/en/api/atom/button
  https://docs.m5stack.com/en/api/atom/led_display
*/
//#include "M5Atom.h"
//
//#define RGB_LED 0
//
//void setup() {
//  M5.begin(true, false, true);
//  Serial.begin(9600);
//  delay(50);
//  //Probliknutí LED (zeleně) jako signalizace, že LED funguje
//  M5.dis.setBrightness(255);
//  M5.dis.drawpix(RGB_LED, 0x00FF00);
//  delay(50);
//  M5.dis.drawpix(RGB_LED, 0x000000);
//}
//
//void loop() {
//  static uint8_t brightLED = 0;
//  static uint8_t brightB = 0;
//  static bool stateB = 0;
//  static byte modeB = 0;
////Proměnné help (mohou se jmenovat jakkoliv jinak) složí pro vytváření tzv. pastí,
////kdy do nich uložíte výsledek operace a následně vypíšete na seriovém monitoru.
////Ve finálním programu by neměly být, slouží čistě pro účely ladění.
////  int help1 = 0;
////  int help2 = 0;
////  int help3 = 0;
////  int help4 = 0;
////Vypsání hodnot proměnných
//  //  Serial.print("stateB ");
//  //  Serial.println(stateB);
//  //  Serial.print("brightLED ");
//  //  Serial.println(brightLED);
//
////Testování dlouhého stisknutí tlačítka (1000 ms)
////Past na detekci dlouhého stisku tlačítka
////  if (help2 = M5.Btn.pressedFor(1000)) {
////    Serial.print("help2 ");
////    Serial.println(help2);
////Pokud se použije past, následující řádek se zakomentuje.
//  if (M5.Btn.pressedFor(1000)) {
//    if (stateB == 1) {
//      modeB = 1;
//      brightLED += 1;
//      if (brightLED > 254) brightLED = 254;
//    }
//    else {
//      modeB = 2;
//      brightLED -= 1;
//      if (brightLED < 1) brightLED = 1;
//    }
//  }
////Testování krátkéhho stisknutí tlačítka (30 ms)
////Past na detekci krátkého stisknutí tlačítka
////  else if (help1 = M5.Btn.pressedFor(30)) {
////    Serial.print("help1 ");
////    Serial.println(help1);
////Pokud se použije past, následující řádek se zakomentuje.
//  else if (M5.Btn.pressedFor(30)) {
//    if (stateB == 1) {
//      modeB = 3;
//    }
//    else {
//      modeB = 4;
//    }
//  }
//
////Pokud bylo tlačítko puštěno, vyhodnotí se a provede krátký
////stisk tlačítka
//  if (M5.Btn.wasReleased()) {
////Past na detekci, zda skutečně došlo k uvolnění tlačítka
////    Serial.println("**********");
//    stateB = !stateB;
//    switch (modeB) {
//      case 1:
//
//        break;
//      case 2:
//
//        break;
//      case 3:
//        brightLED = 255;
//        brightB = 255;
//        break;
//      case 4:
//        brightLED = 0;
//        brightB = 0;
//        break;
//    }
//  }
//
////Na konci se nastaví jas, rozsvítí LED a přečtou vstupy
//  M5.dis.setBrightness(brightLED);
//  M5.dis.drawpix(RGB_LED, 0xff0000);
//  M5.update();
////Určuje rychlost stmívání
//  delay(50);
//}



//***************************************************
//Zapnutí a vypnutí jedním tlačítkem
//***************************************************
/*Funkce pro práci s LED a tlačítkem jsou popsány v
  https://docs.m5stack.com/en/api/atom/system
  https://docs.m5stack.com/en/api/atom/button
  https://docs.m5stack.com/en/api/atom/led_display
*/
//#include "M5Atom.h"
//
//#define RGB_LED 0
//
//void setup() {
//  M5.begin(true, false, true);
//  delay(50);
//  //Na začátku problikne LED pro ověření, že funguje
//  M5.dis.drawpix(RGB_LED, 0x00FF00);
//  delay(50);
//  M5.dis.drawpix(RGB_LED, 0x000000);
//}
//
//void loop() {
//  static bool stateB = 0;
//  //Pokud bylo stisknuto tlačítko, změní se stav LED
//  if(M5.Btn.wasPressed()) {
//    stateB = !stateB;
//  }
//  if(stateB == 0) {
//    M5.dis.drawpix(RGB_LED, 0x000000);
//  }
//  else {
//    M5.dis.drawpix(RGB_LED, 0x0000ff);
//  }
//  M5.update();
//}
