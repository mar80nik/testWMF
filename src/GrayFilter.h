#pragma once

#include "filterbase.h"
#include "MyTime.h"
#include "djvupic.h"
#include "bmpanvas.h"

class GrayFilter: public FilterBase
{
public:
	GrayFilter(void);
	~GrayFilter(void);

	virtual int Main2(DjvuPic*);	
};
