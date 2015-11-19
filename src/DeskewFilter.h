// DeskewFilter.h: interface for the DeskewFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DESKEWFILTER_H__7B133EF7_AE94_45EA_9D80_49FF27C4D7BB__INCLUDED_)
#define AFX_DESKEWFILTER_H__7B133EF7_AE94_45EA_9D80_49FF27C4D7BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "djvupic.h"
#include "my_timer.h"

class DeskewFilter  
{
public:
	DeskewFilter();
	double Amin,Amax,dA; int N;
	double *SinA, *CosA; int *Accumulator;
	MyTimer timer1; BOOL edges; int edge_level;

public:
	void SetupFilter(double,double,double,int);
	double Main(BMPanvas *src);

	virtual ~DeskewFilter();

};

#endif // !defined(AFX_DESKEWFILTER_H__7B133EF7_AE94_45EA_9D80_49FF27C4D7BB__INCLUDED_)
