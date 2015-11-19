#include "stdafx.h"
#include ".\grayfilter.h"

GrayFilter::GrayFilter(void)
{
	Name="Gray Filter";
}

GrayFilter::~GrayFilter(void)
{
}

int GrayFilter::Main2(DjvuPic* CurPic)
{
	int ret=0; 
/*
	BMPanvas &Buffer=*(CurPic->Buffer),tempBuffer(&Buffer);
	BMPINFO TempInfo; 

	int x,y,l; RGBTRIPLE *src; BYTE *dest; BYTE* src1;	
	switch(Buffer.ColorType)
	{
	case BMPanvas::TRUE_COLOR:		
		TempInfo=Buffer.INFO; TempInfo.bmiHeader.biBitCount=8;	
		tempBuffer.Create(&TempInfo); 
		tempBuffer.CreateGrayPallete();
		tempBuffer.LoadBitmapArray(); Buffer.LoadBitmapArray(); 

		src1=Buffer.arr; dest=tempBuffer.arr;
        for(y=0;y<Buffer.h;y++)
		{			
			src=(RGBTRIPLE *)src1;
            for(x=0;x<Buffer.w;x++)		
			{
				RGBTRIPLE &t=src[x];				
				l=(t.rgbtBlue+t.rgbtGreen+t.rgbtRed)/3;
                dest[x]=l;                			
			}
			src1+=Buffer.wbyte; dest+=tempBuffer.wbyte;
		}
		ret+=tempBuffer.SetBitmapArray();
		tempBuffer.CopyTo(&Buffer,CPoint(0,0),tempBuffer.Rgn);
		break;
	case BMPanvas::COLOR_PAL:
		GetDIBColorTable(Buffer.GetSafeHdc(),0,256,Buffer.INFO.bmiColors);

		for(int i=0;i<256;i++) 
		{
			RGBQUAD &t=Buffer.INFO.bmiColors[i]; 
			int l=(t.rgbBlue+t.rgbGreen+t.rgbRed)/3;        
			t.rgbBlue=t.rgbGreen=t.rgbRed=l;
		}
		SetDIBColorTable(Buffer.GetSafeHdc(),0,256,Buffer.INFO.bmiColors);
		break;
	case BMPanvas::GRAY_PAL:
		break;
	}
	*/
	return ret;    
	
}