/* Implemented by Ardian Grezda for the subject "Image processing" (march-june 2004) in University of Prishtina, Kosovo

This program count number of pixels in an grayscale image of type .bmp

Syntax:
	HistogramiGrayscale inputFile
	where 
		inputFile - name of the input file
*/

#include <stdio.h>
#include <windows.h>

// The function verifies if the input file is bitmap file. If yes, value 0 will be returned, 
// otherwise the program will terminate
int CheckIfGrayscaleBitmap(char * szFileName, BITMAPFILEHEADER *bfh, BITMAPINFOHEADER *bih);

// Here will get pixels from input images, which will be saved into variable szPixels
// nNumberOfPixels- number of pixels in the input file
// szFileName - the name of the input file
// If the returned value is 0, everything went OK. 
int PixelBytes(long nNumberOfPixels, unsigned char *szPixels, char * szFileName);

// The function counts the occurence of the pixels of the image
// where szPixels are pixels, and 
// nNumriPikselave is an array which shows number of 
// pixels in an image for all pixels from 0 - 255.
// nNumberOfPixels is the count of the pixels in an image
void NumroPikselat(unsigned char *szPixels, int * nNumriPikselave, long nNumberOfPixels);

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Syntax: HistogramiGrayscale inputfile\n");
		return -1;
	}
	argv[0] = strupr(argv[0]);

	char szInputFile[100];
	strcpy(szInputFile, argv[1]);

	char *Extension;
	
	// Finds the last apperance of the dot (.) in the filename
	// and saves it into Extension variable
	Extension	= strrchr(argv[1],'.');
	if (Extension == NULL)
	{
		strcat(szInputFile, ".BMP");
		Extension = ".BMP";
	}

	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
	
	// This function is called to check if the first image is bitmap
	if (CheckIfGrayscaleBitmap(szInputFile, &bfh, &bih))
	{
		printf("There was an error in the structure of the bitmap 1\n");
		return -1;
	}

	// number of pixels is determined by this expression
	long nNumberOfPixels = bfh.bfSize - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER)
		- sizeof(RGBQUAD[256]);
	
	// buffer for image pixels
	unsigned char *szPixels = new unsigned char [nNumberOfPixels];
	
	// Bytes of the image 1 are saved into variable szPixels
	if (PixelBytes(nNumberOfPixels, szPixels, szInputFile))
	{
		printf("There was an error in the structure of bitmap 1\n");
		delete [] szPixels;
		szPixels = NULL;
		return -1;
	}
	int nNumriPikselave[256];

	NumroPikselat(szPixels, nNumriPikselave, nNumberOfPixels);

	delete [] szPixels;
	szPixels = NULL;
	return 0;
}
int CheckIfGrayscaleBitmap(char * szFileName, BITMAPFILEHEADER *bfh, BITMAPINFOHEADER *bih)
{
	unsigned char tempBuffer[2000];
	FILE *fInputBitmap;
	if ((fInputBitmap = fopen(szFileName, "r+b")) == NULL)
	{
		printf("File %s does not exist or cannot be opened\n", szFileName);
		return 1;
	}
	// controls file size and saves the result into variable lFileSize
	long lFileSize;
	fseek(fInputBitmap, 0, SEEK_END);
	lFileSize = ftell(fInputBitmap);
	// goes to the begining of the file
	fseek(fInputBitmap, 0, SEEK_SET);
	int nNumberRead;
	// reads bytes for BITMAPFILEHEADER and saves them into variable tempBuffer
	nNumberRead = fread(tempBuffer, sizeof(char), sizeof(BITMAPFILEHEADER), fInputBitmap);
	// fills variable tempBuffer with structure BITMAPFILEHEADER
	memcpy((char *) bfh, tempBuffer, sizeof(BITMAPFILEHEADER));
	// verifies file size
	if (bfh->bfSize != lFileSize)
	{
		fclose(fInputBitmap);
		printf("File %s must have %ld bytes\n", szFileName, lFileSize);
		return 1;
	}
	// verifies the first 2 bytes of the file
	if (bfh->bfType != 0x4d42)
	{
		fclose(fInputBitmap);
		printf("File %s is not bitmap file\n", szFileName);
		return 1;
	}
	if (bfh->bfReserved1 != 0)
	{
		fclose(fInputBitmap);
		printf("Bytes 7 and 8 of the file %s must be 0\n", szFileName);
		return 1;
	}

	if (bfh->bfReserved2 != 0)
	{
		fclose(fInputBitmap);
		printf("Bytes 9 and 10 of the file %s must be 0\n", szFileName);
		return 1;
	}

	// read next bytes from the file and the result is saved into variable tempBuffer
	nNumberRead = fread(tempBuffer, sizeof(char), sizeof(BITMAPINFOHEADER), fInputBitmap);
	memcpy((char *) bih, tempBuffer, sizeof(BITMAPINFOHEADER));
	// verifies if the number of bites per pixel is 8
	if (bih->biBitCount != 8)
	{
		fclose(fInputBitmap);
		printf("Number of bites per pixel should be 8\n");
		return 1;
	}
	
	// verifies if the bitmap is compressed
	if (bih->biCompression != BI_RGB)
	{
		fclose(fInputBitmap);
		printf("Bitmap is compressed\n");
		return 1;
	}

	RGBQUAD rgb[256];
	// reads color table.
	// the table color have R,G, B components which should be equal and could have values from 0-255
	for (int i = 0; i < 256; i++)
	{
		nNumberRead = fread(tempBuffer, sizeof(char), sizeof(rgb[i]), fInputBitmap);
		if ((tempBuffer[0] != i) || (tempBuffer[1] != i) || (tempBuffer[2] != i))
		{
			printf("The color table is not correct\n");
			fclose(fInputBitmap);
			return 1;
		}
	}
	fclose(fInputBitmap);
	return 0;
}

int PixelBytes(long nNumberOfPixels, unsigned char *szPixels, char * szFileName)
{
	FILE *fInputBitmap;
	if ((fInputBitmap = fopen(szFileName, "r+b")) == NULL)
	{
		printf("File %s does not exist or cannot be opened\n", szFileName);
		return 1;
	}

	// As the image is grayscale, pixels are placed after the structures
	// BITMAPFILEHEADER, BITMAPINFOHEADER and after the color table RGBQUAD[256]
	int nOffsetBytes = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD[256]);
	// the file pointer goes after structures BITMAPFILEHEADER, BITMAPINFOHEADER and color table
	fseek(fInputBitmap, nOffsetBytes, SEEK_SET);
	// read bytes of the file and the result is saved into variable szPixels
	long nNumberRead = fread(szPixels, sizeof(char), nNumberOfPixels, fInputBitmap);
	fclose(fInputBitmap);
	return 0;
}

void NumroPikselat(unsigned char *szPixels, int * nNumriPikselave, long nNumberOfPixels)
{
	long i;
	// at the begining, the count of all pixels is 0
	for (i = 0; i < 256; i++)
	{
		nNumriPikselave[i] = 0;
	}
	int tempPixelValue;
	for (i = 0; i < nNumberOfPixels; i++)
	{
		tempPixelValue = szPixels[i];
		nNumriPikselave[tempPixelValue]++;
	}	
	// Display the values on the screen
	printf("Count of the occurence of the pixels in the image is:\n");
	printf("\n");
	printf("--------------------------------------------------------------------------\n");
	printf("Pixel\tCount\t\tPixel\tCount\t\tPixel\tCount\t\t\n");
	printf("--------------------------------------------------------------------------\n");
	for (i = 0; i < 256; i++)
	{
		if ((i % 3) == 0 && i != 0)
			printf("\n");
		printf("%d\t%d\t\t", i, nNumriPikselave[i]);
	}
	printf("\n--------------------------------------------------------------------------\n");
}
