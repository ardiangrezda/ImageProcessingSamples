/* Implementuar nga Ardian Grezda per lenden "Procesimi i imixheve" (mars-qershor 2004)

 Ky program krijon bitmapin me 255 nivele gri

 Sintaksa:
	creategraybitmap grayscale filename gjatesia gjeresia
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
		printf("Bitmap-i nuk ka mundur te krijohet\n");
		printf("Sintaksa: creategraybitmap grayscale filename gjatesia gjeresia\n");
		return -1;
	}
	
	argv[0]	= strupr(argv[0]);
	argv[1] = strupr(argv[1]);
	argv[2] = strupr(argv[2]);

	// nese variabla Extension eshte .BMP atehere ky funksion i kthehet funksionit main()
	if (strcmp(argv[1], "GRAYSCALE"))
	{
		printf("Argumenti i dyte duhet te jete grayscale!\n");
		return 1;
	}

	char szFileName[100];
	strcpy(szFileName, argv[2]);
	char *Extension;

	// Gjen paraqitjen e fundit te pikes ne emrin e datotekes dhe rezultatin e ruan
	// ne variablen Extension
	Extension	= strrchr(argv[2],'.');
	if (Extension == NULL)
	{
		strcat(szFileName, ".BMP");
		Extension = ".BMP";
	}
	Extension	= strupr(Extension);
	if (strcmp(Extension, ".BMP"))
	{
		printf("Bitmapi duhet te kete ekstenzion .bmp!\n");
		return -1;
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
	bih.biBitCount		= 8;  // bitmapi eshte grayscale
	bih.biCompression	= BI_RGB;
	bih.biSizeImage		= WIDTHBYTES (lWidth * bih.biBitCount ) * lHeight;
	bih.biXPelsPerMeter	= 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed		= 0;
	bih.biClrImportant	= 0;

	// mbushe strukturen RGBQUAD ashtu qe komponentet R,G,B jane te njejta

	RGBQUAD rgb[256];
	for (int i = 0; i < 256; i++)
	{
		rgb[i].rgbBlue		= i;
		rgb[i].rgbGreen		= i;
		rgb[i].rgbRed		= i;
		rgb[i].rgbReserved	= 0;
	}

	// mbushe strukturen BITMAPINFOHEADER me shenime per bitmapin binar
	bfh.bfSize			= (DWORD) (sizeof(BITMAPFILEHEADER) + 
			bih.biSize + sizeof(rgb) + bih.biSizeImage);
	bfh.bfType			= 0x4d42;  // 0x42 = "B" 0x4d = "M" 
	bfh.bfReserved1		= 0;
	bfh.bfReserved2		= 0;
	bfh.bfOffBits		= (DWORD) sizeof(BITMAPFILEHEADER) + 
		bih.biSize + sizeof (rgb); 

	// numri i bajtave ne gjeresi te bitmapit
	long lNumberBytes = WIDTHBYTES (lWidth * bih.biBitCount);

	// pikselat qe vendosen ne nje rresht
	char *szRow			= new char[lNumberBytes];

	// te gjithe pikselat vendosen ne kete varg
	char *szBytes		= new char[lNumberBytes * lHeight];
	int nCurrentWByte;

	for (int nCurrentHeight = 0; nCurrentHeight < lHeight; nCurrentHeight++)
	{
		// te gjithe bajtat ne nje rresht do te jene 0
		for (nCurrentWByte = 0; nCurrentWByte < lNumberBytes; nCurrentWByte++)
		{
			szRow[nCurrentWByte]  = 0;
		}
		// vetem bajtat e caktuar behen 1
		for (nCurrentWByte = 0; nCurrentWByte < lWidth; nCurrentWByte++)
		{
			szRow[nCurrentWByte] = 0xff;
		}
		memcpy(szBytes + nCurrentHeight * lNumberBytes, szRow, lNumberBytes);
	}

	// shkruaj informatat per BITMAPFILEHEADER, BITMAPINFOHEADER, RGBQUAD dhe 
	// per pikselat ne file-in binar

	fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, fbitmap);
	fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, fbitmap);
	fwrite(&rgb, sizeof(rgb), 1, fbitmap);
	fwrite(szBytes, sizeof(char), lNumberBytes* lHeight, fbitmap);
	
	delete [] szRow;
	delete [] szBytes;
	szRow = NULL;
	szBytes = NULL;
	
	fclose(fbitmap);
	printf ("Datoteka %s u krijua ne rregull\n", szFileName);
	return 0;
}