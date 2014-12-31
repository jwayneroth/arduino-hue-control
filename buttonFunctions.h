void initButtons() {
	for(int i=0; i<NUM_BUTTONS; i++) {
		pinMode(buttons[i], INPUT);
	}
}

boolean checkButtons() {
	for(int i=0; i<NUM_BUTTONS; i++) {
		if(digitalRead(buttons[i]) == HIGH) {
			//Serial.print("click ");
			//Serial.println(i);
			switch(i) {
				case 0:
					incrementRed();
					break;
				case 1:
					incrementGreen();
					break;
				case 2:
					incrementBlue();
					break;
				case 3:
					incrementBrightness();
					break;
				default:
					break;
			}
			delay(100);
			return 1;
		}
	}
	return 0;
}