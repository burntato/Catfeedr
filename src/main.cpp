#include <Wire.h>
#include <RTClib.h>
#include <Servo.h>
#include <Ultrasonic.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>

#define TRIGGER_PIN 12
#define ECHO_PIN 13
#define SERVO_1_PIN D5
#define SERVO_2_PIN D6
#define WATER_LEVEL_PIN A0

#define SERVO_1_OPEN_ANGLE 45
#define SERVO_1_CLOSE_ANGLE 0
#define SERVO_2_FEED_ANGLE 180
#define SERVO_2_RETURN_ANGLE 0

Servo servo1;
Servo servo2;
RTC_DS3231 rtc;
Ultrasonic ultrasonic1(TRIGGER_PIN, ECHO_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2); 

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();
  servo1.attach(SERVO_1_PIN);
  servo2.attach(SERVO_2_PIN);
  pinMode(WATER_LEVEL_PIN, INPUT);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Current Time:");
}

void loop()
{
  long depth = ultrasonic1.read();

  // RTC
  DateTime now = rtc.now();

  if (depth >= 30 && (now.hour() == 7 || now.hour() == 11 || now.hour() == 17))
  {
    // 3s
    servo1.write(SERVO_1_OPEN_ANGLE);
    delay(3000);
    servo1.write(SERVO_1_CLOSE_ANGLE);
  }

  // water level
  int waterLevel = analogRead(WATER_LEVEL_PIN);

  if (waterLevel == 100)
  {
    // 5s
    servo2.write(SERVO_2_FEED_ANGLE);
    delay(5000);
    servo2.write(SERVO_2_RETURN_ANGLE);
  }

  lcd.setCursor(0, 1);
  lcd.print(now.timestamp(DateTime::TIMESTAMP_TIME));

  lcd.setCursor(0, 2);
  lcd.print("Next feeding time is");

  if (now.hour() < 7)
  {
    lcd.print(" 7:00 AM");
  }
  else if (now.hour() < 11)
  {
    lcd.print(" 11:00 AM");
  }
  else if (now.hour() < 17)
  {
    lcd.print(" 5:00 PM");
  }
  else
  {
    lcd.print(" 7:00 AM");
    lcd.print(now.day() + 1);
  }

  delay(1000);
}
