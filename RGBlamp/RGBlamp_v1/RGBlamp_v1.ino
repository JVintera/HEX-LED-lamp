//***************************************************
//Zapnutí a vypnutí jedním tlačítkem
//Tlačítko i LED na M5ATOM
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
 delay(50);
 //Na začátku problikne LED pro ověření, že funguje
 M5.dis.drawpix(RGB_LED, 0x00FF00);
 delay(50);
 M5.dis.drawpix(RGB_LED, 0x000000);
}

void loop() {
 static bool stateB = 0;
 //Pokud bylo stisknuto tlačítko, změní se stav LED
 if(M5.Btn.wasPressed()) {
   stateB = !stateB;
 }
 if(stateB == 0) {
   M5.dis.drawpix(RGB_LED, 0x000000);
 }
 else {
   M5.dis.drawpix(RGB_LED, 0x0000ff);
 }
 M5.update();
}
