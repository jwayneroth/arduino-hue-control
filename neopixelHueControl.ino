#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include <Adafruit_NeoPixel.h>
#include "wifiCredentials.h" //defines WLAN_SSID, WLAN_PASS, WLAN_SECURITY

#define ADAFRUIT_CC3000_IRQ   3              // MUST be an interrupt pin!
#define ADAFRUIT_CC3000_VBAT  5              // any pin
#define ADAFRUIT_CC3000_CS    10             // any pin

Adafruit_CC3000 cc3000 = Adafruit_CC3000(    // On an UNO, SCK = 13, MISO = 12, and MOSI = 11
	ADAFRUIT_CC3000_CS, 
	ADAFRUIT_CC3000_IRQ, 
	ADAFRUIT_CC3000_VBAT,
	SPI_CLOCK_DIV2
); 

#define HUE_IP_STRING   "192.168.2.16"
#define HUE_LIGHT_URL   "/api/jwayneroth/lights/"
#define IDLE_TIMEOUT_MS 3000

uint32_t HUE_IP = cc3000.IP2U32(192,168,2,16);

#define PIXEL_PIN 9

Adafruit_NeoPixel strip = Adafruit_NeoPixel(7, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

int num_active_pixels = strip.numPixels();
int init_brightness = 26;
int max_brightness = 255;
int brightness = init_brightness; // 0 - 255
int red = 255;
int green = 255;
int blue = 255;

#define BTN_1 4
#define BTN_2 6
#define BTN_3 7
#define BTN_4 8
#define BTN_5 A2
#define NUM_BUTTONS 5

int buttons[] = {BTN_1, BTN_2, BTN_3, BTN_4, BTN_5};

#define HUE_TOGGLE A3

#define BUSY_LED A0
#define ERROR_LED A1

boolean hueOn; 
int hueLightNumber = 1;
int hueBrightness; 
long hueHue;  
String hueCmd; 
float hueCIEX;
float hueCIEY;
boolean hueMode = false;

#include "wifiFunctions.h"
#include "hueFunctions.h"
#include "pixelFunctions.h"
#include "buttonFunctions.h"

void setup(void) {

  Serial.begin(115200);

	strip.setBrightness(brightness);
	strip.begin();
	strip.show();
	
	setRGB(red,green,blue); //start pixels white
	
	initButtons();
	
	pinMode(HUE_TOGGLE, INPUT);
	
	pinMode(BUSY_LED, OUTPUT);
	pinMode(ERROR_LED, OUTPUT);

}

void loop(void) {
	
	checkHueModeChange();
	
	int button = checkButtons();
	
	if(button) {
		
		switch(button) {
			case 1:
				incrementRed();
				break;
			case 2:
				incrementGreen();
				break;
			case 3:
				incrementBlue();
				break;
			case 4:
				incrementBrightness();
				break;
			case 5:
				if(hueMode == true) {
					delay(1000);
					if (checkButtons() == 5) {
						toggleHueLight();
						delay(1000);
					}else{
						switchHueLightNumber();
					}
				}
				break;
			default:
				break;
		}

		if(hueMode == true && button < 5) {
		
			setBusyLED(1);
		
			if( updateHue(red, green, blue, brightness) ) {
			
				setErrorLED(0);
			
			}else{
		
				setErrorLED(1);
				
				//try again (uuggh)
				if( updateHue(red, green, blue, brightness) ) {
					setErrorLED(0);
				}
			
			}
		
			setBusyLED(0);
		
		}
	}
		
  delay(100);
  
}

void checkHueModeChange() {
	
	//hueMode turned on
	//open wifi
	if(hueMode == false && digitalRead(HUE_TOGGLE) == HIGH) {
		
		Serial.println("hueMode turned on!");
		
		hueMode = true;
		
		setBusyLED(1);
		
		if(wifiConnection()) {
			
			if( getHueLightStatus() ) {
				
				if (hueOn == false) toggleHueLight();
				
				delay(100);
				
				apeHueLight();
				
			}else{
				
				setErrorLED(1);
			
			}
	
			setBusyLED(0);
			
			delay(100);
			
		}else{
			setErrorLED(1);
		}
	
	//hueMode turned off
	//close wifi
	} else if(hueMode == true && digitalRead(HUE_TOGGLE) == LOW) {
			
			hueMode = false;
			
			Serial.println(F("\nDisconnecting"));
			
			cc3000.disconnect();
			
			setBusyLED(0);
			setErrorLED(0);
			
			delay(100);
	}  
	
}

void switchHueLightNumber() {
	
	Serial.println();Serial.print("switchHueLightNumber ");
	
	if(hueLightNumber == 1) {
		hueLightNumber = 2;
	}else{
		hueLightNumber = 1;
	}

	Serial.println(hueLightNumber);

	setBusyLED(1);
		
	if( getHueLightStatus() ) {
		
		if (hueOn == false) toggleHueLight();
		
		delay(100);
			
		apeHueLight();
			
	}else{
			
		setErrorLED(1);
		
		//try again please (this is a cruddy fix :( )
		if( getHueLightStatus() ) {
			
			if (hueOn == false) toggleHueLight();
			
			delay(100);
			
			apeHueLight();
			
			setErrorLED(0);
		
		}
	
	}
	
	setBusyLED(0);

}

void setBusyLED( byte onoff ) {
	digitalWrite( BUSY_LED, onoff );
}

void setErrorLED( byte onoff ) {
	digitalWrite( ERROR_LED, onoff );
}

