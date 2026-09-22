#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* RFID PINS */
#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);

/* SERVO */
Servo doorServo;

/* OUTPUT DEVICES */
#define BUZZER 7
#define RED_LED 4
#define GREEN_LED 5

/* OLED */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/* VARIABLES */
int wrongAttempts = 0;

/* USERS DATABASE */
const int totalUsers = 2;

byte users[totalUsers][4] = {
  {0x7D, 0xBD, 0x00, 0x07},
  {0xFA, 0x44, 0x28, 0x07}
};

String names[totalUsers] = {
  "Ms. Saba",
  "Ms. Laraib"
};
void setup() {

  Serial.begin(115200);

  SPI.begin();
  rfid.PCD_Init();

  doorServo.attach(6);

  pinMode(BUZZER, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED NOT FOUND");
    while(true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  /* START SCREEN */

  display.setCursor(0,0);
  display.println("<< SMART LOCK >>");
  display.println("RFID System Ready");
  display.println("Tap Card >>");
  display.display();

  /* INITIAL DOOR STATE */

  doorServo.write(90);   // CLOSED POSITION
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
}

/* MAIN LOOP */

void loop() {

  if (!rfid.PICC_IsNewCardPresent()) return;

  if (!rfid.PICC_ReadCardSerial()) return;

  int userIndex = checkUID();

  if(userIndex >= 0) {
    accessGranted(userIndex);
  }
  else {
    accessDenied();
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

/* CHECK CARD */

int checkUID() {

  for(int i=0;i<totalUsers;i++) {

    bool match = true;

    if(rfid.uid.size != 4) continue;

    for(int j=0;j<4;j++) {

      if(rfid.uid.uidByte[j] != users[i][j]) {
        match = false;
        break;
      }
    }

    if(match)
      return i;
  }

  return -1;
}

/* ACCESS GRANTED */

void accessGranted(int userIndex) {

  wrongAttempts = 0;

  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, LOW);

 display.clearDisplay();
display.setCursor(0,0);
display.println("Welcome " + names[userIndex]);   // Shows: Welcome Ms. Saba
display.println("Access Approved");               // Shows: Access Approved
display.display();

  /* OPEN DOOR */

  doorServo.write(180);
  // Change this angle if your door requires different position

  delay(5000);   // Door open time = 5 seconds

  /* CLOSE DOOR */

  doorServo.write(90);
  // Change this angle if needed for closed position

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, HIGH);

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Door Secured [LOCK]");
  display.println("System Ready >>");
  display.println("Tap Card >>");
  display.display();
}

/* ACCESS DENIED */

void accessDenied() {

  wrongAttempts++;

  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);

  display.clearDisplay();
  display.setCursor(0,0);

  display.println("ACCESS DENIED !!");
  display.println("Invalid Card");
  display.print("Attempt: ");
  display.println(wrongAttempts);
  display.display();

  tone(BUZZER,1000);
  delay(500);
  noTone(BUZZER);

  if(wrongAttempts >= 3) {
    alarmSystem();
  }
}

/* ALARM SYSTEM */

void alarmSystem() {

  display.clearDisplay();
  display.setCursor(0,0);

  display.println("!!! SECURITY ALERT !!!");
  display.println("Too Many Attempts");
  display.println("Alarm Activated");
  display.display();

  for(int i=0;i<10;i++) {

    digitalWrite(RED_LED, HIGH);
    tone(BUZZER,2000);
    delay(300);

    digitalWrite(RED_LED, LOW);
    noTone(BUZZER);
    delay(300);
  }

  wrongAttempts = 0;

  rfid.PCD_Init();

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Security Reset");
  display.println("Tap Card >>");
  display.display();
}