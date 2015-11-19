// BMPanvas.cpp: implementation of the BMPanvas class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "testWMF.h"
#include "BMPanvas.h"
#include ".\bmpanvas.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BMPanvas::BMPanvas(CDC* parent):
	w(INFO.bmiHeader.biWidth),h(INFO.bmiHeader.biHeight),wbyte(bitmap.bmWidthBytes)
{
	w=-1; h=-1; last_hbmp=NULL;
	INFO.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	INFO.bmiHeader.biCompression=BI_RGB;
	INFO.bmiHeader.biSizeImage=0; 
	INFO.bmiHeader.biClrImportant=INFO.bmiHeader.biClrUsed=0;
	lpbi=(LPBITMAPINFO)(&INFO); arr=0;
	status=BMP_ERROR; FileName="NoName";
	ColorType=COL_ERROR;
	hdc=CreateCompatibleDC(parent->GetSafeHdc()); 	
}

BMPanvas::BMPanvas(BMPanvas *src):
	w(INFO.bmiHeader.biWidth),h(INFO.bmiHeader.biHeight),wbyte(bitmap.bmWidthBytes)
{
	w=-1; h=-1; last_hbmp=NULL;
	INFO.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	INFO.bmiHeader.biCompression=BI_RGB;
	INFO.bmiHeader.biSizeImage=0; 
	INFO.bmiHeader.biClrImportant=INFO.bmiHeader.biClrUsed=0;
	lpbi=(LPBITMAPINFO)(&INFO); arr=0;
	status=BMP_ERROR; FileName="NoName";
	ColorType=COL_ERROR;
	hdc=CreateCompatibleDC(src->hdc); 	
}

BMPanvas::~BMPanvas()
{
	DeleteDC(hdc);
	if(arr) delete[] arr;
}

int BMPanvas::Destroy()
{
	int ret=ERR_OK; HBITMAP tt=NULL; 	
	SelectObject(hdc,last_hbmp);	
	ret=DeleteObject(hbmp);
	ColorType=COL_ERROR;
	w=h=0; UnloadBitmapArray(); 	
	return ret;
}

int BMPanvas::Create(BMPanvas *src,CRect rgn)
{	
	CPoint& tl=Rgn.TopLeft(); tl=rgn.TopLeft();
    Destroy(); 
	if( (hbmp=CreateCompatibleBitmap( src->hdc,rgn.Width(),rgn.Height() ) ))
	{
		last_hbmp=(HBITMAP)SelectObject(hdc,hbmp); 
		GetBitmap();	
		Rgn=rgn;
	}
	return (int)hbmp;	
}

int BMPanvas::Create(BMPINFO*  info)
{		
    Destroy();
	hbmp=CreateDIBSection(hdc,(LPBITMAPINFO)(info),DIB_RGB_COLORS,0,0,0); 	
	last_hbmp=(HBITMAP)SelectObject(hdc,hbmp); GetBitmap();		
	Rgn=CRect(0,0,w,h);
	return (int)hbmp;
}

int BMPanvas::LoadImage(CString name)
{	
	int ret=ERR_OK;    
	CImage TI; 	
	if(!(ret=TI.Load(name)))
	{		
		GetObject(HBITMAP(TI),sizeof(BITMAP),&bitmap);
		BMPINFO tt(bitmap);
		if(TI.IsIndexed()) TI.GetColorTable(0,256,tt.bmiColors);
		Create(&tt);
		int a=TI.GetBPP();
		TI.BitBlt(hdc,0,0);
		FileName=name;
	}	
	return ret;
}

void BMPanvas::UnloadBitmapArray()
{
	if(arr) {delete[] arr;arr=0;}	
}

int BMPanvas::CopyTo(BMPanvas* dest, CRect destRect, CPoint srcXY)
{
	int ret; 
	ret=BitBlt(dest->hdc,destRect.left,destRect.top,destRect.Width(),destRect.Height(),hdc,srcXY.x,srcXY.y,SRCCOPY);					
	return ret;
}

int BMPanvas::CopyTo(BMPanvas* dest, CPoint destXY, CRect srcRect)
{
	int ret;
	ret=BitBlt(dest->hdc,destXY.x,destXY.y,srcRect.Width(),srcRect.Height(),hdc,srcRect.left,srcRect.top,SRCCOPY);					
	return ret;
}

int BMPanvas::GetBitmap()
{	
	int ret=ERR_OK;
	GetObject(hbmp,sizeof(bitmap),&bitmap);
    
	INFO.bmiHeader.biWidth=bitmap.bmWidth;
	INFO.bmiHeader.biHeight=bitmap.bmHeight;
	INFO.bmiHeader.biBitCount=bitmap.bmBitsPixel;
	INFO.bmiHeader.biPlanes=bitmap.bmPlanes;	
	CheckColorType();
	Rgn=CRect(Rgn.TopLeft(),CSize(w,h));

	return ret;
}


int BMPanvas::LoadBitmapArray(int hmin,int hmax)
{
	int ret=ERR_OK;
	if(hmin<0) hmin=0;
	if(hmax<0) hmax=h-1;
	if(hmax<hmin) {int t=hmax; hmax=hmin; hmin=t;}
	UnloadBitmapArray();	

	arr_size=(wbyte)*(hmax-hmin+1)+1;
	arr=new BYTE[arr_size];	

	ret=GetDIBits(hdc,hbmp,h-1-hmax,hmax-hmin+1,arr,lpbi,DIB_RGB_COLORS);
	GetDIBColorTable(hdc,0,256,INFO.bmiColors);					
	return ret;
}

int BMPanvas::SetBitmapArray(int hmin,int hmax)
{
	if(!arr) return 0;
	int ret=ERR_OK;	
	
	if(hmin<0) hmin=0;
	if(hmax<0) hmax=h-1;

	ret=SetDIBits(hdc,hbmp,h-1-hmax,hmax-hmin+1,arr,lpbi,DIB_RGB_COLORS);
	return ret;
}

/*
unsigned int BMPanvas::GetSum2(CRect& testRect)
{
	int i,i1,i2; unsigned int sum=0;
	if(!arr) return 0;
	int ret;
	if((ret=Lock(READ)))
	{
		i1=((h-1)-(testRect.top))*wbyte; i2=((h-1)-(testRect.bottom-1))*wbyte;
		for(i=testRect.left;i<testRect.right;i++)
		{
			sum+=510-arr[i+i1]-arr[i+i2];			
		}
		i1=((h-1)-(testRect.top+1))*wbyte+testRect.left; i2=((h-1)-(testRect.top+1))*wbyte+testRect.right-1;
		for(i=testRect.top+1;i<testRect.bottom-1;i++)
		{
			sum+=510-arr[i1]-arr[i2]; i1-=wbyte;i2-=wbyte;
		}
	}
	return sum;
}
*/
/*
unsigned int BMPanvas::GetSum3(CPoint *Points,int num, CPoint dd)
{
	int i; unsigned int sum=0;
	if(!arr) return 0;
	CPoint t;
	for(i=0;i<num;i++)
	{
		t=Points[i];
		sum+=255-arr[t.x+dd.x+(h-1-(t.y+dd.y))*wbyte];
	}	
	return sum;
}
*/

int BMPanvas::CreateGrayPallete()
{
	int ret=ERR_OK;	
	RGBQUAD col; 
	for(int i=0;i<256;i++) {col.rgbBlue=col.rgbGreen=col.rgbRed=(BYTE)i; INFO.bmiColors[i]=col;}
	ret=SetDIBColorTable(hdc,0,256,INFO.bmiColors);
	CheckColorType();
	return ret;
}

int BMPanvas::CopyPallete(BMPanvas *src)
{
	int ret=ERR_OK;	
	ret+=GetDIBColorTable(src->hdc,0,256,INFO.bmiColors);
	ret+=SetDIBColorTable(hdc,0,256,INFO.bmiColors);
	CheckColorType();
	return ret;
}

int BMPanvas::CheckColorType()
{
	int ret=ERR_OK;	

    	RGBQUAD pal[256]; int i,k,j;
		switch(INFO.bmiHeader.biBitCount)
		{
		case 32: ColorType=RGBA; break;
		case 24: ColorType=TRUE_COLOR; break;
		case 8:		
			GetDIBColorTable(hdc,0,256,pal);		
			for(i=0,k=0,j=0;i<256;i++) 
			{				
				if(pal[i].rgbBlue==i && pal[i].rgbGreen==i && pal[i].rgbRed==i) k++;			
				if(pal[i].rgbBlue==0 && pal[i].rgbGreen==0 && pal[i].rgbRed==0) j++;			
			}
			ColorType=COLOR_PAL; 
			if(k==256) ColorType=GRAY_PAL;
			if(j==255) ColorType=BW;
			break;
		case 1:
			ColorType=BW; break;
		default:
			ColorType=COL_ERROR;	
		}
	
    return ret;
}

BITMAPINFOHEADER GetBMPInfo(CString name)
{
	BITMAPFILEHEADER header;
	BITMAPINFOHEADER info;
	CFile f;
	if(f.Open(name,CFile::modeRead))
	{
		f.Read(&header,sizeof(header));
		f.Read(&info,sizeof(info));
		f.Close();
	}
    return info;
};

HDC BMPanvas::GetDC(void)
{
	return hdc;
}

HBITMAP BMPanvas::GetHBMP(void)
{
	return hbmp;
}

int BMPanvas::PatBlt( DWORD mode )
{
	return ::PatBlt(GetDC(),0,0,w,h,mode);
}
