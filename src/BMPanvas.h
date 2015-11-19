// BMPanvas.h: interface for the BMPanvas class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BMPANVAS_H__1D024EEE_E5D6_4B1B_A5A4_3AE8B8FABC20__INCLUDED_)
#define AFX_BMPANVAS_H__1D024EEE_E5D6_4B1B_A5A4_3AE8B8FABC20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <gdiplus.h>
using namespace Gdiplus;

#include "MyTime.h"
#include "afxmt.h"
struct BMPINFO
	{
		BITMAPINFOHEADER bmiHeader;
		RGBQUAD bmiColors[256];

		void operator=(BMPINFO& src)
		{
			bmiHeader=src.bmiHeader;		
		};
		BMPINFO() 
		{
			bmiHeader.biSize=40;
			bmiHeader.biPlanes=1;	
			bmiHeader.biClrUsed=0;
			bmiHeader.biClrImportant=0;
			bmiHeader.biCompression=0;
			bmiHeader.biSizeImage=0;
			memset(bmiColors,0,256*sizeof(RGBQUAD));
		};
		BMPINFO(BITMAP src)
		{			
			bmiHeader.biSize=40;
			bmiHeader.biPlanes=1;	
			bmiHeader.biClrUsed=0;
			bmiHeader.biClrImportant=0;
			bmiHeader.biCompression=0;
			bmiHeader.biSizeImage=0;
			memset(bmiColors,0,256*sizeof(RGBQUAD));

			bmiHeader.biWidth=src.bmWidth;
			bmiHeader.biHeight=src.bmHeight;
			bmiHeader.biBitCount=src.bmBitsPixel;			
		};
	};

class BMPanvas//: public CDC
{
protected:
    HDC hdc;
	HBITMAP hbmp,last_hbmp;

	int GetBitmap();
	int CheckColorType();
public:
	enum {COL_ERROR=0,BW=1,COLOR_PAL=128,GRAY_PAL=256,TRUE_COLOR=1<<24, RGBA=1<<31} ColorType;
	enum {READ,WRITE};
	BMPINFO INFO;
	LPBITMAPINFO lpbi;
	BITMAP bitmap;

	MyTimer Timer1; 	
	long &w,&h,&wbyte; int arr_size;
	CRect Rgn; BYTE *arr; CString FileName;
	
	BMPanvas(CDC *parent);	
	BMPanvas(BMPanvas*);	
	virtual ~BMPanvas();

	int Create(BMPanvas *src,CRect rgn);
	int Create(BMPINFO*  info);
	int Destroy();	

	int LoadImage(CString name);		
	int LoadBitmapArray(int hmin=-1,int hmax=-1);
	void UnloadBitmapArray();
	int SetBitmapArray(int hmin=-1,int hmax=-1);

//	unsigned int GetSum2(CRect& testRect);
//	unsigned int GetSum3(CPoint *Points,int num, CPoint dd);

	enum BMPStatus {BMP_ERROR, BMP_EMPTY, BMP_LOAD} status;	
	
	int CreateGrayPallete();
	int PatBlt(DWORD mode);
	int CopyPallete(BMPanvas *src);
	int CopyTo(BMPanvas* dest, CRect destRect, CPoint srcXY);
	int CopyTo(BMPanvas* dest, CPoint destXY, CRect srcRect);
	HDC GetDC(void);
	HBITMAP GetHBMP(void);
};

BITMAPINFOHEADER GetBMPInfo(CString name);


#endif // !defined(AFX_BMPANVAS_H__1D024EEE_E5D6_4B1B_A5A4_3AE8B8FABC20__INCLUDED_)
