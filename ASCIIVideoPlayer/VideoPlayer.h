#pragma once
#include <string>
#include "Renderer.h"
#include "opencv2/opencv.hpp"

#ifdef _DEBUG
#pragma comment(lib, "opencv/lib/opencv_world481d.lib")
#else
#pragma comment(lib, "opencv/lib/opencv_world481.lib")
#endif // DEBUG


class VideoPlayer
{
public:
	static void init();

	static void playVideo(std::string _filePath);

	static void computeFrame(cv::Mat& _resizedFrame, int _width, std::string& _textFrame);

private:
	static std::string frameBuffer;

};

