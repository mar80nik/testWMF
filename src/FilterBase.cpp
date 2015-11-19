#include "stdafx.h"
#include ".\filterbase.h"

FilterBase::FilterBase(void)
{
	Name="NoName Filter";
}

FilterBase::~FilterBase(void)
{
}

/*
int FilterBase::Main(DjvuPic* CurPic)
{
	Timer1.Start();
	int ret=Main2(CurPic);
	Timer1.Stop();
	return ret;
}
*/