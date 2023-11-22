#include "VideoPlayer.h"

void VideoPlayer::init()
{	
	/*
	DWORD mode;
	GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &mode);
	mode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_INSERT_MODE | ENABLE_EXTENDED_FLAGS;
	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), mode);
	*/


	// make cursor invisible
	CONSOLE_CURSOR_INFO cuinfo;
	cuinfo.dwSize = sizeof(CONSOLE_CURSOR_INFO);
	cuinfo.bVisible = false;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cuinfo);

	// set console font size
	CONSOLE_FONT_INFOEX coinfo;
	coinfo.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	coinfo.nFont = 0;
	coinfo.dwFontSize.X = 0;
	coinfo.dwFontSize.Y = 2;
	coinfo.FontFamily = FF_DONTCARE;
	coinfo.FontWeight = 0;
	LPCWCHAR fontName = L"Consolas";
	memcpy_s(coinfo.FaceName, sizeof(coinfo.FaceName), fontName, sizeof(fontName));

	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), false, &coinfo);

	// set console size
	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r);

	MoveWindow(console, r.left, r.top, 1900, 1000, TRUE); // width, height
}

void VideoPlayer::computeFrame(cv::Mat& _resizedFrame, int _width, std::string& _textFrame)
{
	for (int i = 0, pixelIndex = 0; i < _resizedFrame.rows; i++)
	{
		for (int j = 0; j < _resizedFrame.cols; j++, pixelIndex++)
		{
			unsigned __int8* vec = &((_resizedFrame.data + _resizedFrame.step.p[0] * i))[j * 3];

			char pixel = ((char*)Renderer::getBuffer((vec[0] + vec[1] + vec[2]) / 3))[0];
			_textFrame[(_width * 2 + 1) * i + (j + 1) * 2] = pixel;
			_textFrame[(_width * 2 + 1) * i + (j + 1) * 2 - 1] = pixel;
		}
	}
}
