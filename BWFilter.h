// BWFilter.h: interface for the BWFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BWFILTER_H__472C5812_6AC0_40D5_87D4_182247998AED__INCLUDED_)
#define AFX_BWFILTER_H__472C5812_6AC0_40D5_87D4_182247998AED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "filterbase.h"
//#include "djvupic.h"
//#include "my_timer.h"

class BWFilter: public FilterBase
{
public:
	BYTE threshold;

	BWFilter();
	virtual ~BWFilter();
	virtual int Main(BMPanvas* src);
};

class BW_FloydSteinberg: public BWFilter
{
	double c23,c31,c32,c33;
public:

	BW_FloydSteinberg();
	virtual ~BW_FloydSteinberg() {};
	virtual int Main(BMPanvas* src);
};


#endif // !defined(AFX_BWFILTER_H__472C5812_6AC0_40D5_87D4_182247998AED__INCLUDED_)
