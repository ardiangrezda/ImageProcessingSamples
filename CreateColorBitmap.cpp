/* Implemented by Ardian Grezda for the subject "Image processing" (march-june 2004) in University of Prishtina, Kosovo

 The program creates color bitmap

 Syntax:
	createcolorbitmap color filename length width red green blue
*/


#include <stdio.h>
#include <windows.h>

// WIDTHBYTES performs DWORD-aligning of DIB scanlines.  The "bits" 
// parameter is the bit count for the scanline (biWidth * biBitCount), 
// and this macro returns the number of DWORD-aligned bytes needed  
// to hold those bits.  
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4) 

int main(int argc, char *argv[])
{
	if (!(argc >= 5 && argc <= 8))
	{
		printf("The bitmap could not be created\n");
		printf("syntax:  createcolorbitmap color filename length width red green blue\n");
		return -1;
	}
	
	argv[0]	= strupr(argv[0]);
	argv[1] = strupr(argv[1]);
	argv[2] = strupr(argv[2]);

	if (strcmp(argv[1], "COLOR"))
	{
		printf("The second argument must be color!\n");
		return 1;
	}

	char *szFileName	= argv[2];
	char *Extension;

	// Finds the last apperance of the dot (.) in the filename
	// and saves it into Extension variable
	Extension	= strrchr(szFileName,'.');
	Extension	= strupr(Extension);

	// if variable Extension is .BMP, then the program will terminate
	if (strcmp(Extension, ".BMP"))
	{
		printf("The bitmap must have extension .bmp!\n");
		return 1;
	}


	// verifies if the forth argument is number, and if it is not, then the program will terminate
	int a;
	for (a = 0; a < strlen(argv[3]); a++)
	{
		if (!isdigit(argv[3][a]))
		{
			printf("The forth argument (for bitmap length) should be a number\n");
 			exit(1);
		}
	}
	
	for (a = 0; a < strlen(argv[4]); a++)
	{
		if (!isdigit(argv[4][a]))
		{
			printf("The fifth argument (for bitmap width) should be a number\n");
 			exit(1);
		}
	}

	long lHeight		= atol(argv[3]);
	long lWidth		= atol(argv[4]);
	if (lHeight > 2000)
	{
		printf("The bitmap length must be maximum 2000 pixels\n");
		return -1;
	}

	if (lWidth > 2000)
	{
		printf("The bitmap width must be maximum 2000 pixels\n");
		return -1;
	}

	if (argv[5] == NULL)
	{
		argv[5] = "0";
		argv[6] = "0";
		argv[7] = "0";
	}

	if (argv[6] == NULL)
	{
		argv[6] = "0";
		argv[7] = "0";
	}

	if (argv[7] == NULL)
	{
		argv[7] = "0";
	}

	for (a = 0; a < strlen(argv[5]); a++)
	{
		if (!isdigit(argv[5][a]))
		{
			printf("The sixth argument (for red color) must be a number\n");
 			exit(1);
		}
	}

	for (a = 0; a < strlen(argv[6]); a++)
	{
		if (!isdigit(argv[6][a]))
		{
			printf("The seventh argument (for green color) must be a number\n");
 			exit(1);
		}
	}

	for (a = 0; a < strlen(argv[7]); a++)
	{
		if (!isdigit(argv[7][a]))
		{
			printf("The eighth argument (for blue color) must be a number\n");
 			exit(1);
		}
	}

	int nRed	= atoi(argv[5]);
	int nGreen	= atoi(argv[6]);
	int nBlue	= atoi(argv[7]);

	if (nRed > 255)
	{
		printf("The sixth argument (for red color) should be from 0-255, we are puting it with value 255");
		nRed = 255;
	}

	if (nGreen > 255)
	{
		printf("The seventh argument (for green color) should be from 0-255, we are puting it with value 255");
		nGreen = 255;
	}

	if (nBlue > 255)
	{
		printf("The eighth argument (for blue color) should be from 0-255, we are puting it with value 255");
		nBlue = 255;
	}

	FILE *fbitmap;
	if ((fbitmap = fopen(szFileName, "w+b")) == NULL)
	{
		printf("There has been an error in file: %s ", szFileName);
		return -1;
	}

	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;

	// fills BITMAPINFOHEADER structure with information from color bitmap
	bih.biSize		= sizeof(BITMAPINFOHEADER);
	bih.biWidth		= lWidth;
	bih.biHeight		= lHeight;
	bih.biPlanes		= 1;
	bih.biBitCount		= 24;  // bitmapi is color
	bih.biCompression	= BI_RGB;
	bih.biSizeImage		= WIDTHBYTES (lWidth * bih.biBitCount ) * lHeight;
	bih.biXPelsPerMeter	= 0;
	bih.biYPelsPerMeter 	= 0;
	bih.biClrUsed		= 0;
	bih.biClrImportant	= 0;

	bfh.bfSize		= (DWORD) (sizeof(BITMAPFILEHEADER) + 
			bih.biSize + bih.biSizeImage);
	bfh.bfType		= 0x4d42;  // 0x42 = "B" 0x4d = "M" 
	bfh.bfReserved1		= 0;
	bfh.bfReserved2		= 0;
	bfh.bfOffBits		= (DWORD) sizeof(BITMAPFILEHEADER) + bih.biSize;

	// number of bytes on bitmap width
	long lNumberBytes = WIDTHBYTES (lWidth * bih.biBitCount);

	// all the pixels are saved on this array
	char *szBytes		= new char[lNumberBytes * lHeight];
	int nCurrentWByte;
	int nControl = 0;

	for (int nCurrentHeight = 0; nCurrentHeight < lHeight; nCurrentHeight++)
	{
		// all the bytes in a row will have value 0
		for (nCurrentWByte = 0; nCurrentWByte < lNumberBytes; nCurrentWByte++)
		{
			szBytes[nCurrentWByte + nCurrentHeight * lNumberBytes]  = 0;
		}

		for (nCurrentWByte = 0 ; nCurrentWByte < lNumberBytes / 3; nCurrentWByte++)
		{
			szBytes[nCurrentWByte * 3 + nCurrentHeight * lNumberBytes + nControl]		= (char) nBlue;
			szBytes[nCurrentWByte * 3 + nCurrentHeight * lNumberBytes + nControl + 1]	= (char) nGreen;
			szBytes[nCurrentWByte * 3 + nCurrentHeight * lNumberBytes + nControl + 2]	= (char) nRed;
		}
	}

	// writes information for BITMAPFILEHEADER, BITMAPINFOHEADER dhe 
	// and pixels in binary file

	fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, fbitmap);
	fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, fbitmap);
	fwrite(szBytes, sizeof(char), lNumberBytes* lHeight, fbitmap);
	
	delete [] szBytes;
	szBytes = NULL;
	
	fclose(fbitmap);
	printf ("File %s was successfuly created\n", szFileName);
	return 0;
}
