//#include "cudaFunctions.h"

#include "VideoPlayer.h"
#include "frameCompute.h"


namespace CudaFrameCompute {
    __global__ void computeRow(uchar* _frame, char* _data, int* _width, size_t* _p0, char* _shadesDevice, int* _shadesDeviceLen)
    {
        int i = threadIdx.x;
        for (int j = 0; j < *_width; j++)
        {
            int offset = *_width * 3 * i + j * 3;

            // calculate grayscale
            unsigned char color = (_frame[offset] + _frame[offset + 1] + _frame[offset + 2]) / 3;
            // find fitting ASCII character
            if (color == 255)
            {
                color = _shadesDevice[*_shadesDeviceLen - 1];
            }
            else {
                color = _shadesDevice[(int)(color / (255.0 / *_shadesDeviceLen))];
            }
            // set ASCII character in final frame
            _data[((*_width) * 2 + 1) * i + (j + 1) * 2] = color;
            _data[((*_width) * 2 + 1) * i + (j + 1) * 2 - 1] = color;
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
        cudaMalloc(&CudaASCIIFrame, _ASCIIFrame.size());
        cudaMalloc(&frame, _frame.dataend - _frame.datastart);
        
        // alloc constants
        cudaMalloc(&width, sizeof(int));
        cudaMalloc(&p0, sizeof(size_t));
        cudaMalloc(&shadesDevice, strlen(shades));
        cudaMalloc(&shadesDeviceLen, sizeof(int));
        
        // set constants these need to be copied once
        cudaMemcpy(width, &_width, sizeof(int), cudaMemcpyHostToDevice);
        cudaMemcpy(p0, &_frame.size.p[0], sizeof(size_t), cudaMemcpyHostToDevice);
        cudaMemcpy(shadesDevice, shades, strlen(shades), cudaMemcpyHostToDevice);
        cudaMemcpy(shadesDeviceLen, &shadesLen, sizeof(int), cudaMemcpyHostToDevice);
        cudaMemcpy(CudaASCIIFrame, _ASCIIFrame.data(), _ASCIIFrame.size(), cudaMemcpyHostToDevice);

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
    }


    bool computeFrameCuda(cv::Mat& _frame, std::string& _ASCIIFrame, int _width, int _height)
    {
        cudaError_t cudaErr;
        // copy image data
        cudaMemcpy(frame, _frame.data, _frame.dataend - _frame.datastart, cudaMemcpyHostToDevice);

        // launch kernel
        computeRow<<<1, _height>>>(frame, CudaASCIIFrame, width, p0, shadesDevice, shadesDeviceLen);

        cudaErr = cudaGetLastError();
        if (cudaErr != cudaSuccess) {
            fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaErr));
            return false;
        }

        // wait for kernel to finish
        cudaErr = cudaDeviceSynchronize();
        if (cudaErr != cudaSuccess) {
            fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaErr);
            return false;
        }

        cudaMemcpy((void*)_ASCIIFrame.data(), CudaASCIIFrame, _ASCIIFrame.size(), cudaMemcpyDeviceToHost);
        return true;
    }
}


void playVideo(std::string _filePath, bool _cudaAccelEnabled)
{
    cv::Mat frame;
    cv::Mat frameResized;

    cv::VideoCapture vidCap;
    // TODO: handle audio

    //vidCap.open(1); // CAM
    vidCap.open(_filePath, cv::CAP_FFMPEG);

    if (!vidCap.isOpened())
    {
        // todo: error msg
        return;
    }

    std::string textFrame;

    int height = 500;
    int width = vidCap.get(cv::CAP_PROP_FRAME_WIDTH) / vidCap.get(cv::CAP_PROP_FRAME_HEIGHT) * height;

    std::vector<int64_t> frameConvertTimes;
    //frameConvertTimes.reserve(vidCap.get(cv::CAP_PROP_FRAME_COUNT));

    textFrame.resize(height * (width * 2 + 1));
    // add new line to end of each row
    for (size_t i = 1; i < height; i++)
    {
        textFrame[(width * 2 + 1) * i] = '\n';
    }

    // measuring execute time
    long double computeFramesTotal = 0;
    long double printFramesTotal = 0;

    vidCap.get(cv::CAP_PROP_FRAME_COUNT);
    int frameTime = 1000 / vidCap.get(cv::CAP_PROP_FPS) * 1000;// frame time in MicroS
    bool cudeAllocated = false;
    for (auto startTime = std::chrono::high_resolution_clock::now();; startTime = std::chrono::high_resolution_clock::now()) {
        // read frame
        vidCap.read(frame);

        if (frame.empty())
            break;// video has ended

        // TODO: skip frames if to slow
                // resize frame
        cv::resize(frame, frameResized, cv::Size(width, height));// TODO: find best resize method

        // init cuda
        if (_cudaAccelEnabled && !cudeAllocated)
        {
#ifndef __CUDA_RUNTIME_H__
            std::cout << "No Cuda runtime detected!\n";
            return;
#else 
            if (!CudaFrameCompute::computeFrameAlloc(width, frameResized, textFrame))
            {
            	return;
            }
            cudeAllocated = true;
#endif
        }


        // measure execute time
        auto computeFrameStart = std::chrono::high_resolution_clock::now();
        // create ASCII frame
        if (_cudaAccelEnabled)
        {// GPU
#ifdef __CUDA_RUNTIME_H__
            CudaFrameCompute::computeFrameCuda(frameResized, textFrame, width, height);
#endif // !__CUDA_RUNTIME_H__
        }
        else
        {// CPU
            VideoPlayer::computeFrame(frameResized, width, textFrame);
        }
        computeFramesTotal = (std::chrono::high_resolution_clock::now() - computeFrameStart).count() / 1000;
        std::string info = "Compute Time: " + std::to_string(computeFramesTotal) + " Print Time: " + std::to_string(printFramesTotal) + "                      ";
        memcpy_s((void*)textFrame.data(), textFrame.size(), (void*)info.data(), info.size());

        auto printFrameStart = std::chrono::high_resolution_clock::now();
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 0 });
        fwrite(textFrame.data(), textFrame.size(), 1, stdout);
        printFramesTotal = (std::chrono::high_resolution_clock::now() - printFrameStart).count() / 1000;
        
        // wait
        while ((std::chrono::high_resolution_clock::now() - startTime).count() / 1000 /* to MicroS */ < frameTime);
    }
    if (_cudaAccelEnabled)
    {
#ifdef __CUDA_RUNTIME_H__
        CudaFrameCompute::computeFrameFree();
#endif
    }
}

int main()
{
	// setup
	Renderer::setWidth(200);
	Renderer::allocBuffers();

	VideoPlayer::init();

	playVideo("./videos/Rick Astley - Never Gonna Give You Up.mp4", true);

	Renderer::freeBuffers();
    return 0;
}

