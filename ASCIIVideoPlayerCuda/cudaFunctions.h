#pragma once
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#ifndef __CUDA_FUNCTIONS__
#define __CUDA_FUNCTIONS__

#define cudaMalloc(ptr, size) {\
cudaError_t cudaErr = cudaMalloc(ptr, size); \
if (cudaErr != cudaSuccess) {\
	std::cout << "cudaMalloc failed! Error: " <<  cudaGetErrorString(cudaErr) << "\n";\
	return false;\
}}

#define cudaMemcpy(dst, source, count, kind) {\
cudaError_t cudaErr = cudaMemcpy(dst, source, count, kind); \
if (cudaErr != cudaSuccess) {\
	std::cout << "cudaMemcpy failed! Error: " <<  cudaGetErrorString(cudaErr) << "\n";\
	return false;\
}}

#define cudaFree(ptr) {\
cudaError_t cudaErr = cudaFree(ptr); \
if (cudaErr != cudaSuccess) {\
	std::cout << "cudaFree failed! Error: " <<  cudaGetErrorString(cudaErr) << "\n";\
	return false;\
}}

#endif // !__CUDA_FUNCTIONS__
