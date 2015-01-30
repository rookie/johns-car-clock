

//TODO: fix this
void tftPrintTemp(int engineTempValue) {   
  
  static int lastOffset = -1;
  
  int x = 45;
  int y = 60;
  int point = 3;
  
  tft.setTextWrap(false);
  tft.setTextSize(point);

  tft.setTextColor( fgColor, bgColor);
  tft.setCursor(x, y);
  tft.print(engineTempValue);
  
  
  //Dont code like this
  int offset = 1;
  if(abs(engineTempValue) >= 10) offset++;
  if(abs(engineTempValue) >= 100) offset++;
  if(abs(engineTempValue) >= 1000) offset++;
  if(engineTempValue < 0) offset++;
  
  if(offset != lastOffset)
  {
    tft.print(" ");
    lastOffset = offset;
  }
  
  tftdrawDegree(45+offset*6*point, 60, point, fgColor, bgColor);
  
  offset++;
  tft.setCursor(45+offset*6*point, 60);
  tft.print("F  ");
  Serial.println("Updated temp");
  
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

void tftdrawDegree(int x, int y, int point, int main, int back){
  //tft.drawRect(x, y, 4*point, 4*point, main);
  
  tft.fillRect(x+1*point, y,         2*point, 1*point, main);
  tft.fillRect(x+1*point, y+3*point, 2*point, 1*point, main);
  tft.fillRect(x        , y+1*point, 1*point, 2*point, main);
  tft.fillRect(x+3*point, y+1*point, 1*point, 2*point, main);

}

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

