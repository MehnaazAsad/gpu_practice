#include <stdio.h>
#include <math.h>
#include <stdlib.h>

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

int main() 
{
	Image mandelbrot;
	mandelbrot.width = 1920;
	mandelbrot.height = 1080;
	mandelbrot.colour_arr_start = (Colour*)malloc(mandelbrot.height * mandelbrot.width * sizeof(Colour));

	//Overwriting garbage values from malloc to white rgb values
	Colour black = { 0, 0, 0 };
	for (int y = 0; y < mandelbrot.height; y++)
	{
		for (int x = 0; x < mandelbrot.width; x++)
		{
			write_to_pixel(x, y, black, mandelbrot);
		}
	}

	float zoom = 0.9;
	Complex c;
	Complex z;
	z.real = 0.0f;
	z.im = 0.0f;
	Colour blue = { 0, 0, 255 };
	for (int y = 0; y < mandelbrot.height; y++)
	{
		for (int x = 0; x < mandelbrot.width; x++)
		{
			bool bounded = true;
			//Normalize to be from 0-1
			c.real = x / (float)mandelbrot.height - 1.9;
			c.real *= zoom;
			c.im = y / (float)mandelbrot.height - 0.5;
			c.im *= zoom;
			for (int i = 0; i < 50; i++)
			{
				z = mandelbruh(z, c);
				float rad = radius(z);
				if (rad > 2.0f)
				{
					bounded = false;
					break;
				}

			}
			printf("%d %d\n", x, y);
			z.real = 0.0f;
			z.im = 0.0f;
			if (!bounded)
			{
				write_to_pixel(x, y, blue, mandelbrot);
			}
		}
	}

	FILE* output_image;
	fopen_s(&output_image, "output.ppm", "w");
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
	
	free(mandelbrot.colour_arr_start);
	
	return 0;
}