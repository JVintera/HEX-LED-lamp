//***************************************************
// Stmívání LED jedním tlačítkem
// Tlačítko i LED na M5ATOM
//***************************************************
/*Funkce pro práci s LED a tlačítkem jsou popsány v
  https://docs.m5stack.com/en/api/atom/system
  https://docs.m5stack.com/en/api/atom/button
  https://docs.m5stack.com/en/api/atom/led_display
*/
#include "M5Atom.h"

#define RGB_LED 0

void setup() {
 M5.begin(true, false, true);
 Serial.begin(9600);
 delay(50);
 //Probliknutí LED (zeleně) jako signalizace, že LED funguje
 M5.dis.setBrightness(255);
 M5.dis.drawpix(RGB_LED, 0x00FF00);
 delay(50);
 M5.dis.drawpix(RGB_LED, 0x000000);
}

void loop() {
 static uint8_t brightLED = 0;
 static uint8_t brightB = 0;
 static bool stateB = 0;
 static byte modeB = 0;
//Proměnné help (mohou se jmenovat jakkoliv jinak) složí pro vytváření tzv. pastí,
//kdy do nich uložíte výsledek operace a následně vypíšete na seriovém monitoru.
//Ve finálním programu by neměly být, slouží čistě pro účely ladění.
//  int help1 = 0;
//  int help2 = 0;
//  int help3 = 0;
//  int help4 = 0;
//Vypsání hodnot proměnných
 //  Serial.print("stateB ");
 //  Serial.println(stateB);
 //  Serial.print("brightLED ");
 //  Serial.println(brightLED);

//Testování dlouhého stisknutí tlačítka (1000 ms)
//Past na detekci dlouhého stisku tlačítka
//  if (help2 = M5.Btn.pressedFor(1000)) {
//    Serial.print("help2 ");
//    Serial.println(help2);
//Pokud se použije past, následující řádek se zakomentuje.
 if (M5.Btn.pressedFor(1000)) {
   if (stateB == 1) {
     modeB = 1;
     brightLED += 1;
     if (brightLED > 254) brightLED = 254;
   }
   else {
     modeB = 2;
     brightLED -= 1;
     if (brightLED < 1) brightLED = 1;
   }
 }
//Testování krátkéhho stisknutí tlačítka (30 ms)
//Past na detekci krátkého stisknutí tlačítka
//  else if (help1 = M5.Btn.pressedFor(30)) {
//    Serial.print("help1 ");
//    Serial.println(help1);
//Pokud se použije past, následující řádek se zakomentuje.
 else if (M5.Btn.pressedFor(30)) {
   if (stateB == 1) {
     modeB = 3;
   }
   else {
     modeB = 4;
   }
 }

//Pokud bylo tlačítko puštěno, vyhodnotí se a provede krátký
//stisk tlačítka
 if (M5.Btn.wasReleased()) {
//Past na detekci, zda skutečně došlo k uvolnění tlačítka
//    Serial.println("**********");
   stateB = !stateB;
   switch (modeB) {
     case 1:

       break;
     case 2:

       break;
     case 3:
       brightLED = 255;
       brightB = 255;
       break;
     case 4:
       brightLED = 0;
       brightB = 0;
       break;
   }
 }

//Na konci se nastaví jas, rozsvítí LED a přečtou vstupy
 M5.dis.setBrightness(brightLED);
 M5.dis.drawpix(RGB_LED, 0xff0000);
 M5.update();
//Určuje rychlost stmívání
 delay(50);
}

