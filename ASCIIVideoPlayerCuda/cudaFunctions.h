#pragma once
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include "../ASCIIVideoPlayer/Renderer.h"

#include <stdio.h>

#include "opencv2/opencv.hpp"
#ifdef _DEBUG
#pragma comment(lib, "opencv/lib/opencv_world481d.lib")
#else
#pragma comment(lib, "opencv/lib/opencv_world481.lib")
#endif // DEBUG

#define cudaMalloc(ptr, size) \
cudaError_t cudaErr = cudaMalloc(ptr, size); \
if (cudaErr != cudaSuccess) {\
std::cout << "cudaMalloc failed! Do you have a CUDA-capable GPU installed? " <<  cudaGetErrorString(cudaErr) << "\n";\
return false;\
}

#define cudaMemcpy(dst, source, count, kind) \
cudaError_t cudaErr = cudaMemcpy(dst, source, count, kind); \
if (cudaErr != cudaSuccess) {\
std::cout << "cudaMemcpy failed! Error: " <<  cudaGetErrorString(cudaErr) << "\n";\
return false;\
}

#define cudaFree(ptr) \
cudaError_t cudaErr = cudaFree(ptr); \
if (cudaErr != cudaSuccess) {\
std::cout << "cudaFree failed! Error: " <<  cudaGetErrorString(cudaErr) << "\n";\
return false;\
}
