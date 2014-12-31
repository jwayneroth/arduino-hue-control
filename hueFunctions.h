/*
 * calculateXY
 * https://github.com/PhilipsHue/PhilipsHueSDK-iOS-OSX/blob/master/ApplicationDesignNotes/RGB%20to%20xy%20Color%20conversion.md
*/
void calculateXY (int red, int green, int blue, float* newCIE) {
	
	float r = red;//(red   > 0.04) ? pow((red   + 0.06) / (1.0 + 0.056), 2.4) : (red   / 12.92);
	float g = green;//(green > 0.04) ? pow((green + 0.06) / (1.0 + 0.056), 2.4) : (green / 12.92);
	float b = blue;//(blue  > 0.04) ? pow((blue  + 0.06) / (1.0 + 0.056), 2.4) : (blue  / 12.92);

	float X = r * 0.65 + g * 0.10 + b * 0.20;
	float Y = r * 0.23 + g * 0.74 + b * 0.02;
	float Z = r * 0.0 + g * 0.05 + b * 1.04;

	float cx = X / (X + Y + Z);
	float cy = Y / (X + Y + Z);

	if (isnan(cx)) {
		cx = 0.0;
	}

	if (isnan(cy)) {
		cy = 0.0;
	}
	
	newCIE[0] = cx;
	newCIE[1] = cy;

}

/*
 * colorFromXY
 * https://github.com/PhilipsHue/PhilipsHueSDK-iOS-OSX/blob/master/ApplicationDesignNotes/RGB%20to%20xy%20Color%20conversion.md
*/
void colorFromXY( float x, float y, int bri, int* rgbValues) {

	//Serial.println("colorFromXY");

	// Calculate XYZ values Convert using the following formulas:
	float Y = 1.0;//bri;
	float X = (Y / y) * x;
	float Z = (Y / y) * (1.0 - x - y);

	// Convert to RGB using Wide RGB D65 conversion.
	float r = X  * 3.24 - Y * 1.54 - Z * 0.50;
	float g = -X * 0.97 + Y * 1.88 + Z * 0.041;
	float b = X  * 0.06 - Y * 0.20 + Z * 1.057;
	
	/*
	Serial.print("red: ");Serial.print(r);
	Serial.print(" green: ");Serial.print(g);
	Serial.print(" blue: ");Serial.println(b);Serial.println();
	*/
	
	if (r > b && r > g && r > 1.0) {
		// red is too big
		g = g / r;
		b = b / r;
		r = 1.0;
	}
	else if (g > b && g > r && g > 1.0) {
		// green is too big
		r = r / g;
		b = b / g;
		g = 1.0;
	}
	else if (b > r && b > g && b > 1.0) {
		// blue is too big
		r = r / b;
		g = g / b;
		b = 1.0;
	}
	
	/*
	Serial.print("red: ");Serial.print(r);
	Serial.print(" green: ");Serial.print(g);
	Serial.print(" blue: ");Serial.println(b);Serial.println();
	*/
	
	/*
	// Apply gamma correction
	r = r <= 0.003 ? 12.92 * r : (1.0 + 0.06) * pow(r, (1.0 / 2.4)) - 0.055;
	g = g <= 0.003 ? 12.92 * g : (1.0 + 0.06) * pow(g, (1.0 / 2.4)) - 0.055;
	b = b <= 0.003 ? 12.92 * b : (1.0 + 0.06) * pow(b, (1.0 / 2.4)) - 0.055;
	*/
	
	/*
	Serial.print("red: ");Serial.print(r);
	Serial.print(" green: ");Serial.print(g);
	Serial.print(" blue: ");Serial.println(b);Serial.println();
	*/
	
	if (r > b && r > g) {
		// red is biggest
		if (r > 1.0) {
			g = g / r;
			b = b / r;
			r = 1.0;
		}
	}
	else if (g > b && g > r) {
		// green is biggest
		if (g > 1.0) {
			r = r / g;
			b = b / g;
			g = 1.0;
		}
	}
	else if (b > r && b > g) {
		// blue is biggest
		if (b > 1.0) {
			r = r / b;
			g = g / b;
			b = 1.0;
		}
	}
	
	if(r < 0) r = 0;
	if(g < 0) g = 0;
	if(b < 0) b = 0;
	
	/*
	Serial.print("red: ");Serial.print(r);
	Serial.print(" green: ");Serial.print(g);
	Serial.print(" blue: ");Serial.println(b);Serial.println();
	*/
	
	rgbValues[0] = (int)(r * 255);
	rgbValues[1] = (int)(g * 255);
	rgbValues[2] = (int)(b * 255);

}

/*
 * sendData 
*/
void sendData( Adafruit_CC3000_Client& client, String input, int length, int chunkSize ) {

  int max_iteration = (length > chunkSize) ? (int)(length/chunkSize) + 1 : 1;
  
  for (int i = 0; i < max_iteration; i++) {
    client.print(input.substring(i * chunkSize, (i+1) * chunkSize));
    //Serial.println(input.substring(i * chunkSize, (i+1) * chunkSize));
    //wdt_reset();
  }  

}


/*
 * getHueLightStatus
*/
boolean getHueLightStatus() {
	
	Serial.println("getHueLightStatus");
	
	Adafruit_CC3000_Client client = cc3000.connectTCP( HUE_IP , 80);
		
	if (client.connected()) {

		client.fastrprint(F("GET "));client.fastrprint(HUE_LIGHT_URL);client.print(hueLightNumber);client.fastrprint(F(" HTTP/1.1\r\n"));
		client.fastrprint(F("Host: ")); client.fastrprint(HUE_IP_STRING); client.fastrprint(F("\r\n"));
		client.fastrprint(F("Content-type: application/json\r\n"));
		client.fastrprint(F("keep-alive\r\n"));
		client.fastrprint(F("\r\n"));

		Serial.println(F("-------------------------------------"));

		/* Read data until either the connection is closed, or the idle timeout is reached. */ 
		/*
		unsigned long lastRead = millis();
		while (client.connected() && (millis() - lastRead < IDLE_TIMEOUT_MS)) {
			while (client.available()) {
				char c = client.read();
				Serial.print(c);
				lastRead = millis();
			}
		}
		*/
		///*
		while(client.connected()) {
			if(client.available()) {
				findUntil(client, "\"on\":", "\0");
				hueOn = (readStringUntil(client, ',' ) == "true");  // if light is on, set variable to true
	
				findUntil(client, "\"bri\":", "\0");
				hueBrightness = readStringUntil(client, ',').toInt();  // set variable to brightness value

				//findUntil("\"hue\":", "\0");
				//hueHue = readStringUntil(',').toInt();  // set variable to hue value
		
				//String.toCharArray(buffer, 10);
				//float pH = atof(buffer);
		
				char buffer[10];
		
				findUntil(client, "\"xy\":[", "\0");
				readStringUntil(client, ',').toCharArray(buffer, 10);//toFloat();  
				hueCIEX = atof(buffer);
		
				//findUntil("\"hue\":", "\0");
				readStringUntil(client, ']').toCharArray(buffer, 10);//toFloat();  
				hueCIEY = atof(buffer);
		
				Serial.print("hueOn : ");Serial.println(hueOn);
				Serial.print("hueBrightness : ");Serial.println(hueBrightness);
				//Serial.print("hueHue : ");Serial.println(hueHue);
				Serial.print("hueCIEX : ");Serial.println(hueCIEX);
				Serial.print("hueCIEY : ");Serial.println(hueCIEY);
				
				while (client.available()) {
					char c = client.read();
					//Serial.print(c);
					//lastRead = millis();
				}
			
				client.close();
				return 1;
				
			}
		}
		
		client.close();

	} else {
		Serial.println(F("Connection to Hue failed"));    
		return 0;
	}

	return 0;

}

/*
 * updateHue
*/
boolean updateHue(int red, int green, int blue, int brightness) {
	
	Serial.println("updateHue");
	
	Adafruit_CC3000_Client client = cc3000.connectTCP( HUE_IP , 80);
	
	if (client.connected()) {
		while(client.connected()) {
			
			float newCIE[2];
			String hueCommand = "";
			int commandLength = 0;
			int cieX;
			int cieY;
			
			calculateXY(red, green, blue, newCIE);
			
			//newCIE[0] = 0.9;
			//newCIE[1] = 0.9;
			
			cieX = newCIE[0] * 100;//(newCIE[0] < 1) ? newCIE[0] * 100 : 1;
			cieY = newCIE[1] * 100;//(newCIE[1] < 1) ? newCIE[1] * 100 : 1;
			
			Serial.print("cieX: ");Serial.print(cieX);
			Serial.print(" cieY: ");Serial.println(cieY);

			hueCommand = hueCommand + "\n{\"xy\":[";
			hueCommand = (cieX == 100) ? hueCommand + "1" : hueCommand + "0.";
			hueCommand = hueCommand + String(cieX) + ",";
			hueCommand = (cieY == 100) ? hueCommand + "1" : hueCommand + "0.";
			hueCommand = hueCommand + String(cieY) + "],\"bri\":";
			hueCommand = hueCommand + String(brightness) + "}";
			
			Serial.println(hueCommand);
			
			commandLength = hueCommand.length();
			
			Serial.print("commandLength: ");Serial.println(commandLength);
			
			client.fastrprint(F("PUT "));
			client.fastrprint(HUE_LIGHT_URL);
			client.print(hueLightNumber);
			client.fastrprint(F("/state"));
			client.fastrprint(F(" HTTP/1.1\r\n"));
			
			client.fastrprint(F("keep-alive\r\n"));
			
			client.fastrprint(F("Host: ")); 
			client.fastrprint(HUE_IP_STRING); 
			client.fastrprint(F("\r\n"));
			
			client.fastrprint(F("Content-Length: "));
			client.println(commandLength);
			
			client.fastrprint(F("Content-type: text/plain;charset=UTF-8\r\n"));
			
			client.fastrprint(F("\r\n"));
			
			sendData(client, hueCommand, commandLength, 20);
			
			break;
			
		}
		
		//response?
		//Read data until either the connection is closed, or the idle timeout is reached. */ 
		///*
		unsigned long lastRead = millis();
		while (client.connected() && (millis() - lastRead < IDLE_TIMEOUT_MS)) {
			while (client.available()) {
				char c = client.read();
				//Serial.print(c);
				lastRead = millis();
			}
		}
		//*/
		client.close();
		return true;
	} else {
		Serial.println(F("Connection to Hue failed"));    
		return false;
	}
}

/*
// Convert double to string
String doubleToString(float input,int decimalPlaces){
  if(decimalPlaces!=0){
    String string = String((int)(input*pow(10,decimalPlaces)));
      if(abs(input)<1){
        if(input>0)
          string = "0"+string;
        else if(input<0)
          string = string.substring(0,1)+"0"+string.substring(1);
      }
      return string.substring(0,string.length()-decimalPlaces)+"."+string.substring(string.length()-decimalPlaces);
    }
  else {
    return String((int)input);
  }
}
*/
