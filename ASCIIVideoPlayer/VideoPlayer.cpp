#include "VideoPlayer.h"

void VideoPlayer::init()
{
}

void VideoPlayer::playVideo(std::string _filePath)
{
	cv::Mat frame;
	cv::Mat frameResized;

	cv::VideoCapture vidCap;
	// TODO: handle audio


	vidCap.open(_filePath, cv::CAP_ANY);

	if (!vidCap.isOpened())
	{

		return;
	}

	std::string textFrame;
	textFrame.resize();
	for (;;) {
		// read frame
		vidCap.read(frame);;
		
		if (frame.empty())
			break;// video has ended
		
		// TODO: skip frames iif to slow
		
		// resize frame
		cv::resize(frame, frameResized, cv::Size());// TODO: find brest resize method

		
		for (int i = 0; i < frame.rows; i++)
		{
			for (int j = 0; j < frame.cols; j++)
			{
				textFrame[i] = ((char*)Renderer::getBuffer(frame.at<cv::Vec<unsigned __int8, 3>>(i, j)[2]))[0];
			}
		}
		



	}

	




}
