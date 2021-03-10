/* Implemented by Ardian Grezda for the subject "Image processing (march-June 2004)
The programs adds two grayscale images of type bitmap
Syntax:
	OperatoriMbledhjes InputFile1 InputFile2 OutputFile over
	where
		InputFile1 is the name of the input file 1
		InputFile2 is the name of the input file 2
		OutputFile is the name of the file obtained after adding images 1 and 2
		over will deal with pixels where the sum of the pixels is bigger than 255. 
				If user enters 0, the obtained sum is deducted by 255 
				If user enters 1, the value of pixel is 255
*/

#include <stdio.h>
#include <windows.h>

// The function verifies if input file is a bitmap. If yes, the returned value is 0, otherwise exit from the program follows
//
int CheckIfGrayscaleBitmap(char * szFileName, BITMAPFILEHEADER *bfh, BITMAPINFOHEADER *bih);

// Here will get pixels from input images, which will be saved into variable szPixels
// nNumberOfPixels- number of pixels in the input file
// szFileName - the name of the input file
// If the returned value is 0, everything went OK.
int PixelBytes(long nNumberOfPixels, unsigned char *szPixels, char * szFileName);

// Adds two images.
// szOutputFile - the name of output file
// szPixelsIm1 - pixels of the 1-st image
// szPixelsIm2 - pixels of the 2-nd image
// nNumberOfPixels - number of pixels in output iamge
// bfh - structure BITMAPFILEHEADER
// bih - structure BITMAPINFOHEADER
// nTejkalimi - overflow in case the sum of the pixels is biger than 255
//			If nTejkalimi is 0, the obtained sum is substracted for 255 and is assigned to the corresponding pixel
//			If nTejkalimi is 1, the pixel value is 255
int MbledhjaPikselave(char *szOutputFile, unsigned char *szPixelsIm1, unsigned char *szPixelsIm2, 
			  long nNumberOfPixels, BITMAPFILEHEADER bfh, BITMAPINFOHEADER bih, int nTejkalimi);

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4) 
int main(int argc, char *argv[])
{
	if (argc != 5)
	{
		printf("Syntax: OperatoriMbledhjes InputFile1 InputFile2 OutputFile overflow(0 or 1)\n");
		return -1;
	}
	argv[0] = strupr(argv[0]);
	argv[1] = strupr(argv[1]);
	argv[2] = strupr(argv[2]);
	argv[3] = strupr(argv[3]);

	char szFirstInputFile[100], szSecondInputFile[100], szOutputFile[100];
	strcpy(szFirstInputFile, argv[1]);
	strcpy(szSecondInputFile, argv[2]);
	strcpy(szOutputFile, argv[3]);

	char *Extension;
	// Finds the last apperance of the dot (.) in the filename
	// and saves it into Extension variable
	// this is done for the first input file
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

	// Determine extension for the second input file
	Extension	= strrchr(argv[2],'.');
	if (Extension == NULL)
	{
		strcat(szSecondInputFile, ".BMP");
		Extension = ".BMP";
	}
	Extension	= strupr(Extension);
	if (strcmp(Extension, ".BMP"))
	{
		printf("Bitmap must have extension .bmp!\n");
		return -1;
	}

	// Determine extension for the output file
	Extension	= strrchr(argv[3],'.');
	if (Extension == NULL)
	{
		strcat(szOutputFile, ".BMP");
		Extension = ".BMP";
	}
	Extension	= strupr(Extension);
	if (strcmp(Extension, ".BMP"))
	{
		printf("Bitmap must have extension .bmp!\n");
		return -1;
	}

	int a;
	for (a = 0; a < strlen(argv[4]); a++)
	{
		if (!isdigit(argv[4][a]))
		{
			printf("The fifth argument (pixel overflow) should be a number \n");
 			exit(1);
		}
	}

	// overflow should have values 0 or 1
	int nTejkalimi = atoi(argv[4]);
	if (nTejkalimi != 0 && nTejkalimi != 1)
	{
		printf("Overflow should be 0 (overflowed pixels are substracted by 255) or 1 (overflowed pixels become 255)\n");
		return -1;
	}

	// structure BITMAPFILEHEADER for the first and the second input file
	BITMAPFILEHEADER bfh1, bfh2;
	// structure BITMAPINFOHEADER for the first and the second input file
	BITMAPINFOHEADER bih1, bih2;
	// This function is called to check if the first image is grayscale
	if (CheckIfGrayscaleBitmap(szFirstInputFile, &bfh1, &bih1))
	{
		printf("There has been an error in the structure of the bitmap 1\n");
		return -1;
	}

	// number of pixels is evaluated by this expression
	long nNumberOfPixels1 = bfh1.bfSize - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER)
		- sizeof(RGBQUAD[256]);
	
	// bufferi per pikselat e imixhit 1
	unsigned char *szPixels1 = new unsigned char [nNumberOfPixels1];
	
	// buffer for pixels of the image 1
	if (PixelBytes(nNumberOfPixels1, szPixels1, szFirstInputFile))
	{
		printf("There has been an error in the structure of the bitmap 1\n");
		delete [] szPixels1;
		szPixels1 = NULL;
		return -1;
	}

	// This function is called to check if the second image is of type bitmap
	if (CheckIfGrayscaleBitmap(szSecondInputFile, &bfh2, &bih2))
	{
		printf("There has been an error in the structure of the bitmap 2\n");
		delete [] szPixels1;
		szPixels1 = NULL;
		return -1;
	}
	
	// Checks if the width of the two input images are equal
	if (bih1.biWidth != bih2.biWidth)
	{
		printf("Width of the bitmaps 1 and 2 are not the same\n");
		delete [] szPixels1;
		szPixels1 = NULL;
		return -1;
	}

	// Checks if the height of the two input images are equal 
	if (bih1.biHeight != bih2.biHeight)
	{
		printf("Height of the bitmaps 1 and 2 are not the same\n");
		delete [] szPixels1;
		szPixels1 = NULL;
		return -1;
	}
	// number of pixels of image 2 is determind by the expression
	long nNumberOfPixels2 = bfh2.bfSize - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER)
		- sizeof(RGBQUAD[256]);
	// buffer for pixels of the image 2
	unsigned char *szPixels2 = new unsigned char [nNumberOfPixels2];
	if (PixelBytes(nNumberOfPixels2, szPixels2, szSecondInputFile))
	{
		printf("There has been an error in the structure of the bitmap 2\\n");
		delete [] szPixels1;
		delete [] szPixels2;
		szPixels1 = NULL;
		szPixels2 = NULL;
		return -1;
	}
	// if size of the bitmap is the same and all other test are OK,
	// then we can ADD (SUM) the two images
	if (MbledhjaPikselave(szOutputFile, szPixels1, szPixels2, nNumberOfPixels1, 
					  bfh1, bih1, nTejkalimi))
	{
		printf("There has been an error in ADD operator of the images 1 and 2\n");
		return -1;
	}

	delete [] szPixels1;
	delete [] szPixels2;

	szPixels1 = NULL;
	szPixels2 = NULL;

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
	// checks the size of the file and the result is saved into variable lFileSize
	long lFileSize;
	fseek(fInputBitmap, 0, SEEK_END);
	lFileSize = ftell(fInputBitmap);
	// shko ne fillim te file-it
	fseek(fInputBitmap, 0, SEEK_SET);
	int nNumberRead;
	// reads bytes for BITMAPFILEHEADER and saves them in the variable tempBuffer
	nNumberRead = fread(tempBuffer, sizeof(char), sizeof(BITMAPFILEHEADER), fInputBitmap);
	// fills variable tempBuffer with BITMAPFILEHEADER structure
	memcpy((char *) bfh, tempBuffer, sizeof(BITMAPFILEHEADER));
	// verifies size of the file
	if (bfh->bfSize != lFileSize)
	{
		fclose(fInputBitmap);
		printf("File %s must have %ld bytes\n", szFileName, lFileSize);
		return 1;
	}
	// Verifies the first two bytes of the file
	if (bfh->bfType != 0x4d42)
	{
		fclose(fInputBitmap);
		printf("File %s is not of type bitmap\n", szFileName);
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

	// reads next bytes from the file and result is saved into variable tempBuffer
	nNumberRead = fread(tempBuffer, sizeof(char), sizeof(BITMAPINFOHEADER), fInputBitmap);
	memcpy((char *) bih, tempBuffer, sizeof(BITMAPINFOHEADER));
	// verifies if the number of bits is 8 per pixel
	if (bih->biBitCount != 8)
	{
		fclose(fInputBitmap);
		printf("Numer of bits per pixel should be 8\n");
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
	// read color table which should have components R, G, B the same.
	// the values should be from 0-255
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
		printf("File %s does not exist or could not be opened\n", szFileName);
		return 1;
	}
	// As the image is grayscale, pixels will be placed after structures 
	// BITMAPFILEHEADER, BITMAPINFOHEADER and color table RGBQUAD[256]
	int nOffsetBytes = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD[256]);
	// pointer file goes after structures BITMAPFILEHEADER, BITMAPINFOHEADER and table color
	fseek(fInputBitmap, nOffsetBytes, SEEK_SET);
	// read bytes of the file and save the result into variable szPixels
	long nNumberRead = fread(szPixels, sizeof(char), nNumberOfPixels, fInputBitmap);
	fclose(fInputBitmap);
	return 0;
}

int MbledhjaPikselave(char *szOutputFile, unsigned char *szPixelsIm1, unsigned char *szPixelsIm2, 
					  long nNumberOfPixels, BITMAPFILEHEADER bfh, BITMAPINFOHEADER bih, int nTejkalimi)
{
	FILE *fOutputBitmap;
	if ((fOutputBitmap = fopen(szOutputFile, "w+b")) == NULL)
	{
		printf("File %s could not be created\n", szOutputFile);
		return 1;
	}
	long i;
	// Variable szOutputPixels stores the bytes of output file
	char *szOutputPixels = new char[nNumberOfPixels];
	// the sum of the pixels of the images 1 and 2 is stored into tempPixel variable
	int tempPixel;
	for (i = 0; i < nNumberOfPixels; i++)
	{
		tempPixel = szPixelsIm1[i] + szPixelsIm2[i];
		if (tempPixel > 255)
		{
			if (nTejkalimi)
				szOutputPixels[i] = 255;
			else
				szOutputPixels[i] = tempPixel - 255;
		}
		else
		{
			szOutputPixels[i] = tempPixel;
		}
	}
	// fill structure RGBQUAD so the components R,G,B are the same
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

