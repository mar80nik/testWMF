// RotateFilter.h: interface for the RotateFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROTATEFILTER_H__814530A7_4CF3_4133_94AF_03D5B814A398__INCLUDED_)
#define AFX_ROTATEFILTER_H__814530A7_4CF3_4133_94AF_03D5B814A398__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "generic.h"
#include "math.h"
#include "djvupic.h"

class RotateFilter : public RotateFilterParam
{
	double Cos, Sin, dx,dy;
	double C1,C2,D1,D2; 
	
	inline RealPoint ApplyForward(MyPoint CurPoint);
	inline RealPoint ApplyBackward(MyPoint CurPoint);	
	inline RealPoint ApplyForward(RealPoint CurPoint);
	inline RealPoint ApplyBackward(RealPoint CurPoint);	
	int MainGray(BMPanvas *src, BMPanvas *dest);
	int MainBW(BMPanvas *src, BMPanvas *dest);
public:
	int Main(BMPanvas *src);
	RotateFilter();
	RotateFilter(RotateFilterParam& src);
	void SetFilter(double ang,MyPoint d);
	void SetQuality(Quality q) {Q=q;}
	virtual ~RotateFilter();

};

#endif // !defined(AFX_ROTATEFILTER_H__814530A7_4CF3_4133_94AF_03D5B814A398__INCLUDED_)
