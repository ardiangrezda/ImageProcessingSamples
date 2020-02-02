/* Implementuar nga Ardian Grezda per lenden "Procesimi i imixheve" (mars-qershor 2004)

 Ky program aplikon operatorin AND ne dy imixhe te tipit bitmap te cilat jane imixha binar
 ose 1 piksel representohet me 1 bit
 Sintaksa:
	OperatoriAND FileHyresNr1 FileHyresNr2 FileDales 
	ku 
		FileHyresNr1 eshte emri i file-it hyres 1
		FileHyresNr2 eshte emri i file-it hyres 2
		FileDales eshte emri i file-it i cili fitohet pas aplikimit te operatorit AND ne imixhat 1 dhe 2
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
int PixelBytes(long nNumberOfPixels, unsigned char *szPixels, char * szFileName);

// Zbaton operatorin AND ne dy imixhe. 
// szOutputFile - emri i file-it dales
// szPixelsIm1 - pikselat e imixhit 1
// szPixelsIm2 - pikselat e imixhit 2
// nNumberOfPixels - numri i pikselave ne imixhin dales
// bfh - struktura BITMAPFILEHEADER
// bih - struktura BITMAPINFOHEADER
int OperatoriANDPikselave(char *szOutputFile, unsigned char *szPixelsIm1, unsigned char *szPixelsIm2, 
			  long nNumberOfPixels, BITMAPFILEHEADER bfh, BITMAPINFOHEADER bih);

// Ky funksion i inverton bajtat, dmth nese vlera e pikselave diku eshte 0, atehere behet 1,
// ndersa nese eshte 1, atehere behet 0
void InvertBytes(unsigned char *szBytes, long nNumberOfBytes);
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4) 
int main(int argc, char *argv[])
{
	if (argc != 4)
	{
		printf("Sintaksa: OperatoriAND FileHyresNr1 FileHyresNr2 FileDales\n");
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
	// Gjen paraqitjen e fundit te pikes ne emrin e datotekes dhe rezultatin e ruan
	// ne variablen Extension
	// kjo behet per file-in e pare hyres
	Extension	= strrchr(argv[1],'.');
	if (Extension == NULL)
	{
		strcat(szFirstInputFile, ".BMP");
		Extension = ".BMP";
	}
	Extension	= strupr(Extension);
	if (strcmp(Extension, ".BMP"))
	{
		printf("Bitmapi duhet te kete ekstenzion .bmp!\n");
		return -1;
	}

	// Cakto ekstenzionin per file-in e dyte hyres
	Extension	= strrchr(argv[2],'.');
	if (Extension == NULL)
	{
		strcat(szSecondInputFile, ".BMP");
		Extension = ".BMP";
	}
	Extension	= strupr(Extension);
	if (strcmp(Extension, ".BMP"))
	{
		printf("Bitmapi duhet te kete ekstenzion .bmp!\n");
		return -1;
	}

	// Cakto ekstenzionin per file-in dales 
	Extension	= strrchr(argv[3],'.');
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

	// struktura BITMAPFILEHEADER per file-in e pare dhe te dyte hyres
	BITMAPFILEHEADER bfh1, bfh2;
	// struktura BITMAPINFOHEADER per file-in e pare dhe te dyte hyres
	BITMAPINFOHEADER bih1, bih2;
	
	int nNgjyra1, nNgjyra2;
	// Thirret ky funksion per te kontrolluar a eshte imixhi i pare bitmap
	if (CheckIfBinaryBitmap(szFirstInputFile, &bfh1, &bih1, &nNgjyra1))
	{
		printf("Ka ardhe deri te ndonje gabim ne strukturen e bitmapit 1\n");
		return -1;
	}

	// numri i pikselave caktohet nga kjo shprehje
	long nNumberOfBytes1 = bfh1.bfSize - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER)
		- sizeof(RGBQUAD[2]);
	
	// bufferi per pikselat e imixhit 1
	unsigned char *szBytes1 = new unsigned char [nNumberOfBytes1];
	
	// Bajtat e imixhit 1 ruhen ne variablen szBytes1
	if (PixelBytes(nNumberOfBytes1, szBytes1, szFirstInputFile))
	{
		printf("Ka ardhe deri te ndonje gabim ne strukturen e bitmapit 1\n");
		delete [] szBytes1;
		szBytes1 = NULL;
		return -1;
	}

	// Thirret ky funksion per te kontrolluar a eshte imixhi i dyte bitmap
	if (CheckIfBinaryBitmap(szSecondInputFile, &bfh2, &bih2, &nNgjyra2))
	{
		printf("Ka ardhe deri te ndonje gabim ne strukturen e bitmapit 2\n");
		delete [] szBytes1;
		szBytes1 = NULL;
		return -1;
	}
	
	// Kontrollo a eshte gjeresia e bitmapave hyres e njejte 
	if (bih1.biWidth != bih2.biWidth)
	{
		printf("Gjeresia e bitmapit 1 dhe 2 nuk eshte e njejte\n");
		delete [] szBytes1;
		szBytes1 = NULL;
		return -1;
	}

	// Kontrollo a eshte gjatesia e bitmapave hyres e njejte 
	if (bih1.biHeight != bih2.biHeight)
	{
		printf("Gjatesia e bitmapit 1 dhe 2 nuk eshte e njejte\n");
		delete [] szBytes1;
		szBytes1 = NULL;
		return -1;
	}

	// kontrollo a eshte tabela e ngjyrave te bitmapi 1 dhe 2 e njejte
	if (nNgjyra1 != nNgjyra2)
	{
		printf("Tabela e ngjyrave te bitmapi 1 dhe 2 nuk perputhen\n");
		delete [] szBytes1;
		szBytes1 = NULL;
		return -1;
	}

	// numri i bajtave te imixhit 2 caktohet nga kjo shprehje
	long nNumberOfBytes2 = bfh2.bfSize - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER)
		- sizeof(RGBQUAD[2]);
	// bufferi per pikselat e imixhit 2
	unsigned char *szBytes2 = new unsigned char [nNumberOfBytes2];
	if (PixelBytes(nNumberOfBytes2, szBytes2, szSecondInputFile))
	{
		printf("Ka ardhe deri te ndonje gabim ne strukturen e bitmapit 1\n");
		delete [] szBytes1;
		delete [] szBytes2;
		szBytes1 = NULL;
		szBytes2 = NULL;
		return -1;
	}

	// nese tabela e ngjyrave eshte se pari ngjyra e bardhe (ff ff ff 00) e pastaj
	// ngjyra e zeze (00 00 00 00), bytat qe permbajne imixhin duhet te invertohen
	if (nNgjyra1 != 0)
	{
		InvertBytes(szBytes1, nNumberOfBytes1);
		InvertBytes(szBytes2, nNumberOfBytes2);
	}
	// nese madhesia e bitmapave eshte e njejte dhe te gjithe testet tjera kane
	// kaluar ne rregull, mund ta bejme AND-ing te ketyre dy imixheve
	if (OperatoriANDPikselave(szOutputFile, szBytes1, szBytes2, nNumberOfBytes1, 
					  bfh1, bih1))
	{
		printf("Ka ardhe deri te ndonje gabim ne operatorin AND te imixheve 1 dhe 2\n");
		return -1;
	}

	delete [] szBytes1;
	delete [] szBytes2;

	szBytes1 = NULL;
	szBytes2 = NULL;

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

int OperatoriANDPikselave(char *szOutputFile, unsigned char *szBytesIm1, unsigned char *szBytesIm2, 
					  long nNumberOfBytes, BITMAPFILEHEADER bfh, BITMAPINFOHEADER bih)
{
	FILE *fOutputBitmap;
	if ((fOutputBitmap = fopen(szOutputFile, "w+b")) == NULL)
	{
		printf("Datoteka %s nuk ka mundur te krijohet\n", szOutputFile);
		return 1;
	}
	long i;

	// Variabla szOutputPixels ruan bajtat e file-it dales
	unsigned char *szOutputBytes	= new unsigned char[nNumberOfBytes];
	int nWidth						= bih.biWidth;
	int nHeight						= bih.biHeight;
	int nWidthBytes					= WIDTHBYTES(nWidth);
	unsigned char *szRow1			= new unsigned char[nWidthBytes];
	unsigned char *szRow2			= new unsigned char[nWidthBytes];
	// bajti qe momentalisht procesohet
	int nCurrentWByte;
	// biti qe momentalisht procesohet
	int l;
	// vlera e bitave te imixhit 1 dhe 2
	int BitIm1, BitIm2;

	// ne fillim te gjithe bajtat jane zero
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

	// mbushe strukturen RGBQUAD ashtu qe komponentet R,G,B jane te njejta
	RGBQUAD rgb[2];
	rgb[0].rgbBlue		= 0;
	rgb[0].rgbGreen		= 0;
	rgb[0].rgbRed		= 0;
	rgb[0].rgbReserved	= 0;

	rgb[1].rgbBlue		= 255;
	rgb[1].rgbGreen		= 255;
	rgb[1].rgbRed		= 255;
	rgb[1].rgbReserved	= 255;


	// shkruaj ne file-in dales
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