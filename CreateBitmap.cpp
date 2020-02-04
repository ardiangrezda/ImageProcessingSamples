/* Implemented by Ardian Grezda for the subject "Image processing" (march-june 2004) in University of Prishtina, Kosovo

 The program creates monochromatic bitmap

 Syntax:
	createmonobitmap mono filename length width
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
	if (argc != 5)
	{
		printf("The bitmap could not be created\n");
		printf("syntax: createmonobitmap mono filename length width\n");
		return -1;
	}
	
	argv[0]	= strupr(argv[0]);
	argv[1] = strupr(argv[1]);
	argv[2] = strupr(argv[2]);

	char szFileName[100];
	strcpy(szFileName, argv[2]);
	char *Extension;

	// Finds the last apperance of the dot (.) in the filename
	// and saves it into Extension variable
	Extension	= strrchr(argv[2],'.');
	if (Extension == NULL)
	{
		strcat(szFileName, ".BMP");
		Extension = ".BMP";
	}
	Extension	= strupr(Extension);
	if (strcmp(Extension, ".BMP"))
	{
		printf("Bitmap should have extension .bmp!\n");
		return -1;
	}

	if (strcmp(argv[1], "MONO"))
	{
		printf("The second argument should be mono!\n");
		return -1;
	}

	// verifies if the forth argument is number
	// if it is not a number, the exit from the program will follow
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

	FILE *fbitmap;
	if ((fbitmap = fopen(szFileName, "w+b")) == NULL)
	{
		printf("There is an error in the file %s ", szFileName);
		return -1;
	}

	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;

	// fills the structure BITMAPINFOHEADER  with information for binary bitmap
	bih.biSize		= sizeof(BITMAPINFOHEADER);
	bih.biWidth		= lWidth;
	bih.biHeight		= lHeight;
	bih.biPlanes		= 1;
	bih.biBitCount		= 1;  // bitmap is binary
	bih.biCompression	= BI_RGB;
	bih.biSizeImage		= WIDTHBYTES (lWidth * bih.biBitCount ) * lHeight;
	bih.biXPelsPerMeter	= 0;
	bih.biYPelsPerMeter 	= 0;
	bih.biClrUsed		= 0;
	bih.biClrImportant	= 0;

	// fills RGBQUAD structure with black and white color 
	RGBQUAD rgb[2];
	rgb[0].rgbBlue		= 0;
	rgb[0].rgbGreen		= 0;
	rgb[0].rgbRed		= 0;
	rgb[0].rgbReserved	= 0;

	rgb[1].rgbBlue		= 0xff;
	rgb[1].rgbGreen		= 0xff;
	rgb[1].rgbRed		= 0xff;
	rgb[1].rgbReserved	= 0;

	// fills BITMAPINFOHEADER structure with data for binary bitmap
	bfh.bfSize		= (DWORD) (sizeof(BITMAPFILEHEADER) + 
			bih.biSize + bih.biBitCount * sizeof(rgb) + bih.biSizeImage);
	bfh.bfType			= 0x4d42;  // 0x42 = "B" 0x4d = "M" 
	bfh.bfReserved1		= 0;
	bfh.bfReserved2		= 0;
	bfh.bfOffBits		= (DWORD) sizeof(BITMAPFILEHEADER) + 
		bih.biSize + bih.biBitCount * sizeof (rgb); 

	// number of bytes in the length of bitmap
	long lNumberBytes = WIDTHBYTES (lWidth * bih.biBitCount);

	// pixels in one row
	char *szRow			= new char[lNumberBytes];

	// all the pixels will be saved here 
	char *szBytes		= new char[lNumberBytes * lHeight];
	int l;

	for (int nCurrentHeight = 0; nCurrentHeight < lHeight; nCurrentHeight++)
	{
		// all the bytes in one row will be zero
		for (int nCurrentWByte = 0; nCurrentWByte < lNumberBytes; nCurrentWByte++)
		{
			szRow[nCurrentWByte]  = 0;
		}
		// only some of the bytes will be 1 (according to width)
		for (int nCurrentBit = 0; nCurrentBit < lWidth; nCurrentBit++)
		{
			nCurrentWByte = nCurrentBit / 8;
			l = nCurrentBit % 8;
			szRow[nCurrentWByte] |= 1 << (8 - l - 1);
		}
		memcpy(szBytes + nCurrentHeight * lNumberBytes, szRow, lNumberBytes);
	}

	// writes the information about BITMAPFILEHEADER, BITMAPINFOHEADER, RGBQUAD and 
	// the pixels in binary file

	fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, fbitmap);
	fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, fbitmap);
	fwrite(&rgb, sizeof(rgb), 1, fbitmap);
	fwrite(szBytes, sizeof(char), lNumberBytes* lHeight, fbitmap);
	
	delete [] szRow;
	delete [] szBytes;
	szRow = NULL;
	szBytes = NULL;
	
	fclose(fbitmap);
	printf ("File %s was succesfuly created\n", szFileName);
	return 0;
}
