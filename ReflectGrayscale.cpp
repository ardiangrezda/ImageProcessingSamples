/* Implemented by Ardian Grezda for the subject "Image processing" (march-june 2004) in University of Prishtina, Kosovo

 This program reflects 2 images grayscale bitmap images
 Syntax
		ReflectGrayscale InputFile1 OutputFile
*/

#include <stdio.h>
#include <windows.h>

// This function verifies if the input file is of type bitmap. If yes,
// it returns 0, and if it is not, then follows exit from the program
//
int CheckIfGrayscaleBitmap(char * szFileName, BITMAPFILEHEADER *bfh, BITMAPINFOHEADER *bih);

// Here will get pixels from input images, which will be saved into variable szPixels
// nNumberOfPixels- number of pixels in the input file
// szFileName - the name of the input file
// If the returned value is 0, everything went OK.
int PixelBytes(long nNumberOfPixels, unsigned char *szPixels, char * szFileName);

int ReflectGrayscale(char *szOutputFile, unsigned char *szPixels, long nNumberOfPixels,
				BITMAPFILEHEADER bfh, BITMAPINFOHEADER bih);

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4) 
int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("Syntax: ReflectGrayscale InputFile1 OutputFile \n");
		return -1;
	}
	argv[0] = strupr(argv[0]);
	argv[1] = strupr(argv[1]);
	argv[2] = strupr(argv[2]);

	char szFirstInputFile[100], szOutputFile[100];
	strcpy(szFirstInputFile, argv[1]);
	strcpy(szOutputFile, argv[2]);

	char *Extension;
	// Finds the last apperance of the dot (.) in the filename
	// and saves it into Extension variable
	Extension	= strrchr(argv[1],'.');
	if (Extension == NULL)
	{
		strcat(szFirstInputFile, ".BMP");
		Extension = ".BMP";
	}
	Extension	= strupr(Extension);
	if (strcmp(Extension, ".BMP"))
	{
		printf("Bitmap must have extension .bmp!\n");
		return -1;
	}

	// extension for output file
	Extension	= strrchr(argv[2],'.');
	if (Extension == NULL);
	{
		strcat(szOutputFile, ".BMP");
		Extension = ".BMP";
	};
	Extension	= strupr(Extension);
	if (strcmp(Extension, ".BMP"))
	{
		printf("Bitmap must have extension .bmp!\n");
		return -1;
	}

	int a;
	// BITMAPFILEHEADER structure for input file
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
	
	// This function get call to check if the image is of type bitmap
	if (CheckIfGrayscaleBitmap(szFirstInputFile, &bfh, &bih))
	{
		printf("There was an error in bitmap structure\n");
		return -1;
	}

	// pixel number is determined by this expression
	long nNumberOfPixels = bfh.bfSize - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER)
		- sizeof(RGBQUAD[256]);
	
	// buffer for pixels
	unsigned char *szPixels = new unsigned char [nNumberOfPixels];
	
	// bytes of input image are saved in the variable szPixels
	if (PixelBytes(nNumberOfPixels, szPixels, szFirstInputFile))
	{
		printf("There has been an error in the structure of input image\n");
		delete [] szPixels;
		szPixels = NULL;
		return -1;
	}


	// if the bitmap size is the same and the test went OK,
	// reflection operation can begin
	if (ReflectGrayscale(szOutputFile, szPixels, nNumberOfPixels, 
					  bfh, bih))
	{
		printf("There has been an error in reflection\n");
		return -1;
	}

	delete [] szPixels;

	szPixels = NULL;

	printf("Image %s was successfuly created\n", szOutputFile);
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
	// check file size and the result is saved in variable lFileSize
	long lFileSize;
	fseek(fInputBitmap, 0, SEEK_END);
	lFileSize = ftell(fInputBitmap);
	// go at the begining of the file
	fseek(fInputBitmap, 0, SEEK_SET);
	int nNumberRead;
	// read bytes for BITMAPFILEHEADER and saves the result in variable tempBuffer
	nNumberRead = fread(tempBuffer, sizeof(char), sizeof(BITMAPFILEHEADER), fInputBitmap);
	// fill variable tempBuffer with sturcture BITMAPFILEHEADER
	memcpy((char *) bfh, tempBuffer, sizeof(BITMAPFILEHEADER));
	// verifies file size
	if (bfh->bfSize != lFileSize)
	{
		fclose(fInputBitmap);
		printf("File %s must have %ld byte\n", szFileName, lFileSize);
		return 1;
	}
	// verifies 2 bytes of the file
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

	// read next bytes from the file and result is saved on variable tempBuffer
	nNumberRead = fread(tempBuffer, sizeof(char), sizeof(BITMAPINFOHEADER), fInputBitmap);
	memcpy((char *) bih, tempBuffer, sizeof(BITMAPINFOHEADER));
	// verifies if the number of bits per pixel is 8
	if (bih->biBitCount != 8)
	{
		fclose(fInputBitmap);
		printf("Number of bits per pixel must be 8\n");
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
	// read color table and table color components have R,G,B same values which
	// can have values 0-255
	for (int i = 0; i < 256; i++)
	{
		nNumberRead = fread(tempBuffer, sizeof(char), sizeof(rgb[i]), fInputBitmap);
		if ((tempBuffer[0] != i) || (tempBuffer[1] != i) || (tempBuffer[2] != i))
		{
			printf("Table color is not correct\n");
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

int ReflectGrayscale(char *szOutputFile, unsigned char *szPixels, long nNumberOfPixels,
				BITMAPFILEHEADER bfh, BITMAPINFOHEADER bih)
{
	FILE *fOutputBitmap;
	if ((fOutputBitmap = fopen(szOutputFile, "w+b")) == NULL)
	{
		printf("File %s could not be created\n", szOutputFile);
		return 1;
	}
	long i, j;
	// variable szOutputPixels saves bytes of output file
	char *szOutputPixels = new char[nNumberOfPixels];
	int nHeight = bih.biHeight;
	int nWidth = bih.biWidth;

	int nWidthBytes = WIDTHBYTES(bih.biBitCount * bih.biWidth);
	for (i = 0; i < nHeight; i++)
	{ //qarkullon neper rreshta
		for(j = 0; j < nWidthBytes;j++)
		{ 
			szOutputPixels[j + i * nWidthBytes] = 
				szPixels[i * nWidthBytes + nWidth - j - 1];
		}
	}
	// fills RGBQUAD structure so that R,G,B are the same
	RGBQUAD rgb[256];
	for (i = 0; i < 256; i++)
	{
		rgb[i].rgbBlue		= i;
		rgb[i].rgbGreen		= i;
		rgb[i].rgbRed		= i;
		rgb[i].rgbReserved	= 0;
	}

	// writes on output file
	fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, fOutputBitmap);
	fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, fOutputBitmap);
	fwrite(&rgb, sizeof(rgb), 1, fOutputBitmap);
	fwrite(szOutputPixels, sizeof(char), nNumberOfPixels, fOutputBitmap);

	delete [] szOutputPixels;
	szOutputPixels = NULL;
	fclose(fOutputBitmap);
	return 0;
}


