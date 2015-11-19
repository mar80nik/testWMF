#pragma once
#include "afxcmn.h"
#include "MessageContainers.h"
#include "afxmt.h"
#include "generic.h"
#include "resource.h"
#include "djvupic.h"
#include "globalheader.h"

class CTestWMFDlg;

void GetRandomString(CString &T);

enum ThreadStatus {Error, Preparing, Working, Rename, Finished, Waiting };

enum MessageMasks {DONE_MSK=1<<0, STATUS_MSK=1<<1, TIME_MSK=1<<2,TIME_LEFT_MSK=1<<3};

class StatMessage: public AbstractMessage
{
public:
	int FLAGS;
	int ThreadPID, Done, Status;
	double Time, TimeLeft;

	StatMessage() {FLAGS=0; Done=0; Status=Error; Time=TimeLeft=-1;};
};

class ThreadParams
{
protected:
	MessagesInspector Inspector1;	
public:
	CEvent Terminator;
	CWinThread *Parent;
    bool StopWaiting;
	int PID, Status;
	CString Name;
	double Time, TimeLeft;
	CTime StartTime; MyTimer Timer1;

	ThreadParams();
	virtual ~ThreadParams() {};
	virtual void FormatElement(int subItem, CString&);
	virtual int Update(StatMessage*);
	void _Main();
	virtual void Main()=0;
	CString GetStatusText();
	void PostParentMessage(UINT Msg, StatMessage*);
};

class PrintDialogThreadParams: public ThreadParams
{
public:
	CPrintDialog *PrintDlg;

	PrintDialogThreadParams() {};
	virtual void Main();
	virtual ~PrintDialogThreadParams() {}
};

struct Limiter
{
	struct LimiterData 
	{
		int Total,Done,Working, Printed;
		LimiterData() {Total=Done=Working=Printed=0;}        
		bool Update(int _p, int _d)
		{
			if(_p>=0) Printed=_p;
			if(_d>=0) Done=_d;
            if((Printed-Done)>=5) return true;	
			else return false;
		}
	};

	CEvent semafor; CEvent terminator;
	ProtectedObject<LimiterData> Data;
	Limiter():semafor(true,true), terminator(false,true) {Data.WAIT_TIME=10000;};
	bool Update(int _p, int _d)
	{
        LimiterData *data; bool ret=false;
		if((data=Data.GainAcsess())!=NULL)
		{
			if(data->Update(_p,_d))
			{
				semafor.ResetEvent(); ret=true; 			
			}
			else semafor.SetEvent();
			Data.Unlock();
			return ret;
		}
		else {ASSERT(0); return false;}
	}
};

class PrintThreadParams: public ThreadParams
{
	int Printed;
public:
	CString *Path; //CString RandomText;
	DjvuPicArray *Pics;
	int PicsN, Done;
	Limiter Limit1;

    PrintThreadParams();
	virtual void FormatElement(int subItem, CString&);
	virtual int Update(StatMessage*);
	virtual void Main();
	virtual ~PrintThreadParams() {}
};

class PriorityThreadParams: public ThreadParams
{
public:
	PriorityThreadParams() {};
	virtual void Main();
	virtual ~PriorityThreadParams() {}
};

class RenameThreadParams: public PrintThreadParams
{
public:
	int Moved, Renamed;
	Limiter *Limit1;

	RenameThreadParams() {Moved=Renamed=0;};
	virtual void FormatElement(int subItem, CString&);
	virtual void Main();
	virtual ~RenameThreadParams() {}
};

class ThreadsArray: public CArray<ThreadParams*,ThreadParams*> 
{ 
public:
	ThreadParams* FindElement(int pid,int &n);    
};

enum ThreadsListColumns {Number_COL, Name_COL, Pics_COL, Done_COL, Start_COL,Time_COL, Left_COL, Status_COL};

class ThreadsList :
	public CListCtrl
{	
	int LastN;
public:
	CTestWMFDlg *Parent;

	ThreadsList(void);
	virtual ~ThreadsList(void);
	void CreateColumns(void);
	void UpdateList();
	void UpdateElement(int ListN,int ArrayN=-1);
	int FindElement(int pid);

	DECLARE_MESSAGE_MAP()
};
