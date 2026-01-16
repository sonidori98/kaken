#include <IRremote.hpp>

#define IR_SEND_PIN 4

const uint8_t BUTTON_PIN = 7;
const uint8_t RECV_PIN = 8;
const uint8_t RELOAD_IND_PIN = 5;
const uint8_t RELOAD_PIN = 12;

const uint8_t BLUE_PIN = A1;
const uint8_t RED_PIN = A2;

const unsigned long STUN_DURATION_MS = 5000;
const unsigned long RELOAD_COOLDOWN_MS = 5000;
const uint8_t MAX_BULLET_NUM = 17;

const uint8_t DATA = 0xF;

unsigned long stunStartTime = 0;
unsigned long lastReloadTime = 0;
unsigned long lastBlinkTime = 0;

bool isStunned = false;
uint8_t bulletNum;

bool prevButtonState = HIGH;
bool prevReloadState = HIGH;
bool blinkState = LOW;

void setup() {
  bulletNum = MAX_BULLET_NUM;
  Serial.begin(115200);

  delay(2000);

  pinMode(RELOAD_IND_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(RELOAD_PIN, INPUT_PULLUP);

  pinMode(BLUE_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);

  IrReceiver.begin(RECV_PIN, true);
  IrSender.begin(IR_SEND_PIN, true, 0);

  Serial.println("Arduino IR Game Ready!");
}

void loop() {
  unsigned long currentMillis = millis();

  // --- 状態更新: スタン解除 ---
  if (isStunned && (currentMillis - stunStartTime >= STUN_DURATION_MS)) {
    isStunned = false;
    Serial.println("Stun duration ended. You can move again!");
  }

  // --- LED表示制御 ---
  bool isReloading = (currentMillis - lastReloadTime < RELOAD_COOLDOWN_MS);

  if (isStunned) {
    // スタン時：赤点灯（最優先）
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(BLUE_PIN, LOW);
  } else if (isReloading) {
    // リロード中：赤点滅 / 青消灯
    digitalWrite(BLUE_PIN, LOW);
    if (currentMillis - lastBlinkTime >= 200) {  // 200ms間隔で反転
      lastBlinkTime = currentMillis;
      blinkState = !blinkState;
      digitalWrite(RED_PIN, blinkState);
    }
  } else {
    // 通常時：青点灯 / 赤消灯
    digitalWrite(RED_PIN, LOW);
    digitalWrite(BLUE_PIN, HIGH);
  }

  // --- ボタン入力処理 ---
  bool currentButtonState = digitalRead(BUTTON_PIN);
  bool currentReloadState = digitalRead(RELOAD_PIN);

  // 攻撃
  if (prevButtonState == HIGH && currentButtonState == LOW && !isStunned && bulletNum > 0 && !isReloading) {

    bulletNum--;
    IrSender.sendNEC(0x6380, DATA, 0);
    Serial.print("Fired! Bullets left: ");
    Serial.println(bulletNum);

  } else if (prevButtonState == HIGH && currentButtonState == LOW && isReloading) {
    Serial.println("Cannot attack during reload cooldown.");
  }

  // リロード開始
  if (prevReloadState == HIGH && currentReloadState == LOW && !isReloading) {
    bulletNum = MAX_BULLET_NUM;
    lastReloadTime = currentMillis;
    Serial.print("Reloaded! New bullet count: ");
    Serial.println(bulletNum);
  }

  // --- IR受信処理 ---
  if (IrReceiver.decode()) {
    if (IrReceiver.decodedIRData.protocol != UNKNOWN && IrReceiver.decodedIRData.command != DATA) {
      if (!isStunned) {
        isStunned = true;
        stunStartTime = currentMillis;
        Serial.println("!! HIT !! You are stunned!");
      }
    }
    IrReceiver.resume();
  }

  prevButtonState = currentButtonState;
  prevReloadState = currentReloadState;
  delay(10);
}