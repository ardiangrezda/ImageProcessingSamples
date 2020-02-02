/* Implementuar nga Ardian Grezda per lenden "Procesimi i imixheve" (mars-qershor 2004)

 Ky program i mbledhe dy imixhe te tipit bitmap te cilat jane grayscale
 Sintaksa:
	OperatoriMbledhjes FileHyresNr1 FileHyresNr2 FileDales tejkalimi
	ku 
		FileHyresNr1 eshte emri i file-it hyres 1
		FileHyresNr2 eshte emri i file-it hyres 2
		FileDales eshte emri i file-it i cili fitohet pas mbledhjes se imixhit 1 dhe 2
		tejkalimi ka te beje me ata piksela kur shuma e dy pikeslave korrespondues (shuma prej Im1 + Im2)
			eshte me e madhe se 255. 
				Nese shfrytezuesi e jep vleren 0, atehere shuma e fituar
					zbritet per 255 dhe i ipet pikselit korrespondues
				Nese shfrytezuesi e jep vleren 1, atehere vlera e pikselit eshte 255
*/

#include <stdio.h>
#include <windows.h>

// Ky funksion verifikon se a eshte file-i hyres si bitmap file. Nese po, kthehet 0, 
// e nese nuk eshte bitmap, pason dalja nga programi
//
int CheckIfGrayscaleBitmap(char * szFileName, BITMAPFILEHEADER *bfh, BITMAPINFOHEADER *bih);

// Ketu do te fitohen vetem pikselat per imixhet hyrese, te cilat ruhen ne variablen szPixels
// nNumberOfPixels- numri i pikselave ne file-in hyres
// szFileName - emri i file-it hyres
// Nese dalja eshte 0, qdo gje ka perfunduar ne rregull, perndryshe do te kemi ndonje gabim
int PixelBytes(long nNumberOfPixels, unsigned char *szPixels, char * szFileName);

// I mbledh dy imixhe. 
// szOutputFile - emri i file-it dales
// szPixelsIm1 - pikselat e imixhit 1
// szPixelsIm2 - pikselat e imixhit 2
// nNumberOfPixels - numri i pikselave ne imixhin dales
// bfh - struktura BITMAPFILEHEADER
// bih - struktura BITMAPINFOHEADER
// nTejkalimi - tejkalimi per rastin kur shuma e dy pikselave eshte me e madhe se 255
//			Nese nTejkalimi eshte 0, atehere shuma e fituar zbritet per 255 
//				dhe i ipet pikselit korrespondues
//			Nese nTejkalimi ka vlere 1, atehere vlera e pikselit eshte 255
int MbledhjaPikselave(char *szOutputFile, unsigned char *szPixelsIm1, unsigned char *szPixelsIm2, 
			  long nNumberOfPixels, BITMAPFILEHEADER bfh, BITMAPINFOHEADER bih, int nTejkalimi);

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4) 
int main(int argc, char *argv[])
{
	if (argc != 5)
	{
		printf("Sintaksa: OperatoriMbledhjes FileHyresNr1 FileHyresNr2 FileDales tejkalimi(0 ose 1)\n");
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

	int a;
	for (a = 0; a < strlen(argv[4]); a++)
	{
		if (!isdigit(argv[4][a]))
		{
			printf("Argumenti i peste (per tejkalim te pikselave) duhet te jete numer\n");
 			exit(1);
		}
	}

	// tejkalimi mund te kete vlere 0 ose 1
	int nTejkalimi = atoi(argv[4]);
	if (nTejkalimi != 0 && nTejkalimi != 1)
	{
		printf("Tejkalimi duhet te jete 0 (pikselat e tejkaluar zbriten per 255) ose 1 (pikselat e tejkaluar behen 255)\n");
		return -1;
	}

	// struktura BITMAPFILEHEADER per file-in e pare dhe te dyte hyres
	BITMAPFILEHEADER bfh1, bfh2;
	// struktura BITMAPINFOHEADER per file-in e pare dhe te dyte hyres
	BITMAPINFOHEADER bih1, bih2;
	
	// Thirret ky funksion per te kontrolluar a eshte imixhi i pare bitmap
	if (CheckIfGrayscaleBitmap(szFirstInputFile, &bfh1, &bih1))
	{
		printf("Ka ardhe deri te ndonje gabim ne strukturen e bitmapit 1\n");
		return -1;
	}

	// numri i pikselave caktohet nga kjo shprehje
	long nNumberOfPixels1 = bfh1.bfSize - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER)
		- sizeof(RGBQUAD[256]);
	
	// bufferi per pikselat e imixhit 1
	unsigned char *szPixels1 = new unsigned char [nNumberOfPixels1];
	
	// Bajtat e imixhit 1 ruhen ne variablen szPixels1
	if (PixelBytes(nNumberOfPixels1, szPixels1, szFirstInputFile))
	{
		printf("Ka ardhe deri te ndonje gabim ne strukturen e bitmapit 1\n");
		delete [] szPixels1;
		szPixels1 = NULL;
		return -1;
	}

	// Thirret ky funksion per te kontrolluar a eshte imixhi i dyte bitmap
	if (CheckIfGrayscaleBitmap(szSecondInputFile, &bfh2, &bih2))
	{
		printf("Ka ardhe deri te ndonje gabim ne strukturen e bitmapit 2\n");
		delete [] szPixels1;
		szPixels1 = NULL;
		return -1;
	}
	
	// Kontrollo a eshte gjeresia e bitmapave hyres e njejte 
	if (bih1.biWidth != bih2.biWidth)
	{
		printf("Gjeresia e bitmapit 1 dhe 2 nuk eshte e njejte\n");
		delete [] szPixels1;
		szPixels1 = NULL;
		return -1;
	}

	// Kontrollo a eshte gjatesia e bitmapave hyres e njejte 
	if (bih1.biHeight != bih2.biHeight)
	{
		printf("Gjatesia e bitmapit 1 dhe 2 nuk eshte e njejte\n");
		delete [] szPixels1;
		szPixels1 = NULL;
		return -1;
	}

	// numri i pikselave te imixhit 2 caktohet nga kjo shprehje
	long nNumberOfPixels2 = bfh2.bfSize - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER)
		- sizeof(RGBQUAD[256]);
	// bufferi per pikselat e imixhit 2
	unsigned char *szPixels2 = new unsigned char [nNumberOfPixels2];
	if (PixelBytes(nNumberOfPixels2, szPixels2, szSecondInputFile))
	{
		printf("Ka ardhe deri te ndonje gabim ne strukturen e bitmapit 1\n");
		delete [] szPixels1;
		delete [] szPixels2;
		szPixels1 = NULL;
		szPixels2 = NULL;
		return -1;
	}

	// nese madhesia e bitmapave eshte e njejte dhe te gjithe testet tjera kane
	// kaluar ne rregull, mund ta bejme mbledhjen e pikselave korrespondues
	if (MbledhjaPikselave(szOutputFile, szPixels1, szPixels2, nNumberOfPixels1, 
					  bfh1, bih1, nTejkalimi))
	{
		printf("Ka ardhe deri te ndonje gabim ne mbledhjen e pikselave te imixheve 1 dhe 2\n");
		return -1;
	}

	delete [] szPixels1;
	delete [] szPixels2;

	szPixels1 = NULL;
	szPixels2 = NULL;

	printf("Imixhi %s u krijua ne rregull\n", szOutputFile);
	return 0;
}

int CheckIfGrayscaleBitmap(char * szFileName, BITMAPFILEHEADER *bfh, BITMAPINFOHEADER *bih)
{
	unsigned char tempBuffer[2000];
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
	// verifiko a eshte numri i bitave per piksel 8
	if (bih->biBitCount != 8)
	{
		fclose(fInputBitmap);
		printf("Numri i bitave per piksel duhet te jete 8\n");
		return 1;

	}
	
	// verifiko a eshte bitmapi i kompresuar
	if (bih->biCompression != BI_RGB)
	{
		fclose(fInputBitmap);
		printf("Bitmapi eshte i kompresuar\n");
		return 1;
	}

	RGBQUAD rgb[256];
	// lexo tabelen e ngjyrave, e ketu tabela e ngjyrave ka komponentat R,G,B te njejta,
	// dhe mund te kete vlera prej 0 deri 255
	for (int i = 0; i < 256; i++)
	{
		nNumberRead = fread(tempBuffer, sizeof(char), sizeof(rgb[i]), fInputBitmap);
		if ((tempBuffer[0] != i) || (tempBuffer[1] != i) || (tempBuffer[2] != i))
		{
			printf("Tabela e ngjyrave nuk eshte korrekte\n");
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
		printf("Datoteka %s nuk ekziston apo nuk mund te hapet\n", szFileName);
		return 1;
	}

	// Pasi imixhi eshte grayscale, pikselat do te jene te vendosura pas strukturave
	// BITMAPFILEHEADER, BITMAPINFOHEADER si dhe pas tabeles se ngjyrave RGBQUAD[256]
	int nOffsetBytes = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD[256]);
	// ketu pointeri i file-it shkon pas struktures BITMAPFILEHEADER, BITMAPINFOHEADER dhe tabeles se ngjyrave
	fseek(fInputBitmap, nOffsetBytes, SEEK_SET);
	// lexo bajtat e file-it dhe rezultati ruhet ne variablen szPixels
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
		printf("Datoteka %s nuk ka mundur te krijohet\n", szOutputFile);
		return 1;
	}
	long i;
	// Variabla szOutputPixels ruan bajtat e file-it dales
	char *szOutputPixels = new char[nNumberOfPixels];
	// shuma e pikselave te imixheve 1 dhe 2 ruhet ne kete variabel
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

	// mbushe strukturen RGBQUAD ashtu qe komponentet R,G,B jane te njejta
	RGBQUAD rgb[256];
	for (i = 0; i < 256; i++)
	{
		rgb[i].rgbBlue		= i;
		rgb[i].rgbGreen		= i;
		rgb[i].rgbRed		= i;
		rgb[i].rgbReserved	= 0;
	}

	// shkruaj ne file-in dales
	fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, fOutputBitmap);
	fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, fOutputBitmap);
	fwrite(&rgb, sizeof(rgb), 1, fOutputBitmap);
	fwrite(szOutputPixels, sizeof(char), nNumberOfPixels, fOutputBitmap);

	delete [] szOutputPixels;
	szOutputPixels = NULL;
	fclose(fOutputBitmap);
	return 0;
}

