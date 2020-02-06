/* Implemented by Ardian Grezda for the subject "Image processing" (march-june 2004) in University of Prishtina, Kosovo

The program applies AND operator over two images of the type bitmap which are 1 bit

Syntax:
	OperatoriAND InputFile1 InputFile2 OutputFile 
	where 
		InputFile1 is the name of input file 1
		InputFile2 is the name of input file 2
		OutputFile is the name of the file which is obtained after applying AND operator over input images 1 and 2
		
*/

#include <stdio.h>
#include <windows.h>

// The function verifies if the input file is bitmap file. If yes, value 0 will be returned, 
// otherwise the program will terminate
//
int CheckIfBinaryBitmap(char * szFileName, BITMAPFILEHEADER *bfh, BITMAPINFOHEADER *bih, int *ngjyra);

// Here will get pixels from input images, which will be saved into variable szPixels
// nNumberOfPixels- number of pixels in the input file
// szFileName - the name of the input file
// If the returned value is 0, everything went OK.
int PixelBytes(long nNumberOfPixels, unsigned char *szPixels, char * szFileName);

// Applies AND operator over two images
// szOutputFile - name of the output file
// szPixelsIm1 - pixels of the image 1
// szPixelsIm2 - pixels of the image 2
// nNumberOfPixels - number of the pixels in output image
// bfh - BITMAPFILEHEADER structure
// bih - BITMAPINFOHEADER structure
int OperatoriANDPikselave(char *szOutputFile, unsigned char *szPixelsIm1, unsigned char *szPixelsIm2, 
			  long nNumberOfPixels, BITMAPFILEHEADER bfh, BITMAPINFOHEADER bih);

// This function invert bytes, i.e. if the value of the pixels is 0, then after applying this function, it will become 1
// If the input pixel is 1, after applying this function, it will become 0
void InvertBytes(unsigned char *szBytes, long nNumberOfBytes);
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4) 
int main(int argc, char *argv[])
{
	if (argc != 4)
	{
		printf("Sytax: OperatoriAND inputFile1 inputFile2 OutputFile\n");
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

	// Determine extension for output file
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

	// structure BITMAPFILEHEADER for the first and the second input file
	BITMAPFILEHEADER bfh1, bfh2;
	// structure BITMAPINFOHEADER for the first and the second input file
	BITMAPINFOHEADER bih1, bih2;
	
	int nNgjyra1, nNgjyra2;
	// This function is called to check if the first image is of type bitmap
	if (CheckIfBinaryBitmap(szFirstInputFile, &bfh1, &bih1, &nNgjyra1))
	{
		printf("There has been an error in the structure of the bitmap 1\n");
		return -1;
	}

	// number of pixels is evaluated by this expression
	long nNumberOfBytes1 = bfh1.bfSize - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER)
		- sizeof(RGBQUAD[2]);
	
	// buffer for pixels of the image 1
	unsigned char *szBytes1 = new unsigned char [nNumberOfBytes1];
	
	// Bytes of the image 1 are saved on variable szBytes1
	if (PixelBytes(nNumberOfBytes1, szBytes1, szFirstInputFile))
	{
		printf("There has been an error in the structure of the bitmap 1\n");
		delete [] szBytes1;
		szBytes1 = NULL;
		return -1;
	}

	// This function is called to check if the second image is of type bitmap
	if (CheckIfBinaryBitmap(szSecondInputFile, &bfh2, &bih2, &nNgjyra2))
	{
		printf("There has been an error in the structure of the bitmap 2\n");
		delete [] szBytes1;
		szBytes1 = NULL;
		return -1;
	}
	
	// Checks if the width of the two input images are equal
	if (bih1.biWidth != bih2.biWidth)
	{
		printf("Width of the bitmaps 1 and 2 are not the same\n");
		delete [] szBytes1;
		szBytes1 = NULL;
		return -1;
	}

	// Checks if the height of the two input images are equal 
	if (bih1.biHeight != bih2.biHeight)
	{
		printf("Height of the bitmaps 1 and 2 are not the same\n");
		delete [] szBytes1;
		szBytes1 = NULL;
		return -1;
	}

	// check if the color table of the bitmaps 1 and 2 are the same
	if (nNgjyra1 != nNgjyra2)
	{
		printf("Color table of the bitmaps 1 and 2 does not match\n");
		delete [] szBytes1;
		szBytes1 = NULL;
		return -1;
	}

	// number of bytes is evaluated by this expression
	long nNumberOfBytes2 = bfh2.bfSize - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER)
		- sizeof(RGBQUAD[2]);
	// buffer for pixels of the image 2
	unsigned char *szBytes2 = new unsigned char [nNumberOfBytes2];
	if (PixelBytes(nNumberOfBytes2, szBytes2, szSecondInputFile))
	{
		printf("There has been an error in the structure of the bitmap 2\n");
		delete [] szBytes1;
		delete [] szBytes2;
		szBytes1 = NULL;
		szBytes2 = NULL;
		return -1;
	}
	
	// if the color table is first white color (ff ff ff 00) and then 
	// black color (00 00 00 00), bytes that contain the image must invert
	if (nNgjyra1 != 0)
	{
		InvertBytes(szBytes1, nNumberOfBytes1);
		InvertBytes(szBytes2, nNumberOfBytes2);
	}
	// if size of the bitmap is the same and all other test are OK,
	// then we can do AND of these two images
	if (OperatoriANDPikselave(szOutputFile, szBytes1, szBytes2, nNumberOfBytes1, 
					  bfh1, bih1))
	{
		printf("There has been an error in AND operator of the images 1 and 2\n");
		return -1;
	}

	delete [] szBytes1;
	delete [] szBytes2;

	szBytes1 = NULL;
	szBytes2 = NULL;

	printf("Image %s was successfuly created\n", szOutputFile);
	return 0;
}

int CheckIfBinaryBitmap(char * szFileName, BITMAPFILEHEADER *bfh, BITMAPINFOHEADER *bih, int *ngjyra)
{
	unsigned char tempBuffer[200];
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
	// goes on the beginning of the file
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
	// verifies if the number of bits is 1 per pixel
	if (bih->biBitCount != 1)
	{
		fclose(fInputBitmap);
		printf("Numer of bits per pixel should be 1\n");
		return 1;

	}
	// verifies if the bitmap is compressed
	if (bih->biCompression != BI_RGB)
	{
		fclose(fInputBitmap);
		printf("Bitmap is compressed\n");
		return 1;
	}

	RGBQUAD rgb[2];

	int nNgjyra1, nNgjyra2;
	nNumberRead = fread(tempBuffer, sizeof(char), sizeof(rgb[0]), fInputBitmap);
	nNgjyra1 = tempBuffer[0];
	if (nNgjyra1 != tempBuffer[1])
	{
		printf("The table color is not correct\n");
		fclose(fInputBitmap);
		return 1;
	}
	else if (nNgjyra1 != tempBuffer[2])
	{
		printf("The table color is not correct\n");
		fclose(fInputBitmap);
		return 1;
	}

	nNumberRead = fread(tempBuffer, sizeof(char), sizeof(rgb[0]), fInputBitmap);
	nNgjyra2 = tempBuffer[0];
	if (nNgjyra2 != tempBuffer[1])
	{
		printf("The table color is not correct\n");
		fclose(fInputBitmap);
		return 1;
	}
	else if (nNgjyra2 != tempBuffer[2])
	{
		printf("The table color is not correct\n");
		fclose(fInputBitmap);
		return 1;
	}

	if (nNgjyra1 != 0 && nNgjyra1 != 255)
	{
		printf("The table color is not correct\n");
		fclose(fInputBitmap);
		return 1;
	}
	
	if (nNgjyra2 != 0 && nNgjyra2 != 255)
	{
		printf("The table color is not correct\n");
		fclose(fInputBitmap);
		return 1;
	}

	*ngjyra = nNgjyra1;
	fclose(fInputBitmap);
	return 0;
}

int PixelBytes(long nNumberOfBytes, unsigned char *szBytes, char * szFileName)
{
	FILE *fInputBitmap;
	if ((fInputBitmap = fopen(szFileName, "r+b")) == NULL)
	{
		printf("File %s does not exist or could not be opened\n", szFileName);
		return 1;
	}

	// As the image is binary, pixels will be placed after structures
	// BITMAPFILEHEADER, BITMAPINFOHEADER and after table color RGBQUAD[2]
	int nOffsetBytes = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD[2]);
	// pointer file goes after structures BITMAPFILEHEADER, BITMAPINFOHEADER and table color
	fseek(fInputBitmap, nOffsetBytes, SEEK_SET);
	// read bytes of the file and save the result into variable szPixels
	long nNumberRead = fread(szBytes, sizeof(char), nNumberOfBytes, fInputBitmap);
	fclose(fInputBitmap);
	return 0;
}

int OperatoriANDPikselave(char *szOutputFile, unsigned char *szBytesIm1, unsigned char *szBytesIm2, 
					  long nNumberOfBytes, BITMAPFILEHEADER bfh, BITMAPINFOHEADER bih)
{
	FILE *fOutputBitmap;
	if ((fOutputBitmap = fopen(szOutputFile, "w+b")) == NULL)
	{
		printf("File %s could not be created\n", szOutputFile);
		return 1;
	}
	long i;

	// Variable szOutputPixels saves the bytes of the output file
	unsigned char *szOutputBytes		= new unsigned char[nNumberOfBytes];
	int nWidth				= bih.biWidth;
	int nHeight				= bih.biHeight;
	int nWidthBytes				= WIDTHBYTES(nWidth);
	unsigned char *szRow1			= new unsigned char[nWidthBytes];
	unsigned char *szRow2			= new unsigned char[nWidthBytes];
	// byte that is currently processed
	int nCurrentWByte;
	// bit that is currently processed
	int l;
	// value of the bits of the images 1 and 2
	int BitIm1, BitIm2;

	// at the begining, all the bytes are zero
	for (i = 0; i < nNumberOfBytes; i++)
	{
		szOutputBytes[i] = 0;
	}

	for (int nCurrentHeight = 0; nCurrentHeight < nHeight; nCurrentHeight++)
	{
		memcpy(szRow1, szBytesIm1 + nCurrentHeight * nWidthBytes, nWidthBytes);
		memcpy(szRow2, szBytesIm2 + nCurrentHeight * nWidthBytes, nWidthBytes);
		for (int nCurrentBit = 0; nCurrentBit < nWidth; nCurrentBit++)
		{
			nCurrentWByte = nCurrentBit / 8;
			l = nCurrentBit % 8;
			BitIm1 = (szRow1[nCurrentWByte] >> (8 - l - 1))  & 1;
			BitIm2 = (szRow2[nCurrentWByte] >> (8 - l - 1))  & 1;
			szOutputBytes[nCurrentWByte + nCurrentHeight * nWidthBytes] 
					|= (BitIm1 & BitIm2) << (8 - l - 1);
		}
	}

	// fill structure RGBQUAD so the components R,G,B are the same
	RGBQUAD rgb[2];
	rgb[0].rgbBlue		= 0;
	rgb[0].rgbGreen		= 0;
	rgb[0].rgbRed		= 0;
	rgb[0].rgbReserved	= 0;

	rgb[1].rgbBlue		= 255;
	rgb[1].rgbGreen		= 255;
	rgb[1].rgbRed		= 255;
	rgb[1].rgbReserved	= 255;


	// writes on output file
	fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, fOutputBitmap);
	fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, fOutputBitmap);
	fwrite(&rgb, sizeof(rgb), 1, fOutputBitmap);
	fwrite(szOutputBytes, sizeof(char), nNumberOfBytes, fOutputBitmap);

	delete [] szOutputBytes;
	delete [] szRow1;
	delete [] szRow2;

	szOutputBytes = NULL;
	szRow1 = NULL;
	szRow2 = NULL;

	fclose(fOutputBitmap);
	return 0;
}


void InvertBytes(unsigned char *szBytes, long nNumberOfBytes)
{
	for (int i = 0; i < nNumberOfBytes; i++)
	{
		szBytes[i] = 255 - szBytes[i];
	}
}
