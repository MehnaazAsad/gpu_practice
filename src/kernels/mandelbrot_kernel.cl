__kernel void mandelbrot(__global int *colour_channels) 
{
	// Get the index of the current element to be processed
    int pixel_id = get_global_id(0);
	int width = 3840;
	int height = 2160;
	// from [1,2,3,4, 3840*2160]
	// [[1,1,1][2,2,2][3,3,3]]
	// to [[r,g,b][r,g,b]...]
	int index_to_colour_channel = pixel_id*3;
	int x = pixel_id % width;
	int y = pixel_id / width;


	global int *red = &colour_channels[index_to_colour_channel];
	global int *green = &colour_channels[index_to_colour_channel+1];
	global int *blue = &colour_channels[index_to_colour_channel+2];
	if (x < width / 3)
	{
		*blue = 255;
	}
	else if (x > 2 * width / 3)
	{
		*red = 255;
	}
	else
	{
		*red = 255;
		*blue = 255;
		*green = 255;
	}
	//*red = 255;
	//*blue = 255;
	//*green = 0;

	/*
	one pixel = one thread
	one pixel = 3 ints
	num pixels = width * height
	num ints = 3 * num pixels
	num threads = num pixels
	[r,g,b,r,g,b,...]
	*/

}