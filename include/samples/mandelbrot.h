#pragma once

struct Complex
{
	Complex static square(Complex _num);
	Complex static mandelbruh(Complex z, Complex c);
	float radius();
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
	void clear();
	void write_to_pixel(int x_coord, int y_coord, Colour rgb);
	int size_in_bytes() { return this->width * this->height * sizeof(Colour); }
	int size() { return this->width * this->height; }
};

int mandelbrot_cuda(void);


