// NiblackFilter.h: interface for the NiblackFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NIBLACKFILTER_H__0602D1AD_52C9_4176_ACC3_7E6AFF249296__INCLUDED_)
#define AFX_NIBLACKFILTER_H__0602D1AD_52C9_4176_ACC3_7E6AFF249296__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "djvupic.h"
#include "my_timer.h"

class NiblackFilter  
{
public:
	double kk;
	MyTimer timer1;
	void Main(DjvuPic&);
	NiblackFilter();
	virtual ~NiblackFilter();

};

#endif // !defined(AFX_NIBLACKFILTER_H__0602D1AD_52C9_4176_ACC3_7E6AFF249296__INCLUDED_)
