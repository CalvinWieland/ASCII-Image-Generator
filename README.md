# ASCII-Image-Generator

Takes an image, runs canny edge detection on the image, splits the image into smaller 
rectangular chunks, takes the average value of the color in each section, and uses 
that color to map it to an ASCII character. Then it prints it out to the console.

Examples and argument information:

![Example 2](./ImageToASCII%20Example%202.jpg)
![Example 3](./ImageToASCII%20Example%203.jpg)
![Example 4](./ImageToASCII%20Example%204.jpg)
![Example 1](./ImageToASCII%20Example%201.jpg)

Arguments:
  1. Path to your image
  (2.) pixel height and (3.) width of each ASCII character (meaning, your image will be split into blocks of height x width 
  blocks and one character will represent each block). It's recommended that the width is approximately half of the height,
  since ASCII character slots are twice as tall as they are wide.  A good default is a width of 6 and a height of 12.
  (4.) The low and (5.) high values of the double threshold step. The lower the values are, the more edges there will be,
  and the higher they are, the more strict the edge detection algorithm will be, meaning less edges.
  Good defaults are low = .3 and high = .5
Sample input: ./ImageToASCII input.png 12 6 .3 .5
