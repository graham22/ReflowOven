#include <UTouch.h>
#include <UTouchCD.h>


// Setting class which contains buttons to increment or decremement parameters of the reflow profile

#ifndef SETTING_h
#define SETTING_h

#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>
#include "Configuration.h"
#include "Button.h"


class Setting {  
  public:
    Setting(String, String, float, float, float, float, 
            int, Adafruit_ILI9341*, UTouch*);
    void updateMe(void);
    void drawMe(void);
    float getValue(void);

  private:
    String setting;
    String units;
    float value;
    float maximum;
    float minimum;
    float increment;
    int yLoc;
    
    Button increase;
    Button decrease;
    
    uint8_t width;
    uint8_t height;
    
    uint16_t boxColor;
    
    Adafruit_ILI9341* ptr_tft;
    UTouch* ptr_ctp;
    
    boolean drawn;
    void drawValue(uint16_t);
    boolean areNearlyEqual(float, float);
};


#endif // SETTING_h

