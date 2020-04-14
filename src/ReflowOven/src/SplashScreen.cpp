
#ifndef SPLASHSCREEN_cpp
#define SPLASHSCREEN_cpp

#include "SplashScreen.h"

namespace ReflowOven
{
SplashScreen::SplashScreen(Adafruit_ILI9341 *ptr_tft, UTouch *ptr_ctp)
{
  this->ptr_tft = ptr_tft;
  this->ptr_ctp = ptr_ctp;
}

void SplashScreen::drawMe(void)
{
  ptr_tft->fillScreen(ILI9341_BLACK);

  int16_t color0 = ILI9341_GREEN;
  int16_t color1 = ILI9341_RED;

  ptr_tft->setTextColor(color1);
  ptr_tft->setTextSize(4);
  ptr_tft->setCursor(52, 15);
  ptr_tft->println(F("Reflow"));

  //  for (int i = 0; i < 48; i+=8){
  //    int16_t color;
  //    // select the color according to the face
  //    // dark grey
  //    if (i/8 <= 2){
  //     color = color0;
  //    }
  //    else {
  //      color = color1;
  //    }
  //    ptr_tft->fillTriangle(fj[i+0]+xLoc, fj[i+1]+yLoc, fj[i+2]+xLoc,
  //                          fj[i+3]+yLoc, fj[i+4]+xLoc, fj[i+5]+yLoc, color);
  //    ptr_tft->fillTriangle(fj[i+0]+xLoc, fj[i+1]+yLoc, fj[i+4]+xLoc,
  //                          fj[i+5]+yLoc, fj[i+6]+xLoc, fj[i+7]+yLoc, color);
  //  }

  ptr_tft->setTextColor(color0);
  ptr_tft->setTextSize(2);
  ptr_tft->setCursor(18, 180);
  ptr_tft->println(F("touch to continue"));
  ptr_tft->setTextSize(2);
  ptr_tft->setCursor(27, 300);
  ptr_tft->println(F("SkyeTracker.com"));
  while (!ptr_ctp->dataAvailable())
    ;
}
} // namespace ReflowOven

#endif // SPLASHSCREEN_cpp
