

// Plot the temperature vs. time setpoint and current temperature curves

#ifndef GRAPH_h
#define GRAPH_h

#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>
#include <UTouch.h>
#include "Configuration.h"


class Graph {  
  public:
    Graph(void);
    void set(float, float, float, float, float, float, float, Adafruit_ILI9341*, UTouch*);
    void drawLines(void);
    float getSetpoint(float);
    float plotLine(float, float);
    void printValues(float, float);
    
  private:
    void openDoor(void);
    Adafruit_ILI9341* ptr_tft;
    UTouch* ptr_ctp;
    Point points[6];
    Point pointToDisplay(Point);
    float lastTime, lastTemp;
    float floatMap(float, float, float, float, float);
};


#endif // GRAPH_h

