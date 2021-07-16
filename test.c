#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include "qrencode.h"
#include "qrcode.h"
#include "print_msg.h"

typedef unsigned int UINT;
typedef unsigned long DWORD;
typedef long int LONG;
typedef unsigned short WORD;
typedef unsigned char BYTE;

#pragma pack(1)
typedef struct tagBITMAPFILEHEADER {
    WORD    bfType;
    WORD    bfSize;
    WORD    bfReserved1;
    WORD    bfReserved2;
    WORD    xxx;
    DWORD   bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{
    DWORD   biSize;
    LONG    biWidth;
    LONG    biHeight;
    WORD    biPlanes;
    WORD    biBitCount;
    DWORD   biCompression;
    DWORD   biSizeImage;
    LONG    biXPelsPerMeter;
    LONG    biYPelsPerMeter;
    DWORD   biClrUsed;
    DWORD   biClrImportant;
} BITMAPINFOHEADER;
#pragma pack()

typedef struct COLORREF_RGB
{
    BYTE cRed;
    BYTE cGreen;
    BYTE cBlue;
}COLORREF_RGB;

// WIDTHBYTES takes # of bits in a scanline and rounds up to nearest word.
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)
int makeBMPfile(int xRes, int yRes)
{
    BITMAPFILEHEADER fheader = {0};
    BITMAPINFOHEADER iheader = {0};
    COLORREF_RGB rgb = {0,0,0};
    FILE *fp;
    int i,j, padbytes;
	
    iheader.biSize      = sizeof (BITMAPINFOHEADER);
    iheader.biWidth     = xRes;
    iheader.biHeight    = yRes;
    iheader.biPlanes    = 1;
    iheader.biBitCount = 24;
    iheader.biSizeImage= WIDTHBYTES(xRes*24) * yRes;
    padbytes= WIDTHBYTES(xRes*24) - (xRes*24/8);

    fheader.bfType = 0x4D42;
    fheader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof (BITMAPINFOHEADER) /*no palette for 24 bits color*/;
    fheader.bfSize = fheader.bfOffBits + iheader.biSizeImage;

    if ((fp = fopen("E:/work/hx-kong/embed/xmap/bin/Debug/x64/testASP.bmp", "wb"))==NULL) return(0);

    fwrite(&fheader, sizeof(BITMAPFILEHEADER),1,fp);
	fwrite(&iheader, sizeof(BITMAPINFOHEADER),1,fp);

    for(i=0;i<yRes; i++){
        for(j=0;j<xRes;j++){
            fwrite(&rgb, sizeof(COLORREF_RGB),1,fp);
        }
        if (padbytes) fwrite(&rgb, padbytes,1,fp);
    }
    fclose(fp);
    return(1);
}

void QRCode(void)
{
    char*           szSourceSring = "1255335584";
    unsigned int    unWidth, x, y, l, n, unWidthAdjusted, unDataBytes;
    unsigned char*  pRGBData, *pSourceData, *pDestData;
    QRcode*         pQRC;
    FILE*           f;

	//makeBMPfile(100,100);
	//exit(1);
    if (pQRC = QRcode_encodeString(szSourceSring, 0, QR_ECLEVEL_H, QR_MODE_8, 1))
    {
        unWidth = pQRC->width;
        unWidthAdjusted = unWidth *  3;
        if (unWidthAdjusted % 4)
            unWidthAdjusted = (unWidthAdjusted / 4 + 1) * 4;
        unDataBytes = unWidthAdjusted * unWidth ;

        // Allocate pixels buffer

        if (!(pRGBData = (unsigned char*)malloc(unDataBytes)))
        {
			ERR_PRINTF("QRCode malloc rgb data error , memory not enough");
            return;
        }

        // Preset to white
        memset(pRGBData, 0xff, unDataBytes);

        // Convert QrCode bits to bmp pixels
        pSourceData = pQRC->data;
        for (y = 0; y < unWidth; y++)
        {
            pDestData = pRGBData + unWidthAdjusted * y;
            for (x = 0; x < unWidth; x++)
            {
                if (*pSourceData & 1)
                {
                            *(pDestData +   unWidthAdjusted ) = 0xff;
                            *(pDestData + 1 +   unWidthAdjusted ) = 0;
                            *(pDestData + 2 +   unWidthAdjusted ) = 0;
                }
                pDestData += 3;
                pSourceData++;
            }
        }
		
		BITMAPFILEHEADER fheader = {0};
		BITMAPINFOHEADER iheader = {0};
		int xRes=unWidth,yRes=unWidth;
		int padbytes;
		iheader.biSize      = sizeof (BITMAPINFOHEADER);
		iheader.biWidth     = xRes;
		iheader.biHeight    = yRes;
		iheader.biPlanes    = 1;
		iheader.biBitCount = 24;
		iheader.biSizeImage= WIDTHBYTES(xRes*24) * yRes;
		padbytes= WIDTHBYTES(xRes*24) - (xRes*24/8);

        // Output the bmp file
		fheader.bfType = 0x4D42;
		fheader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof (BITMAPINFOHEADER) /*no palette for 24 bits color*/;
		fheader.bfSize = fheader.bfOffBits + iheader.biSizeImage;
		
	/*	int i,j;
		for(i=0;i<yRes; i++){
			for(j=0;j<xRes;j++){
				fwrite(&rgb, sizeof(COLORREF_RGB),1,f);
			}
			if (padbytes){ fwrite(&rgb, padbytes,1,f);}
		}
		*/
		f = fopen("E:/work/hx-kong/embed/xmap/bin/Debug/x64/111.bmp", "wb");
        if (f)
        {
			fwrite(&fheader, sizeof(BITMAPFILEHEADER),1,f);
			fwrite(&iheader, sizeof(BITMAPINFOHEADER),1,f);
            fwrite(pRGBData, sizeof(unsigned char), unDataBytes, f);
            fclose(f);
        }
        else
        {
			ERR_PRINTF("qrencode Unable to open file");
        }

        // Free data
        free(pRGBData);
        QRcode_free(pQRC);
    }
    else
    {
        ERR_PRINTF("qrencode create fail.");
    }
}
