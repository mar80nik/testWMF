#if !defined(AFX_MYTHREAD_H__192EEA8B_2C40_4C22_9F15_F8F602DC6038__INCLUDED_)
#define AFX_MYTHREAD_H__192EEA8B_2C40_4C22_9F15_F8F602DC6038__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyThread.h : header file
/////////////////////////////////////////////////////////////////////////////
//CRuntimeClass* prt = RUNTIME_CLASS(NewThread);
//NewThreadPointer=(NewThread*)AfxBeginThread(prt,THREAD_PRIORITY_NORMAL,0,0);	
//NewThreadPointer->Create(ParentThreadPointer,NewThreadID, ExternTerminateEventPointer) 
//...
//...
//NewThreadPointer->TerminateThread();
//Header DECLARE_DYNCREATE(NewThread)		
//Source IMPLEMENT_DYNCREATE(NewThread, MyThread)

UINT StartThreadFunction( LPVOID pParam );
UINT StopThreadFunction( LPVOID pParam );

#include "my_timer.h"
#include <afxmt.h>
#include <afxtempl.h>

//#define WAIT_TIME 1000
#define AssistantID 123456

enum UserDefinedMessages
{	
	UM_EVENTLOG=WM_USER + 76, UM_SENSOR_PREPARE,UM_SENSOR_OK,	
	UM_UPDATE_INDICATORS, UM_WAVELEN_SET, UM_COMUTATOR,
	UM_STAGE_UPDATE, UM_SENSOR_UPDATE,
	UM_STOP, UM_PAUSE, UM_CONTINUE, UM_START, UM_EXIT_THREAD, UM_OSCIL,
	UM_BACKUP_SAVE, UM_UPDATE_CONFIG, UM_ADD, UM_DATA_UPDATE
};

enum ErrorCodes {ERR_OK=0,ERR_BAD_STAGE=1<<1,ERR_MUTEX_LOCK=1<<2,ERR_MUTEX_UNLOCK=1<<3,
				ERR_SENSOR=1<<4, ERR_VOLT_VALUES_ARRAY=1<<5, ERR_THRD_ALRD_RUNNING=1<<6,
				ERR_THREAD_STOP=1<<7,ERR_STAGE_STOP=1<<8, ERR_GRAPHICS=1<<9};

enum StopState {None=0, ToDo, Done};

class MyThread;
struct StartParams 
{
	WPARAM wParam; LPARAM lParam; CWinThread *src, *dest;
	StartParams() {wParam=lParam=0; src=dest=NULL;};
	StartParams(WPARAM wP, LPARAM lP, CWinThread* _dest): wParam(wP), lParam(lP), dest(_dest) 
	{
		src=AfxGetThread();
	};
};

/////////////////////////////////////////////////////////////////////////////
class MyThread : public CWinThread
{
	friend UINT StartThreadFunction( LPVOID pParam);
	friend UINT StopThreadFunction( LPVOID pParam);

	DECLARE_DYNCREATE(MyThread)		
protected:
	enum _ThreadState {Suspended, Resumed} ThreadState;	
	CMutex RunMutex; CEvent *TerminateEvent;	
	CSingleLock ThreadLock_Run;
	int ret;		
	StartParams startParam1;

	CWinThread* Parent;
	CString Name; int PersonalID;		

	MyThread();           // protected constructor used by dynamic creation	
	virtual int BeforeExecute();
	virtual void AfterExecute(int);		
	void MyResume();
	void MySuspend();	
	virtual int Main(WPARAM wParam, LPARAM lParam);
	
public:		
	enum _ThreadStatus {Stoped=0, Paused, Running, Error} ThreadStatus;
	StopState Stop, Terminate;	MyTimer Timer1; int	WAIT_TIME;
	int pass_In, pass_Out;
	
	virtual void Create(CWinThread* parent,int ID, CEvent* evnt);	
	virtual void ContinueThread();	
	virtual void PauseThread();	
	virtual void StartThread(WPARAM,LPARAM);
	virtual void StopThread();	
	virtual int TerminateThread();
	
	friend CString GetString(enum MyThread::_ThreadState a);
	friend CString GetString(enum MyThread::_ThreadStatus a);

	CWnd* FromHandle(HWND handle);
	CDC* FromHandle(HDC handle);
	CBitmap* FromHandle(HBITMAP handle);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MyThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~MyThread();

	// Implementation
	//{{AFX_MSG(MyThread)	
	afx_msg void OnExitThread(WPARAM wParam, LPARAM lParam );	
	afx_msg void OnStart(WPARAM wParam, LPARAM lParam );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYTHREAD_H__192EEA8B_2C40_4C22_9F15_F8F602DC6038__INCLUDED_)
