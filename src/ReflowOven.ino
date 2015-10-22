
#include "SPI.h"
#include "PID.h"
#include "Configuration.h"
#include "SplashScreen.h"
#include "Screensaver.h"
#include "Button.h"
#include "Graph.h"
#include "Setting.h"
#include "Wire.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "UTouch.h"
#include "UTouchCD.h"

UTouch ctp(CTP_CLK, CTP_CS, CTP_IN, CTP_OUT, CTP_IRK);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
SplashScreen splash = SplashScreen(&tft, &ctp);
Screensaver screensaver = Screensaver(&tft, &ctp);
PID pid(TEMP_PID_Kp, TEMP_PID_Ki, TEMP_PID_Kd, DIRECT);

Setting preheatSetting = Setting("Preheat", "C/s", 1.4, 1.3, 1.5, 0.01, 0, &tft, &ctp);
Setting soakTempSetting = Setting("Soak temp", "C", 150, 140, 180, 1, 45, &tft, &ctp);
Setting soakTimeSetting = Setting("Soak time", "s", 60, 40, 70, 1, 90, &tft, &ctp);
Setting rampUpSetting = Setting("Ramp up", "C/s", 0.6, 0.5, 0.7, 0.01, 135, &tft, &ctp);
Setting peakTempSetting = Setting("Peak temp", "C", 225, 210, 240, 1, 180, &tft, &ctp);
Setting peakTimeSetting = Setting("Peak time", "s", 25, 20, 30, 1, 225, &tft, &ctp);

Graph graph = Graph();
Button reflowButton = Button();
Button cancelButton = Button();

boolean isReflowing = false;
long lastPressedTime = 0;
long reflowStartTime = 0;
long lastPrintTime = 0;


void setup(void) {

	//Serial.begin(115200);
	// sd card off
	pinMode(SD_CS, OUTPUT);
	digitalWrite(SD_CS, HIGH);
	delay(500);
	tft.begin();
	delay(500);
	pid.begin();
	ctp.InitTouch(PORTRAIT);
	ctp.setPrecision(PREC_HI);
	splash.drawMe();
	tft.fillScreen(ILI9341_BLACK);
	pid.setSetpoint(0);
	reflowButton.set("Reflow", 55, 272, 130, 45, 3, &tft, &ctp);
	cancelButton.set("Cancel", 5, 5, 90, 60, 2, &tft, &ctp);
	reflowButton.drawMe();
	isReflowing = false;
	lastPressedTime = millis();
}


void loop(void) {

	if (ctp.dataAvailable()) {
		lastPressedTime = millis();
	}
	if (millis() - lastPressedTime > SCREEN_SAVER_LIMIT && !isReflowing) {
		screensaver.drawMe();
		resetSettings();
		lastPressedTime = millis();
	}

	//Serial.print("F = ");
	//Serial.println(pid.getTemperature());

	//delay(1000);

	if (isReflowing) {
		reflow();
	}
	else {
		setReflowButtonActive();
		settings();
	}
}


void reflow(void) {
	float time = (millis() - reflowStartTime) / 1000.00;
	float setpoint = graph.getSetpoint(time);
	if (cancelButton.updateMe() || setpoint == 0) {
		cancel();
		return;
	}
	pid.setSetpoint(setpoint);
	float temp = pid.updateMe();
	if (millis() - lastPrintTime > 1000) {
		lastPrintTime = millis();
		graph.plotLine(time, temp);
		graph.printValues(time, temp);
	}
}


void settings(void) {
	preheatSetting.updateMe();
	soakTempSetting.updateMe();
	soakTimeSetting.updateMe();
	rampUpSetting.updateMe();
	peakTempSetting.updateMe();
	peakTimeSetting.updateMe();
	if (reflowButton.updateMe()) {
		beginReflow();
		reflowStartTime = millis();
	}
}


void cancel(void) {
	isReflowing = false;
	pid.setSetpoint(0);
	digitalWrite(TOP_ELEMENT, LOW);
	digitalWrite(BOTTOM_ELEMENT, LOW);
	resetSettings();
	lastPressedTime = millis();
}


void beginReflow(void) {
	isReflowing = true;
	graph.set(pid.updateMe(), preheatSetting.getValue(), soakTempSetting.getValue(), soakTimeSetting.getValue(),
		rampUpSetting.getValue(), peakTempSetting.getValue(), peakTimeSetting.getValue(), &tft, &ctp);
	tft.fillScreen(ILI9341_BLACK);
	graph.drawLines();
	cancelButton.drawMe();


}


void resetSettings(void) {
	tft.fillScreen(ILI9341_BLACK);
	preheatSetting.drawMe();
	soakTempSetting.drawMe();
	soakTimeSetting.drawMe();
	rampUpSetting.drawMe();
	peakTempSetting.drawMe();
	peakTimeSetting.drawMe();
	setReflowButtonActive();
	reflowButton.drawMe();
}


void setReflowButtonActive(void) {
	float temp = pid.getTemperature();
	if (temp >= soakTempSetting.getValue()) {
		reflowButton.setActive(false);
	}
	else {
		reflowButton.setActive(true);
	}
}

