#include "8x8Matrix74LC595.h"

byte RAINBOW[3][8] = { {	0b11111111,		// Line 1 - RED
							0b00000000,		// Line 2 - RED
							0b00000000,		// Line 3 - RED
							0b00000000,		// Line 4 - RED
							0b11111111,		// Line 5 - RED
							0b00000000,		// Line 6 - RED
							0b00000000,		// Line 7 - RED
							0b00000000 },	// Line 8 - RED
						{	0b00000000,		// Line 1 - GREEN
							0b11111111,		// Line 2 - GREEN
							0b00000000,		// Line 3 - GREEN
							0b11111111,		// Line 4 - GREEN
							0b00000000,		// Line 5 - GREEN
							0b11111111,		// Line 6 - GREEN
							0b00000000,		// Line 7 - GREEN
							0b11111111 },	// Line 8 - GREEN
						{	0b00000000,		// Line 1 - BLUE
							0b00000000,		// Line 2 - BLUE
							0b11111111,		// Line 3 - BLUE
							0b11111111,		// Line 4 - BLUE
							0b00000000,		// Line 5 - BLUE
							0b00000000,		// Line 6 - BLUE
							0b11111111,		// Line 7 - BLUE
							0b11111111 } };	// Line 8 - BLUE



byte placeholder[3][8]; // This is used for ascii letters or random pixels.


LedMatrix ledMatrix(500, 1000, 10); // Create matrix instance. display line on for 500uS, blanking for 1000uS, Pin for latching is D10.
// LedMatrix ledMatrix(500, 1000, 10, 15, 16); // (With software SPI)


void setup() {
		ledMatrix.showOnDisplay(RAINBOW); // On startup show rainbow
		Serial.begin(115200);
}



void loop() {
	static uint8_t color=1; // The color of letters shown on display.

	ledMatrix.refreshDisplay(); // Constantly keep calling this to keep display updated.

	if (Serial.available() > 0) { // Listen on serial what to do.
		char incomingByte = Serial.read();
		if (incomingByte == '9') { // serial char '9' prints our rainbow
			ledMatrix.showOnDisplay(RAINBOW); 
		} else if (incomingByte == '8') { // serial char '8' draws random pixels
			randomPixels();
			ledMatrix.showOnDisplay(placeholder);
		} else if (incomingByte >= '1' && incomingByte<='3') { // serial char from '1' to '3' sets the default text color (1=red,2=green,3=blue)
			color = incomingByte - '1';
			ledMatrix.clearDisplay(placeholder);
			ledMatrix.getLetterData(incomingByte, color, placeholder);
			ledMatrix.showOnDisplay(placeholder);
		} else { // Any letter received will be printed on the display
			ledMatrix.clearDisplay(placeholder);
			ledMatrix.getLetterData(incomingByte, color, placeholder);
			ledMatrix.showOnDisplay(placeholder);
		}
	}
}



// Draws random pixels on the 8x8 display 
void randomPixels() {
	ledMatrix.clearDisplay(placeholder);
	for (uint8_t line = 0; line < 8; line++) {
		placeholder[random(0, 3)][line] = random(0, 255);
	}
}