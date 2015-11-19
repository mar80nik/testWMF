// RotateFilter.cpp: implementation of the RotateFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "testWMF.h"
#include "RotateFilter.h"
#include "bwfilter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RotateFilter::RotateFilter()
{
	angle=dx=dy=Cos=Sin=0.;	Q=HI;
}

RotateFilter::~RotateFilter()
{

}

RotateFilter::RotateFilter(RotateFilterParam &src): RotateFilterParam(src)
{    
}

void RotateFilter::SetFilter(double ang,MyPoint _d) 
{
	angle=ang; Cos=cos(angle); Sin=sin(angle);
	dx=_d.x;dy=_d.y;
	C1=-Cos*dx+Sin*dy+dx;
	C2=-Cos*dy-Sin*dx+dy;
	D1=-Cos*dx-Sin*dy+dx;
	D2=-Cos*dy+Sin*dx+dy;
}

RealPoint RotateFilter::ApplyForward(MyPoint x)
{
	RealPoint X;
	X.x=Cos*x.x-Sin*x.y+C1;
	X.y=Sin*x.x+Cos*x.y+C2;
	return X;
}

RealPoint RotateFilter::ApplyBackward(MyPoint X)
{
	RealPoint x;
	x.x=Cos*X.x+Sin*X.y+D1;
	x.y=-Sin*X.x+Cos*X.y+D2;
	return x;
}

RealPoint RotateFilter::ApplyForward(RealPoint x)
{
	RealPoint X;
	X.x=Cos*x.x-Sin*x.y+C1;
	X.y=Cos*x.y+Sin*x.x+C2;
	return X;
}

RealPoint RotateFilter::ApplyBackward(RealPoint X)
{
	RealPoint x;
	x.x=Cos*X.x+Sin*X.y+D1;
	x.y=Cos*X.y-Sin*X.x+D2;
	return x;
}

int RotateFilter::Main(BMPanvas *src)
{		
	BMPINFO TempInfo;
	
	if(!(angle)) return 1;
	
	BMPanvas &Buffer=*(src),tempBuffer(&Buffer);
	TempInfo=Buffer.INFO; TempInfo.bmiHeader.biBitCount=8;	
	tempBuffer.Create(&TempInfo); 
	tempBuffer.CreateGrayPallete();
	//tempBuffer.PatBlt(0, 0, tempBuffer.w,tempBuffer.h, WHITENESS);
	PatBlt(tempBuffer.GetDC(),0, 0, tempBuffer.w,tempBuffer.h, WHITENESS);
	Buffer.LoadBitmapArray(); tempBuffer.LoadBitmapArray(); 	
	
	if(src->ColorType==BMPanvas::BW) 
	{		
		BW_FloydSteinberg Filter2; Filter2.threshold=160; 
		MainGray(src,&tempBuffer); tempBuffer.SetBitmapArray(); Filter2.Main(&tempBuffer);
		//MainBW(src,&tempBuffer); tempBuffer.SetBitmapArray();
	}
	else 
	{
		MainGray(src,&tempBuffer);	    
		tempBuffer.SetBitmapArray();
	}
	
	tempBuffer.CopyTo(src,CPoint(0,0),tempBuffer.Rgn);
	return 0;
}

int RotateFilter::MainGray(BMPanvas *src, BMPanvas *dest)
{
	int x,y,W,H; RealPoint t; MyPoint CurPoint;
	double z1,z2,z3,z,dx,dy; int ty,ty1,dty1=Q*dest->wbyte;
	W=src->w; H=src->h;	

	for(CurPoint.y=0,ty1=0;CurPoint.y<H;CurPoint.y+=Q,ty1+=dty1)
	{
		for(CurPoint.x=0;CurPoint.x<W;CurPoint.x+=Q)
		{			
			t=ApplyForward(CurPoint);			
			if(t.x>=0 && t.x<=W-2 && t.y>=0 && t.y<=H-2)		
			{
				x=(int)t.x; y=(int)t.y; ty=y*src->wbyte;				
				z1=src->arr[ty+x];				
				z3=src->arr[ty+src->wbyte+(x+1)];
				dx=t.x-x; dy=t.y-y;
				if(dy<dx)
                {					
					z2=src->arr[ty+(x+1)];
					z=z1-(z1-z2)*dx-(z2-z3)*dy;
                }
				else
                {					
					z2=src->arr[ty+src->wbyte+(x)];
					z=z1-(z2-z3)*dx-(z1-z2)*dy;
                }				
				dest->arr[ty1+CurPoint.x]=(int)z;			
			}			
		}	
	}
    return ERR_OK;
}

int RotateFilter::MainBW(BMPanvas *src, BMPanvas *dest)
{
	int x,y,W,H; RealPoint t; MyPoint CurPoint;
	double z1,z2,z3,z,dx,dy; int ty,ty1,dty1=Q*dest->wbyte;
	W=src->w; H=src->h;	

	for(CurPoint.y=0,ty1=0;CurPoint.y<H;CurPoint.y+=Q,ty1+=dty1)
	{
		for(CurPoint.x=0;CurPoint.x<W;CurPoint.x+=Q)
		{			
			t=ApplyForward(CurPoint);			
			if(t.x>=0 && t.x<=W-2 && t.y>=0 && t.y<=H-2)		
			{
				x=(int)t.x; y=(int)t.y; ty=y*src->wbyte;				
				z1=src->arr[ty+x];				
				z3=src->arr[ty+src->wbyte+(x+1)];
				dx=t.x-x; dy=t.y-y;
				if(dy<dx)
                {					
					z2=src->arr[ty+(x+1)];
					z=z1-(z1-z2)*dx-(z2-z3)*dy;
                }
				else
                {					
					z2=src->arr[ty+src->wbyte+(x)];
					z=z1-(z2-z3)*dx-(z1-z2)*dy;
                }				
				dest->arr[ty1+CurPoint.x]=255*((int)(z/130.));						
			}			
		}	
	}

    return ERR_OK;
}