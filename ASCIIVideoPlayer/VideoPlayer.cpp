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

void VideoPlayer::playVideo(std::string _filePath)
{
	cv::Mat frame;
	cv::Mat frameResized;

	cv::VideoCapture vidCap;
	// TODO: handle audio

	vidCap.open(1); // CAM
	//vidCap.open(_filePath, cv::CAP_FFMPEG);

	if (!vidCap.isOpened())
	{
		// todo: error msg
		return;
	}

	std::string textFrame;
	


	int height = 100;
	int width = vidCap.get(cv::CAP_PROP_FRAME_WIDTH) / vidCap.get(cv::CAP_PROP_FRAME_HEIGHT) * height;

	std::vector<int64_t> frameConvertTimes;
	//frameConvertTimes.reserve(vidCap.get(cv::CAP_PROP_FRAME_COUNT));

	textFrame.resize(height * (width * 2 + 1));
	// add new line to end of each row
	for (size_t i = 1; i < height; i++)
	{
		textFrame[(width * 2 + 1) * i] = '\n';
	}
	

	vidCap.get(cv::CAP_PROP_FRAME_COUNT);
	int frameTime = 1000 / vidCap.get(cv::CAP_PROP_FPS) * 1000;// frame time in MicroS
	for (auto startTime = std::chrono::high_resolution_clock::now();; startTime = std::chrono::high_resolution_clock::now()) {
		
		// read frame
		vidCap.read(frame);
		
		if (frame.empty())
			break;// video has ended
		
		// TODO: skip frames if to slow
				// resize frame
		cv::resize(frame, frameResized, cv::Size(width, height));// TODO: find best resize method

		// create ASCII frame
		computeFrame(frameResized, width, textFrame);
		
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 0 });
		fwrite(textFrame.data(), textFrame.size(), 1, stdout);
		// wait
		while ((std::chrono::high_resolution_clock::now() - startTime).count() / 1000 /* to MicroS */ < frameTime);
	}
}

void VideoPlayer::computeFrame(cv::Mat& _resizedFrame, int _width, std::string& _textFrame)
{
#if EnableCudeAcceleration

	return;
#endif // EnableCudeAcceleration

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
