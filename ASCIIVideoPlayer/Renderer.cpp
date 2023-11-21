#include "Renderer.h"
#include "AGIFF.h"

// max of 255 shades

uint64_t Renderer::width = 100;
std::vector<std::string> Renderer::shadesBuffer;
std::string Renderer::shades = ".'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@";


void Renderer::allocBuffers()
{
	shadesBuffer.clear();
	for (char i : shades)
	{
		shadesBuffer.emplace_back().resize(width, i);
	}
}

void Renderer::freeBuffers()
{
	shadesBuffer.clear();
	shadesBuffer.shrink_to_fit();
}

void Renderer::drawText(std::string _str)
{

}

void Renderer::renderAGIFF(Coord _pos, const std::string& _raw)
{// todo: add size checks
	AGIFF agiff;
	memcpy_s(&agiff, 10, _raw.data(), 10);
	
	size_t off = 10;
	AGIFFFrame frame;
	for ( ; agiff.frameCount > 0; agiff.frameCount--, off += sizeof(frame))
	{
		memcpy_s(&frame, sizeof(frame), _raw.data() + off, sizeof(frame));
		agiff.frames.emplace_back(frame);
	}

	for (auto& frame : agiff.frames)
	{
		switch (frame.instruction)
		{
		case 1:// single pixel
			point({ frame.start.x, frame.start.y }, frame.color);
			break;
		case 2:// line
			line({ frame.start.x, frame.start.y }, { frame.end.x, frame.end.y }, frame.color);
			break;
		case 3:// rect
			rect({ { frame.start.x, frame.start.y }, { frame.end.x, frame.end.y } }, frame.color);
			break;
		default:
			throw "Unknown instruction";
			return;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(agiff.frameTime));
	}
}

void Renderer::setWidth(uint64_t _width)
{
	width = _width;
}

void Renderer::point(Coord _pos, Color _color)
{
	setCursorPos(_pos);
	fwrite(getBuffer(_color), 1, 1, stdout);
}

void Renderer::rect(Rect _rect, Color _color)
{
	for (_rect.topLeft.y; _rect.topLeft.y < _rect.bottomRight.y + 1; _rect.topLeft.y++)
	{
		line(_rect.topLeft, _rect.bottomRight, _color);
	}
}

void Renderer::line(const Coord& _start, const Coord& _end, Color _color)
{
	setCursorPos(_start);
	fwrite(getBuffer(_color), _end.x - _start.x, 1, stdout);
}

void Renderer::setCursorPos(Coord _coord)
{
	static const HANDLE outputHanldle = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleCursorPosition(outputHanldle, { (short)_coord.x, (short)_coord.y });
}

void* Renderer::getBuffer(Color _color)
{
	if (_color == 255)
	{
		_color = 254;
	}
	double resolution = 255.0 / shades.size();
	return shadesBuffer[_color / resolution].data();
}