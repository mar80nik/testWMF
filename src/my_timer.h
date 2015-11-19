//---------------------------------------------------------------------------
#ifndef my_timerH
#define my_timerH
//---------------------------------------------------------------------------

CString ConvTimeToStr(double time);

struct MyTimer
{
LARGE_INTEGER start, stop;
double freq;
LARGE_INTEGER _checkPoints[100];
BYTE _checkPointsID[100];
int _checkPointsNumber;

MyTimer();
void Start();
void Stop();
//void StartTrace(int ID);
//void SetCheckPoints(int ID);
double GetValue();
CString GetString(double dim=1e-6);
CString GetStringF(double dim=1e-6);
};
#endif
