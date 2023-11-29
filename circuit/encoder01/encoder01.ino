// Example simulation encoder

#include <LiquidCrystal_I2C.h>
#include "SHT2x.h"
#include <Encoder.h>
#include <I2C_RTC.h>
#include <SPI.h>
#include <SD.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  //set up LCD
SHT2x sht;
Encoder myEnc(2, 3);
static DS1307 RTC;
const int chipSelect = 4;

#define button 6

int counter = 50;
int aState;
int aLastState;

long oldPosition = -999;

// the follow variables is a long because the time, measured in miliseconds
// will quickly become a bigger number than can be stored in an int.

long interval = 100;  // interval at which to blink (milliseconds)
void setup() {
  //pinMode(outputA, INPUT);
  //pinMode(outputB, INPUT);
  pinMode(button, INPUT);

  // Reads the initial state of the outputA
  //aLastState = digitalRead(outputA);
 //lcd.begin();
  lcd.init();

  //sht.begin();

  RTC.begin();

  /*uint8_t stat = sht.getStatus();
  lcd.setCursor(7, 1);
  lcd.print(stat, HEX);*/
  //Serial.print(stat, HEX);

  /*lcd.setCursor(0, 0);
  lcd.print("Is Clock Running : ");
  if (RTC.isRunning()) {
    lcd.setCursor(0, 1);
    lcd.println("Yes");
  } else {
    lcd.setCursor(0, 1);
    lcd.println("No. Time may be Inaccurate");
  }

  delay(1000);
  lcd.setCursor(0, 0);
  lcd.print("Hour Mode : ");
  lcd.setCursor(0, 1);
  if (RTC.getHourMode() == CLOCK_H24)
    lcd.println("24 Hours");
  else
    lcd.println("12 Hours");
  delay(1000);
  lcd.setCursor(0, 0);
  lcd.print("Is Out Pin Enabled : ");
  lcd.setCursor(0, 1);
  if (RTC.isOutPinEnabled())
    lcd.println("Yes");
  else
    lcd.println("No");
  delay(1000);
  lcd.setCursor(0, 0);
  lcd.setCursor(0, 1);
  lcd.print("Is SQWE Enabled : ");
  if (RTC.isSqweEnabled())
    lcd.println("Yes");
  else
    lcd.println("No");

  delay(1000);*/

  lcd.setCursor(0, 0);
  lcd.print("Initializing SD card...");
  
  // see if the card is present and can be initialized:
  lcd.setCursor(0, 1);
  if (!SD.begin(chipSelect)) {
    lcd.print("Card failed, or not present");
    // don't do anything more:
    while (1)
      ;
  }
  lcd.println("card initialized.");
  lcd.clear();
}
void loop() {

  //lcd.setCursor(1, 1);
  //lcd.print(SHT2x_LIB_VERSION);
  //lcd.clear();
  /* sht.read();

  lcd.setCursor(9, 1);
  lcd.print("T: ");
  lcd.print(sht.getTemperature(), 2);

  lcd.setCursor(9, 0);
  lcd.print("H: ");
  lcd.print(sht.getHumidity(), 2);

  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    //Serial.println(newPosition);
    lcd.setCursor(0, 0);
    lcd.print(newPosition);
  }
*/

  /*lcd.setCursor(0, 0);
  lcd.print(RTC.getDay());
  lcd.print("-");
  lcd.print(RTC.getMonth());
  lcd.print("-");
  lcd.print(RTC.getYear());

  lcd.setCursor(0, 1);
  lcd.print(RTC.getHours());
  lcd.print(":");
  lcd.print(RTC.getMinutes());
  lcd.print(":");
  lcd.print(RTC.getSeconds());
  delay(1000);*/

  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  // if the file is available, write to it:
  // make a string for assembling the data to log:
  String dataString = "";
  lcd.setCursor(0, 1);
  if (dataFile) {
    dataString = String(RTC.getHours()) + ":" + String(RTC.getMinutes()) + ":" + String(RTC.getSeconds());
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    lcd.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    lcd.println("error opening datalog.txt");
  }
}
