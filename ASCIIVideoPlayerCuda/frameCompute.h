#pragma once
#ifndef __FRAME_COMPUTE__
#define __FRAME_COMPUTE__

//#include "Renderer.h"
#include "cudaFunctions.h"

#include "opencv2/opencv.hpp"

#ifdef _DEBUG
#pragma comment(lib, "opencv/lib/opencv_world481d.lib")
#else
#pragma comment(lib, "opencv/lib/opencv_world481.lib")
#endif // DEBUG

namespace CudaFrameCompute {
	int* width = nullptr;
	size_t* p0 = nullptr;

	char* shadesDevice = nullptr;
	int* shadesDeviceLen = nullptr;


	char* CudaASCIIFrame = nullptr;
	uchar* frame = nullptr;

	__global__ void computeRow(uchar* _frame, char* _data, int* _width, size_t* _p0, char* _shadesDevice, int* _shadesDeviceLen);

	// @returns true on success false otherwise
	bool computeFrameAlloc(int _width, const cv::Mat& _frame, const std::string& _ASCIIFrame);

	// @returns true on success false otherwise
	bool computeFrameFree();

	// @returns true on success false otherwise
	bool computeFrameCuda(cv::Mat& _frame, std::string& _ASCIIFrame, int _width, int _height);

}

#endif // __FRAME_COMPUTE__