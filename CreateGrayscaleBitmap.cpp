/* Implemented by Ardian Grezda for the subject "Image processing" (march-june 2004) in University of Prishtina, Kosovo


 The program creates bitmap image with 255 gray levels

 Syntax:
	creategraybitmap grayscale filename length width
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
		printf("Syntax: creategraybitmap grayscale filename length width\n");
		return -1;
	}
	
	argv[0]	= strupr(argv[0]);
	argv[1] = strupr(argv[1]);
	argv[2] = strupr(argv[2]);

	if (strcmp(argv[1], "GRAYSCALE"))
	{
		printf("Argumenti i dyte duhet te jete grayscale!\n");
		return 1;
	}

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
		printf("Bitmap must have .bmp extension!\n");
		return -1;
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

	FILE *fbitmap;
	if ((fbitmap = fopen(szFileName, "w+b")) == NULL)
	{
		printf("There has been an error in the file: %s ", szFileName);
		return -1;
	}

	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;

	// fills BITMAPINFOHEADER structure with information from grayscale bitmap
	bih.biSize		= sizeof(BITMAPINFOHEADER);
	bih.biWidth		= lWidth;
	bih.biHeight		= lHeight;
	bih.biPlanes		= 1;
	bih.biBitCount		= 8;  // bitmap is grayscale
	bih.biCompression	= BI_RGB;
	bih.biSizeImage		= WIDTHBYTES (lWidth * bih.biBitCount ) * lHeight;
	bih.biXPelsPerMeter	= 0;
	bih.biYPelsPerMeter 	= 0;
	bih.biClrUsed		= 0;
	bih.biClrImportant	= 0;

	// fills RGBQUAD structure, such that R, G and B componets are equal
	RGBQUAD rgb[256];
	for (int i = 0; i < 256; i++)
	{
		rgb[i].rgbBlue		= i;
		rgb[i].rgbGreen		= i;
		rgb[i].rgbRed		= i;
		rgb[i].rgbReserved	= 0;
	}

	bfh.bfSize			= (DWORD) (sizeof(BITMAPFILEHEADER) + 
			bih.biSize + sizeof(rgb) + bih.biSizeImage);
	bfh.bfType		= 0x4d42;  // 0x42 = "B" 0x4d = "M" 
	bfh.bfReserved1		= 0;
	bfh.bfReserved2		= 0;
	bfh.bfOffBits		= (DWORD) sizeof(BITMAPFILEHEADER) + 
		bih.biSize + sizeof (rgb); 

	// number of bytes on bitmap width
	long lNumberBytes = WIDTHBYTES (lWidth * bih.biBitCount);
	
	char *szRow			= new char[lNumberBytes];

	// all the pixels are saved on this array
	char *szBytes		= new char[lNumberBytes * lHeight];
	int nCurrentWByte;

	for (int nCurrentHeight = 0; nCurrentHeight < lHeight; nCurrentHeight++)
	{
		// all the bytes on a row will have value 0
		for (nCurrentWByte = 0; nCurrentWByte < lNumberBytes; nCurrentWByte++)
		{
			szRow[nCurrentWByte]  = 0;
		}
		// only some of the bytes will have value 1
		for (nCurrentWByte = 0; nCurrentWByte < lWidth; nCurrentWByte++)
		{
			szRow[nCurrentWByte] = 0xff;
		}
		memcpy(szBytes + nCurrentHeight * lNumberBytes, szRow, lNumberBytes);
	}

	// writes information for BITMAPFILEHEADER, BITMAPINFOHEADER dhe 
	// and pixels in binary file

	fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, fbitmap);
	fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, fbitmap);
	fwrite(&rgb, sizeof(rgb), 1, fbitmap);
	fwrite(szBytes, sizeof(char), lNumberBytes* lHeight, fbitmap);
	
	delete [] szRow;
	delete [] szBytes;
	szRow = NULL;
	szBytes = NULL;
	
	fclose(fbitmap);
	printf ("File %s was successfuly created\n", szFileName);
	return 0;
}
