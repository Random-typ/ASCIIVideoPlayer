//#include "cudaFunctions.h"

#include "VideoPlayer.h"
#include "frameCompute.h"


namespace CudaFrameCompute {
    __global__ void computeRow(uchar* _frame, char* _data, int* _width, size_t* _p0, char* _shadesDevice, int* _shadesDeviceLen)
    {
        int i = threadIdx.x;
        for (int j = 0; j < *_width; j++)
        {
            int offset = (*_p0) * i;

            unsigned __int8 v1 = _frame[*_width * 3 * i + j * 3];
            unsigned __int8 v2 = _frame[*_width * 3 * i + j * 3 + 1];
            unsigned __int8 v3 = _frame[*_width * 3 * i + j * 3 + 2];



            /*unsigned __int8* vec = &((_frame + (*_p0) * i))[j * 3];
            int offset = (*_p0) * i + j * 3;

            printf("v0%d\n", vec[0]);*/

            //printf("p%d\n", *_p0);
            //printf("f%d\n", _frame);
            //printf("v%d\n", vec);
            //printf("i%d\n", i);
            //printf("j%d\n", j);
            //printf("n%d\n", index);
            //printf("v%d\n", _frame[20]);
            //printf("t%d\n", _frame[index]);
            //printf("t%d\n", _frame[(*_p0) * i + j * 3]);

            //printf("v0%d\n", vec[0]);
            //printf("v0%d\n", vec[1]);
            //printf("v0%d\n", vec[2]);
            
            // calculate grayscale
            char color = (v1 + v2 + v3) / 3;
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
        
        // set constants
        cudaMemcpy(width, &_width, sizeof(int), cudaMemcpyHostToDevice);
        cudaMemcpy(p0, &_frame.size.p[0], sizeof(size_t), cudaMemcpyHostToDevice);
        cudaMemcpy(shadesDevice, shades, strlen(shades), cudaMemcpyHostToDevice);
        cudaMemcpy(shadesDeviceLen, &shadesLen, sizeof(int), cudaMemcpyHostToDevice);
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
        cudaMemcpy(CudaASCIIFrame, _ASCIIFrame.data(), _ASCIIFrame.size(), cudaMemcpyHostToDevice);
        // copy ACII frame
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

    vidCap.open(1); // CAM
    //vidCap.open(_filePath, cv::CAP_FFMPEG);

    if (!vidCap.isOpened())
    {
        // todo: error msg
        return;
    }

    std::string textFrame;

    int height = 80;
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

        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 0 });
        fwrite(textFrame.data(), textFrame.size(), 1, stdout);
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

	playVideo("./videos/badapple!.mp4", true);

	Renderer::freeBuffers();
    return 0;
}

