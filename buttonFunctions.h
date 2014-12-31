void initButtons() {
	for(int i=0; i<NUM_BUTTONS; i++) {
		pinMode(buttons[i], INPUT);
	}
}

int checkButtons() {
	for(int i=0; i<NUM_BUTTONS; i++) {
		if(digitalRead(buttons[i]) == HIGH) {
			delay(100);
			return i+1;
		}
	}
	return 0;
}