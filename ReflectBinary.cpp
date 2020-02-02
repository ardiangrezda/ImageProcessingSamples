/* Implementuar nga Ardian Grezda per lenden "Procesimi i imixheve" (mars-qershor 2004)

 Ky program e ben reflektimin e nje imazhi binar pergjate boshtit vertikal, 
	ku si qender e boshtit merret qendra e imazhit hyres

 Sintaksa:
	 ReflectBinary FileHyres FileDales
	ku 
		FileHyres eshte emri i file-it hyres
		FileDales eshte emri i file-it dales
 */
#include <stdio.h>
#include <windows.h>

// Ky funksion verifikon se a eshte file-i hyres si bitmap file. Nese po, kthehet 0, 
// e nese nuk eshte bitmap, pason dalja nga programi
//
int CheckIfBinaryBitmap(char * szFileName, BITMAPFILEHEADER *bfh, BITMAPINFOHEADER *bih, int *ngjyra);

// Ketu do te fitohen vetem pikselat per imixhet hyrese, te cilat ruhen ne variablen szPixels
// nNumberOfPixels- numri i pikselave ne file-in hyres
// szFileName - emri i file-it hyres
// Nese dalja eshte 0, qdo gje ka perfunduar ne rregull, perndryshe do te kemi ndonje gabim
int PixelBytes(long nNumberOfBytes, unsigned char *szBytes, char * szFileName);

// Ky funksion e ben reflektimin e imazhit binar dhe rezultain e ruan ne file-in dales 
int Reflect(char *szOutputFile, unsigned char *szBytesIm, long nNumberOfBytes, 
			BITMAPFILEHEADER bfh, BITMAPINFOHEADER bih);

// Ky funksion i inverton bajtat, dmth nese vlera e pikselave eshte 0, atehere behet 1,
// ndersa nese eshte 1, atehere behet 0
void InvertBytes(unsigned char *szBytes, long nNumberOfBytes);

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4) 
int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("Sintaksa: ReflectBinary FileHyres FileDales\n");
		return -1;
	}
	argv[1] = strupr(argv[1]);
	argv[2] = strupr(argv[2]);

	char szInputFile[100], szOutputFile[100];
	strcpy(szInputFile, argv[1]);
	strcpy(szOutputFile, argv[2]);
	char *Extension;

	// Gjen paraqitjen e fundit te pikes ne emrin e datotekes dhe rezultatin e ruan
	// ne variablen Extension
	// kjo behet per file-in e pare hyres
	Extension	= strrchr(argv[1],'.');
	if (Extension == NULL)
	{
		strcat(szInputFile, ".BMP");
		Extension = ".BMP";
	}
	Extension	= strupr(Extension);

	if (strcmp(Extension, ".BMP"))
	{
		printf("Bitmapi duhet te kete ekstenzion .bmp!\n");
		return -1;
	}

	// Cakto ekstenzionin per file-in dales 
	Extension	= strrchr(argv[2],'.');
	if (Extension == NULL)
	{
		strcat(szOutputFile, ".BMP");
		Extension = ".BMP";
	}
	Extension	= strupr(Extension);
	if (strcmp(Extension, ".BMP"))
	{
		printf("Bitmapi duhet te kete ekstenzion .bmp!\n");
		return -1;
	}
	// struktura BITMAPFILEHEADER per file-in hyres
	BITMAPFILEHEADER bfh;
	// struktura BITMAPINFOHEADER per file-in hyres
	BITMAPINFOHEADER bih;
	
	int nNgjyra;

	// Thirret ky funksion per te kontrolluar a eshte imixhi hyres bitmap
	if (CheckIfBinaryBitmap(szInputFile, &bfh, &bih, &nNgjyra))
	{
		printf("Ka ardhe deri te ndonje gabim ne strukturen e bitmapit hyres\n");
		return -1;
	}

	// numri i pikselave caktohet nga kjo shprehje
	long nNumberOfBytes = bfh.bfSize - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER)
		- sizeof(RGBQUAD[2]);
	
	// bufferi per pikselat e imixhit hyres
	unsigned char *szBytes = new unsigned char [nNumberOfBytes];

	// Bajtat e imixhit hyres ruhen ne variablen szBytes
	if (PixelBytes(nNumberOfBytes, szBytes, szInputFile))
	{
		printf("Ka ardhe deri te ndonje gabim ne strukturen e bitmapit 1\n");
		delete [] szBytes;
		szBytes = NULL;
		return -1;
	}

	// nese tabela e ngjyrave eshte se pari ngjyra e bardhe (ff ff ff 00) e pastaj
	// ngjyra e zeze (00 00 00 00), bytat qe permbajne imixhin duhet te invertohen
	if (nNgjyra != 0)
	{
		InvertBytes(szBytes, nNumberOfBytes);
	}

	// e ben reflektimin e imazhit hyres pergjate boshtit vertikal, ku si 
	// qender e boshtit merret qendra e imazhit hyres
	if (Reflect(szOutputFile, szBytes, nNumberOfBytes, bfh, bih))
	{
		printf("Ka ardhe deri te ndonje gabim ne operatorin e refletimit\n");
		return -1;
	}

	delete [] szBytes;
	szBytes = NULL;
	printf("Imixhi %s u krijua ne rregull\n", szOutputFile);
	return 0;
}

int CheckIfBinaryBitmap(char * szFileName, BITMAPFILEHEADER *bfh, BITMAPINFOHEADER *bih, int *ngjyra)
{
	unsigned char tempBuffer[200];
	FILE *fInputBitmap;
	if ((fInputBitmap = fopen(szFileName, "r+b")) == NULL)
	{
		printf("Datoteka %s nuk ekziston apo nuk mund te hapet\n", szFileName);
		return 1;
	}
	// kontrollo madhesine e file-it dhe rezultatin e ruan ne variablen lFileSize
	long lFileSize;
	fseek(fInputBitmap, 0, SEEK_END);
	lFileSize = ftell(fInputBitmap);
	// shko ne fillim te file-it
	fseek(fInputBitmap, 0, SEEK_SET);
	int nNumberRead;
	// lexo bajtat per BITMAPFILEHEADER dhe ruaji ne var. tempBuffer
	nNumberRead = fread(tempBuffer, sizeof(char), sizeof(BITMAPFILEHEADER), fInputBitmap);
	// mbushe variablen tempBuffer me strukturen BITMAPFILEHEADER
	memcpy((char *) bfh, tempBuffer, sizeof(BITMAPFILEHEADER));
	// verifiko madhesine e file-it
	if (bfh->bfSize != lFileSize)
	{
		fclose(fInputBitmap);
		printf("Datoteka %s duhet te kete %ld bajte\n", szFileName, lFileSize);
		return 1;
	}
	// verifiko 2 bajtat e pare te file-it
	if (bfh->bfType != 0x4d42)
	{
		fclose(fInputBitmap);
		printf("Datoteka %s nuk eshte bitmap file\n", szFileName);
		return 1;
	}
	if (bfh->bfReserved1 != 0)
	{
		fclose(fInputBitmap);
		printf("Bajtat 7 dhe 8 te datotekes %s duhet te jene 0\n", szFileName);
		return 1;
	}

	if (bfh->bfReserved2 != 0)
	{
		fclose(fInputBitmap);
		printf("Bajtat 9 dhe 10 te datotekes %s duhet te jene 0\n", szFileName);
		return 1;
	}

	// lexo bajtat e ardhshem prej file-it dhe rezultatin ruaje ne variablen tempBuffer
	nNumberRead = fread(tempBuffer, sizeof(char), sizeof(BITMAPINFOHEADER), fInputBitmap);
	memcpy((char *) bih, tempBuffer, sizeof(BITMAPINFOHEADER));
	// verifiko a eshte numri i bitave per piksel 1
	if (bih->biBitCount != 1)
	{
		fclose(fInputBitmap);
		printf("Numri i bitave per piksel duhet te jete 2\n");
		return 1;

	}
	// verifiko a eshte bitmapi i kompresuar
	if (bih->biCompression != BI_RGB)
	{
		fclose(fInputBitmap);
		printf("Bitmapi eshte i kompresuar\n");
		return 1;
	}

	RGBQUAD rgb[2];
	// lexo tabelen e ngjyrave, e ketu tabela e ngjyrave ka dy elemente, ku elementi
	// i pare i ka komponentet R=G=B =0, ndersa elementi i dyte i ka komponentat 
	// R = G = B = 255
	int nNgjyra1, nNgjyra2;
	nNumberRead = fread(tempBuffer, sizeof(char), sizeof(rgb[0]), fInputBitmap);
	nNgjyra1 = tempBuffer[0];
	if (nNgjyra1 != tempBuffer[1])
	{
		printf("Tabela e ngjyrave nuk eshte korrekte\n");
		fclose(fInputBitmap);
		return 1;
	}
	else if (nNgjyra1 != tempBuffer[2])
	{
		printf("Tabela e ngjyrave nuk eshte korrekte\n");
		fclose(fInputBitmap);
		return 1;
	}

	nNumberRead = fread(tempBuffer, sizeof(char), sizeof(rgb[0]), fInputBitmap);
	nNgjyra2 = tempBuffer[0];
	if (nNgjyra2 != tempBuffer[1])
	{
		printf("Tabela e ngjyrave nuk eshte korrekte\n");
		fclose(fInputBitmap);
		return 1;
	}
	else if (nNgjyra2 != tempBuffer[2])
	{
		printf("Tabela e ngjyrave nuk eshte korrekte\n");
		fclose(fInputBitmap);
		return 1;
	}

	if (nNgjyra1 != 0 && nNgjyra1 != 255)
	{
		printf("Tabela e ngjyrave nuk eshte korrekte\n");
		fclose(fInputBitmap);
		return 1;
	}
	
	if (nNgjyra2 != 0 && nNgjyra2 != 255)
	{
		printf("Tabela e ngjyrave nuk eshte korrekte\n");
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
		printf("Datoteka %s nuk ekziston apo nuk mund te hapet\n", szFileName);
		return 1;
	}

	// Pasi imixhi eshte binar, pikselat do te jene te vendosura pas strukturave
	// BITMAPFILEHEADER, BITMAPINFOHEADER si dhe pas tabeles se ngjyrave RGBQUAD[2]
	int nOffsetBytes = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD[2]);
	// ketu pointeri i file-it shkon pas struktures BITMAPFILEHEADER, BITMAPINFOHEADER dhe tabeles se ngjyrave
	fseek(fInputBitmap, nOffsetBytes, SEEK_SET);
	// lexo bajtat e file-it dhe rezultati ruhet ne variablen szPixels
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
		printf("Datoteka %s nuk ka mundur te krijohet\n", szOutputFile);
		return 1;
	}

	// Variabla szOutputPixels ruan bajtat e file-it dales
	unsigned char *szOutputBytes	= new unsigned char[nNumberOfBytes];
	int nWidth						= bih.biWidth;
	int nHeight						= bih.biHeight;
	int nWidthBytes					= WIDTHBYTES(nWidth);

	unsigned char *szRow			= new unsigned char[nWidthBytes];

	// ne fillim te gjithe bajtat dales jane zero
	int i;
	for (i = 0; i < nNumberOfBytes; i++)
	{
		szOutputBytes[i] = 0;
	}

	// bajti qe momentalisht procesohet
	int nCurrentImageByte, nCurrentReflectByte;
	// biti brenda bajtit qe momentalisht procesohet
	int nCurrentImageBitFromByte, nCurrentReflectBitFromByte;
	// vlera e bitave te imixhit origjinal dhe te reflektuar
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
		// mbushe strukturen RGBQUAD ashtu qe komponentet R,G,B jane te njejta
	RGBQUAD rgb[2];
	rgb[0].rgbBlue		= 0;
	rgb[0].rgbGreen		= 0;
	rgb[0].rgbRed		= 0;
	rgb[0].rgbReserved	= 0;

	rgb[1].rgbBlue		= 255;
	rgb[1].rgbGreen		= 255;
	rgb[1].rgbRed		= 255;
	rgb[1].rgbReserved	= 0;


	// shkruaj ne file-in dales
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