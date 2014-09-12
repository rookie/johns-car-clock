/***************************************************
  This sketch is a replacement clock for a 1987 Nissan 300zx
  It uses the Adafruit 1.8" TFT LCD, and DS1302 Real Time CLock
 ****************************************************/

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <virtuabotixRTC.h>   
#include <math.h>

const int IndicatorLightPin = A0;      //IN, interior illumination
const int engineSensor = A1;           //IN, coolant sensor
const int backlight = 6;               //OUT, backlight

const int selectButton = 8;            //IN, press to change mode displayed
const int hhButton = 7;                //IN, press to increment hour
const int mmButton = 5;                //IN, press to increment minute
const int resetButton = A3;            //IN, no function

// variables will change:
int indicatorLight = 0;         // variable for reading the pushbutton status
double engineTempSensor = 0;
int hh = 0;
int mm = 0;
int select = 0;
double oldTempSensor = 0;
int engineTempReadout = 0;


#define DEFAULT_COLOR   ST7735_WHITE
#define INDICATOR_COLOR ST7735_CYAN

int16_t fgColor = DEFAULT_COLOR;
int16_t bgColor = ST7735_BLACK;


// Creation of the Real Time Clock Object\
//SCLK -> 2, I/O -> 3, CS -> 4
virtuabotixRTC myRTC(2, 3, 4);

//CS -> 10, DC -> 9, RST -> 0
Adafruit_ST7735 tft = Adafruit_ST7735(10, 9, 0);

void setup(void) {
  
  analogWrite(backlight, 255);
  
  Serial.begin(9600);  // for debug
  
  // initialize the pushbutton pin as an input:
  //pinMode(IndicatorLightPin, INPUT); 
  pinMode(hhButton, INPUT); 
  pinMode(mmButton, INPUT);
  pinMode(selectButton, INPUT);  
  
  // Virtual Box Library: Set the current date, and time in the following format:
  // seconds, minutes, hours, day of the week, day of the month, month, year
  //myRTC.setDS1302Time(00, 28, 13, 30, 8, 1, 2014);
  
  setupLCD();

  runSplashScreen();
}

void setupLCD() {
  
  // Our supplier changed the 1.8" display slightly after Jan 10, 2012
  // so that the alignment of the TFT had to be shifted by a few pixels
  // this just means the init code is slightly different. Check the
  // color of the tab to see which init code to try. If the display is
  // cut off or has extra 'random' pixels on the top & left, try the
  // other option!
  // If you are seeing red and green color inversion, use Black Tab

  // If your TFT's plastic wrap has a Black Tab, use the following:
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip,Black tab
  // If your TFT's plastic wrap has a Red Tab, use the following:
  //tft.initR(INITR_REDTAB);   // initialize a ST7735R chip, red tab
  // If your TFT's plastic wrap has a Green Tab, use the following:
  //tft.initR(INITR_GREENTAB); // initialize a ST7735R chip, green tab

  //Serial.println("init");

  tft.fillScreen(bgColor);    // Clear the display to black 
  tft.setRotation(3);              // Place the display in landscape 270° Mode
  
}

void runSplashScreen() {
  
  tftdrawNissan(0,60,fgColor,bgColor);
  tftFadeUp(250, 25, 5, 30);
  tftFadeDown(25,250, 5, 30);
  tftdrawNissan(0,60,bgColor,bgColor);
  tft.setCursor(20, 70);  //Set position
  tft.setTextColor(fgColor,bgColor);  //Set font to white, fill negative space
  tft.setTextSize(2);
  tft.print("BY");
  tftdrawJPD(55,60,fgColor,bgColor);
  tftFadeUp(250, 25, 5, 30);
  tftFadeDown(25,250, 5, 30);
  tft.setTextColor(bgColor,bgColor);
  tft.setCursor(20, 70);
  tft.print("BY");
  tftdrawJPD(55,60,bgColor,bgColor);
  tftPrintTime(true);
  tftFadeUp(250, 25, 5, 30);
  
}

#define MODE_TIME    0
#define MODE_NISSAN  1
#define MODE_COOLANT 2
#define MODE_COUNT   3          //Helper
#define MODE_DEFAULT MODE_TIME  //Default

void setBacklight(int value)
{
  static int lastBacklightValue = -1;
  
  if (lastBacklightValue == value) {
    value = -1;
  } else {
    lastBacklightValue = value;
  }
  
  switch (value) {
    case 0:
      //tftFadeUp(250, 150, 5, 30);
      analogWrite(backlight, 125);        //this is dim
    break;
    case 1:
      //tftFadeDown(150, 250, 5, 30);
      analogWrite(backlight, 25);         //this is bright
    break;
    
    default:
    case -1:
      //nothing
    break;
    
  }
  
}

void loop() {
  static int mode = MODE_DEFAULT;
  static int lastFgColor = -1;
  
  //myRTC.updateTime();   // Update Time from RAM
  //Serial.print(myRTC.hours);
  //Serial.print(" : ");
  //Serial.println(myRTC.minutes);
  //delay(500);
  
  indicatorLight = analogRead(IndicatorLightPin);
  
  if(indicatorLight > 512){
      //fgColor = INDICATOR_COLOR;
      setBacklight(0);
    }
    else {
      //fgColor = DEFAULT_COLOR;
      setBacklight(1);
    }

  //Check for mode change
  select = digitalRead(selectButton);
  if (select == 1){
    //Remove old one
    removeMode(mode);
    
    mode++;
    if (mode >= MODE_COUNT) {
      mode = MODE_DEFAULT;
    }
    Serial.print("mode = ");
    Serial.println(mode);
    
    //Draw new one
    drawMode(mode);
    
    delay(500);
  }
  
  if (fgColor != lastFgColor) {
    drawMode(mode);
    lastFgColor = fgColor; 
  }
  
  if (mode == MODE_TIME) {
    tftPrintTime();   

    //TODO: fix this
    hh = digitalRead(hhButton);
    mm = digitalRead(mmButton);
    // increment hours if button has been pressed
    if(hh == 1){                         //Check hh button
      myRTC.updateTime();                //Update RTC values
      int hourUpdate = myRTC.hours;      
      if (hourUpdate < 24){              //When less than 24 increment
        hourUpdate++;
      }
      else{
        hourUpdate = 1;                  //When 24 rollover (RAM permits erronious values)
      }
      myRTC.setDS1302Time(myRTC.seconds, myRTC.minutes, hourUpdate, myRTC.dayofweek, myRTC.dayofmonth, myRTC.month, myRTC.year);  //Set Time with "hourUpdate
      tftPrintTime(true);   
      delay(300);
    }
    
    // increment minutes if button has been pressed
    if(mm == 1){                         //Check mm button
      myRTC.updateTime();                //Update RTC values
      int minuteUpdate = myRTC.minutes; 
      if (minuteUpdate < 59){            //When less than 59 minutes increment
        minuteUpdate++;
      }
      else{
        minuteUpdate = 1;                //When 59 rollover (RAM permits erronious values)
      }
      //delay(250);
      myRTC.setDS1302Time(myRTC.seconds, minuteUpdate, myRTC.hours, myRTC.dayofweek, myRTC.dayofmonth, myRTC.month, myRTC.year);
      tftPrintTime(true);   
      delay(200);
    }
    
  }
  
  if (mode == MODE_COOLANT) {
     tftPrintTemp();  
  }
  
  
  delay(100);
}

void drawMode(int mode) {
  switch(mode) {
    case MODE_TIME:
     tftPrintTime(true);
    break;
    case MODE_NISSAN:
      tftdrawNissan( 0, 60, fgColor, bgColor);
    break;
    case MODE_COOLANT:
      tftdrawCoolant( 20, 55, fgColor, bgColor);
      tftPrintTemp(true);
      //tftPrintTemp( 45, 60, fgColor, bgColor, 3);
    break;
  }
}

void removeMode(int mode) {
  switch(mode) {
    case MODE_TIME:
      tft.drawRect(10, 60, 25, 160, bgColor);
    break;
    case MODE_NISSAN:
      tftdrawNissan(0,60,bgColor, bgColor);
    break;
    case MODE_COOLANT:
      tft.fillRect(20, 50, 16, 40, bgColor);
    break;
  }
  
}














void tftPrintTemp() {   
  tftPrintTemp(false);
}

//TODO: fix this
void tftPrintTemp(int shouldUpdate) {   
    
  static int lastCoolantValue = -1;
  
  int x = 45;
  int y = 60;
  int point = 3;
  
  tft.setTextWrap(false);
  //tft.setCursor(x, y);  //Set position
  tft.setTextSize(point);
  
  
  //Read and convert for display
  engineTempSensor = analogRead(engineSensor);
  engineTempSensor = log(10000.0*((1024.0/engineTempSensor-1))); // for pull-up configuration
  engineTempSensor = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * engineTempSensor * engineTempSensor ))* engineTempSensor );
  engineTempSensor = engineTempSensor - 273.15;            // Convert Kelvin to Celcius
  engineTempSensor = (engineTempSensor * 9.0)/ 5.0 + 32.0; // Convert Celcius to Fahrenheit
  
  engineTempReadout = engineTempSensor / 5;
  engineTempReadout = engineTempReadout * 5;
  
  
  if (lastCoolantValue != engineTempReadout) {
    shouldUpdate = true;
    lastCoolantValue = engineTempReadout;
  }  
  
  if (shouldUpdate) {
    
    tft.setTextColor( fgColor, bgColor);
    tft.setCursor(x, y);
    tft.print(engineTempReadout);
    tft.print("<F ");
    //Serial.println(oldTempSensor);
    /*
        if (oldTempSensor != engineTempSensor){
          //tft.setCursor(x, y);  //Set position
          //tft.setTextColor(backColor,backColor);
          //tft.print(oldTempSensor);
          //tft.drawRect(10, 60, 160, 25, 0x0000);
          tft.setTextColor(fontColor,backColor);  //Set font to white, fill negative space
          tft.setCursor(x, y);
          tft.print(engineTempSensor);
          oldTempSensor = engineTempSensor;
          //Serial.println(engineTempSensor);
        } */
    
    Serial.println("Updated temp");
  }
}



//"12:36 PM"
//" 2:36 AM"

void tftPrintTime() {
  tftPrintTime(false);
}

//TODO: fix this
// Display the time with chosen font color
void tftPrintTime(bool shouldUpdate) {   
  static int lastMins = -1;
  
  
  int fontColor = fgColor;
  int backColor = bgColor;
  
  myRTC.updateTime();   // Update Time from RAM
  
  if (lastMins != myRTC.minutes) {
    shouldUpdate = true;
    lastMins = myRTC.minutes;
  }
  
  if (shouldUpdate) {
    tft.setTextWrap(false);
    tft.setCursor(10, 60);  //Set position
    tft.setTextColor(fontColor,backColor);  //Set font to white, fill negative space
    tft.setTextSize(3);
    if (myRTC.hours <= 12){  //Display hours when the time is 12 or less
      if (myRTC.hours <= 9){ //Add a space if the hour is less than 10
        tft.print(" ");
        tft.print(myRTC.hours);
        tft.print(":");
      }
      else{
          tft.print(myRTC.hours);  //Display hours with 10 or later,But not later than 12 as previously tested
          tft.print(":");
        }
      }
    else{
      if (myRTC.hours <= 21){   //Display hours with a space if less than 22 and greater than 12 as previously tested
        tft.print(" ");
        tft.print(myRTC.hours - 12);
        tft.print(":");
      }
      else{                     //Display raw hours when 22 and over
        tft.print(myRTC.hours - 12);
        tft.print(":");
      }
      }
    if (myRTC.minutes < 10){   //Display minutes less than 10 with a leading zero
      tft.print("0");
      tft.print(myRTC.minutes);
      }
    else{
      tft.print(myRTC.minutes); //Display minutes 10 and over with no spaces
      }
    if (myRTC.hours <= 11){  //Display "AM" from 1AM to 11AM
      tft.print(" AM  ");
      }
    else{
      if (myRTC.hours <=23){ //Display "PM" from 1PM to 11PM
      tft.print(" PM  ");
      }
      else{
        tft.print(" AM  ");  //Don't forget 12AM, that weirdo
      }
    }
    Serial.println("Updated time");
  }
}

//16x35
const unsigned char PROGMEM icon16_thermometer[] =
{ B00000011, B11000000,
  B00000110, B01100000,
  B00001100, B00110000,
  B00001100, B00110000,
  B00001111, B00110000,
  B00001100, B00110000,
  B00001100, B00110000,
  B00001111, B00110000,
  B00001100, B00110000,
  B00001100, B00110000,
  B00001111, B00110000,
  B00001100, B00110000,
  B00001100, B00110000,
  B00001111, B00110000,
  B00001100, B00110000,
  B00001100, B00110000,
  B00001111, B00110000,
  B00001100, B00110000,
  B00001100, B00110000,
  B00001111, B00110000,
  B00001100, B00110000,
  B00011100, B00111000,
  B00111100, B00111100,
  B01110000, B00001110,
  B01100000, B00000110,
  B11100000, B00000111,
  B11000000, B00000011,
  B11000000, B00000011,
  B11100000, B00000111,
  B01100000, B00000110,
  B01110000, B00001110,
  B00111000, B00011100,
  B00011110, B01111000,
  B00001111, B11110000,
  B00000011, B11000000
};

//180*27
const unsigned char PROGMEM icon_nissan[] =
//160*24
{
B11111111,B11000000,B00000111,B11100000,B11111100,B00000111,B11111111,B11111111,B11100000,B01111111,B11111111,B11111111,B00000000,B00011111,B11111110,B00000000,B00011111,B11111110,B00000000,B01111111,
B11111111,B11100000,B00000111,B11100000,B11111100,B00001111,B11111111,B11111111,B11100000,B11111111,B11111111,B11111111,B00000000,B00011111,B11111110,B00000000,B00011111,B11111110,B00000000,B01111111,
B11111111,B11110000,B00000111,B11100000,B11111100,B00011111,B11111111,B11111111,B11100000,B11111111,B11111111,B11111111,B00000000,B00111111,B11111111,B00000000,B00011111,B11111111,B00000000,B01111111,
B11111111,B11111000,B00000111,B11100000,B11111100,B00111111,B11111111,B11111111,B11100001,B11111111,B11111111,B11111111,B00000000,B00111111,B11111111,B00000000,B00011111,B11111111,B00000000,B01111111,
B11111111,B11111000,B00000111,B11100000,B11111100,B00111111,B11111111,B11111111,B11100001,B11111111,B11111111,B11111111,B00000000,B01111111,B11111111,B00000000,B00011111,B11111111,B10000000,B01111111,
B11111111,B11111100,B00000111,B11100000,B11111100,B00111111,B10000000,B00000000,B00000001,B11111100,B00000000,B00000000,B00000000,B01111111,B00111111,B00000000,B00011111,B11111111,B10000000,B01111111,
B11111111,B11111100,B00000111,B11100000,B11111100,B00111111,B00000000,B00000000,B00000001,B11111100,B00000000,B00000000,B00000000,B01111111,B00011111,B10000000,B00011111,B10111111,B11000000,B01111111,
B11111101,B11111110,B00000111,B11100000,B11111100,B00111111,B10000000,B00000000,B00000001,B11111100,B00000000,B00000000,B00000000,B11111111,B00011111,B11000000,B00011111,B10011111,B11100000,B01111111,
B11111100,B11111111,B00000111,B11100000,B11111100,B00111111,B11111111,B11111110,B00000001,B11111111,B11111111,B11110000,B00000000,B11111110,B00001111,B11000000,B00011111,B10001111,B11110000,B01111111,
B11111100,B11111111,B00000111,B11100000,B11111100,B00111111,B11111111,B11111111,B10000001,B11111111,B11111111,B11111110,B00000000,B11111100,B00001111,B11100000,B00011111,B10000111,B11111000,B01111111,
B11111100,B01111111,B10000111,B11100000,B11111100,B00011111,B11111111,B11111111,B11000000,B11111111,B11111111,B11111111,B00000001,B11111100,B00001111,B11100000,B00011111,B10000111,B11111000,B01111111,
B11111100,B01111111,B10000111,B11100000,B11111100,B00011111,B11111111,B11111111,B11100000,B11111111,B11111111,B11111111,B00000001,B11111100,B00000111,B11100000,B00011111,B10000011,B11111100,B01111111,
B11111100,B00111111,B11000111,B11100000,B11111100,B00000111,B11111111,B11111111,B11100000,B01111111,B11111111,B11111111,B00000001,B11111000,B00000111,B11110000,B00011111,B10000001,B11111110,B01111111,
B11111100,B00111111,B11100111,B11100000,B11111100,B00000001,B11111111,B11111111,B11110000,B00001111,B11111111,B11111111,B00000011,B11111000,B00000111,B11110000,B00011111,B10000001,B11111110,B01111111,
B11111100,B00001111,B11110111,B11100000,B11111100,B00000000,B00000000,B11111111,B11110000,B00000000,B00000001,B11111111,B00000011,B11111111,B11111111,B11111000,B00011111,B10000000,B11111111,B01111111,
B11111100,B00001111,B11110111,B11100000,B11111100,B00000000,B00000000,B00000111,B11110000,B00000000,B00000000,B00111111,B00000111,B11111111,B11111111,B11111000,B00011111,B10000000,B11111111,B01111111,
B11111100,B00000111,B11111111,B11100000,B11111100,B00000000,B00000000,B00000011,B11110000,B00000000,B00000000,B00111111,B00000111,B11111111,B11111111,B11111100,B00011111,B10000000,B01111111,B11111111,
B11111100,B00000011,B11111111,B11100000,B11111100,B00111111,B11111111,B11111111,B11110000,B11111111,B11111111,B11111111,B00001111,B11111111,B11111111,B11111110,B00011111,B10000000,B01111111,B11111111,
B11111100,B00000011,B11111111,B11100000,B11111100,B00111111,B11111111,B11111111,B11110000,B11111111,B11111111,B11111111,B00011111,B11000000,B00000000,B11111110,B00011111,B10000000,B00111111,B11111111,
B11111100,B00000001,B11111111,B11100000,B11111100,B00111111,B11111111,B11111111,B11100000,B11111111,B11111111,B11111111,B00011111,B11000000,B00000000,B11111110,B00011111,B10000000,B00011111,B11111111,
B11111100,B00000000,B11111111,B11100000,B11111100,B00111111,B11111111,B11111111,B11100000,B11111111,B11111111,B11111111,B00011111,B10000000,B00000000,B11111111,B00011111,B10000000,B00011111,B11111111,
B11111100,B00000000,B11111111,B11100000,B11111100,B00111111,B11111111,B11111111,B11000000,B11111111,B11111111,B11111110,B00111111,B10000000,B00000000,B01111111,B00011111,B10000000,B00001111,B11111111,
B11111100,B00000000,B01111111,B11100000,B11111100,B00111111,B11111111,B11111111,B11000000,B11111111,B11111111,B11111110,B00111111,B10000000,B00000000,B01111111,B00011111,B10000000,B00001111,B11111111,
B11111100,B00000000,B00111111,B11100000,B11111100,B00011111,B11111111,B11111111,B10000000,B11111111,B11111111,B11100000,B00111111,B00000000,B00000000,B00111111,B00011111,B10000000,B00000111,B11111111,
};

void tftdrawCoolant(int x, int y, int main, int back){
  tft.drawBitmap(x, y,  icon16_thermometer, 16, 35, main, back);
}

void tftFadeUp(int first, int last, int value, int dly){
  for(int fadeValue = first ; fadeValue >= last; fadeValue -= value) { 
    // sets the value (range from first to last):
    analogWrite(backlight, fadeValue);         
    // wait for dly milliseconds to see the dimming effect    
    delay(dly);                            
  } 
}

void tftFadeDown(int first, int last, int value, int dly){
for(int fadeValue = first ; fadeValue <= last; fadeValue += value) { 
    // sets the value (range from 0 to 255):
    analogWrite(backlight, fadeValue);         
    // wait for 30 milliseconds to see the dimming effect    
    delay(dly);                            
  } 
}

void tftdrawNissan(int x, int y, int main, int back)
{
  tft.drawBitmap(x, y,  icon_nissan, 160, 24, main, back);
}

void tftdrawJPD(int x, int y, int main, int back)
{
  tft.fillTriangle(12+x, 24+y, 0+x, 12+y, 12+x, 12+y, main);
  tft.drawRect(18+x, 0+y, 5, 25, main);
  tft.drawRect(28+x, 0+y, 5, 25, main);
  tft.fillTriangle(38+x, 0+y, 38+x, 12+y, 50+x, 12+y, main);
  tft.drawRect(55+x, 0+y, 5, 25, main);
  tft.fillTriangle(65+x, 0+y, 65+x, 24+y, 77+x, 12+y, main);
}



double Thermistor(int RawADC) {
 double Temp;
 //Temp = log(10000.0*((1024.0/RawADC-1))); 
 Temp = log(10000.0/(1024.0/RawADC-1)); // for pull-up configuration
 Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
 Temp = Temp - 273.15;            // Convert Kelvin to Celcius
 Temp = (Temp * 9.0)/ 5.0 + 32.0; // Convert Celcius to Fahrenheit
 return Temp;
}

