#include <ESP8266WiFi.h>
#include <WiFiudp.h>
#include <time.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "gift_muka.h"

const char* ssid = "ancaman";
const char* password = "Akudewewani";
const char* ntpServer = "pool.ntp.org";

const int led[] = {D5,D6,D7};
const int total_led = 3;
const int button = D3;
const int buzzer = D8;

unsigned long previousMillisLED = 0;
int stateLED = 0;
int currentLED = 0;

const long gmtOffset_sec = 25200;
const int daylightOffset_sec = 0;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define FRAME_ARRAY muka
#define FRAME_COUNT mukaFrameCount

void initWiFi() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Menghubungkan!");
    display.display();
  }

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

//fungsi  utuk mengambil dan mengubah format menjadi waktu 
void printLocalTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Gagal Mendapatkan Waktu");
    return;
  }

  char time_output[9];
  char date_output[17];

  strftime(time_output, 9, "%H:%M", &timeinfo);
  strftime(date_output, 17, "%a, %d/%m/%y", &timeinfo);

   //set oled dan tampilkan 
   display.clearDisplay();
   display.setTextSize(1);
   display.setCursor(25, 17);
   display.setTextColor(WHITE);
   display.println(date_output);

   display.setTextSize(3);
   display.setCursor(19, 28);
   display.println(time_output);

   display.display();

}
void lampRGB() {
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillisLED >= 1000) {
    previousMillisLED = currentMillis;

    if(stateLED == 0) {
      if(currentLED < total_led) {
        digitalWrite(led[currentLED], HIGH);
        currentLED++;
      } else {
        stateLED = 1;
        currentLED = 0;
      }
    } else {
      if(currentLED < total_led) {
        digitalWrite(led[currentLED], LOW);
        currentLED++;
      }else {
        stateLED = 0;
        currentLED = 0;
      }
    }
  }
}

void animasi() {
  static uint8_t frame_index = 0;
  static unsigned long lastFrameChange = 0;
  const long frameInterval = 100;

  if (millis() - lastFrameChange >= frameInterval) {
    lastFrameChange = millis();
    display.clearDisplay();
    display.drawBitmap(0, 0, FRAME_ARRAY[frame_index], SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
    display.display();

    frame_index++;
    if (frame_index >= FRAME_COUNT) {
      frame_index = 0;
    }
  }
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(button,INPUT_PULLUP);
  pinMode(buzzer,OUTPUT);
  
  for(int i = 0; i < total_led; i++) {
    pinMode(led[i], OUTPUT);
  }

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for(;;); //jangan lanjut jika gagal
  }
  display.clearDisplay();
  initWiFi();
}

int lastcond = HIGH;
int slide = 0;
bool waktu = false;

void loop() {
  lampRGB();
  //baca tombol IO0 untuk berganti dari mode 1 ke mode 2
  int position = digitalRead(button);

  if (position == LOW && lastcond == HIGH){
    digitalWrite(buzzer,HIGH);
    delay(50);
    digitalWrite(buzzer,LOW);

    slide++;

    if (slide > 1) slide = 0;
    delay(200);
  }

  switch(slide) {
    case 0:
      animasi();
      break;
    case 1:
      waktu = true;
      printLocalTime();
      break;
  }

  lastcond = position;
 
  // put your main code here, to run repeatedly:

}
