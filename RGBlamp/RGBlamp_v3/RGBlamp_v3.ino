//***************************************************
//Externí LED WB2812B přepínané tlačítkem na pinu G25
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
*/
#include "M5Atom.h"
#include "FastLED.h"

//LED pasek/modul WS2812B
#define Neopixel_PIN 26
#define NUM_LEDS 24
CRGB leds[NUM_LEDS];
uint8_t gHue = 0;
int LED_brightness = 20;

//Integrovaná LED v M5Atom
#define Neopixel_PIN2 27
#define NUM_LEDS2 1
CRGB leds2[NUM_LEDS2];
int LED_brightness2 = 5;

//Tlačítko
#define BTN_PIN 25
int BTN_last = 0; //Proměnná ro uložení předešlého stavu tlačítka
int BTN_cur = 0; //Proměnná ro uložení současného stavu tlačítka

void setup() {
 M5.begin();
 pinMode(BTN_PIN, INPUT_PULLUP); //Nastavení tlačítka - vnitřní Pull-up rezistor
 // Neopixel initialization
 FastLED.addLeds<WS2812B, Neopixel_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
 FastLED.setBrightness(LED_brightness);
 FastLED.addLeds<WS2812B, Neopixel_PIN2, GRB>(leds2, NUM_LEDS2).setCorrection(TypicalLEDStrip);
 FastLED.setBrightness(LED_brightness2);
 //Na začátku se LED zhasnou
 fill_solid(leds, NUM_LEDS, CRGB(0, 0, 0)); //LED pásek zhasne
 fill_solid(leds2, NUM_LEDS2, CRGB(0, 0, 255)); //LED se rozsvítí modře a měla by po 50 ms změnit barvu
 FastLED.show();
 delay(50);
}

void loop() {
 static bool stateB = 1; //Pomocná proměnná pro přepínání stavu LED
 BTN_cur = digitalRead(BTN_PIN); //Čtení stavu tlačítka

 //Pokud došlo k uvolnění tlačítka (was released)
 if (BTN_cur && !BTN_last) {
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
 M5.update();
}

