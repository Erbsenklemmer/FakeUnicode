#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>

#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBoldOblique9pt7b.h>

#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>

#include <Fonts/FreeSansOblique9pt7b.h>
#include <Fonts/FreeSansBoldOblique9pt7b.h>

#include <Fonts/FreeSerifBold9pt7b.h>

#include <map>

#define ENABLE_GxEPD2_GFX 0

#include "GxEPD2_3C.h"

GxEPD2_3C<GxEPD2_420c, GxEPD2_420c::HEIGHT> display(GxEPD2_420c(D8, D3, D1, D2));

const uint16_t COLOR_FOREGROUND = GxEPD_BLACK;
const uint16_t COLOR_BACKGROUND = GxEPD_WHITE;
const uint16_t COLOR_RED = GxEPD_RED;

void setup() {
  Serial.begin(115200);

  display.init(115200);

  display.setRotation(1);
  display.setFullWindow();

  // do 
  // {
  //   display.fillScreen(COLOR_BACKGROUND);    
  // } 
  // while(display.nextPage());

  //tests
  
//  display.hibernate();
}

void loop() {
  // put your main code here, to run repeatedly:

  //String text = "ÜÄaä°dÖaÜifüäÜÜ";
  String text = "ÜberwiegendÜ bäwölkt";

  Serial.println(text);
  Serial.println(preUnicode(text));
  
  display.setTextColor(COLOR_FOREGROUND);
  
  do 
  {
    display.fillScreen(COLOR_BACKGROUND);    

    display.setCursor(10, 50);
    display.setFont(&FreeMono9pt7b);
    printUnicode(text, COLOR_FOREGROUND, false);

    display.setCursor(10, 80);
    display.setFont(&FreeSans9pt7b);
    printUnicode(text, COLOR_FOREGROUND, false);

    display.setCursor(10, 110);
    display.setFont(&FreeSansBold9pt7b);
    printUnicode(text, COLOR_FOREGROUND, true);

    display.setCursor(10, 140);
    display.setFont(&FreeSansBold12pt7b);
    printUnicode(text, COLOR_FOREGROUND, true);
  } 
  while(display.nextPage());

 display.hibernate();

  while(true) {
    delay(1000);
  }
}

String preUnicode(const String& text) {
  String ret = text;
  ret.replace("Ä", "A");
  ret.replace("Ö", "O");
  ret.replace("Ü", "U");

  ret.replace("ä", "a");
  ret.replace("ö", "o");
  ret.replace("ü", "u");

  ret.replace("°", " ");
  return ret;
}

void printUnicode(const String& text, uint16_t textColor, bool isBold) {

  Serial.println(text + ": Hello from printUnicode ************************");
  Serial.println("Unicode length: " + String(text.length()));

  std::map<int, String> umls;

  findUmls(text, "Ä", umls);
  findUmls(text, "Ö", umls);
  findUmls(text, "Ü", umls);

  findUmls(text, "ä", umls);
  findUmls(text, "ö", umls);
  findUmls(text, "ü", umls);

  findUmls(text, "°", umls);

  // for (std::map<int, String>::iterator iter = umls.begin(); iter != umls.end(); iter++) {
  //   Serial.println(iter->second + " at map: " + String(iter->first));
  // }

  int16_t cursorX = display.getCursorX();
  int16_t cursorY = display.getCursorY();

  int16_t tbx, tby; 
  uint16_t tbw, tbh, heightUpper, heightLower;

  display.getTextBounds("U", 0, 0, &tbx, &tby, &tbw, &heightUpper);
  display.getTextBounds("a", 0, 0, &tbx, &tby, &tbw, &heightLower);

  const String preUni = preUnicode(text);
  display.print(preUni);

  int umlOffset = 0;
  for (std::map<int, String>::iterator iter = umls.begin(); iter != umls.end(); iter++) {
    String start = preUni.substring(0, umlOffset + iter->first);
    String end = preUni.substring(0, umlOffset + iter->first + 1);
    umlOffset--;

    Serial.println(iter->second + ": Start: " + start + ", end: " + end);

    uint16_t widthStart, widthEnd;
    if (start.length() == 0) {
      widthStart = 0;
    }
    else {
      display.getTextBounds(start, 0, 0, &tbx, &tby, &widthStart, &tbh);
    }
    display.getTextBounds(end, 0, 0, &tbx, &tby, &widthEnd, &tbh);

    Serial.println("widthStart: " + String(widthStart) + " widthEnd: " + String(widthEnd));

    //reduce some pixels at start
    uint16_t marginLeft;
    if (start.length() == 0) {
      marginLeft = (widthEnd - widthStart) / 4;
    }
    else {
      marginLeft = (widthEnd - widthStart) / 3;
    }
    widthStart += marginLeft;

    uint16_t marginRight = (widthEnd - widthStart) / 4 + 1;
    widthEnd -= marginRight;

    uint16_t x1 = cursorX + widthStart;
    uint16_t x2 = cursorX + widthEnd;
    uint16_t y = -1;
    bool drawDegree = false;
    const String uml = iter->second;
    if(uml == "ä" || uml == "ö" || uml == "ü") {
      y = cursorY - heightLower-1;
    }
    else if(uml == "Ä" || uml == "Ö" || uml == "Ü") {
      y = cursorY - heightUpper-1;
    }
    uint16_t color = COLOR_RED;
    if (drawDegree == false) {
      display.drawPixel(x1  , y, color);
      display.drawPixel(x1+1, y, color);
      display.drawPixel(x1  , y-1, color);
      display.drawPixel(x1+1, y-1, color);
      
      if (isBold){
        display.drawPixel(x1+2, y, color);
        display.drawPixel(x1  , y-2, color);
        display.drawPixel(x1+1, y-2, color);
        display.drawPixel(x1+2, y-2, color);
      }

      display.drawPixel(x2  , y, color);
      display.drawPixel(x2-1, y, color);
      display.drawPixel(x2  , y-1, color);
      display.drawPixel(x2-1, y-1, color);

      if (isBold) {
        display.drawPixel(x2+2, y, color);
        display.drawPixel(x2  , y-2, color);
        display.drawPixel(x2+1, y-2, color);
        display.drawPixel(x2+2, y-2, color);
      }
      //display.drawLine(x1, y, x2, y, color);

      Serial.println(uml + " x: " + String(x1) + ", y: " + String(y));
      Serial.println(uml + " x: " + String(x2) + ", y: " + String(y));
    }
  }
}

void findUmls(const String& text, const String& toFind, std::map<int, String>& umls) {
  int pos = text.indexOf(toFind);
  while (pos > -1) {
    //Serial.println(toFind + " at: " + String(pos));
    umls[pos] = toFind;
    pos = text.indexOf(toFind, pos + 1);
  }
}