/******************************************\
|     Digital Watch Operating System       |
|      Written by and Copywrited to        |
|         Katja Veronika DeCuir            |
\******************************************/
// Arduino Mini x 2
// to buy: 12.70 USD (6 USD each) / 392.05 rubles on ebay

// RTC AT24C32 DS1307 Real Time Clock Module
// RTC clock modual SDA to analog 4 SCL to analog 5
// to buy: 3.88 USD / 119.78 rubles on ebay

// Nokia 5110 LCD screen
// to buy: 4.00 USD / 123.48 rubles on ebay
// pin 9 - Serial clock out (SCLK)
// pin 8 - Serial data out (DIN)
// pin 7 - Data/Command select (D/C)
// pin 6 - LCD chip select (CS)
// pin 5 - LCD reset (RST)

// Menu buttons (5 way button comming soon.. will be attached to CD4021BE shift register) 
// to buy: about 5 USD / 154.35 rubles https://www.sparkfun.com/products/11187 https://www.sparkfun.com/products/97
// pushbutton attached to pin 2 from +5V (menu button)
// 10K resistor attached to pin 2 from ground

// SD card modual attached to SPI bus as follows: (not implemented yet)
// to buy: 1.65 USD / 50.94 rubles on ebay
// MOSI - pin 11
// MISO - pin 12
// CLK - pin 13
// CS - pin 10

// Wtv020sd16p mp3 modual(not implemented yet)
// to buy: 5.00 USD / 154.35 rubles on ebay
// Reset to pin 0
// Clock to pin 1
// Data to pin 3
// Busy to pin 4

// DHT11 sensor
// to buy: 1.52 USD / 46.92 rubles on ebay
// DHTpin1 to 5v
// DHTpin2 to pin 4 and 10k resistor to DHTpin1
// DHTpin4 to ground

//IR Receiver Module 38 kHz TSOP4838
// to buy: 0.99 USD / 30.58 rubles on ebay
//pin1 to vcc
//pin2 to pin X
//pin3 to ground

//PCB board to mount the chip shift register chip and solder to the wires
// to buy 3.18 USD / 98.17 rubles on ebay

// total money spent on project so far: 37.92 USD
// analog pins left over: 0, 1, 2, 3

#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <DHT11.h>

dht11 DHT11;

#define DHT11PIN 4 //sensor pin

Adafruit_PCD8544 display = Adafruit_PCD8544(9, 8, 7, 6, 5);

RTC_DS1307 RTC;

int battery = 0;
//what mode the watch starts in accorting to selections[]
int menu = 1;
boolean selected = true;
char selection = ' ';
//menu options            ////////////Rearrange this later!////////////
String selections[] = { "", "Clock", "Data", "Set Time", "Countdown", "Stopwatch", "IR Remote", "Alarm", "Config", "SD Scripts", "Mp3 Player", "Pictures?" };
int maxselections = 11;
//scroll bar values in menu
int scroll[] = { 0, 18, 19, 20, 22, 23, 24, 26, 27, 28, 29, 30 };
int x = 1;
boolean set = false;
DateTime toset;
int settimeboxes[6][2] = {{(display.width()/2)-26,(display.height()/2)-3},{(display.width()/2)-7,(display.height()/2)-3},{(display.width()/2)+10,(display.height()/2)-3},{(display.width()/2)-33,(display.height()/2)+7},{(display.width()/2)-14,(display.height()/2)+7},{(display.width()/2)+4,(display.height()/2)+7}};
char charhour[3]; // used for setting the clock
char charminute[3];
char charsecond[3];
int hourint;
int minuteint;
int secondint;
char charmonth[3];
char charday[3];
char charyear[4];
int monthint;
int dayint;
int yearint;


void setup () {
    Serial.begin(57600);
    Wire.begin();
    RTC.begin();
    display.begin();
    display.setContrast(50);
    display.display();
    display.clearDisplay();
    
    for (int16_t i=0; i<display.height(); i+=2) {
      display.drawCircle(display.width()/2, display.height()/2, i, BLACK);
      display.display();
      }
    delay(2000);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.setCursor((display.width()/2)-30,(display.height()/2)-5);
    display.println("Loading...");
    display.display();
    //do loading stuff here
    RTC.adjust(DateTime(__DATE__, __TIME__));
    attachInterrupt(0, interruptstuff, RISING);
    //end loading stuff
    display.clearDisplay();
}

void interruptstuff() {
x = menu;
menu = 0;
}

void loop () {
  
    //get button value here
    /*
    switch (buttonpress) { //buttonpress is binary string from output of shiftregister
      case '00010000':
        selection = 'u';
        break;
      case '00001000':
        selection = 'd';
        break;
      default:
        selection = ' ';
    } */
   
    switch (menu) {
      case 0: //Menu
        showstatus(menu);
        menuselect();
        break;
      case 1: //Clock
        showstatus(menu);
        clock();
        break;
      case 2: //Data - tempature, humitidy, and other data inputs
        showstatus(menu);
        data();
        break;
      case 3: //Set Time
        showstatus(menu); //need to work on inputtime(); for both countdown and set time
        settime();
        break;
      case 4: //Countdown
        showstatus(menu);
        countdown();
        break;
      case 5: //Stopwatch
        showstatus(menu);
        //Item5
        break;
      case 6: //IR Remote
        showstatus(menu);
        //Item6
        break;
      case 7: //Alarm
        showstatus(menu);
        //stuff
        break;
      case 8: //Config
        showstatus(menu);
        //Item3
        break;
      case 9: //SD Scripts
        showstatus(menu);
        //Item4
        break;
      case 10: //Mp3 Player
        showstatus(menu);
        //Item5
        break;
      case 11: //Pictures?
      showstatus(menu);
        //Item6
        break;
      default:
        showstatus(-1);
        display.setCursor((display.width()/2)-25,(display.height()/2)-3);
        display.print("Error");
        display.display();
    }
    selection = ' ';
}

void menuselect() {
    display.setTextSize(1);
    display.setTextColor(BLACK);
    //u to go up. d to go down. and c to choose
    
    
    display.fillTriangle(display.width()-5, display.height()-2,
                     display.width()-8, display.height()-5,
                     display.width()-2, display.height()-5, BLACK);
    display.setCursor((display.width()/2)-25,(display.height()/2)+3);
    display.fillTriangle(display.width()-5, 12,
                     display.width()-8, 15,
                     display.width()-2, 15, BLACK);
                     
    if (selection == 'u') {x = x - 1; selection = ' ';}
    if (selection == 'd') {x = x + 1; selection = ' ';}
    if (x < 1) {x=1;}
    if (selection == 'c') {menu = x; selected = true; selection = ' '; x = 0; set = false;}
    if (x > maxselections) {x=maxselections;}
    display.fillRect(display.width()-7, scroll[x], 5, 10, BLACK);
    display.setCursor((display.width()/2)-25,(display.height()/2)-8);
    display.print(selections[x-1]);
    display.fillRect(display.width()/2-30, display.height()/2-1, 60, 10, BLACK);
    display.setCursor((display.width()/2)-25,(display.height()/2));
    display.setTextColor(WHITE);
    display.print(selections[x]);
    display.setTextColor(BLACK);
    display.setCursor((display.width()/2)-25,(display.height()/2)+8);
    if ((x+1) <= maxselections) display.print(selections[x+1]);
    display.setCursor((display.width()/2)-25,(display.height()/2)+16);
    if ((x+2) <= maxselections) display.print(selections[x+2]);
    display.display();    
    delay(1000);
    display.clearDisplay();
    display.display();
}

void serialEvent() {
  while (Serial.available()) {
    selection = Serial.read();
  }
}

void inputtime() {
  
    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.setCursor((display.width()/2)-25,(display.height()/2)-3);

    if (hourint < 10) display.print("0");
    display.print(hourint);
    display.print(':');
    if (minuteint < 10) display.print("0");
    display.print(minuteint);
    display.print(':');
    if (secondint < 10) display.print("0");
    display.print(secondint);
    


    
    display.setCursor((display.width()/2)-32,(display.height()/2)+7);
    if (monthint < 10) display.print("0");
    display.print(monthint);
    display.print('/');
    if (dayint < 10) display.print("0");
    display.print(dayint);
    display.print('/');
    display.print(yearint);

    if (selection == 'l') {x = x - 1; selection = ' ';}
    if (selection == 'r') {x = x + 1; selection = ' ';}
    if (x < 0) {x=0;}
    if (x > 6 ) {x=6;} 
    int length = 12;
    int height = 8;
    
    switch (x) {
      case 0:
        display.fillRect(settimeboxes[x][0], settimeboxes[x][1], length, height, BLACK);
        display.setCursor(settimeboxes[x][0],settimeboxes[x][1]);
        display.setTextColor(WHITE);
        if (hourint < 10) display.print("0");
        display.print(hourint);
        display.setTextColor(BLACK);
        if (selection == 'u') {hourint = hourint + 1; selection = ' ';}
        if (selection == 'd') {hourint = hourint - 1; selection = ' ';}
        break;
      case 1:
        display.fillRect(settimeboxes[x][0], settimeboxes[x][1], length, height, BLACK);
        display.setCursor(settimeboxes[x][0],settimeboxes[x][1]);
        display.setTextColor(WHITE);
        if (minuteint < 10) display.print("0");
        display.print(minuteint);
        display.setTextColor(BLACK);
        if (selection == 'u') {minuteint = minuteint + 1; selection = ' ';}
        if (selection == 'd') {minuteint = minuteint - 1; selection = ' ';}
        break;
      case 2:
        display.fillRect(settimeboxes[x][0], settimeboxes[x][1], length, height, BLACK);
        display.setCursor(settimeboxes[x][0],settimeboxes[x][1]);
        display.setTextColor(WHITE);
        if (secondint < 10) display.print("0");
        display.print(secondint);
        display.setTextColor(BLACK);
        if (selection == 'u') {secondint = secondint + 1; selection = ' ';}
        if (selection == 'd') {secondint = secondint - 1; selection = ' ';}
        break;
      case 3:
        display.fillRect(settimeboxes[x][0], settimeboxes[x][1], length, height, BLACK);
        display.setCursor(settimeboxes[x][0],settimeboxes[x][1]);
        display.setTextColor(WHITE);
        if (monthint < 10) display.print("0");
        display.print(monthint);
        display.setTextColor(BLACK);
        if (selection == 'u') {monthint = monthint + 1; selection = ' ';}
        if (selection == 'd') {monthint = monthint - 1; selection = ' ';}
        break;
      case 4:
        display.fillRect(settimeboxes[x][0], settimeboxes[x][1], length, height, BLACK);
        display.setCursor(settimeboxes[x][0],settimeboxes[x][1]);
        display.setTextColor(WHITE);
        if (dayint < 10) display.print("0");
        display.print(dayint);
        display.setTextColor(BLACK);
        if (selection == 'u') {dayint = dayint + 1; selection = ' ';}
        if (selection == 'd') {dayint = dayint - 1; selection = ' ';}
        break;
      case 5:
        display.fillRect(settimeboxes[x][0], settimeboxes[x][1], length*2, height, BLACK);
        display.setCursor(settimeboxes[x][0],settimeboxes[x][1]);
        display.setTextColor(WHITE);
        display.print(yearint);
        display.setTextColor(BLACK);
        if (selection == 'u') {yearint = yearint + 1; selection = ' ';}
        if (selection == 'd') {yearint = yearint - 1; selection = ' ';}
        break;
      case 6:
        display.clearDisplay();
        display.setCursor(0,(display.height()/2)-3);
        display.print("<Yes>\n");
        display.print("Confirm?\n");
        display.print("<No>");
        if (selection == 'u') {
          DateTime stuff (yearint, monthint, dayint, hourint, minuteint, secondint);
          RTC.adjust(stuff);
          set = !set;
        }
        if (selection == 'd') {set = !set;}
        break;
      default:
        display.setCursor((display.width()/2)-25,(display.height()/2)-3);
        display.print("Error");
    }
    
    
    display.display();
    delay(200);
    display.clearDisplay();
    

}

void clock() {
    DateTime now = RTC.now();
    String hour1 = String(now.hour());
    String minute1 = String(now.minute());
    String second1 = String(now.second());
    if (hour1.length() < 2) { hour1 = "0" + hour1; }
    if (minute1.length() < 2) { minute1 = "0" + minute1; }
    if (second1.length() < 2) { second1 = "0" + second1; }
    
    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.setCursor((display.width()/2)-25,(display.height()/2)-3);

    display.print(hour1);
    display.print(':');
    display.print(minute1);
    display.print(':');
    display.print(second1);
    
    String day1 = String(now.day());
    String month1 = String(now.month());
    if (day1.length() < 2) { day1 = "0" + day1; }
    if (month1.length() < 2) { month1 = "0" + month1; }
    
    display.setCursor((display.width()/2)-32,(display.height()/2)+7);
    display.print(month1);
    display.print('/');
    display.print(day1);
    display.print('/');
    display.print(now.year());
    
    display.setCursor((display.width()/2)-22,(display.height()/2)+15);
    display.print(dayofweek());
    
    display.display();
    delay(1000);
    display.clearDisplay();
}

void data() {
    int check = DHT11.read(DHT11PIN);
    
    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.setCursor((display.width()/2)-25,(display.height()/2)-3);
    
    switch (check)
  {
    case DHTLIB_OK:
                
                display.print("Hum: ");
                display.print((float)DHT11.humidity, 2);
                display.setCursor((display.width()/2)-38,(display.height()/2)+7);
                display.print("Temp: ");
                display.print((float)DHT11.temperature, 2);
                display.print("*C"); 
                //display.setCursor((display.width()/2)-22,(display.height()/2)+15);
                //display.print("other data here");
                menu = 2; //no idea why, this case value resets menu to 0, somehow. this is needed to fix that error.
                break;
    case DHTLIB_ERROR_CHECKSUM:
                display.print("Checksum error");
                break;
    case DHTLIB_ERROR_TIMEOUT:
                display.print("Time out error");
                break;
    default:
                display.print("Unknown error");
                break;
  }
  display.display();
  delay(1000);
  display.clearDisplay();
  display.display();
}

String dayofweek() {
  DateTime now = RTC.now();
  int day_of_week;
  char month2[3];
  String(now.month()).toCharArray(month2, 3);     
  
  char day2[3];
  String(now.day()).toCharArray(day2, 3);       
  
  char year2[4];
  String(now.year()).toCharArray(year2, 5);     
  
  int month1 = atoi(month2);
  int day1 =  atoi(day2);
  int year1 =  atoi(year2);
  if (month1 < 3)
  {
      month1 += 12;
      --year1;
    }
   day_of_week = day1 + (13 * month1 - 27) / 5 + year1 + year1/4 - year1/100 + year1/400;
   switch ((day_of_week % 7) + 1) {
   case 1:
     return "Sunday";
   case 2:
     return "Monday";
   case 3:
     return "Tuesday";
   case 4:
     return "Wednesday";
   case 5:
     return "Thursday";
   case 6:
     return "Friday";
   case 7:
     return "Saturday";
   default:
     return "Error";
   }
}

void settime() {
  if (selection == 'c') {set = !set; selection = ' ';}
  switch (set) {
    case true:
      inputtime();
      break;
    case false:
      toset = RTC.now();
      String(toset.hour()).toCharArray(charhour, 3);     
      String(toset.minute()).toCharArray(charminute, 3);       
      String(toset.second()).toCharArray(charsecond, 3);     
    
      hourint = atoi(charhour);
      minuteint =  atoi(charminute);
      secondint =  atoi(charsecond);
         
      String(toset.month()).toCharArray(charmonth, 3);     
      String(toset.day()).toCharArray(charday, 3);       
      String(toset.year()).toCharArray(charyear, 5);     
      monthint = atoi(charmonth);
      dayint =  atoi(charday);
      yearint =  atoi(charyear);
      clock();
      break;
    default:
      display.setCursor((display.width()/2)-25,(display.height()/2)-3);
      display.print("Error");
      display.display();
  }
}

#define SECONDSINDAY 86400
#define SECONDSINHOUR 3600
#define SECONDSINMINUTE 60

void countdown() {
  if (selection == 'c') {set = !set; selection = ' ';}
  switch (set) {
    case true:
      inputtime();
      break;
    case false:
       display.setTextSize(1);
       display.setTextColor(BLACK);
       display.setCursor((display.width()/2)-25,(display.height()/2)-3);
       
       
       /*
       display.print(hourdown);
       display.print(':');
       display.print(minutedown);
       display.print(':');
       display.print(seconddown);*/
       break;
    default:
      display.setCursor((display.width()/2)-25,(display.height()/2)-3);
      display.print("Error");
      display.display();
  }
}

void showstatus(int menu) {
  //status stuff
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0, 0);
  //get menu choice
  switch (menu) { 
    case 0: 
      display.print("Menu");
      break;
    case -1:
      display.print("Error");
      break;
    default:
      display.print(selections[menu]);
     }
  
  //battery stuff
  checkbattery();
  display.drawLine(75, 2, 75, 8, BLACK);
  display.drawLine(60, 2, 60, 8, BLACK);
  display.drawLine(60, 4, 60, 6, WHITE);
  display.drawLine(59, 4, 59, 6, BLACK);
  display.drawLine(60, 2, 75, 2, BLACK);
  display.drawLine(60, 8, 75, 8, BLACK);
  if (battery > 75) {
  display.fillRect(62, 4, 12, 3, BLACK);
  }
  else if (battery > 50) {
  display.fillRect(65, 4, 9, 3, BLACK);
  }
  else if (battery > 25) {
  display.fillRect(68, 4, 6, 3, BLACK);
  }
  else if (battery < 25) {
  display.fillRect(71, 4, 3, 3, BLACK);
  }
  //line
  display.drawLine(0, 10, display.width(), 10, BLACK);
  display.display();
}

void checkbattery() {
//check battery
battery = 100;
}
