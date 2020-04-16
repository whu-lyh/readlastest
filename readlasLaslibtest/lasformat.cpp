/*
*	To learn the las file format to better write or read a las file
*	And this file is just a introduction of las file format
*	Reference:
*	http://www.asprs.org/wp-content/uploads/2019/03/LAS_1_4_r14.pdf
*/

/*
*	What information could be saved in a las file, different version of las file could save different informations

	(p[0] != 'x') && // we expect the x coordinate
	(p[0] != 'y') && // we expect the y coordinate
	(p[0] != 'z') && // we expect the z coordinate
	(p[0] != 't') && // we expect the gps time
	(p[0] != 'R') && // we expect the red channel of the RGB field
	(p[0] != 'G') && // we expect the green channel of the RGB field
	(p[0] != 'B') && // we expect the blue channel of the RGB field
	(p[0] != 'I') && // we expect the NIR channel
	(p[0] != 's') && // we expect a string or a number that we don't care about
	(p[0] != 'i') && // we expect the intensity
	(p[0] != 'a') && // we expect the scan angle
	(p[0] != 'n') && // we expect the number of returns of given pulse
	(p[0] != 'r') && // we expect the number of the return
	(p[0] != 'h') && // we expect the with<h>eld flag
	(p[0] != 'k') && // we expect the <k>eypoint flag
	(p[0] != 'g') && // we expect the synthetic fla<g>
	(p[0] != 'o') && // we expect the <o>verlap flag
	(p[0] != 'l') && // we expect the scanner channe<l>
	(p[0] != 'E') && // we expect terrasolid echo encoding
	(p[0] != 'c') && // we expect the classification
	(p[0] != 'u') && // we expect the user data
	(p[0] != 'p') && // we expect the point source ID
	(p[0] != 'e') && // we expect the edge of flight line flag
	(p[0] != 'd') && // we expect the direction of scan flag
	(p[0] != 'H') && // we expect hexadecimal coded RGB(I) colors
	(p[0] != 'J'))   // we expect a hexadecimal coded intensity
*/

/*
	U8 point_type = header.point_data_format;
	U16 point_size = header.point_data_record_length;
*---------------------------------------------------------------------------*
|	las version 1.2 (initial bits is 30)									|
*---------------------------------------------------------------------------*
|	point_type:																|
|	0		|	1			|	2			|	3			|	4,5(None)	|
|	xyz		|	xyzt		|	xyzrgb		|	xyztrgb		|add wave packet|
*---------------------------------------------------------------------------*
|	point_size:																|
|	20		|	28			|	26			|	34			|	53-67		|
*---------------------------------------------------------------------------*
|	las version 1.4	(initial bits is 30)									|
|	The difference to the core 20 bytes of Point Data Record Formats 0 to 5 |
|is that there are more bits for return numbers in order to support up to	|
|15 returns, there are more bits for point classifications to support up	|
|to 256 classes, there is a higher precision scan angle (16 bits instead	|
|of 8), and the GPS time is mandatory.										|
*---------------------------------------------------------------------------*
|	point_type:																|
|	6		|	7			|	8			|								|
|	xyzt	|	xyztrgb		|	xyzrgbi		|								|
*---------------------------------------------------------------------------*
|	point_size:																|																	|
|	30		|	36			|	38			|								|
*---------------------------------------------------------------------------*
*/
/*
	X long 4 bytes yes
	Y long 4 bytes yes
	Z long 4 bytes yes
	Intensity unsigned short 2 bytes no
	Return Number 3 bits (bits 0-2) 3 bits yes
	Number of Returns (Given Pulse) 3 bits (bits 3-5) 3 bits yes
	Scan Direction Flag 1 bit (bit 6) 1 bit yes
	Edge of Flight Line 1 bit (bit 7) 1 bit yes
	Classification unsigned char 1 byte yes
	Scan Angle Rank (-90 to +90) ¨C Left Side signed char 1 byte yes
	User Data unsigned char 1 byte no
	Point Source ID unsigned short 2 bytes yes
	Minimum PDRF Size1 20 bytes
*/

/*
Bytes occupided:

- char (1 byte)
- unsigned char (1 byte)
- short (2 bytes)
- unsigned short (2 bytes)
- long (4 bytes)
- unsigned long (4 bytes)
- long long (8 bytes)
- unsigned long long (8 bytes)
- float (4 byte IEEE floating point format)
- double (8 byte IEEE floating point format)
*/