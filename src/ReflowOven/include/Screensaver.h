

// Draw a little "Duke ECE" screensaver to bounce around the display

#ifndef SCREENSAVER_h
#define SCREENSAVER_h

#include <UTouch.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>

namespace ReflowOven
{
class Screensaver
{
public:
  Screensaver(Adafruit_ILI9341 *, UTouch *);
  void drawMe(void);

private:
  Adafruit_ILI9341 *ptr_tft;
  UTouch *ptr_ctp;
};
} // namespace ReflowOven

#endif // SCREENSAVER_h
