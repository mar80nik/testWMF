#ifndef GlobalHeaderH
#define GlobalHeaderH

#include <afxmt.h>
#include <afxtempl.h>
#include <math.h>

enum ThreadsID
{	
	MainTaskID =101, MainAppID,
	MVThreadID=201, MVThreadID1, MVThreadID2,
	TThreadID=301, TThreadID1, TThreadID2,
	OscilThreadID=401, ChipID=501, ChipDrvID=601,
	UART_ReciverID=701,

};

enum UserDefinedMessages1
{	
	UM_MESSAGE=WM_USER+200, UM_MSG_OUT, UM_NOTIFY, UM_LAST_MESSAGE1
};

enum UserDefinedMessages2
{	
	UM_SERIES_UPDATE=UM_LAST_MESSAGE1+1, UM_LAST_MESSAGE2
};

struct IndicatorsText {CString device, running, paused;};

void GetErrorText(int err_code,CStringArray &strings);

/*
struct PerfomanceStaff
{
	MyTimer Timer1;
	bool LogEvents;
	int pass_num;

	PerfomanceStaff() {LogEvents=false;pass_num=0;}	
};
*/
union FloatInt
{
	float flt;
	int Int;
};

int MyRound(double x);

class SystemConfig; struct Wavelen; struct WavelenPica; struct WavelenSpeed; struct WavelenNanoSpeed;

struct WavelenParent {static SystemConfig* cfg;};

struct WavelenNano: public WavelenParent
{
protected:
	double l;
public:
	WavelenNano(double _l=0) {l=_l;};
	WavelenNano& operator= (double _l) {l=_l; return (*this);};
	WavelenNano& operator= (Wavelen&);
	WavelenNano& operator= (WavelenPica&);
	operator double() const {return l;}
};

struct WavelenPica: public WavelenParent
{
protected:
	double l;
public:
	WavelenPica(double _l=0) {l=_l;};
	WavelenPica& operator= (double _l) {l=_l; return (*this);};
	WavelenPica& operator= (WavelenNano&);
	WavelenPica& operator= (Wavelen&);
	operator double() const {return l;}
};

struct WavelenNanoSpeed: public WavelenParent
{
protected:
	double lps;
public:
	WavelenNanoSpeed(double _l=0) {lps=_l;};
	WavelenNanoSpeed& operator= (double _l) {lps=_l;return (*this);};
	WavelenNanoSpeed& operator= (WavelenSpeed&);
	virtual void Serialize(CArchive &ar);
	operator double() const {return lps;}
};

struct WavelenSpeed: public WavelenParent
{
protected:
	int lps;
public:
	WavelenSpeed(int _l=0) {lps=_l;};
	WavelenSpeed(WavelenNanoSpeed _s);
	WavelenSpeed& operator= (int _l) {lps=_l; return (*this);};
	WavelenSpeed& operator= (WavelenNanoSpeed&);
	operator int() {return lps;}
};

struct Wavelen: public WavelenParent
{
protected:
	int l;
public:
	Wavelen(int _l=0) {l=_l;};
	Wavelen& operator= (int _l) {l=_l; return (*this);};
	Wavelen& operator= (WavelenNano&);
	Wavelen& operator= (WavelenPica&);

	Wavelen operator-(Wavelen& _l) {Wavelen ret; ret.l=l-_l.l; return ret;}
	Wavelen operator+(Wavelen& _l) {Wavelen ret; ret.l=l+_l.l; return ret;}
	int operator/(Wavelen& _l) {int ret; ret=MyRound((double)l/_l.l); return ret;}
	double operator/(WavelenSpeed _s) {double ret; ret=(double)l/_s; return ret;}
	Wavelen operator*(int m) {Wavelen ret; ret.l=l*m; return ret;}
	Wavelen operator*(double m) {Wavelen ret; ret.l=MyRound(l*m); return ret;}
	void operator+=(Wavelen& _l) {l+=_l.l; return;}
	
	bool operator!=(Wavelen& _l) {return (l!=_l.l);}	
	bool operator>(Wavelen& _l) {return (l>_l.l);}
	bool operator>=(Wavelen& _l) {return (l>=_l.l);}
	bool operator<(Wavelen& _l) {return (l<_l.l);}
	bool operator==(int _l) {return (l==_l);}
	bool operator!=(int _l) {return (l!=_l);}

	static Wavelen Abs(Wavelen& _l) {Wavelen ret; ret.l=abs(_l.l); return ret;}
	virtual void Serialize(CArchive &ar);
	void MakeMultipleTo(Wavelen& _l) {l=_l.l*(l/_l.l); return;}
	operator int() const {return l;};
};

template <class T>
class ProtectedObject
{
	CMutex WriteProtector;
protected:
	T obj;
public:
	int WAIT_TIME;

	ProtectedObject() {WAIT_TIME=1000;}
	virtual ~ProtectedObject() {};
	T* GainAcsess()
	{
		T* ret=NULL;
		if(WriteProtector.Lock(WAIT_TIME))
		{
			ret=&obj;
		}
		return ret;
	}
	BOOL Unlock() {return WriteProtector.Unlock();};
};

void CreateBufferTemplate(BMPINFO& INFO);

class LoopArrayElement 
{	
public:	
	LoopArrayElement *prev,*next; 

	int PID;
	LoopArrayElement() {prev=next=NULL;};
	virtual ~LoopArrayElement() {};
};

typedef CArray<LoopArrayElement*,LoopArrayElement*> LAEArray;

template <class T>
class LoopArray: public LAEArray
{
public:
	T* GetNextBuf(T* _elmnt)
	{
		if(_elmnt==NULL) return NULL;
		return (T*)(_elmnt->next);
	}
	int Add(T* _elmnt)
	{
		int ret;
		ret=LAEArray::Add(_elmnt); 
		_elmnt->PID=ret;
		if(ret>0)
		{
			_elmnt->prev=(*this)[ret-1];
			_elmnt->next=(*this)[0];	
			_elmnt->prev->next=_elmnt->next->prev=_elmnt;
		}
		else
		{
			_elmnt->next=_elmnt->prev=_elmnt;	
		}
		return ret;    
	}
	void RemoveAll()
	{		
		for(int i=0;i<GetSize();i++)
		{
			delete (*this)[i];
		}
		LAEArray::RemoveAll();	
	}
	~LoopArray()
	{
		RemoveAll();
	}
	T* operator[](int i)
	{
		return 	(T*)(LAEArray::operator [](i));
	}
};

#endif

