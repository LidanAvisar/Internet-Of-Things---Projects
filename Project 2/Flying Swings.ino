/*
  Flying Swings
  In this project a miniature Flying Swings model is ingeniously constructed from a small box, 
  incorporating a joystick and a light sensor for interactive play. The joystick cleverly enables the user to simulate flying,
  creating a dynamic and engaging experience. The integration of a light sensor transforms light changes into signals, turning the 
  lights to red whenever it detects proximity, adding a layer of interactivity that mimics the excitement of real amusement park rides. 
  Throughout the experience, continuous music plays from a speaker located at the base,
  enriching the atmosphere and enhancing the illusion of a joyful day at the park. 

  The circuit:
  list the components attached to each input:
  * JOYSTICK (34,35,32) 
  * LIGHT_SENSOR (27)
  * 
  * list the components attached to each output:
  * speaker
  * LED strip
  * Servo

  Video link: https://drive.google.com/file/d/1cuks1_aKCEay51OxzJMfuHX2xbvrtNMb/view?usp=sharing
  
  Created By:
  Student1_Alon_Regenbogen #315371518
  Student2_Lidan_Avisar #211357751
  Student3_Benny_Mestel #326865946
*/ 


#include <WiFi.h>
#include "SPIFFS.h"
#include "AudioFileSourceSPIFFS.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"
#include <ESP32Servo.h>
#define LIGHT_SENSOR_PIN 27
#include <Adafruit_NeoPixel.h>
#define LED_PIN 15
#define LED_COUNT 12
#define JOYSTICK_X_PIN 34
#define JOYSTICK_Y_PIN 35
#define JOYSTICK_SW_PIN 32


AudioGeneratorMP3 *mp3;
AudioFileSourceSPIFFS *file;
AudioOutputI2S *out;
AudioFileSourceID3 *id3;
  
Servo myservo;
int pos = 0;   
int servoPin = 14;
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Wiring:
//
// ESP32             MAX 98367A
// =============================
// GPIO25            LRC 
// GPIO26            BCLK
// GPIO22            DIN
// Not Connected     SD
// GND               GND
// VCC               Vin

// Use the "Tools->ESP8266/ESP32 Sketch Data Upload" menu to write the MP3 to SPIFFS
// Then upload the sketch normally.  

// pno_cs from https://ccrma.stanford.edu/~jos/pasp/Sound_Examples.html


// Called when a metadata event occurs (i.e. an ID3 tag, an ICY block, etc.
void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string)
{
  (void)cbData;
  Serial.printf("ID3 callback for: %s = '", type);
  if (isUnicode) {
    string += 2;
  }
  
  while (*string) {
    char a = *(string++);
    if (isUnicode) {
      string++;
    }
    Serial.printf("%c", a);
  }
  Serial.printf("'\n");
  Serial.flush();
}



void theaterChaseRainbow(unsigned long wait) {
  static unsigned long lastUpdate = 0; // Last update of theater chase
  static int firstPixelHue = 0; // Starting hue
  static int a = 0; // Animation step a
  static int b = 0; // Animation step b

  unsigned long currentTime = millis();
  if (currentTime - lastUpdate > wait) {
    lastUpdate = currentTime;

    if (b == 0) { // Only clear the strip once per cycle
      strip.clear();
    }
    
    for(int c = b; c < strip.numPixels(); c += 3) {
      int hue = firstPixelHue + c * 65536L / strip.numPixels();
      uint32_t color = strip.gamma32(strip.ColorHSV(hue));
      strip.setPixelColor(c, color);
    }
    strip.show();

    b++;
    if (b >= 3) { // Reset b, increment a for the next cycle
      b = 0;
      a++;
      firstPixelHue += 65536 / 90; // Increase the starting hue
    }
    if (a >= 30) { // Reset a and firstPixelHue after full cycle
      a = 0;
      firstPixelHue = 0;
    }
  }
}


void colorWipe(uint32_t color, unsigned long wait) {
  static unsigned long lastUpdate = 0; // Last update of color wipe
  static int i = 0; // Current pixel being updated

  unsigned long currentTime = millis();
  if (currentTime - lastUpdate > wait) {
    lastUpdate = currentTime;
    if (i < strip.numPixels()) {
      strip.setPixelColor(i, color);
      strip.show();
      i++;
    } else {
      i = 0; // Reset for next cycle
    }
  }
}

void restartMP3Playback() {
  delete mp3; // Free up the mp3 object
  delete file; // Free up the file object

  file = new AudioFileSourceSPIFFS("/song1.mp3");
  id3 = new AudioFileSourceID3(file);
  id3->RegisterMetadataCB(MDCallback, (void*)"ID3TAG");
  mp3 = new AudioGeneratorMP3();
  
  mp3->begin(id3, out);
}

void setup() {
  WiFi.mode(WIFI_OFF); 
  Serial.begin(115200);
  SPIFFS.begin();
  strip.begin();
  strip.show();
  strip.setBrightness(150);

  audioLogger = &Serial;
  file = new AudioFileSourceSPIFFS("/song1.mp3");
  id3 = new AudioFileSourceID3(file);
  id3->RegisterMetadataCB(MDCallback, (void*)"ID3TAG");
  out = new AudioOutputI2S();
  mp3 = new AudioGeneratorMP3();
  mp3->begin(id3, out);


  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(servoPin, 500, 2500); 
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  pinMode(JOYSTICK_SW_PIN, INPUT_PULLUP);
  

}

int targetServoPos = 90; // Default target position is the middle
int currentServoPos = 90; // Current servo position
unsigned long lastServoUpdateTime = 0; // Last time the servo was updated
const long servoUpdateInterval = 10; // Time between servo updates in milliseconds

void loop() {
  // Handle MP3 playback
  if (mp3->isRunning()) {
    if (!mp3->loop()) mp3->stop();
  } else {
    // Restart MP3 playback
    restartMP3Playback();
  }

  // Read light sensor and control LEDs
  int lightSensor = analogRead(LIGHT_SENSOR_PIN); 
  if (lightSensor > 0 && lightSensor < 2000) {
    theaterChaseRainbow(50); 
  } else if (lightSensor > 2300) {
    colorWipe(strip.Color(255, 0, 0), 50); 
  }

  unsigned long currentMillis = millis();

  int x = analogRead(JOYSTICK_X_PIN); // Read the joystick value
  
  // Update target position based on joystick input
  if (x > 1900) { // If joystick is forward
    targetServoPos = 180;
  } else if (x < 1400) { // If joystick is backward
    targetServoPos = 0;
  }

  // Smoothly move the servo to the target position without blocking
  if (currentMillis - lastServoUpdateTime >= servoUpdateInterval) {
    if (currentServoPos < targetServoPos) {
      currentServoPos++; // Increment current position
      myservo.write(currentServoPos);
    } else if (currentServoPos > targetServoPos) {
      currentServoPos--; // Decrement current position
      myservo.write(currentServoPos);
    }
    lastServoUpdateTime = currentMillis; // Update the last update time
  }

  
}
