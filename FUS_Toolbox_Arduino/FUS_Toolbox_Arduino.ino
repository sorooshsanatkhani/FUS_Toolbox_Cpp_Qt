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

const int microsteps = 125; // 125 microsteps per 10mm

volatile bool stopRequested = false; // Flag to signal stop

// Command queue setup
const int maxCommands = 100; // Maximum number of commands to store
String commandQueue[maxCommands]; // Array to store commands
int queueHead = 0; // Points to the head of the queue
int queueTail = 0; // Points to the tail of the queue
int commandCount = 0; // Number of commands in the queue

void setup()
{
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

void loop()
{
  // Check for new commands and enqueue them
  while (Serial.available() > 0) {
    enqueueCommand(Serial.readStringUntil('\n'));
  }

  // Process the next command in the queue if available
  if (commandCount > 0) {
    String command = dequeueCommand();
    processCommand(command);
  }
}

void enqueueCommand(String command)
{
  if (commandCount < maxCommands) {
    commandQueue[queueTail] = command;
    queueTail = (queueTail + 1) % maxCommands;
    Serial.println("Command recieved!");
    commandCount++;
  } else {
    // Queue is full, handle overflow here
    Serial.println("Command queue is full. Command ignored.");
  }
}

String dequeueCommand()
{
  if (commandCount > 0) {
    String command = commandQueue[queueHead];
    queueHead = (queueHead + 1) % maxCommands;
    commandCount--;
    return command;
  }
  return "G"; // Return an G as in "green light" if the queue is empty
}

void processCommand(String command)
{
  Serial.println("Processing command: " + command);
  if (command.charAt(0) == 'O') { // Check if the received data is the ON command
    digitalWrite(ENA_LR_Pin, LOW);
    digitalWrite(ENA_UD_Pin, LOW);
    digitalWrite(ENA_FB_Pin, LOW);
  }
  else if (command.charAt(0) == 'C') { // Check if the received data is the OFF command
    digitalWrite(ENA_LR_Pin, HIGH);
    digitalWrite(ENA_UD_Pin, HIGH);
    digitalWrite(ENA_FB_Pin, HIGH);
  }
  else if (command.charAt(0) == 'S') { // Check if the received data is the stop command
    stopRequested = true; // Set the stop flag
  }
  else {
    stopRequested = false; // Reset the stop flag
    processMovementCommand(command);
  }
  if (stopRequested) {
    stopMotors();
  }
}

void processMovementCommand(String command)
{
  char direction = command.charAt(0);
  int firstCommaIndex = command.indexOf(',');
  int secondCommaIndex = command.indexOf(',', firstCommaIndex + 1);
  float distance = command.substring(firstCommaIndex + 1, secondCommaIndex).toFloat();
  float speed = command.substring(secondCommaIndex + 1).toFloat();
  float TravelTime = distance/speed;
  // Calculate steps and delay
  long steps = 20 * distance * microsteps;
  long Frequency = steps/TravelTime;
  long HalfPeriod = double((1000000.0)/(2.0*Frequency)); // Delay in microseconds
  Serial.print("Direction: ");
  Serial.print(direction);
  Serial.print(",  Distance: ");
  Serial.print(distance);
  Serial.println(" mm");
  // Control Left-Right movement
  if (direction == 'L' || direction == 'R') {
    controlStepper(DIR_LR_Pin, PUL_LR_Pin, ENA_LR_Pin, direction == 'L', steps, HalfPeriod);
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

void controlStepper(byte dirPin, byte pulPin, byte enaPin, bool direction, long steps, long HalfPeriod) {
  digitalWrite(dirPin, direction ? HIGH : LOW); // Set direction
  delay(300); //minimum enable time is 200ms
  for (long i = 0; i < steps; i++) {
    if (Serial.available() > 0) {
      String receivedData = Serial.readStringUntil('\n');
      if (receivedData.charAt(0) == 'S') { // If stop command is received
        digitalWrite(pulPin, LOW); // Disable motor immediately
        stopRequested = true; // Set the stop flag
        stopMotors(); // Call stopMotors function to handle stopping
        return; // Exit the function
      } else {
        // If it's not a stop command, enqueue the command for later processing
        enqueueCommand(receivedData);
      }
    }
    digitalWrite(pulPin, HIGH);
    delayMicroseconds(HalfPeriod);
    digitalWrite(pulPin, LOW);
    delayMicroseconds(HalfPeriod);
  }
}

void stopMotors() {
  // Stop all motors
  digitalWrite(PUL_LR_Pin, LOW);
  digitalWrite(PUL_UD_Pin, LOW);
  digitalWrite(PUL_FB_Pin, LOW);
  queueHead = 0;
  queueTail = 0;
  commandCount = 0;
  stopRequested = false;
}