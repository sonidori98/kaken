#include <IRremote.hpp>

#define IR_SEND_PIN 10      // IR送信ピン
const uint8_t BUTTON_PIN = 11;  // 攻撃ボタン入力ピン
const uint8_t RECV_PIN = 13;    // IR受信ピン
const uint8_t LED_PIN = 9;      // LED出力ピン（ヒット表示用）
const uint8_t REROAD_PIN = 12;  // リロードボタンピン

const unsigned long STUN_DURATION_MS = 5000;    // スタン時間
const unsigned long RELOAD_COOLDOWN_MS = 5000;  // リロードのクールタイム
const uint8_t MAX_BULLET_NUM = 9;              // 弾数の最大値

// 7セグに表示するパターン
const int DIGIT_PATTERN[10][7] = {
  { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, LOW },  // 0
  { LOW, HIGH, HIGH, LOW, LOW, LOW, LOW },      // 1
  { HIGH, HIGH, LOW, HIGH, HIGH, LOW, HIGH },   // 2
  { HIGH, HIGH, HIGH, HIGH, LOW, LOW, HIGH },   // 3
  { LOW, HIGH, HIGH, LOW, LOW, HIGH, HIGH },    // 4
  { HIGH, LOW, HIGH, HIGH, LOW, HIGH, HIGH },   // 5
  { HIGH, LOW, HIGH, HIGH, HIGH, HIGH, HIGH },  // 6
  { HIGH, HIGH, HIGH, LOW, LOW, LOW, LOW },     // 7
  { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH }, // 8
  { HIGH, HIGH, HIGH, HIGH, LOW, HIGH, HIGH }   // 9
};

const int LOADING_PATTERN[6][7] = {
  { HIGH, LOW, LOW, LOW, LOW, LOW, LOW },   // 1
  { LOW, HIGH, LOW, LOW, LOW, LOW, LOW },   // 2
  { LOW, LOW, HIGH, LOW, LOW, LOW, LOW },   // 3
  { LOW, LOW, LOW, HIGH, LOW, LOW, LOW },   // 4
  { LOW, LOW, LOW, LOW, HIGH, LOW, LOW },   // 5
  { LOW, LOW, LOW, LOW, LOW, HIGH, LOW }    // 6
};

const int pinNo[7] = { 2, 3, 4, 5, 6, 7, 8 };

// 7セグ制御用
const long interval = 80;
unsigned long lastLEDUpdateTime = 0;
int currentSeg = 0;

unsigned long stunStartTime = 0;
unsigned long lastReloadTime = 0;

bool isStunned = false;
uint8_t bulletNum;

bool prevButtonState = HIGH;
bool prevReloadState = HIGH;

// 追加する状態変数
bool isReloading = false; // リロード中かどうかを示すフラグ

void displayDigit(int digit, const int pattern[][7]) {
  for (int i = 0; i < 7; i++) {
    digitalWrite(pinNo[i], !pattern[digit][i]);
  }
}

void displayLoading() {
  unsigned long currentTime = millis();
  if (currentTime - lastLEDUpdateTime >= interval) {
    lastLEDUpdateTime = currentTime;
    displayDigit(currentSeg, LOADING_PATTERN);
    currentSeg++;
    if (currentSeg >= 6) {
      currentSeg = 0; // アニメーションを繰り返す
    }
  }
}

void setup() {
  bulletNum = MAX_BULLET_NUM;

  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(REROAD_PIN, INPUT_PULLUP);

  IrReceiver.begin(RECV_PIN, true);
  IrSender.begin(IR_SEND_PIN, true, 0);

  for (int i = 0; i < 7; i++) {
    pinMode(pinNo[i], OUTPUT);
    digitalWrite(pinNo[i], HIGH);
  }

  Serial.println("Arduino IR Game Ready!");
}

void loop() {
  unsigned long currentTime = millis(); // 現在時刻を一度取得

  // リロードクールタイムが終了したらisReloadingフラグを解除
  if (isReloading && (currentTime - lastReloadTime >= RELOAD_COOLDOWN_MS)) {
    isReloading = false;
    currentSeg = 0; // リロードアニメーションの状態をリセット
    Serial.println("Reload cooldown ended. Ready to fire!");
  }

  // 7セグの表示ロジック
  if (isReloading) {
    displayLoading(); // リロード中はローディングアニメーションを表示
  } else {
    displayDigit(bulletNum, DIGIT_PATTERN); // 通常は弾数を表示
  }

  // スタン状態解除
  if (isStunned && (currentTime - stunStartTime >= STUN_DURATION_MS)) {
    isStunned = false;
    digitalWrite(LED_PIN, LOW);
    Serial.println("Stun duration ended. You can move again!");
  }

  // ボタン状態読み取り
  bool currentButtonState = digitalRead(BUTTON_PIN);
  bool currentReloadState = digitalRead(REROAD_PIN);

  // 攻撃ボタン（押された瞬間） & クールタイム外 & スタン状態でない & 弾がある
  if (prevButtonState == HIGH && currentButtonState == LOW && !isStunned && bulletNum > 0 && !isReloading) {
    bulletNum--;
    IrSender.sendNEC(0x6380, 0xF, 0);
    Serial.print("Fired! Bullets left: ");
    Serial.println(bulletNum);

  } else if (prevButtonState == HIGH && currentButtonState == LOW && isReloading) {
    Serial.println("Cannot attack during reload cooldown.");
  } else if (prevButtonState == HIGH && currentButtonState == LOW && bulletNum == 0) {
    Serial.println("Out of bullets! Reload first.");
  }


  // リロードボタン（押された瞬間 & クールタイム終了）
  if (prevReloadState == HIGH && currentReloadState == LOW && !isReloading) { // isReloadingを使って二重リロード防止
    bulletNum = MAX_BULLET_NUM;
    lastReloadTime = currentTime;   // リロード時間記録
    isReloading = true;             // リロード開始フラグを立てる
    currentSeg = 0;                 // アニメーションを最初から開始
    Serial.print("Reloading... New bullet count: ");
    Serial.println(bulletNum);
    
  } else if (prevReloadState == HIGH && currentReloadState == LOW && isReloading) {
    Serial.println("Already reloading. Please wait...");
  }


  // 被弾処理（IR受信）
  if (IrReceiver.decode()) {
    if (IrReceiver.decodedIRData.protocol != UNKNOWN) {
      isStunned = true;
      stunStartTime = currentTime;
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