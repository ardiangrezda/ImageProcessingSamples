/* Implemented by Ardian Grezda for the subject "Image processing" (march-june 2004) in University of Prishtina, Kosovo

The program will reflect a binary image through vertical axis, where the center axis will be the center of input image

Syntax:
	 ReflectBinary InputFile OutputFile
	where 
		InputFile is name of the input file
		OutputFile is name of the output file
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
int PixelBytes(long nNumberOfBytes, unsigned char *szBytes, char * szFileName);

// The function will reflect the binary image and save the result into output file
int Reflect(char *szOutputFile, unsigned char *szBytesIm, long nNumberOfBytes, 
			BITMAPFILEHEADER bfh, BITMAPINFOHEADER bih);

// The function invert bytes, i.e. if the pixel value is 0, it will became 1 and vise versa
void InvertBytes(unsigned char *szBytes, long nNumberOfBytes);

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4) 
int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("Syntax: ReflectBinary InputFile OutputFile\n");
		return -1;
	}
	argv[1] = strupr(argv[1]);
	argv[2] = strupr(argv[2]);

	char szInputFile[100], szOutputFile[100];
	strcpy(szInputFile, argv[1]);
	strcpy(szOutputFile, argv[2]);
	char *Extension;

	// Finds the last apperance of the dot (.) in the filename
	// and saves it into Extension variable
	// this is done for the first input file
	Extension	= strrchr(argv[1],'.');
	if (Extension == NULL)
	{
		strcat(szInputFile, ".BMP");
		Extension = ".BMP";
	}
	Extension	= strupr(Extension);

	if (strcmp(Extension, ".BMP"))
	{
		printf("Bitmap must have extension .bmp!\n");
		return -1;
	}

	// Determine extension for output file
	Extension	= strrchr(argv[2],'.');
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
	BITMAPFILEHEADER bfh;
	// structure BITMAPINFOHEADER for the first and the second input file
	BITMAPINFOHEADER bih;
	
	int nNgjyra;
	// This function is called to check if the image is of type bitmap
	if (CheckIfBinaryBitmap(szInputFile, &bfh, &bih, &nNgjyra))
	{
		printf("There has been an error in the structure of the bitmap\n");
		return -1;
	}

	// number of pixels is evaluated by this expression
	long nNumberOfBytes = bfh.bfSize - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER)
		- sizeof(RGBQUAD[2]);
	
	// buffer for pixels of the input image
	unsigned char *szBytes = new unsigned char [nNumberOfBytes];

	// Bytes of the image are stored on variable szBytes
	if (PixelBytes(nNumberOfBytes, szBytes, szInputFile))
	{
		printf("There has been an error in the structure of the bitmap\n");
		delete [] szBytes;
		szBytes = NULL;
		return -1;
	}

	// if the color table is first white color (ff ff ff 00) and then 
	// black color (00 00 00 00), bytes that contain the image must invert
	if (nNgjyra != 0)
	{
		InvertBytes(szBytes, nNumberOfBytes);
	}
	// relfects input image through vertical axis, where the center of the axis is center of the input image
	if (Reflect(szOutputFile, szBytes, nNumberOfBytes, bfh, bih))
	{
		printf("There has been an error in reflection\n");
		return -1;
	}

	delete [] szBytes;
	szBytes = NULL;
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

int Reflect(char *szOutputFile, unsigned char *szBytesIm, long nNumberOfBytes, 
			BITMAPFILEHEADER bfh, BITMAPINFOHEADER bih)
{
	FILE *fOutputBitmap;
	if ((fOutputBitmap = fopen(szOutputFile, "w+b")) == NULL)
	{
		printf("File %s could not be created\n", szOutputFile);
		return 1;
	}

	// Variable szOutputPixels stores the bytes of the output file
	unsigned char *szOutputBytes	= new unsigned char[nNumberOfBytes];
	int nWidth						= bih.biWidth;
	int nHeight						= bih.biHeight;
	int nWidthBytes					= WIDTHBYTES(nWidth);

	unsigned char *szRow			= new unsigned char[nWidthBytes];

	// initally all output bytes are zeros
	int i;
	for (i = 0; i < nNumberOfBytes; i++)
	{
		szOutputBytes[i] = 0;
	}
	
	// the current byte processed
	int nCurrentImageByte, nCurrentReflectByte;
	// the bit inside byte which is currently processed
	int nCurrentImageBitFromByte, nCurrentReflectBitFromByte;
	// the value of the bits of the original and reflected image
	int ValueImageBit, ValueReflectedBit;
	for (int nCurrentHeight = 0; nCurrentHeight < nHeight; nCurrentHeight++)
	{
		memcpy(szRow, szBytesIm + nCurrentHeight * nWidthBytes, nWidthBytes);
		for (int nCurrentBit = 0; nCurrentBit < nWidth / 2; nCurrentBit++)
		{
			nCurrentImageByte = nCurrentBit / 8;
			nCurrentReflectByte = (nWidth - nCurrentBit - 1) / 8;
			nCurrentImageBitFromByte = nCurrentBit % 8;
			nCurrentReflectBitFromByte = (nWidth - nCurrentBit - 1) % 8;
			ValueImageBit = szRow[nCurrentImageByte] >> (8 - nCurrentImageBitFromByte - 1) & 1;
			ValueReflectedBit = szRow[nCurrentReflectByte] >> (8 - nCurrentReflectBitFromByte - 1) & 1;

			szOutputBytes[nCurrentImageByte + nCurrentHeight * nWidthBytes] 
					|= ValueReflectedBit << (8 - nCurrentImageBitFromByte - 1);
			szOutputBytes[nCurrentReflectByte + nCurrentHeight * nWidthBytes] 
					|= ValueImageBit << (8 - nCurrentReflectBitFromByte - 1);
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
	rgb[1].rgbReserved	= 0;


	// writes on output file
	fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, fOutputBitmap);
	fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, fOutputBitmap);
	fwrite(&rgb, sizeof(rgb), 1, fOutputBitmap);
	fwrite(szOutputBytes, sizeof(char), nNumberOfBytes, fOutputBitmap);

	delete [] szOutputBytes;
	delete [] szRow;

	szOutputBytes = NULL;
	szRow = NULL;

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
