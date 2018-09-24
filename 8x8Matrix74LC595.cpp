#include "arduino.h"
#include "8x8Matrix74LC595.h"



/* When creating an instance of the LedMatrix you need to provide:
   drawingTime: How many microseconds each line in the display will be on
   blankingTime: how many microseconds each line in the display will be blanked.. The ratio between these two gives the overall brightness
   latchPin: The pinnumber used for latching to the 74lc595's.
   If HWSPI is used you just connect the 74lc595 to arduinos MOSI and CLK. If not uncomment HWSPI in .header and call constructor
   wiht clock and data pin */
#ifdef HWSPI
#include <SPI.h>
LedMatrix::LedMatrix(uint16_t _drawingTime, uint8_t _blankingTime, uint8_t _latchPin) {
	latchPin = _latchPin;
	drawingTime = _drawingTime;
	blankingTime = _blankingTime;
	pinMode(latchPin, OUTPUT);
	SPI.begin();
}
#else
LedMatrix::LedMatrix(uint16_t _drawingTime, uint8_t _blankingTime, uint8_t _latchPin, uint8_t _clockPin, uint8_t _dataPin) {
	latchPin = _latchPin;
	clockPin = _clockPin;
	dataPin = _dataPin;
	drawingTime = _drawingTime;
	blankingTime = _blankingTime;
	pinMode(latchPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(dataPin, OUTPUT);
}
#endif // HWSPI



/* Use this public method when you want to display something new.
   You give this method a pointer to the 8x8 displaydata. */
void LedMatrix::showOnDisplay(byte _displayData[3][8]) {
	displayData = _displayData;
}



/* Public method to clear the displaydata, filling it with zeroes */
void LedMatrix::clearDisplay(byte _displayData[3][8]) {
	for (uint8_t line = 0; line < 8; line++) {
		for (uint8_t color = 0; color < 3; color++) {
			_displayData[color][line] = 0;
		}
	}
}



/* Public method. It should be given an ascii letter (0-127), a color (0-7) and a pointer
   to the variable where the letter should be stored */
void LedMatrix::getLetterData(uint8_t letter, uint8_t color, byte displayData[3][8]) {
	for (uint8_t line = 0; line < 8; line++) { // Traverse each 8 lines of the display
		char lineContent = pgm_read_byte_near(FONTMAP[letter] + line); // Look up in the fonts what to draw on that line
		displayData[color][line] = lineContent; 
	}
}



/* This public method should be called as often as possible from main loop. It keeps the display refreshed. 
   The content of 'displayData' is drawn on the display */
void LedMatrix::refreshDisplay() {
	uint32_t updateTime;
	updateTime = drawing ? drawingTime : blankingTime;
	if (micros() - lastUpdate > updateTime) {
		if (drawing) { // If in drawing mode we need to prepare the current line for the display
			uint8_t col_r = displayData[0][currentDisplayLine];
			uint8_t col_g = displayData[1][currentDisplayLine];
			uint8_t col_b = displayData[2][currentDisplayLine];
			rawDispLine(col_r, col_g, col_b, currentDisplayLine);
		} else { // We are blanking.
			rawDispLine(0, 0, 0, currentDisplayLine);
		}
		currentDisplayLine++; // Get ready for next line on display
		if (currentDisplayLine >= 8) {
			currentDisplayLine = 0; // We start on first line after the last is reached
			drawing = !drawing; // Toggle between drawing mode and blanking mode
		}
		lastUpdate = micros();
	}
}



/* Private method for latching out one line to the display 
   It takes 3 x 8bit R, G, and B as input and a linenumber */
void LedMatrix::rawDispLine(uint8_t col_r, uint8_t col_g, uint8_t col_b, uint8_t line) {
	digitalWrite(latchPin, LOW); // Tells 74lc595 that data will be latched out to it.
#ifdef HWSPI
	SPI.beginTransaction(SPISettings(100000000, MSBFIRST, SPI_MODE0)); // Go fast on the SPI. It can handle it.
	SPI.transfer(255 - col_r); // Latch out one color at the time
	SPI.transfer(255 - col_b);
	SPI.transfer(255 - col_g);
	SPI.transfer(1 << line); // The last byte for 74lc595 number 4 gets the current linenumber. We only do one line at the time.
	SPI.endTransaction();
#else
	shiftOut(dataPin, clockPin, MSBFIRST, 255 - col_r); // Latch out one color at the time
	shiftOut(dataPin, clockPin, MSBFIRST, 255 - col_b);
	shiftOut(dataPin, clockPin, MSBFIRST, 255 - col_g);
	shiftOut(dataPin, clockPin, MSBFIRST, 1 << line); // The last byte for 74lc595 number 4 gets the current linenumber. We only do one line at the time.
#endif // HWSPI
	digitalWrite(latchPin, HIGH); // Tells the 74lc595 That it can show the content.
}