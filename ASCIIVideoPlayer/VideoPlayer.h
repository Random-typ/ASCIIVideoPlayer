#pragma once
#include <string>
#include "Renderer.h"
#include "opencv2/opencv.hpp"

#ifdef DEBUG
#pragma comment(lib, "opencv/lib/opencv_world481d.lib")
#else
#pragma comment(lib, "opencv/lib/opencv_world481.lib")
#endif // DEBUG


class VideoPlayer
{
public:
	static void init();

	static void playVideo(std::string _filePath);



private:
	static std::string frameBuffer;

};

