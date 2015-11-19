#include "stdafx.h"
#include "my_timer.h"
#include "math.h"


MyTimer::MyTimer() {QueryPerformanceFrequency(&start);freq=(double)start.u.LowPart;}
void MyTimer::Start() {QueryPerformanceCounter(&start);stop=start;}
void MyTimer::Stop() {QueryPerformanceCounter(&stop);}

double MyTimer::GetValue()
    {double output;
    if(stop.u.HighPart!=start.u.HighPart) {output=-1.;return output;}
	output=(stop.u.LowPart-start.u.LowPart)/freq;
    return output;
    }
CString MyTimer::GetString(double dim)
    {
    CString output;
    if(stop.u.HighPart!=start.u.HighPart) output="OverFlow";
    int i=(int)(((stop.u.LowPart-start.u.LowPart)/freq)/dim);
	output.Format("%d s",i);
    return output;
    }

CString MyTimer::GetStringF(double dim)
    {
    CString output; double value;
    if(stop.u.HighPart!=start.u.HighPart) output="OverFlow";
    value=(stop.u.LowPart-start.u.LowPart)/freq;	
    return ConvTimeToStr(value);
    }

CString ConvTimeToStr(double time)
{
	int hour,min,sec,msec; double t;
	CString T, Out;

	if(time<0) {Out="???"; return Out;}

	modf(time/3600., &t); 
	time=fmod(time,3600.); 
	hour=(int)t;
	modf(time/60., &t); 
	time=fmod(time,60.);
	min=(int)t;
	msec=(int)(1e3*modf(time,&t)); 
	sec=(int)t;
	
	if(hour) {T.Format("%d h ",hour); Out+=T;}
	if(min)	{T.Format("%d m ",min); Out+=T;}
	if(sec) {T.Format("%d s ",sec); Out+=T;}	
	if(!hour && !min ) {T.Format("%d ms",msec); Out+=T;}
	return Out;
}
