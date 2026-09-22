# RFID Smart Door Lock

An Arduino-based smart door lock system that uses **RFID authentication** to control access to a door. The system identifies registered RFID cards, displays the user's name on an OLED screen, and controls a servo motor to lock and unlock the door.

If an unauthorized card is scanned, the system denies access and keeps track of failed attempts. After three incorrect attempts, a security alarm is activated.

## Features

* RFID-based user authentication
* RC522 RFID reader
* Arduino Uno control
* Servo motor for door locking and unlocking
* OLED display for system messages
* Green LED for authorized access
* Red LED for denied access / locked state
* Buzzer for security alerts
* Displays the authorized user's name
* Counts incorrect RFID attempts
* Security alarm after 3 failed attempts
* Automatically locks the door after 5 seconds
* Resets the security attempt counter after successful access

## Hardware Used

| Component         | Purpose                      |
| ----------------- | ---------------------------- |
| Arduino Uno       | Main controller              |
| RC522 RFID Reader | Reads RFID cards             |
| RFID Cards/Tags   | User authentication          |
| Servo Motor       | Controls door lock           |
| OLED Display      | Displays system information  |
| Green LED         | Shows authorized access      |
| Red LED           | Shows locked/denied status   |
| Buzzer            | Provides warning/alarm sound |
| Jumper Wires      | Connections                  |
| Breadboard        | Circuit assembly             |

## Software / Libraries

The project is programmed using **Arduino C/C++**.

Required libraries:

* SPI
* MFRC522
* Wire
* Servo
* Adafruit GFX Library
* Adafruit SSD1306 Library

Make sure these libraries are installed in the Arduino IDE before uploading the program.

## How It Works

The system starts with the door in the locked position.

The OLED displays:

```text
<< SMART LOCK >>
RFID System Ready
Tap Card >>
```

The user then places an RFID card near the RC522 reader.

### 1. Authorized Card

The system compares the scanned RFID UID with the registered users.

If the UID matches:

```text
Welcome Ms. Saba
Access Approved
```

or:

```text
Welcome Ms. Laraib
Access Approved
```

The following actions are performed:

1. Green LED turns ON.
2. Red LED turns OFF.
3. Servo moves to the open position.
4. Door remains open for 5 seconds.
5. Servo returns to the locked position.
6. Green LED turns OFF.
7. Red LED turns ON.
8. The system returns to the ready state.

The failed-attempt counter is also reset after successful authentication.

### 2. Unauthorized Card

If the RFID card is not registered, the system displays:

```text
ACCESS DENIED !!
Invalid Card
Attempt: 1
```

The buzzer produces a warning sound and the red LED remains ON.

The failed-attempt counter increases each time an unauthorized card is scanned.

### 3. Security Alarm

If the user scans an unauthorized card **three times**, the security alarm is activated.

The OLED displays:

```text
!!! SECURITY ALERT !!!
Too Many Attempts
Alarm Activated
```

The red LED and buzzer repeatedly turn ON and OFF to indicate the security alert.

After the alarm sequence finishes, the failed-attempt counter is reset and the RFID system is initialized again.

## Registered Users

The current program contains two registered users:

| User       | RFID UID      |
| ---------- | ------------- |
| Ms. Saba   | `7D BD 00 07` |
| Ms. Laraib | `FA 44 28 07` |

> These UIDs are specific to the RFID cards used during development. If different cards are used, their UIDs must be added to the program.

## Pin Configuration

### RFID RC522

| RC522 Pin | Arduino Uno |
| --------- | ----------- |
| SDA / SS  | D10         |
| RST       | D9          |
| MOSI      | D11         |
| MISO      | D12         |
| SCK       | D13         |
| VCC       | 3.3V        |
| GND       | GND         |

### Other Components

| Component    | Arduino Pin |
| ------------ | ----------- |
| Servo Signal | D6          |
| Buzzer       | D7          |
| Red LED      | D4          |
| Green LED    | D5          |

### OLED

The OLED uses the **I2C interface**.

| OLED Pin | Arduino Uno |
| -------- | ----------- |
| SDA      | A4          |
| SCL      | A5          |
| VCC      | 5V          |
| GND      | GND         |

The OLED address used in the program is:

```text
0x3C
```

## Door Operation

The servo positions used in the current program are:

```cpp
doorServo.write(90);
```

for the locked position, and:

```cpp
doorServo.write(180);
```

for the unlocked position.

The door stays unlocked for:

```cpp
delay(5000);
```

which means approximately **5 seconds**.

These servo angles may need to be changed depending on the physical lock mechanism.

## Project Flow

```text
System Start
     |
     v
Initialize RFID, OLED, Servo and LEDs
     |
     v
Display "Tap Card"
     |
     v
RFID Card Scanned
     |
     v
Check RFID UID
     |
   +---+---+
   |       |
Valid    Invalid
   |       |
   v       v
Access   Access
Granted  Denied
   |       |
   v       v
Open    Increase
Door    Attempts
   |       |
   |     3 Attempts?
   |       |
   |      Yes
   |       |
   |       v
   |     Alarm
   |       |
   +---<---+
     |
     v
System Ready
```

## Installation

### 1. Install Arduino IDE

Install the Arduino IDE on your computer.

### 2. Connect the Hardware

Connect the Arduino Uno with the RC522, servo, OLED, LEDs, and buzzer according to the pin configuration.

### 3. Install Required Libraries

Install the following libraries through the Arduino IDE Library Manager:

* MFRC522
* Adafruit GFX Library
* Adafruit SSD1306

The following libraries are normally included with the Arduino environment:

* SPI
* Wire
* Servo

### 4. Open the Project

Open:

```text
RFID-Smart-Door-Lock.ino
```

in Arduino IDE.

### 5. Select Board

Select:

```text
Arduino Uno
```

and choose the correct COM port.

### 6. Upload

Connect the Arduino Uno to your computer and upload the program.

## Customizing Users

To add or change registered RFID cards, modify the users database in the code:

```cpp
byte users[totalUsers][4] = {
  {0x7D, 0xBD, 0x00, 0x07},
  {0xFA, 0x44, 0x28, 0x07}
};
```

Then update the corresponding names:

```cpp
String names[totalUsers] = {
  "Ms. Saba",
  "Ms. Laraib"
};
```

Make sure that each RFID UID has the correct number of bytes.

## Security Logic

The project uses a simple failed-attempt security mechanism:

```text
1 wrong attempt  → Warning
2 wrong attempts → Warning
3 wrong attempts → Security alarm
```

After the alarm completes, the counter is reset.

A successful RFID scan also resets the failed-attempt counter.

## Project Structure

A recommended GitHub structure is:

```text
RFID-Smart-Door-Lock/
│
├── RFID-Smart-Door-Lock.ino
├── README.md
│
├── circuit/
│   └── circuit-diagram.png
│
├── images/
│   ├── project.jpg
│   ├── oled.jpg
│   └── circuit.jpg
│
└── documentation/
    └── project-report.pdf
```

## Future Improvements

Possible improvements for future versions include:

* Adding more RFID users
* Storing users in EEPROM
* Adding an admin/master RFID card
* Adding a keypad as a second authentication method
* Adding a real-time clock
* Adding an automatic door sensor
* Adding Wi-Fi or Bluetooth monitoring
* Sending security notifications
* Storing access logs
* Adding a mobile application
* Adding a rechargeable battery system

## Author

**Aliyan Hussain**

Arduino / Embedded Systems Project

## License

This project is available for educational and learning purposes.
