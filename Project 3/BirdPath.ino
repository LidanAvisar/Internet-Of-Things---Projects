/*
  BirdPath:
  Using the Google Maps api, we check the travel time and display it using LED lights.
  When the time is suitable for travel base on trafic, we will get a green indicator, and additionally, the motor 
  will rotate to show us this in a better time.
  The led lights change based on the time- more than 6 it is red,
  and led it is green.

  The circuit:
  * 
  * list the components attached to each output:
  * Led Strip(15)
  * Servo(23)
  * 

  Video link: https://drive.google.com/file/d/1_v9YePVDM57gTP1fM2GSUiMCRlWVcrzW/view?usp=sharinghttps://drive.google.com/file/d/1_v9YePVDM57gTP1fM2GSUiMCRlWVcrzW/view?usp=sharing
  
  Created By:
  Student1_Alon_Regenbogen #315371518
  Student2_Lidan_Avisar #211357751
  Student3_Benny_Mestel #326865946
*/ 

#define BLYNK_TEMPLATE_ID "TMPL6IsMPCiZY"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "oqCHWthEyFX6T-aNPZQenxw_yjdsWIWg"
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Adafruit_NeoPixel.h>
#include <ESP32Servo.h>

#define LED_PIN 15
#define LED_COUNT 12
#define ANALOG_INPUT_PIN 34
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

Servo myservo;
int servoPin = 23;
int pos = 0; 

char ssid[] = "RUNI-Wireless";
char pass[] = "";


void setup() {
  Serial.begin(115200);
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "sgp1.blynk.cloud", 80);
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(150); // Set BRIGHTNESS to about 1/5 (max = 255)
  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(servoPin, 500, 2500); 
}


void loop() {
  Blynk.run();
}



BLYNK_WRITE(V0)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  Serial.printf("got value v0: %d\n",pinValue);
  
  if (pinValue <= 6) {
    // If pinValue is smaller than 7, turn lights green
    for (int i = 0; i < pinValue; i++) {
      strip.setPixelColor(i, 0<<16 | 255<<8 | 0); // Set to green
    }
  } else {
    // If pinValue is 7 or more, turn lights red
    for (int i = 0; i < pinValue && i < 12; i++) { // Ensure it does not exceed the strip length
      strip.setPixelColor(i, 255<<16 | 0<<8 | 0); // Set to red
    }
  }

  // Turn off the remaining LEDs.
  for (int i = pinValue; i < 12; i++) { 
    strip.setPixelColor(i, 0); // Turn off
  }

  strip.show(); // Update the strip with new settings

  if(pinValue <= 6){
    for (pos = 0; pos <= 180; pos += 3) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      myservo.write(pos);    // tell servo to go to position in variable 'pos'
      delay(15);             // waits 15ms for the servo to reach the position
    }
    for (pos = 180; pos >= 0; pos -= 3) { // goes from 180 degrees to 0 degrees
      myservo.write(pos);    // tell servo to go to position in variable 'pos'
      delay(15);             // waits 15ms for the servo to reach the position
    }
  }
  
}
