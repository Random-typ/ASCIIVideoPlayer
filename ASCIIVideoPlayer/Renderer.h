#pragma once
#include <cstdint>
#include <vector>
#include <thread>
#include <chrono>
#include <Windows.h>

struct Coord {
	uint64_t x;
	uint64_t y;
};

class Rect {
public:
	Rect() : topLeft({0, 0}), bottomRight({0, 0}) {};

	Rect(const Coord& topLeft, const Coord& bottomRight)
		: topLeft(topLeft), bottomRight(bottomRight)
	{}

	Coord topLeft;
	Coord bottomRight;
};

enum class DrawInstructions : uint8_t {
	rect,
	line
};

using Color = uint8_t;
using InstructionParam = void*;

class InstructionObject {
public:
	DrawInstructions drawInstructions;
	InstructionParam instructionParam;

	InstructionObject(const DrawInstructions& drawInstructions, const InstructionParam& instructionParam)
		: drawInstructions(drawInstructions), instructionParam(instructionParam)
	{}

	void draw();
};

// Build for a single render thread
class Renderer
{
public:
	// allocates buffers for faster printing
	static void allocBuffers();
	static void freeBuffers();

	static void drawText(std::string _str);
	
	static void renderAGIFF(Coord _pos, const std::string& _raw);


	static void setWidth(uint64_t _width);
	
	static void point(Coord _pos, Color _color);

	static void rect(Rect _rect, Color _color);
	// only works width straight horizontal lines where _start.x < _end.x
	// Note: _end.y is ignored
	static void line(const Coord& _start, const Coord& _end, Color _color);


	static void setCursorPos(Coord _coord);

	static void* getBuffer(Color _color);
private:
	static uint64_t width;

	static std::vector<std::string> shadesBuffer;
	
	static std::string shades;
};

