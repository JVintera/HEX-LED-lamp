// Desku připojíte jako Deneyap Mini (možná i jiná deska)
// Aby se dala programovat, je třeba držet boot a resetovat čip
// Jak naprogramovat ESP32 S2F
// https://youtu.be/Ull2gCKb1Yw?feature=shared

// Projekt podle
// https://randomnerdtutorials.com/esp32-esp8266-web-server-physical-button/

//***************************************************
// Vytvořte soubor wifiConfig.h
// a vyplnte v něm následující údaje
// Device ID a Secret Key vygeneruje Arduino cloud

/* VYPLNTE NAZEV WIFI SITE A HESLO  */
// #define SECRET_SSID "SSID"
// #define SECRET_OPTIONAL_PASS "password"
//
/* Nastavení sítě */
// IPAddress local_IP(192, 168, 30, 30);
// IPAddress gateway(192, 168, 1, 1);
// IPAddress subnet(255, 255, 255, 0);
// //IPAddress primaryDNS(8, 8, 8, 8);   // optional
// //IPAddress secondaryDNS(8, 8, 4, 4); // optional
//***************************************************

//knihovny
#include "wifiConfig.h"
#include <WiFi.h>
#include <AsyncTCP.h>           // AsyncTCP by dvarrel, v. 1.1.4 - https://github.com/me-no-dev/AsyncTCP
#include <ESPAsyncWebServer.h>  // ESPAsyncWebServer by lacamera - https://github.com/me-no-dev/ESPAsyncWebServer

// parametry k připojení k síti
const char *ssid = SECRET_SSID;               // název sítě
const char *password = SECRET_OPTIONAL_PASS;  // heslo k síti

const char *PARAM_INPUT_1 = "state";

const int output = 1;     //nastevení pinu pro ovládání tranzitoru
const int buttonPin = 2;  //nastavení vstupního pinu tlačítka


int ledState = LOW;         //nastavení první hodnoty LED
int buttonState;            //definování vstupního pinu tlačítka jako dvouhodnotovou proměnnou
int lastButtonState = LOW;  //pomocná porměnná pro fungování

//doba stisku tlačítka pro zaregistrování stisku
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

AsyncWebServer server(80);  //zavedení serveru na port 80

//zde začíná část HTML programu pro webové rozhraní

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
    input:checked+.slider {background-color: #2196F3}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>
<body>
  <h2>ESP Web Server</h2>
  %BUTTONPLACEHOLDER%
<script>function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?state=1", true); }
  else { xhr.open("GET", "/update?state=0", true); }
  xhr.send();
}
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var inputChecked;
      var outputStateM;
      if( this.responseText == 1){ 
        inputChecked = true;
        outputStateM = "On";
      }
      else { 
        inputChecked = false;
        outputStateM = "Off";
      }
      document.getElementById("output").checked = inputChecked;
      document.getElementById("outputState").innerHTML = outputStateM;
    }
  };
  xhttp.open("GET", "/state", true);
  xhttp.send();
}, 1000 ) ;
</script>
</body>
</html>
)rawliteral";

// Funkce pro nahrazení placeholderu v HTML kódu - změní vzhled checkboxu
// Replaces placeholder with button section in your web page
String processor(const String &var) {
  //Serial.println(var);
  if (var == "BUTTONPLACEHOLDER") {
    String buttons = "";
    String outputStateValue = outputState();
    buttons += "<h4>Output - GPIO 2 - State <span id=\"outputState\"></span></h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"output\" " + outputStateValue + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  return String();
}

//Nastavení stavu checkboxu
String outputState() {
  if (digitalRead(output)) {
    return "checked";
  } else {
    return "";
  }
  return "";
}

void setup() {
  Serial.begin(115200);

  pinMode(output, OUTPUT);    //nastavení pinu pro výstup jako výstup
  digitalWrite(output, LOW);  //naství hodnotu 0 na výstupu pro případ výpadek elektriky
  pinMode(buttonPin, INPUT_PULLUP);  //nastavení pinu tlačítka jako vstup

  byte countConnect = 0;
  Serial.println("Connecting to WiFi");
  
  //Nastavení pevné IP adresy
  //if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
  if (!WiFi.config(local_IP, gateway, subnet))
  {
    Serial.println("STA Failed to configure");
  }
  
  WiFi.begin(ssid, password);  //zahájení wifi komunikace
  while (WiFi.status() != WL_CONNECTED && countConnect < 10)
  {
    delay(500);
    Serial.println(".");
    countConnect++;
  }

  Serial.println(WiFi.localIP()); //vypsání IP adresy

// Nastavení serveru
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor);
  });

// Nastavení serveru pro ovládání LED přes web
  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
      digitalWrite(output, inputMessage.toInt());
      ledState = !ledState;
    } else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });

// Nastavení serveru pro získání stavu LED
  server.on("/state", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(digitalRead(output)).c_str());
  });

  // Spuštění serveru
  server.begin();
}

void loop() {

  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {

    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {


    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == HIGH) {
        ledState = !ledState;
      }
    }
  }

  digitalWrite(output, ledState);

  lastButtonState = reading;
}
