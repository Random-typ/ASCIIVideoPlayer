#pragma once
#include "cudaFunctions.h"

__global__ void computeRow(uchar** _frame, char* _data, int _width, int _p0) {
    int i = threadIdx.x;
    cv::Mat* _mat;
    for (int j = 0; j < _mat->cols; j++)
    {
        unsigned __int8* vec = &((*_frame + _p0 * i))[j * 3];
        char pixel = ((char*)Renderer::getBuffer((vec[0] + vec[1] + vec[2]) / 3))[0];
        _data[(_mat->cols * 2 + 1) * i + (j + 1) * 2] = pixel;
        _data[(_mat->cols * 2 + 1) * i + (j + 1) * 2 - 1] = pixel;
    }
}
int* width = nullptr;
int* p0 = nullptr;

char** CudaASCIIFrame = nullptr;
uchar** data = nullptr;


bool computeFrameAlloc(int _width, int _p0, const cv::Mat& _frame, const std::string& _ASCIIFrame) {
    cudaError_t cudaErr = cudaSetDevice(0);
    if (cudaErr != cudaSuccess) {
        fprintf(stderr, "cudaSetDevice failed! Do you have a CUDA-capable GPU installed?");
        return false;
    }

    // alloc constant space
    cudaMalloc(CudaASCIIFrame, _ASCIIFrame.size());
    cudaMalloc(data, _frame.dataend - _frame.datastart);

    // alloc constants
    cudaMalloc(&width, sizeof(int));
    cudaMalloc(&p0, sizeof(int));
    // set constants
    cudaMemcpy(width, &_width, sizeof(int), cudaMemcpyDeviceToHost);
    cudaMemcpy(p0, &_p0, sizeof(int), cudaMemcpyDeviceToHost);
}

bool computeFrameFree(int _width, int _p0) {
    // free allocated 
    cudaFree(width);
    cudaFree(p0);
    cudaFree(CudaASCIIFrame);
    cudaFree(data);

    cudaError_t cudeErr = cudaDeviceReset();
    if (cudeErr != cudaSuccess) {
        fprintf(stderr, "cudaDeviceReset failed!");
        return false;
    }
}


bool computeFrameCuda(cv::Mat& _frame, std::string& _ASCIIFrame, int _width, int _height) {
    cudaError_t cudaErr;
    // copy image data
    cudaMemcpy(CudaASCIIFrame, _ASCIIFrame.data(), _ASCIIFrame.size(), cudaMemcpyDeviceToHost);
    // copy ACII frame
    cudaMemcpy(data, &_frame, sizeof(_frame), cudaMemcpyDeviceToHost);

    // launch kernel
    computeRow << <1, _height >> > (_frame.data, _ASCIIFrame.data(), _width, _frame.size.p[0]);

    cudaErr = cudaGetLastError();
    if (cudaErr != cudaSuccess) {
        fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaErr));
        return;
    }

    // wait for kernel to finish
    cudaErr = cudaDeviceSynchronize();
    if (cudaErr != cudaSuccess) {
        fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
        return;
    }

    cudaMemcpy((void*)_ASCIIFrame.data(), CudaASCIIFrame, _ASCIIFrame.size(), cudaMemcpyHostToDevice);



}