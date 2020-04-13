
#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>
#include <UTouch.h>


// Draw and update the buttons on the LCD

#ifndef BUTTON_h
#define BUTTON_h


#include "Configuration.h"

class Button {  
  public:
    Button(void);
    void set(String, int, int, uint8_t, uint8_t, uint8_t, 
             Adafruit_ILI9341*, UTouch*);
    void drawMe(void);
    void setActive(boolean);
    boolean updateMe(void);

  private:
    String name;
    int xLoc;
    int yLoc;
    uint8_t textSize;
    uint8_t width;
    uint8_t height;
    Adafruit_ILI9341* ptr_tft;
    UTouch* ptr_ctp;
    
    boolean over;
    boolean active;
    boolean prevOver;
    
    void isOver(int, int);
};

#endif // BUTTON_h

