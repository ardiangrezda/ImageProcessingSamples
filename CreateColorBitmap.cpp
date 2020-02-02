/* Implementuar nga Ardian Grezda per lenden "Procesimi i imixheve" (mars-qershor 2004)

 Ky program krijon bitmapin me ngjyra

 Sintaksa:
	createcolorbitmap color filename gjatesia gjeresia red green blue
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
	if (!(argc >= 5 && argc <= 8))
	{
		printf("Bitmap-i nuk ka mundur te krijohet\n");
		printf("Sintaksa: createcolorbitmap color filename gjatesia gjeresia red green blue\n");
		return -1;
	}
	
	argv[0]	= strupr(argv[0]);
	argv[1] = strupr(argv[1]);
	argv[2] = strupr(argv[2]);

	// nese variabla Extension eshte .BMP atehere ky funksion i kthehet funksionit main()
	if (strcmp(argv[1], "COLOR"))
	{
		printf("Argumenti i dyte duhet te jete color!\n");
		return 1;
	}

	char *szFileName	= argv[2];
	char *Extension;

	// Gjen paraqitjen e fundit te pikes ne emrin e datotekes dhe rezultatin e ruan
	// ne variablen Extension
	Extension	= strrchr(szFileName,'.');
	Extension	= strupr(Extension);

	// nese variabla Extension eshte .BMP atehere ky funksion i kthehet funksionit main()
	if (strcmp(Extension, ".BMP"))
	{
		printf("Bitmapi duhet te kete ekstenzion .bmp!\n");
		return 1;
	}


	// verifiko a eshte argumenti i katert numer
	// e nese nuk eshte numer, atehere pason dalja nga programi
	int a;
	for (a = 0; a < strlen(argv[3]); a++)
	{
		if (!isdigit(argv[3][a]))
		{
			printf("Argumenti i katert (per gjatesi te bitmapit) duhet te jete numer\n");
 			exit(1);
		}
	}
	
	for (a = 0; a < strlen(argv[4]); a++)
	{
		if (!isdigit(argv[4][a]))
		{
			printf("Argumenti i peste (per gjeresi te bitmapit) duhet te jete numer\n");
 			exit(1);
		}
	}

	long lHeight		= atol(argv[3]);
	long lWidth			= atol(argv[4]);
	if (lHeight > 2000)
	{
		printf("Gjatesia e bitmapit duhet te jete deri ne 2000 piksela\n");
		return -1;
	}

	if (lWidth > 2000)
	{
		printf("Gjeresia e bitmapit duhet te jete deri ne 2000 piksela\n");
		return -1;
	}

	if (argv[5] == NULL)
	{
		argv[5] = "0";
		argv[6] = "0";
		argv[7] = "0";
	}

	if (argv[6] == NULL)
	{
		argv[6] = "0";
		argv[7] = "0";
	}

	if (argv[7] == NULL)
	{
		argv[7] = "0";
	}

	for (a = 0; a < strlen(argv[5]); a++)
	{
		if (!isdigit(argv[5][a]))
		{
			printf("Argumenti i gjashte (per ngjyren e kuqe) duhet te jete numer\n");
 			exit(1);
		}
	}

	for (a = 0; a < strlen(argv[6]); a++)
	{
		if (!isdigit(argv[6][a]))
		{
			printf("Argumenti i shtate (per ngjyren e gjelbert) duhet te jete numer\n");
 			exit(1);
		}
	}

	for (a = 0; a < strlen(argv[7]); a++)
	{
		if (!isdigit(argv[7][a]))
		{
			printf("Argumenti i tete (per ngjyren e kaltert) duhet te jete numer\n");
 			exit(1);
		}
	}

	int nRed	= atoi(argv[5]);
	int nGreen	= atoi(argv[6]);
	int nBlue	= atoi(argv[7]);

	if (nRed > 255)
	{
		printf("Argumenti i gjashte (per ngjyren e kuqe) duhet te jete prej 0-255, e vendosim 255");
		nRed = 255;
	}

	if (nGreen > 255)
	{
		printf("Argumenti i shtate (per ngjyren e gjelber) duhet te jete prej 0-255, e vendosim 255");
		nGreen = 255;
	}

	if (nBlue > 255)
	{
		printf("Argumenti i tete (per ngjyren e nBlue) duhet te jete prej 0-255, e vendosim 255");
		nBlue = 255;
	}

	FILE *fbitmap;
	if ((fbitmap = fopen(szFileName, "w+b")) == NULL)
	{
		printf("Eshte bere nje gabim ne datoteken %s ", szFileName);
		return -1;
	}

	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;

	// mbushe strukturen BITMAPINFOHEADER me informata per grayscale bitmap 
	bih.biSize			= sizeof(BITMAPINFOHEADER);
	bih.biWidth			= lWidth;
	bih.biHeight		= lHeight;
	bih.biPlanes		= 1;
	bih.biBitCount		= 24;  // bitmapi eshte grayscale
	bih.biCompression	= BI_RGB;
	bih.biSizeImage		= WIDTHBYTES (lWidth * bih.biBitCount ) * lHeight;
	bih.biXPelsPerMeter	= 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed		= 0;
	bih.biClrImportant	= 0;

	// mbushe strukturen BITMAPINFOHEADER me shenime per bitmapin binar
	bfh.bfSize			= (DWORD) (sizeof(BITMAPFILEHEADER) + 
			bih.biSize + bih.biSizeImage);
	bfh.bfType			= 0x4d42;  // 0x42 = "B" 0x4d = "M" 
	bfh.bfReserved1		= 0;
	bfh.bfReserved2		= 0;
	bfh.bfOffBits		= (DWORD) sizeof(BITMAPFILEHEADER) + bih.biSize;

	// numri i bajtave ne gjeresi te bitmapit
	long lNumberBytes = WIDTHBYTES (lWidth * bih.biBitCount);

	// te gjithe pikselat vendosen ne kete varg
	char *szBytes		= new char[lNumberBytes * lHeight];
	int nCurrentWByte;
	int nControl = 0;

	for (int nCurrentHeight = 0; nCurrentHeight < lHeight; nCurrentHeight++)
	{
		// te gjithe bajtat ne nje rresht do te jene 0
		for (nCurrentWByte = 0; nCurrentWByte < lNumberBytes; nCurrentWByte++)
		{
			szBytes[nCurrentWByte + nCurrentHeight * lNumberBytes]  = 0;
		}

		for (nCurrentWByte = 0 ; nCurrentWByte < lNumberBytes / 3; nCurrentWByte++)
		{
			szBytes[nCurrentWByte * 3 + nCurrentHeight * lNumberBytes + nControl]		= (char) nBlue;
			szBytes[nCurrentWByte * 3 + nCurrentHeight * lNumberBytes + nControl + 1]	= (char) nGreen;
			szBytes[nCurrentWByte * 3 + nCurrentHeight * lNumberBytes + nControl + 2]	= (char) nRed;
		}
	}

	// shkruaj informatat per BITMAPFILEHEADER, BITMAPINFOHEADER dhe 
	// per pikselat ne file-in binar

	fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, fbitmap);
	fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, fbitmap);
	fwrite(szBytes, sizeof(char), lNumberBytes* lHeight, fbitmap);
	
	delete [] szBytes;
	szBytes = NULL;
	
	fclose(fbitmap);
	printf ("Datoteka %s u krijua ne rregull\n", szFileName);
	return 0;
}