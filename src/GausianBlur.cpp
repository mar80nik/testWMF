// GausianBlur.cpp: implementation of the GausianBlur class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "testWMF.h"
#include "GausianBlur.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GausianBlur::GausianBlur()
{
	Kernel=0;
}

GausianBlur::~GausianBlur()
{
	if(Kernel) delete[] Kernel;
}

void GausianBlur::SetKernel(double s,int N)
{
	sigma=s; KernelSize=N;
	if(!N) 
	{
		if(Kernel) delete[] Kernel;
		Kernel=0;
		return;
	}	
	KerSize2=KernelSize/2; double t,sum; int i,j;
	if(Kernel) delete[] Kernel;
	Kernel=new double[KernelSize];

	{
		sum=0;
		for(i=-KerSize2,j=0;i<=KerSize2;i++,j++)
		{
			t=(double)i/sigma; t*=t/2.;
			Kernel[j]=exp(-t); sum+=Kernel[j];
		}
		for(i=0;i<KernelSize;i++) Kernel[i]/=sum;	
	}
}

void GausianBlur::Main(BMPanvas *src, CRect rect)
{
	if(KernelSize) 
	{
		if(rect.IsRectEmpty()) rect=CRect(0,0,src->w,src->h);
		TwoDTransform(*src,rect);
	}
}

void GausianBlur::TwoDTransform(BMPanvas &canvas,CRect rect)
{	
	int i,j,h=canvas.h;
	BYTE **arr1; 
	canvas.LoadBitmapArray();

	for(i=rect.top;i<rect.bottom;i++) 
	{
		OneDTransform(canvas.arr+i*canvas.wbyte,rect.left,rect.right);	
	}

	arr1=new BYTE* [rect.Height()];	
	for(i=rect.left;i<rect.right;i++)
	{
		for(j=rect.top;j<rect.bottom;j++)
		{
			arr1[j-rect.top]=canvas.arr+(canvas.h-1-j)*canvas.wbyte+i;
		}			
		OneDTransform(arr1,0,rect.Height());	
	}
	
	canvas.SetBitmapArray();

	delete [] arr1;
}

void GausianBlur::OneDTransform(BYTE *arr,int min,int max)
{
	int size=max-min;
	if(size<KernelSize) return;
	int i,j,k; double sum;
	BYTE *arr1=new BYTE[size];

	for(i=min+KerSize2;i<max-KerSize2;i++)
	{
		sum=0;
		for(j=-KerSize2,k=0;j<=KerSize2;j++,k++)
		{
			sum+=arr[i+j]*Kernel[k];
		}
		arr1[i-min]=(BYTE)sum;
		
	}	
	for(i=min+KerSize2;i<max-KerSize2;i++)	arr[i]=arr1[i-min];
	delete[] arr1;
}

void GausianBlur::OneDTransform(BYTE **arr,int min,int max)
{
	int size=max-min;
	if(size<KernelSize) return;
	int i,j,k; double sum;
	BYTE *arr1=new BYTE[size];

	for(i=min+KerSize2;i<max-KerSize2;i++)
	{
		sum=0;
		for(j=-KerSize2,k=0;j<=KerSize2;j++,k++)
		{
			BYTE& t=*(arr[i+j]);
			sum+=t*Kernel[k];
		}
		arr1[i-min]=(BYTE)sum;
	}
	for(i=min+KerSize2;i<max-KerSize2;i++)	*(arr[i-min])=arr1[i-min];
		//*(arr[i-min])=0;
		
	delete[] arr1;
}