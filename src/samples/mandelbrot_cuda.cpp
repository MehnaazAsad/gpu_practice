/*
mandelbrot_cuda.cpp

includes

initializations - width, height, colour array malloc and other constants
write colour black to colour array
kernel
	initialization of complex variables, max_iteration, colour to write, constant value, default
		state
	iteration for loop 
	includes writing colour at position to colour array
open_cl inits
gpu memory buffers
	colour array
make program from kernel source
build 
create opencl kernel
kernel args
	colour array
start timer
execute kernel
read result from gpu to cpu
end timer
write to ppm file - dimensions and rgb values
write to text file - dimensions, number of iterations and execution time
clean and free up colour array malloc
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <CL/cl.h>
#include <assert.h>
#include <chrono>
#include "samples/mandelbrot.h"

#define MAX_SOURCE_SIZE (0x100000) //macro substitution

int mandelbrot_cuda(void) 
{
	// 4k
	Image mandelbrot;
	mandelbrot.width = 3840;
	mandelbrot.height = 2160;
	mandelbrot.colour_arr_start = (Colour*)malloc(mandelbrot.size_in_bytes());

	int max_iteration = 100;
	float zoom = 0.9;

	mandelbrot.clear();

	// Load the kernel source code into the array source_str
	FILE* fp;
	char* source_str;
	size_t source_size;

	fopen_s(&fp, "../src/kernels/mandelbrot_kernel.cl", "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

	// Get platform and device information
	cl_platform_id platform_id = NULL;
	cl_device_id device_id = NULL;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	assert(ret == CL_SUCCESS);
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1,
		&device_id, &ret_num_devices);
	assert(ret == CL_SUCCESS);

	// Create an OpenCL context
	cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
	assert(ret == CL_SUCCESS);

	// Create a command queue
	cl_command_queue command_queue = clCreateCommandQueueWithProperties(context, device_id, 0, &ret);
	assert(ret == CL_SUCCESS);

	// Create memory buffers on the device (GPU) for each vector 
	cl_mem image_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
		mandelbrot.size_in_bytes(), NULL, &ret);
	assert(ret == CL_SUCCESS);

	// Create a program from the kernel source
	cl_program program = clCreateProgramWithSource(context, 1,
		(const char**)&source_str, (const size_t*)&source_size, &ret);
	assert(ret == CL_SUCCESS);

	// Build the program
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	assert(ret == CL_SUCCESS);

	// Create the OpenCL kernel
	cl_kernel kernel = clCreateKernel(program, "mandelbrot", &ret);
	assert(ret == CL_SUCCESS);

	// Set the arguments of the kernel
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&image_obj);
	assert(ret == CL_SUCCESS);

	using namespace std::chrono;
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	//clock_t start = clock();
	// Execute the OpenCL kernel on the list
	size_t global_item_size = mandelbrot.size(); // Process entire image
	size_t local_item_size = 256; // Divide work items into groups of 256
	ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL,
		&global_item_size, &local_item_size, 0, NULL, NULL);
	assert(ret==0);
	
	// Read GPU to CPU
	ret = clEnqueueReadBuffer(command_queue, image_obj, CL_TRUE, 0,
		mandelbrot.size_in_bytes(), mandelbrot.colour_arr_start, 0, NULL, NULL);
	assert(ret == CL_SUCCESS);
	
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
	//clock_t finish = clock();
	//float time_spent = (float)(finish - start) / CLOCKS_PER_SEC;
	FILE* output_time;
	fopen_s(&output_time, "timing.txt", "w");
	fprintf(output_time, "OpenCL (s)\n");
	fprintf(output_time, "%d %d %d %lf\n", mandelbrot.width, mandelbrot.height, max_iteration, time_span.count());
	fclose(output_time);

	FILE* output_image;
	fopen_s(&output_image, "opencl_output.ppm", "w");
	fprintf(output_image, "P3\n%d %d\n255\n", mandelbrot.width, mandelbrot.height);
	Colour* colour_ptr = mandelbrot.colour_arr_start;
	for (int y = 0; y < mandelbrot.height; y++)
	{
		for (int x = 0; x < mandelbrot.width; x++)
		{
			fprintf(output_image, "%d %d %d\t", colour_ptr->r, colour_ptr->g, colour_ptr->b);
			colour_ptr++;
		}
		fprintf(output_image, "\n");
	}
	fclose(output_image);

	// Clean up
	ret = clFlush(command_queue);
	ret = clFinish(command_queue);
	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);
	ret = clReleaseMemObject(image_obj);
	ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);
	free(mandelbrot.colour_arr_start);
}