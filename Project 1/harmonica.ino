/*
  Harmonica
  In this project, a harmonica-inspired musical instrument was crafted using a small cardboard box, integrating a joystick, 
  an ultrasonic sensor, and a light sensor. The joystick allows playing various songs with five notes,
  while the ultrasonic sensor enables playing only when held close for authenticity, 
  and the light sensor adds electronic music when the player hovers their mouth over it. 
  Additionally, the harmonica features a speaker at the bottom.

  The circuit:
  list the components attached to each input:
  * JOYSTICK (34,35,32) 
  * LIGHT_SENSOR (27)
  * ultrasonic sensor(16) 
  * 
  * list the components attached to each output:
  * speaker(26)

  Video link: https://drive.google.com/file/d/1Id3SShsEX0DDhj0r1-G7V6TSeDQ8hdOu/view
  
  Created By:
  Student1_Alon_Regenbogen #315371518
  Student2_Lidan_Avisar #211357751
  Student3_Benny_Mestel #326865946
*/ 

#include "pitches.h"
#define TONE_OUTPUT_PIN 26
#define JOYSTICK_X_PIN 34
#define JOYSTICK_Y_PIN 35
#define JOYSTICK_SW_PIN 32
#define LIGHT_SENSOR_PIN 27
#define TRIGGER_PIN  16 
#define ECHO_PIN     17 
#define MAX_DISTANCE 100 

const int TONE_PWM_CHANNEL = 0;

// Melody arrays for different joystick positions
int melodyX[] = {NOTE_E4};
int melodyY[] = {NOTE_G4};
int melodyUP[] = {NOTE_C4};
int melodyDOWN[] = {NOTE_D4};
int melodyCLICK[] = {NOTE_F4};

int* currentMelody; 

void setup() {
  Serial.begin(115200);
  ledcAttachPin(TONE_OUTPUT_PIN, TONE_PWM_CHANNEL);
  pinMode(JOYSTICK_SW_PIN, INPUT_PULLUP);
  ledcWrite(TONE_PWM_CHANNEL, 0);
  pinMode(LIGHT_SENSOR_PIN, INPUT);
}

void playMelody(int* melody) {
  int noteDuration = 250;
  ledcWriteTone(TONE_PWM_CHANNEL, melody[0]);
  delay(noteDuration);
  ledcWriteTone(TONE_PWM_CHANNEL, 0);

}

int ultrasonic_measure(int trigPin, int echoPin, int max_distance) {
pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);
digitalWrite(trigPin, LOW);
delayMicroseconds(2);
digitalWrite(trigPin, HIGH);
delayMicroseconds(5);
digitalWrite(trigPin, LOW);
int duration = pulseIn(echoPin, HIGH, max_distance * 59);
return duration / 59; 
}

void loop() {
  
  int x = analogRead(JOYSTICK_X_PIN);
  int y = analogRead(JOYSTICK_Y_PIN);
  int sw = digitalRead(JOYSTICK_SW_PIN);
  int cm = ultrasonic_measure(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
  int lightSensor = analogRead(LIGHT_SENSOR_PIN); 

  // Light sensor-based tone generation that create music effect  
  if(lightSensor > 3000){
    int frequency = map(lightSensor, 0, 4095, 120, 2000); 
    int amplitude = 50;  
    int modulation = amplitude * sin(millis() * 2 * PI / 1000); 
    int modulatedFrequency = frequency + modulation; 
    tone(TONE_OUTPUT_PIN, modulatedFrequency);
    delay(10);
    tone(TONE_OUTPUT_PIN, 0);
  }
  
  // Define joystick thresholds and values
  int defaultX = 1750;
  int defaultY = 1750;
  int threshold = 50;
  int maxValue = 4095;


  // Joystick-based melody selection position and playing only when close
  if(cm > 0 && cm < 60){
    if (x < defaultX + threshold && x > defaultX - threshold && y < defaultY + threshold && y > defaultY - threshold) {
        ledcWriteTone(TONE_PWM_CHANNEL, 0);
    } else if(x > defaultX + threshold && y > defaultY + threshold) {
        currentMelody = melodyUP;
        playMelody(currentMelody);
    } else if(x < defaultX - threshold && y > defaultY + threshold) {
        currentMelody = melodyDOWN;
        playMelody(currentMelody);
    } else if(x < defaultX - threshold && y < defaultY - threshold) {
        currentMelody = melodyX;
        playMelody(currentMelody);
    } else if(x > defaultX + threshold && y < defaultY - threshold) {
        currentMelody = melodyY;
        playMelody(currentMelody);
    }
    if(sw == 0){
      playMelody(melodyCLICK);
    }
  }
}
