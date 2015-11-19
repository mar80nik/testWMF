// NiblackFilter.cpp: implementation of the NiblackFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "testWMF.h"
#include "NiblackFilter.h"
#include "math.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NiblackFilter::NiblackFilter()
{

}

NiblackFilter::~NiblackFilter()
{

}

void NiblackFilter::Main(DjvuPic &CurPic)
{
/*
	timer1.Start();
	CurPic.Original->LoadBitmapArray(); 
	CurPic.frg->LoadBitmapArray();
	int w=CurPic.Original->w, h=CurPic.Original->h, x,y, wbyte=CurPic.Original->wbyte;
	BYTE **l,*ll, **front,**tail; int dN=5, dN2=dN/2,dNdN=dN*dN, k,m;	
	l=new BYTE* [dN]; front=new BYTE* [dN]; tail=new BYTE* [dN]; int t3;
	CurPic.frg->PatBlt(0, 0, w, h, WHITENESS); int sum,sum2,mean,mean2; BYTE t1,t2; double thrsh;
	
	for(y=dN2;y<h-dN2;y++)
	{
		l[0]=CurPic.Original->arr+dN2+(y-dN2)*wbyte;		
		for(k=1;k<dN;k++) l[k]=l[k-1]+wbyte;
		ll=CurPic.frg->arr+dN2+y*wbyte;

		sum=sum2=0;
		for(k=0;k<dN;k++)
		{
			for(m=-dN2;m<=dN2;m++) 
			{
				t1=*(l[k]+m-1); 
				sum+=t1; sum2+=t1*t1;		
			}		
		}

		for(k=0;k<dN;k++) {tail[k]=l[k]-dN2-1; front[k]=tail[k]+dN;}

		for(x=dN2;x<w-dN2;x++)
		{
			for(k=0;k<dN;k++)
			{
				t1=*(front[k]); t2=*(tail[k]);
				sum+=t1-t2;			
				sum2+=t1*t1-t2*t2;	
				(front[k])++; (tail[k])++;
			}
			mean=(int)(sum/dNdN); mean2=(int)(sum2/dNdN);
			thrsh=mean-kk*sqrt((double)(mean2-mean*mean));	
			//thrsh=mean;
			t3=*(l[dN2]);
			//if(t3 <= (int)(thrsh)) *ll=0; else *ll=255;
			if(thrsh<127) *ll=0; else *ll=255;
			ll++;
		}		
	}
	CurPic.frg->SetBitmapArray();
	timer1.Stop();

	if(l) delete[] l; if(front) delete[] front; if(tail) delete[] tail;
	*/
}
