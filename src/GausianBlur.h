// GausianBlur.h: interface for the GausianBlur class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAUSIANBLUR_H__1BA355B9_E8ED_4B7D_9F30_C3EBAF97A076__INCLUDED_)
#define AFX_GAUSIANBLUR_H__1BA355B9_E8ED_4B7D_9F30_C3EBAF97A076__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "djvupic.h"
#include "math.h"

class GausianBlur: public GausianBlurParam
{	
	int KerSize2;
	double *Kernel;
public:
	void TwoDTransform(BMPanvas& bmp,CRect rect);
	void OneDTransform(BYTE*,int,int);
	void OneDTransform(BYTE**,int,int);
	//void OneDTransformColor(BYTE*,int);
	//void OneDTransformColor(BYTE**,int);
	void SetKernel(double s,int N);		
	void Main(BMPanvas *src, CRect rect=CRect(0,0,0,0));
	GausianBlur();
	virtual ~GausianBlur();

};

#endif // !defined(AFX_GAUSIANBLUR_H__1BA355B9_E8ED_4B7D_9F30_C3EBAF97A076__INCLUDED_)
