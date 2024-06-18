#include <Arduino.h>

// Pin definitions
byte PUL_LR_Pin = 3;
byte PUL_UD_Pin = 5;
byte PUL_FB_Pin = 6;

byte DIR_LR_Pin = 2;
byte DIR_UD_Pin = 4;
byte DIR_FB_Pin = 7;

byte ENA_LR_Pin = 8;
byte ENA_UD_Pin = 12;
byte ENA_FB_Pin = 10;

byte TEST_Pin = 11;

const int microsteps = 800; // 3200 microsteps per 10mm

volatile bool stopRequested = false; // Flag to signal stop

void setup() {
  Serial.begin(9600);
  // Set pin modes
  pinMode(PUL_LR_Pin, OUTPUT);
  pinMode(PUL_UD_Pin, OUTPUT);
  pinMode(PUL_FB_Pin, OUTPUT);

  pinMode(DIR_LR_Pin, OUTPUT);
  pinMode(DIR_UD_Pin, OUTPUT);
  pinMode(DIR_FB_Pin, OUTPUT);

  pinMode(ENA_LR_Pin, OUTPUT);
  pinMode(ENA_UD_Pin, OUTPUT);
  pinMode(ENA_FB_Pin, OUTPUT);

  pinMode(TEST_Pin, OUTPUT);

  // Disable motors initially
  digitalWrite(ENA_LR_Pin, HIGH);
  digitalWrite(ENA_UD_Pin, HIGH);
  digitalWrite(ENA_FB_Pin, HIGH);
}

void loop() {
  if (Serial.available() > 0) {
    String receivedData = Serial.readStringUntil('\n');
    if (receivedData.charAt(0) == 'S') { // Check if the received data is the stop command
      stopRequested = true; // Set the stop flag
    }
    else {
      stopRequested = false; // Reset the stop flag
      char direction = receivedData.charAt(0);
      int firstCommaIndex = receivedData.indexOf(',');
      int secondCommaIndex = receivedData.indexOf(',', firstCommaIndex + 1);
      float distance = receivedData.substring(firstCommaIndex + 1, secondCommaIndex).toFloat();
      float speed = receivedData.substring(secondCommaIndex + 1).toFloat();
      float TravelTime = distance/speed;
      // Calculate steps and delay
      long steps = 20 * distance * microsteps;
      long Frequency = steps/TravelTime;
      long HalfPeriod = double((1000000.0)/(2.0*Frequency)); // Delay in microseconds
      Serial.print("Direction: ");
      Serial.print(direction);
      Serial.print(",  Distance: ");
      Serial.print(distance);
      Serial.print("mm,  Speed: ");
      Serial.print(speed);
      Serial.print("mm/s,  Steps: ");
      Serial.print(steps);
      Serial.print("  ,  Frequency: ");
      Serial.print(Frequency/1000.0);
      Serial.print("kHz  ,  Half Period: ");
      Serial.print(HalfPeriod);
      Serial.println(" microseconds");
      // Control Left-Right movement
      if (direction == 'L' || direction == 'R') {
        controlStepper(DIR_LR_Pin, PUL_LR_Pin, ENA_LR_Pin, direction == 'R', steps, HalfPeriod);
      }
      // Control Up-Down movement
      else if (direction == 'U' || direction == 'D') {
        controlStepper(DIR_UD_Pin, PUL_UD_Pin, ENA_UD_Pin, direction == 'U', steps, HalfPeriod);
      }
      // Control Forward-Backward movement
      else if (direction == 'F' || direction == 'B') {
        controlStepper(DIR_FB_Pin, PUL_FB_Pin, ENA_FB_Pin, direction == 'F', steps, HalfPeriod);
      }
    }
  }
  if (stopRequested) {
    stopMotors();
  }
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
void controlStepper(byte dirPin, byte pulPin, byte enaPin, bool direction, long steps, long HalfPeriod) {
  digitalWrite(dirPin, direction ? HIGH : LOW); // Set direction
  digitalWrite(enaPin, LOW); // Enable motor
  delay(300); //minimum enable time is 200ms
  for (long i = 0; i < steps; i++) {
    if (Serial.available() > 0) {
      String receivedData = Serial.readStringUntil('\n');
      if (receivedData.charAt(0) == 'S') { // If stop command is received
        digitalWrite(enaPin, HIGH); // Disable motor immediately
        return; // Exit the function
      }
    }
    digitalWrite(pulPin, HIGH);
    delayMicroseconds(HalfPeriod);
    digitalWrite(pulPin, LOW);
    delayMicroseconds(HalfPeriod);
  }
  digitalWrite(enaPin, HIGH); // Disable motor after operation

}

void stopMotors() {
  // Disable all motors
  digitalWrite(ENA_LR_Pin, HIGH);
  digitalWrite(ENA_UD_Pin, HIGH);
  digitalWrite(ENA_FB_Pin, HIGH);
}