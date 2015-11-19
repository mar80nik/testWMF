// BWFilter.cpp: implementation of the BWFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "testWMF.h"
#include "BWFilter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BWFilter::BWFilter()
{
	Name="B/W Filter";
}

BWFilter::~BWFilter()
{

}
int BWFilter::Main(BMPanvas *src)
{	
	int ret=ERR_OK;
/*
	int x,y,l,ret=0; RGBTRIPLE *src; BYTE *dest; BYTE* src1; BMPINFO TempInfo; 
	BMPanvas &Buffer=*(CurPic->Buffer),tempBuffer(&Buffer);

	switch(Buffer.ColorType)
	{
	case BMPanvas::TRUE_COLOR:
		TempInfo=Buffer.INFO; TempInfo.bmiHeader.biBitCount=8;	
		tempBuffer.Create(&TempInfo); 
		tempBuffer.CreateGrayPallete();
		Buffer.LoadBitmapArray(); tempBuffer.LoadBitmapArray(); 

		src1=Buffer.arr; dest=tempBuffer.arr;
		for(y=0;y<Buffer.h;y++)
		{			
			src=(RGBTRIPLE *)src1;
			for(x=0;x<Buffer.w;x++) 
			{
				RGBTRIPLE &t=src[x];				
				l=(t.rgbtBlue+t.rgbtGreen+t.rgbtRed)/3;
				dest[x]=(l<=CurPic->frg_threshold ? 0:255);                			
			}	
			src1+=Buffer.wbyte; dest+=tempBuffer.wbyte;
		}
		tempBuffer.SetBitmapArray(); tempBuffer.CopyTo(&Buffer,CPoint(0,0),tempBuffer.Rgn);
		break;	
	case BMPanvas::COLOR_PAL:
	case BMPanvas::GRAY_PAL:		
		tempBuffer.Create(CurPic->Buffer,CurPic->Buffer->w,CurPic->Buffer->h);
		CurPic->Buffer->CopyTo(&tempBuffer,CPoint(0,0),tempBuffer.Rgn);
		GetDIBColorTable(Buffer.GetSafeHdc(),0,256,Buffer.INFO.bmiColors);
		SetDIBColorTable(tempBuffer.GetSafeHdc(),0,256,Buffer.INFO.bmiColors);		
		
		for(int i=0;i<256;i++) 
		{
			RGBQUAD &t=Buffer.INFO.bmiColors[i]; 
			int l=(t.rgbBlue+t.rgbGreen+t.rgbRed)/3;        
			if(l<=CurPic->frg_threshold) t.rgbBlue=t.rgbGreen=t.rgbRed=0;			
			else t.rgbBlue=t.rgbGreen=t.rgbRed=255;			
		}
		SetDIBColorTable(tempBuffer.GetSafeHdc(),0,256,Buffer.INFO.bmiColors);		
		//CurPic->Buffer->CreateGrayPallete();
		tempBuffer.CopyTo(CurPic->Buffer,CPoint(0,0),tempBuffer.Rgn);
		break;
	default: ret=1;
	}
	*/
	return ret;
}

BW_FloydSteinberg::BW_FloydSteinberg()
{
    c23=7/16.; c31=3/16.; c32=5/16.; c33=1/16.;
}

int BW_FloydSteinberg::Main(BMPanvas *src)
{	
	int ret=ERR_OK;
	int x,y; BYTE *arr; BYTE* p23, *p31, *p32, *p33, *p22, old; int err,t; 
//	short *errs_cur,*errs_next;

	if(src->ColorType!=BMPanvas::TRUE_COLOR)
	{
		src->LoadBitmapArray();
		//errs_cur=new short[src->w]; memset(errs_cur,0,src->w*sizeof(short));
		//errs_next=new short[src->w];memset(errs_next,0,src->w*sizeof(short));

		arr=src->arr;
		for(y=0;y<src->h-1;y++)
		{		
			p22=arr+y*src->wbyte+1;
			p23=p22+1; 
			p32=p22+src->wbyte; p31=p32-1; p33=p32+1;
			for(x=1;x<src->w-1;x++) 
			{
				old=*p22;
				(*p22)=((*p22)>threshold ? 255:0);				
				err=old-(*p22);
				t=*p23+err*7/16; if(t>255) t=255; if(t<0) t=0; *p23=t;
				t=*p31+err*3/16; if(t>255) t=255; if(t<0) t=0;*p31=t;
				t=*p32+err*5/16; if(t>255) t=255; if(t<0) t=0;*p32=t;
				t=*p33+err/16; if(t>255) t=255; if(t<0) t=0;*p33=t;
				p22++; p23++; p31++; p32++; p33++;
			}	
		}
		
		p23=arr+(src->h-1)*src->wbyte;
		for(x=0;x<src->w;x++) 
		{
			(*p23)=((*p23)>threshold ? 255:0);				
			p23++;
		}

		p22=arr+src->wbyte;; p23=p22+src->w-1;
		for(y=1;y<src->h-1;y++) 
		{
			(*p22)=((*p22)>threshold ? 255:0);				
			(*p23)=((*p23)>threshold ? 255:0);				
			p22+=src->wbyte; p23+=src->wbyte;
		}

		src->SetBitmapArray();	//delete errs_cur; delete errs_next;
	}
	else ret=ERR_GRAPHICS;

	return ret;
}
    