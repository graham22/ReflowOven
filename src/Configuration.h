
// Define the pin numbers and other constants

#ifndef CONFIGURATION_h
#define CONFIGURATION_h

/***************************************************************************
 *                              PID Behavior                               *
 ***************************************************************************/
#define SCREEN_SAVER_LIMIT 30000 // Turn on the 

/***************************************************************************
 *                              PID Behavior                               *
 ***************************************************************************/
#define TEMP_UPDATE_INTERVAL 200 // The interval between heater updates, in milliseconds.
#define TEMP_PID_Kp 25.0   // (P)roportional gain
#define TEMP_PID_Ki 8.0   // (I)ntegral gain
#define TEMP_PID_Kd 80.0   // (D)erivative gain

/***************************************************************************
 *                           Store 2D Points                               *
 ***************************************************************************/
struct Point {
  float x;
  float y;
};

/***************************************************************************
 *                                 Pins                                    *
 ***************************************************************************/
#define THERMOCOUPLE_SCLK 13
#define THERMOCOUPLE_CS 3
#define THERMOCOUPLE_MISO 12
#define BOTTOM_ELEMENT 0
#define TOP_ELEMENT 1
#define TFT_DC 9
#define TFT_CS 10
#define SD_CS 4

#define CTP_CLK 5
#define CTP_CS 8
#define CTP_IN 6
#define CTP_OUT 7
#define CTP_IRK 2
   
#endif // CONFIGURATION_h

