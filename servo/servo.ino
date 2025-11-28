#include <Servo.h>
#include <IRremote.hpp>

#define IR_RECEIVE_PIN 2

// 7seg
const int SRCLK = 8;
const int RCLK = 7;
const int SER = A0;

const uint8_t SEG_PATTERNS[] = {
  B00000011,
  B10011111,
  B00100101,
  B00001101,
  B10011001,
  B01001001,
  B01000001,
  B00011111,
  B00000001,
  B00001001,
};

int score = 0;
int old_score = -1;

// button
const uint8_t START_BUTTON = 4;
bool is_startgame = false;

int buttonState;
int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;

void setup() {
  Serial.begin(115200);
  Serial.println(time);
  
  pinMode(START_BUTTON, INPUT_PULLUP);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  for (int i = 0; i < 4; i++) {
    servos[i].attach(SERVO_PINS[i]);
    servos[i].write(0);
  }

  // 最初のサーボをランダムに選択して起動
  currentServo = random(4);
  servos[currentServo].write(90);
  Serial.print("First servo: ");
  Serial.println(currentServo);
}

void loop() {
  if (score != old_score) {
      displayNumber(score);
      old_score = score;
      Serial.println(score);
  }
  if (score > 9) score = 0;
  int reading = digitalRead(START_BUTTON);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        if (!is_startgame) {
          Serial.println("Start button pressed! Game is ON.");
        }
        is_startgame = true;
      }
    }
  }

  lastButtonState = reading;

  if (IrReceiver.decode() && is_startgame) {
    tone(11, 187, 100);
    delay(100);
    noTone(11);
    delay(5);

    score++;
    servos[currentServo].write(0);

    int nextServo = random(4);
    while (nextServo == currentServo) {
      nextServo = random(4);
    }
    currentServo = nextServo;

    servos[currentServo].write(90);

    Serial.print("New servo: ");
    Serial.println(currentServo);

    IrReceiver.resume();
  }
}

void displayNumber(int num) {
  noInterrupts(); // 全ての割り込みを一時的に無効化
  
  digitalWrite(RCLK, LOW); // ラッチピンをLOWに
  shiftOut(SER, SRCLK, LSBFIRST, SEG_PATTERNS[num]); // データをシフトイン
  digitalWrite(RCLK, HIGH); // ラッチピンをHIGHにして、データを確定・表示
  
  interrupts(); // 割り込みを再度有効化
}