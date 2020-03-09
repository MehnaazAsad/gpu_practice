### Notes regarding buffers:
Pixel ID

[0        1        2        3        4       ]

Colour Channels

[r, g, b, r, g, b, r, g, b, r, g, b, r, g, b,]

Colour Channels Index

[0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,]`

Image Coordinates

width  = 3

height = 4 

  c        c      c

[x0y0,0 x0y1,1 x0y2,2]r

[x1y0,3 x1y1,4 x1y2,5]r

[x2y0,6 x2y1,7 x2y2,8]r

[x3y0,6 x3y1,7 x3y2,8]r


x = id % width

y = id / width


[0        1        2        3        4       ]

[p00,   p01,     p02,     p10,     p11, p12, p20, p21, p22]