#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <DS1302.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial1);

const int RST = 36;
const int DATA = 37;
const int CLOCK = 38;
DS1302 rtc(RST, DATA, CLOCK);
uint8_t id;
uint8_t n=1;

int month[] = {1,2,3,4,5,6,7,8,9,10,11,12};
String noti[] = {"schoolbrth","party","1st exam","pres","child day","sport day","2nd exam","festival","graduate","grad party","park","beach"};

int buzzer1 = 33;
int buzzer2 = 31;
int btn = 32;
int btn2 = 34;
int led1=12;
int led2=26;
int led3=6;
int led4=5;
int led5=8;
int led6=11;
int led7=27;
int led8=3;
int led9=28;
int led10=13;
int flag = 0;
String tt = "";
uint8_t nm = 0;
void setup() {
  while (!Serial);
  delay(100);

  // rtc.writeProtect(false);
  // rtc.halt(false);
  // Time t(2024, 7,7,17,55,00,Time::kMonday);
  // rtc.time(t);
  
  pinMode(buzzer1, OUTPUT);
  pinMode(buzzer2, OUTPUT);
  pinMode(btn, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  pinMode(led6, OUTPUT);
  pinMode(led7, OUTPUT);
  pinMode(led8, OUTPUT);
  pinMode(led9, OUTPUT);
  pinMode(led10, OUTPUT);

  lcd.init();

  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  else {
    Serial.println("Waiting for valid finger...");
      Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }
  Serial.begin(9600);
  Serial2.begin(9600);
}

uint8_t add(void){
  return n;
}


uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

//rtc(시간측정기기)
void resetLed(void){
  Time t = rtc.time();
  Serial.println();
  Serial.print(t.hr);
  Serial.print(" : ");
  Serial.print(t.min);
  Serial.print(" : ");
  Serial.println(t.sec);

  String s = (String)t.mon+" "+noti[t.mon-1];
  //Serial.println(s);
  lcd.setCursor(0,0);
  lcd.print(s);


//시간 조정 필요
  if(t.hr==17 && t.min ==57 && t.sec == 0){
    digitalWrite(led1,HIGH);
    digitalWrite(led2,HIGH);
    digitalWrite(led3,HIGH);
    digitalWrite(led4,HIGH);
    digitalWrite(led5,HIGH);
    digitalWrite(led6,HIGH);
    digitalWrite(led7,HIGH);
    digitalWrite(led8,HIGH);
    digitalWrite(led9,HIGH);
    digitalWrite(led10,HIGH);
  }
}
void loop() {
  lcd.backlight();
  resetLed();
//안전 버튼
  if(digitalRead(btn)==LOW){
    Serial.println("ON button clicked");
    digitalWrite(buzzer1, HIGH);
    digitalWrite(buzzer2, HIGH);
  }
  if(digitalRead(btn2)==LOW){
    Serial.println("OFF button clicked");
    digitalWrite(buzzer1, LOW);
    digitalWrite(buzzer2, LOW);
  }

//앱인벤터
  if(Serial2.available())
  {
    String bt = Serial2.readString();

    int pos = bt.indexOf(',');
    if(pos != -1)
    {
      tt = bt.substring(0,pos);
      bt = bt.substring(pos+1);
      nm = bt.toInt();
    }

    if(tt == "a") flag=1;
    else if(tt == "d") flag=2;

    Serial.println(tt);
    Serial.println(nm);
    
  }



  if(flag==1)
  {
    Serial.println("Ready to enroll a fingerprint!");
    Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
    // id = readnumber();
    id = nm;
    if (id == 0) {// ID #0 not allowed, try again!
      return;
    }
    Serial.print("Enrolling ID #");
    Serial.println(id);

    while (!  getFingerprintEnroll() );
    delay(5000);
  }else if(flag==2)
  {
    Serial.println("Please type in the ID # (from 1 to 127) you want to delete...");
    uint8_t id = nm;
    if (id == 0) {// ID #0 not allowed, try again!
     return;
    }

    Serial.print("Deleting ID #");
    Serial.println(id);

    deleteFingerprint(id);
  }else if(flag==0)
  {
    getFingerprintID();
    delay(500);
  }
}

//지문추가
uint8_t getFingerprintEnroll() {

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
    flag = 0;
    nm = 0;
    Serial2.write('p');
    lcd.backlight();
    lcd.setCursor(0,1);
    lcd.print(finger.fingerID);
    lcd.setCursor(2,1);
    lcd.print("added");
    delay(2000);
    lcd.clear();
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
  
  return true;
}

//지문삭제
uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;

  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    Serial.println("Deleted!");
    flag=0;
    nm = 0;
    Serial2.write('z');
    lcd.backlight();
    lcd.setCursor(0,1);
    lcd.print(finger.fingerID);
    lcd.setCursor(2,1);
    lcd.print("deleted");
    delay(2000);
    lcd.clear();
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
  } else {
    Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
  }

  return p;
}

//지문체크
uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
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
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  lcd.backlight();
  lcd.setCursor(0,1);
  lcd.print(finger.fingerID);
  lcd.setCursor(2,1);
  lcd.print("checked");
  delay(2000);
  lcd.clear();

  if(finger.fingerID == 1)
  {
    digitalWrite(led1,LOW);
    Serial2.write('1');
  }
  else if(finger.fingerID == 2)
  {
    digitalWrite(led2,LOW);
    Serial2.write('2');
  }
  else if(finger.fingerID == 3)
  {
    digitalWrite(led3,LOW);
    Serial2.write('3');
  }
  else if(finger.fingerID == 4)
  {
    digitalWrite(led4,LOW);
    Serial2.write('4');
  }
  else if(finger.fingerID == 5)
  {
    digitalWrite(led5,LOW);
    Serial2.write('5');
  }else if(finger.fingerID == 6)
  {
    digitalWrite(led6,LOW);
    Serial2.write('6');
  }else if(finger.fingerID == 7)
  {
    digitalWrite(led7,LOW);
    Serial2.write('7');
  }  else if(finger.fingerID == 8)
  {
    digitalWrite(led8,LOW);
    Serial2.write('8');
  }  else if(finger.fingerID == 9)
  {
    digitalWrite(led9,LOW);
    Serial2.write('9');
  }  else if(finger.fingerID == 10)
  {
    digitalWrite(led10,LOW);
    Serial2.write('10');
  }
  //Serial2.print(finger.fingerID);

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}


