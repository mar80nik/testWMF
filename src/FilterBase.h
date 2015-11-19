#pragma once

#include "djvupic.h"
#include "my_timer.h"
class FilterBase
{
public:
    MyTimer Timer1;
	CString Name;

	FilterBase(void);
	~FilterBase(void);
	virtual int Main(BMPanvas* src)=0;
	
};
