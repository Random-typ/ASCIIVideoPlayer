//#include "frameCompute.h"

namespace CudaFrameCompute {
    /*
    __global__ void computeRow(uchar** _frame, char** _data, int* _width, int* _p0, char** _shadesDevice, int* _shadesDeviceLen)
    {
        int i = threadIdx.x;
        for (int j = 0; j < *_width; j++)
        {
            unsigned __int8* vec = &((*_frame + *_p0 * i))[j * 3];
            char color = (vec[0] + vec[1] + vec[2]) / 3;
            if (color == 255)
            {
                color = (*_shadesDevice)[*_shadesDeviceLen - 1];
            }
            else {
                color = (*_shadesDevice)[(int)(color / (255.0 / *_shadesDeviceLen))];
            }

            (*_data)[(*_width * 2 + 1) * i + (j + 1) * 2] = color;
            (*_data)[(*_width * 2 + 1) * i + (j + 1) * 2 - 1] = color;
        }
    }

    bool computeFrameAlloc(int _width, const cv::Mat& _frame, const std::string& _ASCIIFrame)
    {
        const char* shades = ".'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@";
        int shadesLen = strlen(shades);
        cudaError_t cudaErr = cudaSetDevice(0);
        if (cudaErr != cudaSuccess) {
            fprintf(stderr, "cudaSetDevice failed! Do you have a CUDA-capable GPU installed?");
            return false;
        }

        // alloc constant space
        cudaMalloc(CudaASCIIFrame, _ASCIIFrame.size());
        cudaMalloc(frame, _frame.dataend - _frame.datastart);

        // alloc constants
        cudaMalloc(&width, sizeof(int));
        cudaMalloc(&p0, sizeof(int));
        cudaMalloc(shadesDevice, strlen(shades));
        cudaMalloc(&shadesDeviceLen, sizeof(int));

        // set constants
        cudaMemcpy(width, &_width, sizeof(int), cudaMemcpyDeviceToHost);
        cudaMemcpy(p0, &_frame.size.p[0], sizeof(int), cudaMemcpyDeviceToHost);
        cudaMemcpy(shadesDevice, shades, strlen(shades), cudaMemcpyDeviceToHost);
        cudaMemcpy(shadesDeviceLen, &shadesLen, sizeof(int), cudaMemcpyDeviceToHost);
        return true;
    }

    bool computeFrameFree()
    {
        // free allocated 
        cudaFree(width);
        cudaFree(p0);
        cudaFree(CudaASCIIFrame);
        cudaFree(frame);
        cudaFree(shadesDevice);
        cudaFree(shadesDeviceLen);

        cudaError_t cudeErr = cudaDeviceReset();
        if (cudeErr != cudaSuccess) {
            fprintf(stderr, "cudaDeviceReset failed!");
            return false;
        }
        return true;
    }*/
}