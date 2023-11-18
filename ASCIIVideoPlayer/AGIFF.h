#include <vector>
#pragma once

// ASCII GIF Format
/* V 1.0
* 
* 
* 
* 
* 
* 
*/
struct AGIFFFrameCoord {
	uint16_t x;
	uint16_t y;
};

struct AGIFFFrame {
	uint16_t instruction;
	AGIFFFrameCoord start;
	AGIFFFrameCoord end;

	uint8_t color;
};

// v1.0
struct AGIFF
{
	// HEADER (10 Bytes)
	char magicNumber[4]; // must be set to 0x46494741
	uint16_t version;

	uint32_t frameTime;
	uint32_t frameCount;


	// DATA 
	std::vector<AGIFFFrame> frames;
};