typedef struct comp
{
	float real;
	float im;
}Complex;

Complex square(Complex _num)
{
	Complex temp;
	//a^2 - b^2 + 2abi
	temp.real = _num.real * _num.real - _num.im * _num.im;
	temp.im = 2 * _num.real * _num.im;

	return temp;
}

Complex add(Complex a, Complex b)
{
	Complex temp;
	temp.real = a.real + b.real;
	temp.im = a.im + b.im;

	return temp;

}

Complex mandelbruh(Complex z, Complex c)
{
	return add(square(z), c);
}

float radius(Complex _num)
{
	return (float)sqrt(_num.real * _num.real + _num.im * _num.im);
}

__kernel void mandelbrot(__global int *colour_channels) 
{
	/*
	one pixel = one thread
	one pixel = 3 ints
	num pixels = width * height
	num ints = 3 * num pixels
	num threads = num pixels
	[r,g,b,r,g,b,...]
	*/
	
	// Get the index of the current element to be processed
    int pixel_id = get_global_id(0);
	int width = 3840;
	int height = 2160;
	float zoom = 0.9;
	bool bounded = true;
	int max_iteration = 100;
	// from [1,2,3,4, 3840*2160]
	// [[1,1,1][2,2,2][3,3,3]]
	// to [[r,g,b][r,g,b]...]
	int index_to_colour_channel = pixel_id*3;
	int x = pixel_id % width;
	int y = pixel_id / width;

	Complex z;
	z.real = 0.0;
	z.im = 0.0;

	Complex c;
	c.real = x / (float)height - 1.9;
	c.real *= zoom;
	c.im = y / (float)height - 0.5;
	c.im *= zoom;
	for (int i = 0; i < max_iteration; i++)
	{
		z = mandelbruh(z, c);
		float rad = radius(z);
		if (rad > 2.0)
		{
			bounded = false;
			break;
		}

	}

	global int *red = &colour_channels[index_to_colour_channel];
	global int *green = &colour_channels[index_to_colour_channel+1];
	global int *blue = &colour_channels[index_to_colour_channel+2];

	if (!bounded)
	{
		*red = 0;
		*blue = 255;
		*green = 0;
	}




}