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

#define MODE_TIME    0
#define MODE_NISSAN  1
#define MODE_COOLANT 2
#define MODE_LOOP    3          //Must be last. Will loop through lower modes than it
#define MODE_COUNT   4          //Helper
#define MODE_DEFAULT MODE_TIME  //Default

int mode = MODE_DEFAULT;
int loopMode = 0;
  
int debug_indicator = 0;
  
// variables will change:
int indicatorLight = 0;         // variable for reading the pushbutton status
int hh = 0;
int mm = 0;
int select = 0;
double oldTempSensor = 0;


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

  mode = getMode(); 
  if(mode >= MODE_COUNT) mode = MODE_DEFAULT;
  
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
  
  drawMode(mode);
  
  tftFadeUp(250, 25, 5, 30);
  
}


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
    saveMode(mode);
    
    //Draw new one
    drawMode(mode);
    
    delay(500);
  }
  
  if (fgColor != lastFgColor) {
    drawMode(mode);
    lastFgColor = fgColor; 
  }
  
  if (mode == MODE_TIME || loopMode == MODE_TIME) {
    tftPrintTime();   
  }  
  
  if (mode == MODE_TIME) {

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
        minuteUpdate = 0;                //When 59 rollover (RAM permits erronious values)
      }
      //delay(250);
      myRTC.setDS1302Time(myRTC.seconds, minuteUpdate, myRTC.hours, myRTC.dayofweek, myRTC.dayofmonth, myRTC.month, myRTC.year);
      tftPrintTime(true);   
      delay(200);
    }
    
  }
  
  if (mode == MODE_COOLANT || loopMode == MODE_COOLANT) {
     updateTemp();  
  }
  
  if (mode == MODE_NISSAN) {
     if(debug_indicator) updateDebug();
  }
  
  static unsigned long lastMillis = 0;
  
  if (mode == MODE_LOOP)
  {
    unsigned long currentMillis = millis();
   
    if(currentMillis - lastMillis > 15000) {
      lastMillis = currentMillis;   

      removeMode(loopMode);
      
      loopMode++;
      if (loopMode >= MODE_LOOP) {
        loopMode = MODE_DEFAULT;
      }
      
      //Draw new one
      drawMode(loopMode);
    }
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
      updateTemp(true);
    break;
    case MODE_LOOP:
      drawMode(loopMode);
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
    case MODE_LOOP:
      removeMode(loopMode);
    break;
  }
  
}

void updateDebug() {
  tftPrintTemp(indicatorLight);
}

void updateTemp() {   
   updateTemp(false);
}

void updateTemp(int forceUpdate) {
    
  static int lastCoolantValue = -1;
  
  double engineTempADC = 0;
  int engineTempValue = 0;
  
  
  engineTempADC = analogRead(engineSensor);
  engineTempValue = parseThermistor(engineTempADC);
  
  if (lastCoolantValue != engineTempValue) {
    forceUpdate = true;
    lastCoolantValue = engineTempValue;
  }  
  
  if (forceUpdate) {
    tftPrintTemp(engineTempValue);
    Serial.println("Updated temp");
  }
}






