//CUDA version: 7.0
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#include <limits>
#include <chrono>

using namespace std;

cudaError_t fWithCuda(float* f, float* x, int size);

__global__ void fKernel(float* dev_f, float* dev_x, int size)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	dev_f[i] = sin(5 * powf(dev_x[i], 3)) + cos(7 + powf(dev_x[i], 4)) * dev_x[i];
}

int main()
{
	float a = 0.0f;
	float b = 100.0f;
	float h = 1e-4f;
    int arraySize = (b - a) / h;
	float* f = new float[arraySize];
	float* x = new float[arraySize];

	for (int i = 0; i < arraySize; i++){
		x[i] = a + i * h;
	}

	auto start = chrono::steady_clock::now();

	// calculate f function in parallel.
	cudaError_t cudaStatus = fWithCuda(f, x, arraySize);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "addWithCuda failed!");
        return 1;
    }

	float min_x = a;
	float min_y = numeric_limits<float>::max();

	for (int i = 0; i < arraySize; i++){
		if (f[i] < min_y){
			min_y = f[i];
			min_x = x[i];
		}
	}

	auto end = chrono::steady_clock::now();
	float time_elapsed = float((end - start).count()) *
		chrono::steady_clock::period::num /
		chrono::steady_clock::period::den;

	fprintf(stdout, "CUDA result: %f %f\n", min_x, min_y);
	fprintf(stdout, "time elapsed: %f seconds\n", time_elapsed);

    // cudaDeviceReset must be called before exiting in order for profiling and
    // tracing tools such as Nsight and Visual Profiler to show complete traces.
    cudaStatus = cudaDeviceReset();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaDeviceReset failed!");
        return 1;
    }
    return 0;
}

// Helper function for using CUDA to add vectors in parallel.
cudaError_t fWithCuda(float* f, float* x, int size)
{
	float* dev_f = 0;
	float* dev_x = 0;
    cudaError_t cudaStatus;

    // Choose which GPU to run on, change this on a multi-GPU system.
    cudaStatus = cudaSetDevice(0);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
        goto Error;
    }

    // Allocate GPU buffers for two vectors (one input, one output)    .
	cudaStatus = cudaMalloc((void**)&dev_f, size * sizeof(float));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

	cudaStatus = cudaMalloc((void**)&dev_x, size * sizeof(float));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

    // Copy input vector from host memory to GPU buffer.
	cudaStatus = cudaMemcpy(dev_x, x, size * sizeof(float), cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed!");
        goto Error;
    }

    // Launch a kernel on the GPU with one thread for each element of array.
	fKernel <<<dim3(size / 500, 1), dim3(500, 1)>>> (dev_f, dev_x, size);

    // Check for any errors launching the kernel
    cudaStatus = cudaGetLastError();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
        goto Error;
    }
    
    // cudaDeviceSynchronize waits for the kernel to finish, and returns
    // any errors encountered during the launch.
    cudaStatus = cudaDeviceSynchronize();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
        goto Error;
    }

    // Copy output vector from GPU buffer to host memory.
    cudaStatus = cudaMemcpy(f, dev_f, size * sizeof(float), cudaMemcpyDeviceToHost);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed!");
        goto Error;
    }

Error:
	cudaFree(dev_f);
	cudaFree(dev_x);
    
    return cudaStatus;
}
