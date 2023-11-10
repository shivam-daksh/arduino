// Code that was used to demonstrate in IOTRON 2.0
//Code is working fine 
// Buzzer Pin(13,GND)
// 4 pin RGB LED (Anode)
//    Biggest pin     to        3.3V
//    (End pin adjac  to        9 - Digital (PWM)
//    -ent to
//    biggest pin ) 
//    (middle and     to        10 - Digital (PWM)
//    adjacent to
//    biggest pin)
//    other end pin   to        11 - Digital (PWM)

// Fingerprint Sensor:      
//    1(red)          to        5V
//    2(black)        to        GND
//    3(yellow)       to        2 - Digital
//    4(green)        to        3 - Digital
//    5(blue)         to        unconnected
//    6(white)        to        unconnected

//BT Module Wiring
//    VCC             to        5V
//    GND             to        GND
//    TXD             to        RX(0)               (Note: While uploading code, Always remove RX-0 pin from Arduino)
//    RXD             to        TX(1)


#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3);  // SoftwareSerial for fingerprint sensor
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
int buzzerPin = 13;  // Pin connected to the buzzer

int redPin = 9;    // Connect the red pin to digital pin 9
int greenPin = 10;  // Connect the green pin to digital pin 10
int bluePin = 11;   // Connect the blue pin to digital pin 11

enum State {
  SETTING,
  MATCHING,
  ADDING_FINGERPRINT,
  DELETING_FINGERPRINT,
};

State currentState = MATCHING;

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  setColor(0,0,0);
  Serial.begin(9600);  // Serial monitor
  while (!Serial)
    ;  // Wait for serial connection
  Serial.println("\n\nFingerprint Sensor with Bluetooth");

  mySerial.begin(57600);  // Fingerprint sensor baud rate

  finger.begin(57600);  // You may need to adjust this baud rate to match your sensor's rate
  pinMode(buzzerPin, OUTPUT);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.println("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  } else {
    Serial.println("Sensor contains " + String(finger.templateCount) + " templates");
  }
    Serial.println("Enter '0', 's' or 'S' to go to setting:");

}
char id;
void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    if(c == '0' || c == 's' || c =='S'){
      Serial.println("Enter the number to go to specific setting:");
      Serial.println("1. Add a fingerprint");
      Serial.println("2. Delete a fingerprint");
      Serial.println("3. Exit");
      currentState == SETTING;

    }
    else {
      currentState = MATCHING; 
    } 
  }


  if(currentState == SETTING){
    displaySettings();
  }
  else if (currentState == MATCHING) {
    getFingerprintID();
  } else if (currentState == ADDING_FINGERPRINT) {
    addFingerprint();
  } else if (currentState == DELETING_FINGERPRINT) {
    deleteFingerprint();
  }
  checkBluetoothSerial();  // Check for Bluetooth communication
  delay(50);
}

char readNumber() {
  while (!Serial.available()); // Wait for input
  return Serial.read(); // Read the number from Serial Monitor
}

void getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      // No finger detected, do nothing
      return;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return;
    default:
      Serial.println("Unknown error");
      return;
  }

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return;
    default:
      Serial.println("Unknown error");
      return;
  }

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
    playMatchedTune();
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    playNotMatchedTune();
  } else {
    Serial.println("Unknown error");
  }

  delay(1000);
      if (Serial.available()) {
        char c = Serial.read();
        if(c == '1'){
          Serial.println("Press 0 to go to setting");
          currentState == ADDING_FINGERPRINT;
          return;
        }
        else if(c=='2'){
          currentState = DELETING_FINGERPRINT; 
          return;
        } else if(c=='3'){
          currentState = MATCHING;
          return;
        }
  }
}

// void addFingerprint() {
//   Serial.println("Adding fingerprint. Place your finger on the sensor...");
//   Serial.println("Enter the ID # (from 1 to 127) you want to save this finger as...");
//   uint8_t id = readNumber();
//   if (id == 0) {
//     Serial.println("ID #0 is not allowed, try again!");
//     return;
//   }

//   Serial.print("Enrolling ID #");
//   Serial.println(id);

//   if (finger.storeModel(id) == FINGERPRINT_OK) {
//     Serial.println("Enrollment successful.");
//   } else {
//     Serial.println("Enrollment failed. Please try again.");
//   }

//   currentState = MATCHING; // Return to matching state
// }
uint8_t addFingerprint() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }
      if (Serial.available()) {
        char c = Serial.read();
        if(c == '1'){
          currentState == ADDING_FINGERPRINT;
          return;
        }
        else if(c=='2'){
          currentState = DELETING_FINGERPRINT; 
          return;
        } else if(c=='3'){
          currentState = MATCHING;
          return;
        }
  }
  return true;
}

void deleteFingerprint() {
  Serial.println("Deleting fingerprint. Enter the ID # of the fingerprint to delete... \n Press 'E' or 'e' to exit. ");
  char id = readNumber();
  if (id == 0) {
    Serial.println("ID #0 is not allowed, try again!");
    return;
  } else if (id=='e' || id=='E'){
    currentState = MATCHING;
    return;
  }

  Serial.print("Deleting ID #");
  Serial.println(id);
  
  uint8_t p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    Serial.println("Deleted!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
  } else {
    Serial.print("Unknown error: 0x");
    Serial.println(p, HEX);
  }

}
void displaySettings(){
    Serial.println("Enter the number to go to specific setting:");
    Serial.println("1. Add a fingerprint");
    Serial.println("2. Delete a fingerprint");
    Serial.println("3. Exit");
    if (Serial.available()) {
        char c = Serial.read();
        if(c == '1'){
          currentState == ADDING_FINGERPRINT;
          return;
        }
        else if(c=='2'){
          currentState = DELETING_FINGERPRINT; 
          return;
        } else if(c == '0' || c == 's' || c =='S'){
          currentState = SETTING;
          return;
        }
  }

}
void playNotMatchedTune() {
  setColor(255, 155, 0);
  tone(buzzerPin, 1000);  // Play a tone for a matched fingerprint
  delay(1000);
  noTone(buzzerPin);
    setColor(0, 0, 0);
}

void playMatchedTune() {
  setColor(0, 255, 0);
  tone(buzzerPin, 2000);  // Play a different tone for a not matched fingerprint
  delay(300);
  noTone(buzzerPin);
  delay(100);
  tone(buzzerPin, 500);  // Play a different tone for a not matched fingerprint
  delay(500);
  noTone(buzzerPin);
  setColor(0, 0, 0);
}

void checkBluetoothSerial() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '1') {
      currentState = ADDING_FINGERPRINT;
      Serial.println("Adding fingerprint. Place your finger on the sensor...");
    } else if (c == '2') {
      currentState = DELETING_FINGERPRINT;
      // Serial.println("Deleting fingerprint. Enter the ID # of the fingerprint to delete...");
    } else if (c == '3') {
      currentState = MATCHING;
      Serial.println("Exiting setup. Resuming fingerprint matching.");
    }
  }

  while (mySerial.available()) {
    char c = mySerial.read();
    // Send any data received from the fingerprint sensor to the Bluetooth terminal
    Serial.write(c);
  }
}
void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(redPin, 255-redValue);
  analogWrite(greenPin, 255-greenValue);
  analogWrite(bluePin, 255-blueValue);
}
