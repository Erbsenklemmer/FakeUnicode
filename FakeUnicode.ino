#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>

#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeMonoBoldOblique9pt7b.h>
#include <Fonts/FreeSansBoldOblique9pt7b.h>
#include <Fonts/FreeSerifBold9pt7b.h>

#include <map>

#define ENABLE_GxEPD2_GFX 0

#include "GxEPD2_3C.h"

GxEPD2_3C<GxEPD2_420c, GxEPD2_420c::HEIGHT> display(GxEPD2_420c(D8, D3, D1, D2));

void setup() {
  Serial.begin(115200);

  // display.init(115200);

  // display.setRotation(1);
  // display.setFullWindow();

  display.setCursor(10, 50);
  display.setFont(&FreeMono9pt7b);

  // do 
  // {
  //   display.fillScreen(GxEPD_BLACK);    
  // } 
  // while(display.nextPage());

  //tests
  
//  display.hibernate();
}

void loop() {
  // put your main code here, to run repeatedly:

  String text = "ÜÄaä°dÖaÜifüäÜÜ";

  Serial.println(text);
  Serial.println(preUnicode(text));

  printUnicode(text, &FreeMono9pt7b);

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

void printUnicode(const String& text, const GFXfont *font) {

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

  // int16_t curX = display.getCursorX();
  // int16_t curY = display.getCursorY();

  int16_t tbx, tby; 
  uint16_t tbw, heightUpper, heightLower;

  display.getTextBounds("U", 0, 0, &tbx, &tby, &tbw, &heightUpper);
  display.getTextBounds("a", 0, 0, &tbx, &tby, &tbw, &heightLower);

  String preUni = preUnicode(text);
  display.print(preUni);

  int umlOffset = 0;
  for (std::map<int, String>::iterator iter = umls.begin(); iter != umls.end(); iter++) {
    String start = preUni.substring(0, umlOffset + iter->first);
    String end = preUni.substring(0, umlOffset + iter->first + 1);    
    umlOffset--;

    Serial.println(iter->second + ": Start: " + start + ", end: " + end);
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