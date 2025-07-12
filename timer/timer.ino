#include <IRremote.hpp>

const int pattern[10][7] = { { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, LOW },
                             { LOW, HIGH, HIGH, LOW, LOW, LOW, LOW },
                             { HIGH, HIGH, LOW, HIGH, HIGH, LOW, HIGH },
                             { HIGH, HIGH, HIGH, HIGH, LOW, LOW, HIGH },
                             { LOW, HIGH, HIGH, LOW, LOW, HIGH, HIGH },
                             { HIGH, LOW, HIGH, HIGH, LOW, HIGH, HIGH },
                             { HIGH, LOW, HIGH, HIGH, HIGH, HIGH, HIGH },
                             { HIGH, HIGH, HIGH, LOW, LOW, LOW, LOW },
                             { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH },
                             { HIGH, HIGH, HIGH, HIGH, LOW, HIGH, HIGH } };

const int pinNo[7] = { 2, 3, 4, 5, 6, 7, 8 };

const int IR_RECEIVE_PIN = 9;

int point = 0;
bool isCountdown = true;

void displayDigit(int digit) {
  for (int i = 0; i < 7; i++) {
    digitalWrite(pinNo[i], !pattern[digit][i]);
  }
}

void clearDisplay() {
  for (int i = 0; i < 7; i++) {
    digitalWrite(pinNo[i], HIGH);
  }
}

void countdown() {
  for (int i = 9; i >= 0; i--) {
    displayDigit(i);
    delay(1000);
    if (IrReceiver.decode()) {
      point++;
      IrReceiver.resume();
    }
  }
}

void showPoint() {
  clearDisplay();
  delay(500);
  displayDigit(point);
  delay(500);
}

void setup() {
  for (int i = 0; i < 7; i++) {
    pinMode(pinNo[i], OUTPUT);
    digitalWrite(pinNo[i], HIGH);
  }
  IrReceiver.begin(IR_RECEIVE_PIN, false);
}

void loop() {
  if (isCountdown) {
    countdown();
    isCountdown = false;
  } else {
    showPoint();
  }
}
