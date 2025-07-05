#include <IRremote.hpp>

#define IR_SEND_PIN 4 // IR送信ピン
const uint8_t BUTTON_PIN = 7;     // 攻撃ボタン入力ピン
const uint8_t RECV_PIN = 8;       // IR受信ピン
const uint8_t LED_PIN = 9;        // LED出力ピン（ヒット表示用）
const uint8_t REROAD_PIN = 12;    // リロードボタンピン

const unsigned long STUN_DURATION_MS = 5000;           // スタン時間
const unsigned long RELOAD_COOLDOWN_MS = 5000;         // リロードのクールタイム
const uint8_t MAX_BULLET_NUM = 17;                      // 弾数の最大値

unsigned long stunStartTime = 0;
unsigned long lastReloadTime = 0;

bool isStunned = false;
uint8_t bulletNum;

bool prevButtonState = HIGH;
bool prevReloadState = HIGH;

void setup() {
  bulletNum = MAX_BULLET_NUM;

  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(REROAD_PIN, INPUT_PULLUP);

  IrReceiver.begin(RECV_PIN, true);
  IrSender.begin(IR_SEND_PIN, true, 0);

  Serial.println("Arduino IR Game Ready!");
}

void loop() {
  // スタン状態解除
  if (isStunned && (millis() - stunStartTime >= STUN_DURATION_MS)) {
    isStunned = false;
    digitalWrite(LED_PIN, LOW);
    Serial.println("Stun duration ended. You can move again!");
  }

  // ボタン状態読み取り
  bool currentButtonState = digitalRead(BUTTON_PIN);
  bool currentReloadState = digitalRead(REROAD_PIN);

  // 攻撃ボタン（押された瞬間） & クールタイム外
  if (prevButtonState == HIGH && currentButtonState == LOW &&
      !isStunned &&
      bulletNum > 0 &&
      (millis() - lastReloadTime >= RELOAD_COOLDOWN_MS)) {

    bulletNum--;
    IrSender.sendNEC(0x6380, 0xF, 0);
    Serial.print("Fired! Bullets left: ");
    Serial.println(bulletNum);

  } else if (prevButtonState == HIGH && currentButtonState == LOW &&
             (millis() - lastReloadTime < RELOAD_COOLDOWN_MS)) {

    Serial.println("Cannot attack during reload cooldown.");
  }

  // リロードボタン（押された瞬間 & クールタイム終了）
  if (prevReloadState == HIGH && currentReloadState == LOW &&
      (millis() - lastReloadTime >= RELOAD_COOLDOWN_MS)) {
    bulletNum = MAX_BULLET_NUM;
    lastReloadTime = millis(); // リロード時間記録
    Serial.print("Reloaded! New bullet count: ");
    Serial.println(bulletNum);
  } else if (prevReloadState == HIGH && currentReloadState == LOW) {
    Serial.println("Reload on cooldown. Please wait...");
  }

  // 被弾処理（IR受信）
  if (IrReceiver.decode()) {
    if (IrReceiver.decodedIRData.protocol != UNKNOWN) {
      isStunned = true;
      stunStartTime = millis();
      digitalWrite(LED_PIN, HIGH);
      
      Serial.print("Hit! Received: ");
      IrReceiver.printIRResultShort(&Serial);
      Serial.println("You are stunned!");
    }
    IrReceiver.resume();
  }

  // 状態更新
  prevButtonState = currentButtonState;
  prevReloadState = currentReloadState;

  delay(10);
}
