// Name: __________  Date: 2026-10-25
// Distance password lock: three measured distances control a servo latch.
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

Adafruit_SSD1306 screen(128, 64, &Wire, -1);
Servo latch;
const byte addPin = 2, undoPin = 3, enterPin = 4;
const byte trigPin = 6, echoPin = 7, servoPin = 9;
const int lockedAngle = 0, openAngle = 90;
const int password[3] = {15, 25, 35}; // Move a hand to these distances, in cm.
const int toleranceCm = 3;            // Each value may differ by up to 3 cm.
int enteredDistance[3] = {0, 0, 0};
int currentDistance = -1;
byte entered = 0, mistakes = 0;
bool addWasDown = false, undoWasDown = false, enterWasDown = false;
unsigned long lastMeasure = 0;

// INPUT_PULLUP: pressed is LOW. A held button triggers only once.
bool pressedOnce(byte pin, bool &wasDown) {
  bool down = digitalRead(pin) == LOW;
  if (down != wasDown) {
    delay(20);
    down = digitalRead(pin) == LOW;
    if (down != wasDown) {
      wasDown = down;
      return down;
    }
  }
  return false;
}

int readCentimetres() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  unsigned long echoTime = pulseIn(echoPin, HIGH, 25000);
  if (echoTime == 0) return -1;
  int cm = echoTime / 58;
  if (cm < 5 || cm > 80) return -1;
  return cm;
}

// Take the middle of three readings to reduce occasional noisy echoes.
int measureDistance() {
  int a = readCentimetres();
  delay(60); // Give the previous ultrasound pulse time to fade.
  int b = readCentimetres();
  delay(60);
  int c = readCentimetres();
  int swapValue;
  if (a > b) { swapValue = a; a = b; b = swapValue; }
  if (b > c) { swapValue = b; b = c; c = swapValue; }
  if (a > b) { swapValue = a; a = b; b = swapValue; }
  return b;
}

void showMain() {
  screen.clearDisplay();
  screen.setTextColor(SSD1306_WHITE);
  screen.setTextSize(1);
  screen.setCursor(0, 0);
  screen.print("DISTANCE LOCK");
  screen.setCursor(0, 11);
  screen.print("NOW: ");
  if (currentDistance < 0) screen.print("NO ECHO");
  else {
    screen.print(currentDistance);
    screen.print(" cm");
  }
  screen.setCursor(82, 11);
  screen.print("ERR:");
  screen.print(mistakes);
  screen.setCursor(0, 23);
  screen.print("CODE ");
  screen.print(entered);
  screen.print("/3");
  screen.setTextSize(2);
  for (byte i = 0; i < entered; i++) {
    screen.setCursor(i * 43, 34);
    screen.print(enteredDistance[i]);
  }
  screen.setTextSize(1);
  screen.setCursor(0, 55);
  screen.print("A:ADD B:UNDO C:OK");
  screen.display();
}

void showMessage(const char *message) {
  screen.clearDisplay();
  screen.setTextColor(SSD1306_WHITE);
  screen.setTextSize(2);
  screen.setCursor(0, 22);
  screen.print(message);
  screen.display();
}

void clearEntry() {
  entered = 0;
  showMain();
}

void addDistance() {
  if (entered == 3) return;
  currentDistance = measureDistance(); // Do not use an old screen value.
  if (currentDistance < 0) {
    showMessage("NO ECHO");
    delay(900);
  } else {
    enteredDistance[entered++] = currentDistance;
  }
  showMain();
}

void checkPassword() {
  if (entered != 3) {
    showMessage("NEED 3");
    delay(900);
    showMain();
    return;
  }
  bool correct = true;
  for (byte i = 0; i < 3; i++) {
    if (abs(enteredDistance[i] - password[i]) > toleranceCm) correct = false;
  }
  if (correct) {
    mistakes = 0;
    latch.write(openAngle);
    showMessage("OPEN 5s");
    delay(5000);
    latch.write(lockedAngle);
  } else {
    mistakes++;
    if (mistakes == 3) {
      showMessage("WAIT 30s");
      delay(30000);
      mistakes = 0;
    } else {
      showMessage("WRONG");
      delay(1200);
    }
  }
  clearEntry();
}

void setup() {
  pinMode(addPin, INPUT_PULLUP);
  pinMode(undoPin, INPUT_PULLUP);
  pinMode(enterPin, INPUT_PULLUP);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  latch.write(lockedAngle);
  latch.attach(servoPin);
  Wire.begin();
  Wire.setClock(400000);
  if (!screen.begin(SSD1306_SWITCHCAPVCC, 0x3C)) while (true) {}
  currentDistance = measureDistance();
  showMain();
}

void loop() {
  if (pressedOnce(addPin, addWasDown)) addDistance();
  if (pressedOnce(undoPin, undoWasDown) && entered > 0) {
    entered--;
    showMain();
  }
  if (pressedOnce(enterPin, enterWasDown)) checkPassword();
  if (millis() - lastMeasure >= 500) {
    lastMeasure = millis();
    int newDistance = readCentimetres();
    if (newDistance != currentDistance) {
      currentDistance = newDistance;
      showMain();
    }
  }
}
