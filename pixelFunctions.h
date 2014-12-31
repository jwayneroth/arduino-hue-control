/* 
 * solidWhite
*/
void solidWhite()
{
	for( int i=0; i<num_active_pixels; i++ ) {
		strip.setPixelColor(i, strip.Color(255,255,255));
		strip.show();
	}
}

void setRGB(int red, int green, int blue) {
	if(brightness == 0) {
		brightness = 10;
		strip.setBrightness(brightness);
	}
	for( int i=0; i<num_active_pixels; i++ ) {
		strip.setPixelColor(i, strip.Color(red,green,blue));
		strip.show();
	}
}

/*
 * apeHueLight 
*/
void apeHueLight() {
	
	int rgbValues[3];
	
	colorFromXY(hueCIEX, hueCIEY, hueBrightness, rgbValues);
	
	Serial.println("apeHueLight");
	//Serial.print("red: ");Serial.print(rgbValues[0]);
	//Serial.print(" green: ");Serial.print(rgbValues[1]);
	//Serial.print(" blue: ");Serial.println(rgbValues[2]);
	
	red = rgbValues[0];
	green = rgbValues[1];
	blue = rgbValues[2];
	
	setRGB(rgbValues[0], rgbValues[1], rgbValues[2]);
	
	brightness = hueBrightness;
	strip.setBrightness(brightness);
	strip.show();

}

void incrementRed() {
	red += 26;
	if(red == 260) {
		red = 255;
	} else if(red > 255) {
		red = 0;
	}
	setRGB(red,green,blue);
}

void incrementGreen() {
	green += 26;
	if(green == 260) {
		green = 255;
	} else if(green > 255) {
		green = 0;
	}
	setRGB(red,green,blue);
}

void incrementBlue() {
	blue += 26;
	if(blue == 260) {
		blue = 255;
	} else if(blue > 255) {
		blue = 0;
	}
	setRGB(red,green,blue);
}

void incrementBrightness() {
	brightness += 26;
	if(brightness == 26) {
		strip.setBrightness(brightness);
		setRGB(red,green,blue);
		return;
	}
	if(brightness == 260) {
		brightness = 255;
	} else if(brightness > max_brightness) {
		brightness = 0;
	}
	Serial.print("brightness ");
	Serial.println(brightness);
	strip.setBrightness(brightness);
	strip.show();
}