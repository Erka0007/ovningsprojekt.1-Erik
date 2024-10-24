/*
* Name: pixelring, clock and temp project
* Author: Erik Karlen
* Date: 2024-10-10
* Description: This project uses a ds3231 to measure time and displays the time to an 1306 oled display,
* Further, it measures temprature with a analog temprature module and displays a mapped value to a 9g-servo-motor
*/

// Include Libraries
#include <RTClib.h>
#include <Wire.h>
#include "U8glib.h"
#include <Servo.h>
#include <Adafruit_NeoPixel.h>
// Init constants
float R1 = 10000; // value of R1 on board
float c1 = 0.001129148, c2 = 0.000234125, c3 = 0.0000000876741; 
int tempPin = A1;
int potPin = A2;

// Init global variables
char t[32];
float logR2, R2, T;
int Vo;
// construct objects
RTC_DS3231 rtc;
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);
Adafruit_NeoPixel ring = Adafruit_NeoPixel(24, 5, NEO_GRB + NEO_KHZ800); 
Servo tidServo;
void setup() {
  // init communication
  Serial.begin(9600);
  Wire.begin();
  // Init Hardware
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  pinMode(tempPin, INPUT);
  pinMode(potPin, INPUT);
  u8g.setFont(u8g_font_unifont);
  u8g.firstPage();
  tidServo.attach(9);
  ring.begin();           
  ring.show();            
  ring.setBrightness(50); 
}

void loop() {

  oledWrite(getTime(), String(getTemp()), choosecolor());
  servoWrite(getTemp());
  pixelringcolor(getTemp(), choosecolor());
  delay(100);
  
}


/*
*This function reads time from an ds3231 module and package the time as a String
*Parameters: Void
*Returns: time in hh:mm:ss as String
*/
String getTime() {
  DateTime now = rtc.now();
  return (String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()));
}

/*
* This function reads an analog pin connected to an analog temprature sensor and calculates the corresponding temp
*Parameters: Void
*Returns: temprature as float
*/
float getTemp() {
   Vo = analogRead(tempPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0); //calculate resistance on thermistor
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2)); // temperature in Kelvin
  T = T - 273.15; 
  return T;
}

/*
* This function takes a string and draws it to an oled display. It also takes a color value and displays on of three different string dependig of which value is recived.
* The function also displays a clock icon and a tempature icon.
*Parameters: - text: String to write to display
*Parameters: - temp: String to write to display, showing the tempature¨
*Parameters: -color: an int, which is then converted to a text corresponding to the color the int represents, which is then shown on the display
*Returns: void
*/
void oledWrite(String text, String temp, int color) {
   u8g.firstPage();
  do {

    //the code below displays a clock icon
    u8g.drawCircle(8, 8, 8);
    u8g.drawBox(8, 8, 1, 6);
    u8g.drawBox(8, 8, 4, 1);

    u8g.drawStr(35, 14, "Tid:");
    u8g.drawStr(65, 14, text.c_str());

    u8g.drawStr(26, 62, "Temp:");
    u8g.drawStr(65, 62, temp.c_str());

    //the code below displays a tempature icon
    u8g.drawCircle(8, 57, 6);
    u8g.drawCircle(8, 57, 1);
    u8g.drawLine(11, 52, 11, 42);
    u8g.drawLine(5, 52, 5, 42);
    u8g.drawLine(6, 41, 10, 41);
    u8g.drawLine(8, 57, 8, 44);
    u8g.drawLine(7, 46, 9, 46);
    u8g.drawLine(7, 48, 9, 48);
    u8g.drawLine(7, 54, 9, 54);
    
    //The code below displays which color is chosen
    u8g.drawStr(18, 37, "Color:");
    if (color == 255){
    u8g.drawStr(65, 37, "Blue");
    }else if (color == 100){
    u8g.drawStr(65, 37, "Turquoise");
    }else{
    u8g.drawStr(65, 37, "Green");
    }
     

  } while (u8g.nextPage());

}

/*
* takes a temprature value and maps it to a corresponding degree on a servo
*Parameters: - value: temprature
*Returns: void
*/
void servoWrite(float value) {
 tidServo.write(map(value, 0, 40, 0, 180));
}

/*
* takes a temprature value and a color value. The function maps the temprature value and changes the color of a ring light accordingly. 
* The color value changes which two colors the ringlight moves betweens when the temprature changes.
*Parameters: - temp: temprature
*Parameters: -color: an int that decides the last value in RGB
*Returns: void
*/

void pixelringcolor(float temp, int color){
for(int i = 0; i < 24; i++){
    ring.setPixelColor(i, ring.Color(map(temp, 22, 32, 0, 255),map(temp, 22, 32, 255, 0), color));
    ring.show();
    delay(20);
  }
}

/*
* Reads the value of a potentiometer and returns one of three different color values accordingly
*Parameters: void
*Returns: either 0, 255 or 155 as an int
*/

int choosecolor(){
  int color = 0;
 int Aread = analogRead(potPin);
  Serial.println(analogRead(potPin));
  if(Aread < 600){
     color = 255;
  }else if(Aread > 600 && Aread < 800){
     color = 100;
  }else{
   color = 0;
  }

  return color;
}

