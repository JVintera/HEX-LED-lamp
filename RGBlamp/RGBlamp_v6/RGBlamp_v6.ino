// Opravit rozsvěcení LED
// Když stisknu HW tlačítko, změní se stav sw tlačítka bez ohledu na wifi?
// Jak si to testuje wifi?
// Popsat v nápovědě soubor wifiConfig.h



//***************************************************
// Externí LED WB2812B přepínané tlačítkem a přes Blynk
// Ošetřeno: manuální ovládání i když není spojení přes wi-fi
// Efekt Rainbow
//***************************************************
/*Zajímavé odkazy
  Arduino IoT cloud https://youtu.be/rcCxGcRwCVk?feature=shared
      https://docs.arduino.cc/arduino-cloud/guides/overview/
  Návod https://youtu.be/rcCxGcRwCVk?feature=shared

*/


#include "thingProperties.h"

void setup()
{
  // Initialize serial and wait for port to open:
  Serial.begin(115200);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
//  delay(1500);

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
 */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  
}

void loop()
{
  ArduinoCloud.update();

  BTN_cur = digitalRead(BTN_PIN); // Čtení stavu tlačítka

  // Pokud došlo k uvolnění hw tlačítka (was released),
  // nebo k přepnutí sw tlačítka, změní se stateB
  if (BTN_cur && !BTN_last)
  {
    stateB = !stateB;
  }
  if (BTN_SW_cur && !BTN_SW_last)
  {
    stateB = 1;
  }
  if (BTN_SW_last && !BTN_SW_cur)
  {
    stateB = 0;
  }
  if (stateB && !BTN_SW_RainBow && !BTN_SW_StarrLight && !BTN_SW_PoliceBeacon && !BTN_SW_Beacon)
  {
    // Funkce je popsána v https://fastled.io/docs/3.1/group___colorutils.html
    //fill_solid(leds, NUM_LEDS, CRGB(255, 0, 0));
    fill_solid(leds, NUM_LEDS, CHSV(ledsHue, ledsSaturation, ledsValue));
    fill_solid(leds2, NUM_LEDS2, CRGB(0, 255, 0));
    FastLED.show();
  }
  else
  {
    // Funkce je popsána v https://fastled.io/docs/3.1/group___colorutils.html
    fill_solid(leds, NUM_LEDS, CHSV(ledsHue, ledsSaturation, 0));
    fill_solid(leds2, NUM_LEDS2, CRGB(255, 0, 0));
    FastLED.show();
  }
  //Režim Rainbow
  if (!stateB && BTN_SW_RainBow && !BTN_SW_StarrLight && !BTN_SW_PoliceBeacon && !BTN_SW_Beacon)
  {
    // Funkce je popsána v https://fastled.io/docs/3.1/group___colorutils.html
    fill_rainbow(leds, NUM_LEDS, gHue, 7); // rainbow effect
    fill_solid(leds2, NUM_LEDS2, CRGB(0, 255, 0));
    FastLED.show(); // must be executed for neopixel becoming effective
    EVERY_N_MILLISECONDS(10)
    {
      gHue++;
    }
  }
  // Režim Star light
  if (!stateB && !BTN_SW_RainBow && BTN_SW_StarrLight && !BTN_SW_PoliceBeacon && !BTN_SW_Beacon)
  {
      for (int i = 0; i < NUM_LEDS; i++)
      {
        leds[i] = CRGB::Black;
      }

      int numStars = random(1, 6); // Počet hvězd, které se rozblikají najednou

      for (int i = 0; i < numStars; i++)
      {
        int pos = random(NUM_LEDS); // Náhodná pozice pro hvězdu
        leds[pos] = CRGB::Yellow;   // Nastaví LED na žlutou
      }
      FastLED.show();
      delay(100);
  }
  // Režim Police beacon
  if (!stateB && !BTN_SW_RainBow && !BTN_SW_StarrLight && BTN_SW_PoliceBeacon && !BTN_SW_Beacon)
  {
    for (int blink = 0; blink < 3; blink++)
    {
      for (int i = 0; i < 12; i++)
      {
        leds[i] = CRGB::Blue; // Nastaví LED na modrou
      }
      FastLED.show();
      delay(50); // Zpoždění mezi jednotlivými blikáními

      for (int i = 0; i < 12; i++)
      {
        leds[i] = CRGB::Black; // Vypne LED
      }
      FastLED.show();
      delay(50); // Zpoždění mezi jednotlivými blikáními
    }

    // Blikání červených LED
    for (int blink = 0; blink < 3; blink++)
    {
      for (int i = 12; i < NUM_LEDS; i++)
      {
        leds[i] = CRGB::Red; // Nastaví LED na červenou
      }
      FastLED.show();
      delay(50); // Zpoždění mezi jednotlivými blikáními

      for (int i = 12; i < NUM_LEDS; i++)
      {
        leds[i] = CRGB::Black; // Vypne LED
      }
      FastLED.show();
      delay(50); // Zpoždění mezi jednotlivými blikáními
    }
  }

  if (!stateB && !BTN_SW_RainBow && !BTN_SW_StarrLight && !BTN_SW_PoliceBeacon && BTN_SW_Beacon)
  {
    static int state = 0;

    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = CRGB::Black; // Vypne všechny LED
    }

    if (state < NUM_LEDS)
    { // Pětice svítících LED
      for (int i = state; i < state + 12; i++)
      {
        leds[i % NUM_LEDS] = CRGB::Orange; // Nastaví LED na oranžovou
      }
    }

    FastLED.show();
    delay(20); // Zpoždění mezi jednotlivými rotacemi

    state = (state + 1) % NUM_LEDS;
  }

  BTN_last = BTN_cur;
  BTN_SW_last = BTN_SW_cur;
  M5.update();
}

/*
  Since Led is READ_WRITE variable, onLedChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onLedChange()
{
  // Proměnná {"bri" : "100", "hue" : "19.319", "sat" : "100", "swi" : "true"}
  BTN_SW_cur = led.getValue().swi;

  ledsHue = map(led.getValue().hue, 0, 360, 0, 255);
  //Serial.println(led.getValue().hue);
  //Serial.println(ledsHue);
  ledsSaturation = map(led.getValue().sat, 0, 100, 0, 255);
  //Serial.println(led.getValue().sat);
  //Serial.println(ledsSaturation);
  ledsValue = map(led.getValue().bri, 0, 100, 0, 255);
  //Serial.println(led.getValue().bri);
  //Serial.println(ledsValue);
}

/*
  Since RainBow is READ_WRITE variable, onRainBowChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onRainBowChange()
{
  BTN_SW_RainBow = rainBow;
  Serial.println(rainBow);
}

/*
  Since StarrLight is READ_WRITE variable, onStarrLightChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onStarrLightChange()
{
  BTN_SW_StarrLight = starrLight;
  Serial.println(starrLight);
}

/*
  Since PoliceBeacon is READ_WRITE variable, onPoliceBeaconChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onPoliceBeaconChange()
{
  BTN_SW_PoliceBeacon = policeBeacon;
  Serial.println(policeBeacon);
}

/*
  Since Beacon is READ_WRITE variable, onBeaconChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onBeaconChange()
{
  BTN_SW_Beacon = beacon;
  Serial.println(beacon);
}

