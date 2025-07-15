// #include <IRremote.hpp>

const int DIGIT_PATTERN[10][7] = { { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, LOW },
                                   { LOW, HIGH, HIGH, LOW, LOW, LOW, LOW },
                                   { HIGH, HIGH, LOW, HIGH, HIGH, LOW, HIGH },
                                   { HIGH, HIGH, HIGH, HIGH, LOW, LOW, HIGH },
                                   { LOW, HIGH, HIGH, LOW, LOW, HIGH, HIGH },
                                   { HIGH, LOW, HIGH, HIGH, LOW, HIGH, HIGH },
                                   { HIGH, LOW, HIGH, HIGH, HIGH, HIGH, HIGH },
                                   { HIGH, HIGH, HIGH, LOW, LOW, LOW, LOW },
                                   { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH },
                                   { HIGH, HIGH, HIGH, HIGH, LOW, HIGH, HIGH } };

const int LOADING_PATTERN[6][7] = {
  {HIGH, LOW, LOW, LOW, LOW, LOW, LOW},
  { LOW, HIGH, LOW, LOW, LOW, LOW, LOW },
  { LOW, LOW, HIGH, LOW, LOW, LOW, LOW },
  { LOW, LOW, LOW, HIGH, LOW, LOW, LOW },
  { LOW, LOW, LOW, LOW, HIGH, LOW, LOW },
  { LOW, LOW, LOW, LOW, LOW, HIGH, LOW }
};

const int pinNo[7] = { 2, 3, 4, 5, 6, 7, 8 };

const int TEST_PATTERN[7] = { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, LOW };

// const int IR_RECEIVE_PIN = 9;

void displayTest() {
  for (int i = 0; i < 7; i++) {
    digitalWrite(pinNo[i], !TEST_PATTERN[i]);
  }
}

void displayDigit(int digit) {
  for (int i = 0; i < 7; i++) {
    //digitalWrite(pinNo[i], !DIGIT_PATTEN[digit][i]);
  }
}

void clearDisplay() {
  for (int i = 0; i < 7; i++) {
    digitalWrite(pinNo[i], HIGH);
  }
}

void displayLoading(int n) {
  for (int i = 0; i < 7; i++) {
    digitalWrite(pinNo[i], !LOADING_PATTERN[n][i]);
  }
}

void setup() {
  for (int i = 0; i < 7; i++) {
    pinMode(pinNo[i], OUTPUT);
    digitalWrite(pinNo[i], HIGH);
  }
  // IrReceiver.begin(IR_RECEIVE_PIN, false);
  Serial.begin(115200);
  Serial.println("Initialized");
}

int currentDigit = 0;
unsigned long lastUpdateTime = 0;
const long interval = 80;

void loop() {
  // if (IrReceiver.decode()) {
  //   Serial.println("Received IR signal");
  //   IrReceiver.resume();
  // }

  unsigned long currentTime = millis();
  if (currentTime - lastUpdateTime >= interval) {
    lastUpdateTime = currentTime;
    displayLoading(currentDigit);
    currentDigit++;
    if (currentDigit >= 6) {
      currentDigit = 0;
    }
  }
}
