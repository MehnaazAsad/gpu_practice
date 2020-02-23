#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#include "samples/mandelbrot.h"

void serial(Image image, int max_iteration, float zoom)
{
	clock_t start = clock();
	Complex c;
	Complex z;
	z.real = 0.0f;
	z.im = 0.0f;
	Colour blue = { 0, 0, 255 };
	for (int y = 0; y < image.height; y++)
	{
		for (int x = 0; x < image.width; x++)
		{
			bool bounded = true;
			//Normalize to be from 0-1
			c.real = x / (float)image.height - 1.9;
			c.real *= zoom;
			c.im = y / (float)image.height - 0.5;
			c.im *= zoom;
			for (int i = 0; i < max_iteration; i++)
			{
				z = Complex::mandelbruh(z, c);
				float rad = z.radius();
				if (rad > 2.0f)
				{
					bounded = false;
					break;
				}

			}
			//printf("%d %d\n", x, y);
			z.real = 0.0f;
			z.im = 0.0f;
			if (!bounded)
			{
				image.write_to_pixel(x, y, blue);
			}
		}
	}
	clock_t finish = clock();
	float time_spent = (float)(finish - start) / CLOCKS_PER_SEC;
	FILE* output_time;
	fopen_s(&output_time, "timing.txt", "w");
	fprintf(output_time, "Serial (s)\n");
	fprintf(output_time, "%d %d %d %f\n", image.width, image.height, max_iteration, time_spent);
	fclose(output_time);

	FILE* output_image;
	fopen_s(&output_image, "serial_output.ppm", "w");
	fprintf(output_image, "P3\n%d %d\n255\n", image.width, image.height);

	Colour* colour_ptr = image.colour_arr_start;
	for (int y = 0; y < image.height; y++)
	{
		for (int x = 0; x < image.width; x++)
		{
			fprintf(output_image, "%d %d %d\t", colour_ptr->r, colour_ptr->g, colour_ptr->b);
			colour_ptr++;
		}
		fprintf(output_image, "\n");
	}
	fclose(output_image);

}

void parallel(Image image, int max_iteration, float zoom)
{
	clock_t start = clock();
	Complex c;
	Complex z;
	z.real = 0.0f;
	z.im = 0.0f;
	Colour blue = { 0, 0, 255 };
	#pragma omp parallel for
	for (int y = 0; y < image.height; y++)
	{
		for (int x = 0; x < image.width; x++)
		{
			bool bounded = true;
			//Normalize to be from 0-1
			c.real = x / (float)image.height - 1.9;
			c.real *= zoom;
			c.im = y / (float)image.height - 0.5;
			c.im *= zoom;
			for (int i = 0; i < max_iteration; i++)
			{
				z = Complex::mandelbruh(z, c);
				float rad = z.radius();
				if (rad > 2.0f)
				{
					bounded = false;
					break;
				}

			}
			// printf("%d %d\n", x, y);
			z.real = 0.0f;
			z.im = 0.0f;
			if (!bounded)
			{
				image.write_to_pixel(x, y, blue);
			}
		}
	}
	clock_t finish = clock();
	float time_spent = (float)(finish - start) / CLOCKS_PER_SEC;
	int max_thread_num = omp_get_max_threads();
	FILE* output_time;
	fopen_s(&output_time, "timing.txt", "a");
	fprintf(output_time, "\nParallel (s)\n");
	fprintf(output_time, "%d %d %d %d %f\n", image.width, image.height, max_iteration, max_thread_num, time_spent);
	fclose(output_time);

	FILE* output_image;
	fopen_s(&output_image, "parallel_output.ppm", "w");
	fprintf(output_image, "P3\n%d %d\n255\n", image.width, image.height);

	Colour* colour_ptr = image.colour_arr_start;
	for (int y = 0; y < image.height; y++)
	{
		for (int x = 0; x < image.width; x++)
		{
			fprintf(output_image, "%d %d %d\t", colour_ptr->r, colour_ptr->g, colour_ptr->b);
			colour_ptr++;
		}
		fprintf(output_image, "\n");
	}
	fclose(output_image);

}

int mandelbrot_c() 
{
	Image mandelbrot;
	// 4k
	//mandelbrot.width = 3840;
	//mandelbrot.height = 2160;

	// 16k
	mandelbrot.width = 4 * 3840;
	mandelbrot.height = 4 * 2160;
	mandelbrot.colour_arr_start = (Colour*)malloc(mandelbrot.height * mandelbrot.width * sizeof(Colour));

	int max_iteration = 100;
	float zoom = 0.9;

	mandelbrot.clear();
	serial(mandelbrot, max_iteration, zoom);

	printf("Phew, life is hard");
	mandelbrot.clear();
	parallel(mandelbrot, max_iteration, zoom);

	free(mandelbrot.colour_arr_start);
	return 0;
}

/*
mandelbrot_c.cpp

includes
function definitions

start main

image initializtion
	set width, height and colour array
constants initialization

clear image where garbage values from malloc are overwritten to black rgb values
run serial version
	start timer
	initialization of complex variables and def of colour to write to pixel
	per row
		per column
			default state is bounded i.e black
			initialize constant to be added to mandelbrot equation
			for a given number of iterations
				at first iteration start at 0 and add constant. this is the new
				position and evaluate if this new position is outisde or inside
				a circle with radius 2. if outside, your state is unbounded and
				the colour at that pixel position is changed to blue and written
				to the image along with the pixel position. if NOT outside,
				continue with the same constant being added to the updated
				position until you either become unbounded or exceed number
				of iterations. If exceeded then colour written to that pixel
				position stays black and position is set back to origin.
	end timer
	calculate time taken
	write to text file - dimensions of image, number of iterations and execution time
	write to image file in specific ppm format - dimensions and rgb values at each
		pixel position
end of serial version

clear image to black rgb values
repeat above process except using pragma omp to parallelize

free memory allocated for colour array

end of main
*/