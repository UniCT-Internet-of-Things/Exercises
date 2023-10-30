#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>


#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_RESET -1

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define LED_RED 26
#define LED_YELLOW 27
#define LED_GREEN 14
#define LED_BLUE 12
#define LED_WHITE 13

#define BTN_1 33
#define BTN_2 25

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
bool withDisplay = false;
void initDisplay(){
  //Wire.begin(OLED_SDA, OLED_SCL);
  Serial.println("Init display...");

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
  } else {
    withDisplay = true;
    display.clearDisplay();
    display.display();
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0,0);
    display.println("OK DISPLAY");
    display.display();
    delay(1000);
    display.clearDisplay();
    display.setCursor(0,0);
    display.display();
  }
}

int wifiNetworks = 0;

void startWiFiScan(){
  WiFi.mode(WIFI_STA);
  digitalWrite(LED_YELLOW, HIGH);
  wifiNetworks = WiFi.scanNetworks();
}

void connectToWiFi(const char* ssid, const char* password){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print('.');
      delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void startAccessPointWifi(const char* ssid, const char* password){
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
}


//buttons events
bool btn1Interrupt = false;
void onBTN_1Change(){
  btn1Interrupt = true;
}

bool btn2Interrupt = false;
void onBTN_2Change(){
  btn2Interrupt = true;
}

void onBTN1_Released(){
  Serial.println("Button 1 released");
}
void onBTN1_Pressed(){
  Serial.println("Button 1 pressed");
}

void onBTN2_Released(){
  Serial.println("Button 2 released");
  startWiFiScan();
}
void onBTN2_Pressed(){
  Serial.println("Button 2 pressed");
}

void manageInput(){
  if(btn1Interrupt){
    btn1Interrupt = false;
    if(digitalRead(BTN_1) == HIGH){
      onBTN1_Released();
    } else {
      onBTN1_Pressed();
    }

  }

  if(btn2Interrupt){
    btn2Interrupt = false;
    if(digitalRead(BTN_2) == HIGH){
      onBTN2_Released();
    } else {
      onBTN2_Pressed();
    }

  }
}

/*
* WiFi Events

0  ARDUINO_EVENT_WIFI_READY                     < ESP32 WiFi ready
1  ARDUINO_EVENT_WIFI_SCAN_DONE                 < ESP32 finish scanning AP
2  ARDUINO_EVENT_WIFI_STA_START                 < ESP32 station start
3  ARDUINO_EVENT_WIFI_STA_STOP                  < ESP32 station stop
4  ARDUINO_EVENT_WIFI_STA_CONNECTED             < ESP32 station connected to AP
5  ARDUINO_EVENT_WIFI_STA_DISCONNECTED          < ESP32 station disconnected from AP
6  ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE       < the auth mode of AP connected by ESP32 station changed
7  ARDUINO_EVENT_WIFI_STA_GOT_IP                < ESP32 station got IP from connected AP
8  ARDUINO_EVENT_WIFI_STA_LOST_IP               < ESP32 station lost IP and the IP is reset to 0
9  ARDUINO_EVENT_WPS_ER_SUCCESS                 < ESP32 station wps succeeds in enrollee mode
10 ARDUINO_EVENT_WPS_ER_FAILED                  < ESP32 station wps fails in enrollee mode
11 ARDUINO_EVENT_WPS_ER_TIMEOUT                 < ESP32 station wps timeout in enrollee mode
12 ARDUINO_EVENT_WPS_ER_PIN                     < ESP32 station wps pin code in enrollee mode
13 ARDUINO_EVENT_WIFI_AP_START                  < ESP32 soft-AP start
14 ARDUINO_EVENT_WIFI_AP_STOP                   < ESP32 soft-AP stop
15 ARDUINO_EVENT_WIFI_AP_STACONNECTED           < a station connected to ESP32 soft-AP
16 ARDUINO_EVENT_WIFI_AP_STADISCONNECTED        < a station disconnected from ESP32 soft-AP
17 ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED          < ESP32 soft-AP assign an IP to a connected station
18 ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED         < Receive probe request packet in soft-AP interface
19 ARDUINO_EVENT_WIFI_AP_GOT_IP6                < ESP32 ap interface v6IP addr is preferred
19 ARDUINO_EVENT_WIFI_STA_GOT_IP6               < ESP32 station interface v6IP addr is preferred

*/


void onWifiScanEnded(WiFiEvent_t event, WiFiEventInfo_t info){
  digitalWrite(LED_YELLOW, LOW);
  for(int i = 0; i < wifiNetworks; i++){
    Serial.println(WiFi.SSID(i)+" | RSSI :"+WiFi.RSSI(i)+ " | Security: "+WiFi.encryptionType(i));
  }
}

void onWifiStationStart(WiFiEvent_t event, WiFiEventInfo_t info){
  digitalWrite(LED_RED, HIGH);
}

void onWifiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info){
  digitalWrite(LED_WHITE, HIGH);
}

void onWifiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  digitalWrite(LED_WHITE, LOW);
}

void onWifiAPStart(WiFiEvent_t event, WiFiEventInfo_t info){
  digitalWrite(LED_BLUE, HIGH);
}

int stationsConnected = 0;

void onWifiAPStationConnected(WiFiEvent_t event, WiFiEventInfo_t info){
  stationsConnected++;
  Serial.println("Qualcuno si è connesso");
  digitalWrite(LED_WHITE, HIGH);
  if(withDisplay){
    display.setCursor(0,0);
    display.clearDisplay();
    display.println("Access Point Started");
    display.println("corso-iot");
    display.println("Connessi:");
    display.println(stationsConnected);
    display.display();
  }
}

void onWifiAPStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  stationsConnected--;
  Serial.println("Qualcuno si è disconnesso");
  if(stationsConnected == 0){
    digitalWrite(LED_WHITE, LOW);
  }
  if(withDisplay){
    display.setCursor(0,0);
    display.clearDisplay();
    display.println("Access Point Started");
    display.println("corso-iot");
    display.println("Connessi:");
    display.println(stationsConnected);
    display.display();
  }
}


void manageWiFiEvents(){
  WiFi.onEvent(onWifiScanEnded, WiFiEvent_t::ARDUINO_EVENT_WIFI_SCAN_DONE);
  WiFi.onEvent(onWifiStationStart, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_START);
  WiFi.onEvent(onWifiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(onWifiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  WiFi.onEvent(onWifiAPStart, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_START);
  WiFi.onEvent(onWifiAPStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STACONNECTED);
  WiFi.onEvent(onWifiAPStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STADISCONNECTED);
}

void setupDevices(){
 //Setting leds
  pinMode(LED_RED, OUTPUT);
  digitalWrite(LED_RED, 0);
  pinMode(LED_YELLOW, OUTPUT);
  digitalWrite(LED_YELLOW, 0);
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN, 0);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_BLUE, 0);
  pinMode(LED_WHITE, OUTPUT);
  digitalWrite(LED_WHITE, 0);

  //Setting buttons

  pinMode(BTN_1, INPUT);
  pinMode(BTN_2, INPUT);
  attachInterrupt(digitalPinToInterrupt(BTN_1), onBTN_1Change, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_2), onBTN_2Change, FALLING);
}

void setup() {
  Serial.begin(115200);
  //setupDevices();
  initDisplay();
  //manageWiFiEvents();

  WiFi.onEvent(onWifiAPStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STACONNECTED);
  WiFi.onEvent(onWifiAPStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STADISCONNECTED);

  WiFi.mode(WIFI_AP);
  WiFi.softAP("iot-fedyfausto", "Password");

  // WiFi.mode(WIFI_STA);
  // int reti = WiFi.scanNetworks();
  // for(int i = 0; i < reti; i++){
  //   Serial.println(WiFi.SSID(i)+ " "+WiFi.RSSI(i));
  // }
  // WiFi.begin("PlumCake","Password");
  // while (WiFi.status() != WL_CONNECTED) {
  //   Serial.print('.');
  //   delay(1000);
  // }
  // Serial.println(WiFi.localIP());


  //startAccessPointWifi("corso-iot", "corso-iot-non-votate-nessuno");

  if(withDisplay){
    display.println("Access Point Started");
    display.println("corso-iot");
    display.println("Connessi:");
    display.println(stationsConnected);
    display.display();
  }

}

void loop() {
  manageInput();
}
