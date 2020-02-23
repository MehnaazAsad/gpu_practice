#include "samples/mandelbrot.h"
#include <math.h>

Complex Complex::square(Complex _num)
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

Complex Complex::mandelbruh(Complex z, Complex c)
{
	return square(z) + c;
}

float Complex::radius()
{
	return (float)sqrt(this->real * this->real + this->im * this->im);
}

void Image::write_to_pixel(int x_coord, int y_coord, Colour rgb)
{
	this->colour_arr_start[x_coord + this->width * y_coord] = rgb;
}

void Image::clear()
{
	//Overwriting garbage values from malloc to black rgb values
	Colour black = { 0, 0, 0 };
	for (int y = 0; y < this->height; y++)
	{
		for (int x = 0; x < this->width; x++)
		{
			this->write_to_pixel(x, y, black);
		}
	}
}