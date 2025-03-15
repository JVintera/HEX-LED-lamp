#include "M5Atom.h"
#include "FastLED.h"

// LED pasek/modul WS2812B
#define Neopixel_PIN 26
#define NUM_LEDS 24
CRGB leds[NUM_LEDS];
uint8_t gHue = 0;
int LED_brightness = 5;

// Integrovaná LED v M5Atom
#define Neopixel_PIN2 27
#define NUM_LEDS2 1
CRGB leds2[NUM_LEDS2];
int LED2_brightness = 20;

// Tlačítko
#define BTN_PIN 25
bool stateB = 1;     // Pomocná proměnná pro přepínání stavu LED
int BTN_last = 0;    // Proměnná pro uložení předešlého stavu tlačítka
int BTN_cur = 0;     // Proměnná pro uložení současného stavu tlačítka
//int BTN_SW_last = 0; // Proměnná v Blynku pro uložení předešlého stavu tlačítka
//int BTN_SW_cur = 0;  // Proměnná v Blynku pro uložení současného stavu tlačítka

#include <WiFi.h>
#include <WiFiClient.h>

//***************************************************
// Vytvořte soubor secure.h pro ukládání citlivých dat
// !!! z bezpečnostních důvodů tento soubor přidejte v Gitu do .gitignore, aby se nezálohoval
//
/* VYPLNTE NAZEV WIFI SITE A HESLO  */
// #define SECRET_SSID "SSID"
// #define SECRET_OPTIONAL_PASS "Ro2uJjej"
/* Device ID and Secret Key from Arduino IoT cloud */
// #define SECRET_DEVICE_LOGIN_NAME "f1941273-afe8-4ad8-98e8-fd96fc37f906"
// #define SECRET_DEVICE_KEY "3@tGlYzqWA#vUQM1fHvkjPEib"
//***************************************************
#include "secure.h"

char ssid[] = WIFI_SSID1;
char pass[] = WIFI_PASS1;
//char server[] = "blynk.cloud";
int port = 80;

void setup()
{
    M5.begin();
    Serial.begin(115200);

    pinMode(BTN_PIN, INPUT_PULLUP); // Nastavení tlačítka - vnitřní Pull-up rezistor
    // Neopixel initialization
    FastLED.addLeds<WS2812B, Neopixel_PIN2, GRB>(leds2, NUM_LEDS2).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(LED2_brightness);
    FastLED.addLeds<WS2812B, Neopixel_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(LED_brightness);
    // Na začátku se LED zhasnou
    fill_solid(leds, NUM_LEDS, CRGB(0, 0, 0));     // LED pásek zhasne
    fill_solid(leds2, NUM_LEDS2, CRGB(0, 0, 255)); // LED se rozsvítí modře a měla by po 50 ms změnit barvu
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
