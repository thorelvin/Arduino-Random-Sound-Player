/*
 * Arduino Random Arcade Sound Player
 * Ultrasonic range sensor AJ-SR04M
 * Mp3 shield VS1053
 * Amplifier PCB and speaker
 * 12V supply
 * 
 * 19.04.2023
 */


#include <SPI.h>
#include <Adafruit_VS1053.h>

#define TRIG_PIN 3      // OK Ultrasonic sensor TRIG
#define ECHO_PIN 4      // OK Ultrasonic sensor ECHO
#define VS1053_RESET 8  // OK VS1053 reset pin
#define VS1053_CS 6     // OK VS1053 chip select pin (output)
#define VS1053_DCS 7    // OK VS1053 Data/command select pin (output)
#define CARDCS 9        // OK SD card chip select pin
#define VS1053_DREQ 2   // OK VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer =
  Adafruit_VS1053_FilePlayer(VS1053_RESET, VS1053_CS, VS1053_DCS, VS1053_DREQ, CARDCS);

long duration, distance;
unsigned long previousMillis = 0;
unsigned long interval = 10000; // 10 second interval between plays
int threshold_distance = 100; // minimum distance required to trigger sound (in cm)

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.begin(9600);

  if (!musicPlayer.begin()) {
    Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
    while (1);
  }

  Serial.println(F("VS1053 found"));

  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }
  Serial.println("SD OK!");
}

void loop() {

  checkRange();

  if (distance <= threshold_distance) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      playRandomSound();
    }
  }
}

void playRandomSound() {
  Serial.println("Playing sound!");
  uint8_t fileNumber = random(1, 246);
  String filename = "Track" + String(fileNumber) + ".mp3";
  Serial.print("Playing: ");
  Serial.println(filename);
  musicPlayer.playFullFile(filename.c_str());
}

void checkRange() {
  delayMicroseconds(100);

  // Clears the trigPin condition
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(ECHO_PIN, HIGH);

  // We drops any results measuring 0 cm, because sensor is acting strange.
  if (0 < (duration * 0.034 / 2)) {
    // Speed of sound wave divided by 2 (go and back)
    distance = duration * 0.034 / 2;
  }

  Serial.print("Checking range: ");
  Serial.println(distance);

}
