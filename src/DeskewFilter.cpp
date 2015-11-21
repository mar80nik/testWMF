// DeskewFilter.cpp: implementation of the DeskewFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "testWMF.h"
#include "DeskewFilter.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DeskewFilter::DeskewFilter()
{
	SinA=CosA=0;Accumulator=0; dA=0;Amin=Amax=0; edges=false;
}

DeskewFilter::~DeskewFilter()
{
	if(SinA) delete[] SinA;
	if(CosA) delete[] CosA;
	if(Accumulator) delete[] Accumulator;
}

void DeskewFilter::SetupFilter(double amin, double amax, double da,int el)
{
	if(Amin==amin && Amax==amax && dA==da) return;

	Amin=amin; Amax=amax; dA=da;
	if(SinA) delete[] SinA; edge_level=el;
	if(CosA) delete[] CosA;

	N=(int)(fabs(Amax-Amin)/dA); N++;
	int i,k; double t;
	SinA=new double[N];CosA=new double[N];
	for(i=0,k=0,t=Amin;i<N;i++,t+=dA)
	{
		SinA[i]=sin(t); CosA[i]=cos(t);	k++;
	}	
}

struct pXY {unsigned short x,y;};

double DeskewFilter::Main(BMPanvas *src)
{
	double t11,t12; t11=t12=0;
	BMPINFO TempInfo; BMPanvas &Buffer=*src,tempBuffer;
	TempInfo=Buffer.INFO; TempInfo.bmiHeader.biBitCount=8;	
	tempBuffer.Create(Buffer.GetDC(), &TempInfo); 
	tempBuffer.CreateGrayPallete();
	tempBuffer.LoadBitmapArray(); //Buffer.LoadBitmapArray(); 

	int h=Buffer.h, w=Buffer.w, wbyte=Buffer.wbyte;
	Buffer.LoadBitmapArray(); 

	if(Accumulator) delete[] Accumulator; 
	Accumulator=0;

	int size=N*h;
	int i,j, r,t1,i1; int x1,x2,x3,x4,x5,x6,x7,x8,x9;
	Accumulator=new int[size]; memset(Accumulator,0,size*sizeof(int));
	pXY *Points=0;Points=new pXY[h*w]; BYTE *tr; int sum, PointsNum; pXY tP;

	timer1.Start(); PointsNum=0; BYTE *l1,*l2,*l3;	
	if(edges)
	{
		tempBuffer.LoadBitmapArray();
		for(tP.y=1;tP.y<h-1;tP.y++)
		{
			l1=Buffer.arr+1+(tP.y-1)*wbyte;
			l2=Buffer.arr+1+(tP.y)*wbyte;
			l3=Buffer.arr+1+(tP.y+1)*wbyte;
			for(tP.x=1;tP.x<w-1;tP.x++)
			{
				x1=*(l1-1);	x2=*(l1);	x3=*(l1+1); l1++;
				x4=*(l2-1);	x5=*(l2);	x6=*(l2+1); l2++;
				x7=*(l3-1);	x8=*(l3);	x9=*(l3+1); l3++;			
				sum=x1+x3+x7+x9-2*(x2+x4+x6+x8-x5-x5);			
				if(abs(sum)>edge_level) Points[PointsNum++]=tP;
				tr=tempBuffer.arr+tP.x+tP.y*wbyte;
				if(abs(sum)>edge_level) {*tr=0;}
				else {*tr=255;}
			}		
		}	
		tempBuffer.SetBitmapArray(); tempBuffer.CopyTo(&Buffer,CPoint(0,0),tempBuffer.Rgn);
	}
	else
	{
		for(tP.y=1;tP.y<h-1;tP.y++)
		{
			l1=Buffer.arr+1+(tP.y-1)*wbyte;
			l2=Buffer.arr+1+(tP.y)*wbyte;
			l3=Buffer.arr+1+(tP.y+1)*wbyte;
			for(tP.x=1;tP.x<w-1;tP.x++)
			{
				x1=*(l1-1);	x2=*(l1);	x3=*(l1+1); l1++;
				x4=*(l2-1);	x5=*(l2);	x6=*(l2+1); l2++;
				x7=*(l3-1);	x8=*(l3);	x9=*(l3+1); l3++;			
				sum=x1+x3+x7+x9-2*(x2+x4+x6+x8-x5-x5);			
				if(abs(sum)>edge_level) Points[PointsNum++]=tP;
			}		
		}	
	}
	if(PointsNum<2) return 90*DEGREE;
	for(j=0;j<PointsNum;j++) Points[j].y=h-1-Points[j].y;
	for(i=0;i<N;i++)	
	{
		t1=i*N;
		for(j=0;j<PointsNum;j++)
		{
			r=abs((int)(Points[j].x*CosA[i]+Points[j].y*SinA[i]));
			(Accumulator[r+t1])++;							
		}		
	}

	int *trr; int summ=0; i1=0; int fN=5,fdN=fN/2,k; int *first,*last;
	for(i=0;i<N;i++)
	{		
		sum=0; trr=Accumulator+i*N; 		
		for(k=-1;k<fN-1;k++) sum+=*(trr+k);
		first=trr-1; last=trr+k;
		do
		{
			sum+=*(last++); sum-=*(first++);
			if(sum>summ) {summ=sum;i1=i;} k++;			
		}
		while(k<h);
	}
	timer1.Stop(); //t12=timer1.GetValue();
	double a=Amin+i1*dA;	
	if(Accumulator) delete[] Accumulator; 
	if(Points) delete[] Points;
	Accumulator=0;	
	return a;
	
}
