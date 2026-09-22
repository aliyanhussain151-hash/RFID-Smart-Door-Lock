#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);
Servo doorServo;

#define BUZZER 7
#define RED_LED 4
#define GREEN_LED 5

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int wrongAttempts = 0;

/* ---------- USERS DATABASE ---------- */
const int totalUsers = 2;

byte users[totalUsers][4] = {
  {0x7D, 0xBD, 0x00, 0x07},   // Card 1
  {0xFA, 0x44, 0x28, 0x07}    // Card 2
};

String names[totalUsers] = {
  "Aliyan",
  "Ahmed"
};
/* ----------------------------------- */

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();

  doorServo.attach(6);

  pinMode(BUZZER, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found");
    while(true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("RFID Door Lock");
  display.println("Scan Card...");
  display.display();

  doorServo.write(0);
}

/* ------------ MAIN LOOP ------------ */
void loop() {
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  Serial.print("Card UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  int userIndex = checkUID();

  if(userIndex >= 0) {
    accessGranted(userIndex);
  } else {
    accessDenied();
  }

  // Halt PICC and stop crypto
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  // Wait until card is removed
  while(rfid.PICC_IsNewCardPresent() || rfid.PICC_ReadCardSerial()) {
    delay(50);
  }

  // Re-initialize reader to fix post-alarm issue
  rfid.PCD_Init();
}

/* -------- CHECK CARD FUNCTION -------- */
int checkUID() {
  for(int i = 0; i < totalUsers; i++) {
    bool match = true;

    if(rfid.uid.size != 4) continue;

    for(int j = 0; j < rfid.uid.size; j++) {
      if(rfid.uid.uidByte[j] != users[i][j]) {
        match = false;
        break;
      }
    }

    if(match) return i;
  }
  return -1;
}

/* -------- ACCESS GRANTED -------- */
void accessGranted(int userIndex) {
  wrongAttempts = 0;

  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, LOW);

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Access Granted");
  display.print("Welcome ");
  display.println(names[userIndex]);
  display.println("Door Opening");
  display.display();

  doorServo.write(90);
  delay(1000);
  doorServo.write(0);

  digitalWrite(GREEN_LED, LOW);

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Scan Card...");
  display.display();
}

/* -------- ACCESS DENIED -------- */
void accessDenied() {
  wrongAttempts++;

  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Access Denied");
  display.print("Attempt: ");
  display.println(wrongAttempts);
  display.display();

  tone(BUZZER,1000);
  delay(500);
  noTone(BUZZER);

  digitalWrite(RED_LED, LOW);

  if(wrongAttempts >= 3) {
    alarmSystem();
  }

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Scan Card...");
  display.display();
}

/* -------- ALARM SYSTEM -------- */
void alarmSystem() {
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("WARNING!");
  display.println("3 Wrong Attempts");
  display.println("ALARM ACTIVE");
  display.display();

  for(int i = 0; i < 10; i++) {
    digitalWrite(RED_LED, HIGH);
    tone(BUZZER,2000);
    delay(300);
    digitalWrite(RED_LED, LOW);
    noTone(BUZZER);
    delay(300);
  }

  wrongAttempts = 0;

  // Reinitialize RFID reader after alarm
  rfid.PCD_Init();

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Scan Card...");
  display.display();
}