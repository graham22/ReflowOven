#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <UTouch.h>
#include <UTouchCD.h>

#include "PID.h"
#include "Point.h"
#include "SplashScreen.h"
#include "Screensaver.h"
#include "Button.h"
#include "Graph.h"
#include "Setting.h"
#include "Enumerations.h"
#include "Adafruit_ILI9341.h"

using namespace ReflowOven;

#define SECONDS_PER_PIXEL 0.111111

UTouch ctp(CTP_CLK, CTP_CS, CTP_IN, CTP_OUT, CTP_IRK);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
SplashScreen splash = SplashScreen(&tft, &ctp);
Screensaver screensaver = Screensaver(&tft, &ctp);
PID pid(TEMP_PID_Kp, TEMP_PID_Ki, TEMP_PID_Kd, DIRECT);

//Setting name, units, value, minimum, maximum, increment, yLoc, Adafruit_ILI9341, UTouch
Setting preheatSetting = Setting("Preheat", "C/s", 1.4, 1.3, 1.5, 0.01, 0, &tft, &ctp);
Setting soakTempSetting = Setting("Soak temp", "C", 150, 140, 180, 1, 45, &tft, &ctp);
Setting soakTimeSetting = Setting("Soak time", "s", 60, 40, 70, 1, 90, &tft, &ctp);
Setting rampUpSetting = Setting("Ramp up", "C/s", 0.6, 0.5, 0.7, 0.01, 135, &tft, &ctp);
Setting peakTempSetting = Setting("Peak temp", "C", 225, 210, 240, 1, 180, &tft, &ctp);
Setting peakTimeSetting = Setting("Peak time", "s", 25, 20, 30, 1, 225, &tft, &ctp);

Setting sterilizeTempSetting = Setting("Temperature", "C", 75, 60, 90, 1, 45, &tft, &ctp);
Setting sterilizeTimeSetting = Setting("Time", "s", 1800, 1200, 2400, 1000, 90, &tft, &ctp);

Graph graph = Graph();
Button reflowButton = Button();
Button sterilizeButton = Button();
Button startButton = Button();
Button cancelButton = Button();
ReflowState _state;

long lastPressedTime = 0;
long heatStartTime = 0;
long lastPrintTime = 0;

void setStanby()
{
	_state = Standby;
	tft.fillScreen(ILI9341_BLACK);
	reflowButton.drawMe();
	sterilizeButton.drawMe();
	lastPressedTime = millis();
}

void checkScreenSaver()
{
	if (millis() - lastPressedTime > SCREEN_SAVER_LIMIT)
	{
		screensaver.drawMe();
		_state = Standby;
		setStanby();
	}
}

void configureReflowParameters(void)
{
	_state = ConfigureReflow;
	tft.fillScreen(ILI9341_BLACK);
	preheatSetting.drawMe();
	soakTempSetting.drawMe();
	soakTimeSetting.drawMe();
	rampUpSetting.drawMe();
	peakTempSetting.drawMe();
	peakTimeSetting.drawMe();
	startButton.setActive(pid.getTemperature() < soakTempSetting.getValue());
	startButton.drawMe();
}

void configureSterilizeParameters(void)
{
	_state = ConfigureSterilize;
	tft.fillScreen(ILI9341_BLACK);
	sterilizeTempSetting.drawMe();
	sterilizeTimeSetting.drawMe();
	startButton.setActive(true);
	startButton.drawMe();
}

void cancel(void)
{
	pid.setSetpoint(0);
	digitalWrite(TOP_ELEMENT, LOW);
	digitalWrite(BOTTOM_ELEMENT, LOW);
	setStanby();
}

void beginReflow(void)
{
	_state = Reflow;
	graph.set(pid.updateMe(), preheatSetting.getValue(), soakTempSetting.getValue(), soakTimeSetting.getValue(),
			  rampUpSetting.getValue(), peakTempSetting.getValue(), peakTimeSetting.getValue(), &tft, &ctp);
	tft.fillScreen(ILI9341_BLACK);
	graph.drawLines();
	cancelButton.drawMe();
}

void beginSterilize(void)
{
	_state = Sterilize;
	graph.set(sterilizeTempSetting.getValue(), 1, sterilizeTempSetting.getValue(), 480.0, &tft, &ctp);
	tft.fillScreen(ILI9341_BLACK);
	graph.drawLines();
	cancelButton.drawMe();
}

void processReflowParameters(void)
{
	preheatSetting.updateMe();
	soakTempSetting.updateMe();
	soakTimeSetting.updateMe();
	rampUpSetting.updateMe();
	peakTempSetting.updateMe();
	peakTimeSetting.updateMe();
	if (startButton.updateMe())
	{
		beginReflow();
		heatStartTime = millis();
	}
}

void processSterilizeParameters(void)
{
	sterilizeTempSetting.updateMe();
	sterilizeTimeSetting.updateMe();
	if (startButton.updateMe())
	{
		beginSterilize();
		heatStartTime = millis();
	}
}

void reflow(void)
{
	float time = (millis() - heatStartTime) / 1000.00;
	float setpoint = graph.getSetpoint(time);
	if (cancelButton.updateMe() || setpoint == 0)
	{
		cancel();
		return;
	}
	pid.setSetpoint(setpoint);
	float temp = pid.updateMe();
	if (millis() - lastPrintTime > 1000)
	{
		lastPrintTime = millis();
		graph.plotLine(time, temp);
		graph.printValues(time, temp);
	}
}

void sterilize(void)
{
	if (cancelButton.updateMe())
	{
		cancel();
		return;
	}
	float timeInSeconds = ((millis() - heatStartTime) / 1000.00);
	if ( timeInSeconds < sterilizeTimeSetting.getValue())
	{
		pid.setSetpoint(sterilizeTempSetting.getValue());
		float temp = pid.updateMe();
		if (millis() - lastPrintTime > 1000)
		{
			lastPrintTime = millis();
			graph.plotDot(timeInSeconds, temp);
			graph.printValues(timeInSeconds, temp);
		}
	}
	else
	{
		cancel();
		return;
	}
}

void setup(void)
{
	// sd card off
	pinMode(SD_CS, OUTPUT);
	digitalWrite(SD_CS, HIGH);
	delay(500);
	tft.begin();
	delay(500);
	pid.begin();
	pid.setSetpoint(0);
	ctp.InitTouch(PORTRAIT);
	ctp.setPrecision(PREC_HI);
	splash.drawMe(); // waits here for someone to touch
	tft.fillScreen(ILI9341_BLACK);
	startButton.set("Start", 55, 272, 125, 45, 3, &tft, &ctp); // name, xLoc, yLoc, width, height, textSize, Adafruit_ILI9341*, UTouch*
	cancelButton.set("Cancel", 5, 5, 90, 60, 2, &tft, &ctp);
	reflowButton.set("Reflow", 20, 115, 200, 45, 3, &tft, &ctp);
	sterilizeButton.set("Sterilize", 20, 180, 200, 45, 3, &tft, &ctp);
	_state = Standby;
	reflowButton.drawMe();
	sterilizeButton.drawMe();
	lastPressedTime = millis();
}

void loop(void)
{
	if (ctp.dataAvailable())
	{
		lastPressedTime = millis();
	}

	switch (_state)
	{
	case Standby:
		checkScreenSaver();
		if (reflowButton.updateMe())
		{
			_state = ConfigureReflow;
			configureReflowParameters();
		}
		if (sterilizeButton.updateMe())
		{
			_state = ConfigureSterilize;
			configureSterilizeParameters();
		}
		break;
	case ConfigureReflow:
		checkScreenSaver();
		processReflowParameters();
		break;
	case ConfigureSterilize:
		checkScreenSaver();
		processSterilizeParameters();
		break;
	case Reflow:
		reflow();
		break;
	case Sterilize:
		sterilize();
		break;
	case Complete:
		checkScreenSaver();
		break;
	}

}