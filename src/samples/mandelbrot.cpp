#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

struct Complex 
{
	float real;
	float im;
};

struct Colour
{
	int r;
	int g;
	int b;
};

struct Image
{
	int width;
	int height;
	Colour* colour_arr_start;
};

Complex square(Complex _num) 
{
	Complex temp;
	//a^2 - b^2 + 2abi
	temp.real = _num.real * _num.real - _num.im * _num.im;
	temp.im = 2 * _num.real * _num.im;

	return temp;
}

Complex operator+(Complex lhs, Complex rhs)
{
	Complex temp;
	temp.real = lhs.real + rhs.real;
	temp.im = lhs.im + rhs.im;

	return temp;
}

Complex mandelbruh(Complex z, Complex c)
{
	return square(z) + c;
}

float radius(Complex _num)
{
	return (float)sqrt(_num.real * _num.real + _num.im * _num.im);
}

void write_to_pixel(int x_coord, int y_coord, Colour rgb, Image target)
{
	target.colour_arr_start[x_coord + target.width * y_coord] = rgb;
}

void clear(Image image)
{
	//Overwriting garbage values from malloc to white rgb values
	Colour black = { 0, 0, 0 };
	for (int y = 0; y < image.height; y++)
	{
		for (int x = 0; x < image.width; x++)
		{
			write_to_pixel(x, y, black, image);
		}
	}
}

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
				z = mandelbruh(z, c);
				float rad = radius(z);
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
				write_to_pixel(x, y, blue, image);
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
				z = mandelbruh(z, c);
				float rad = radius(z);
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
				write_to_pixel(x, y, blue, image);
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

int main() 
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

	clear(mandelbrot);
	serial(mandelbrot, max_iteration, zoom);

	printf("Phew, life is hard");
	clear(mandelbrot);
	parallel(mandelbrot, max_iteration, zoom);

	free(mandelbrot.colour_arr_start);
	return 0;
}