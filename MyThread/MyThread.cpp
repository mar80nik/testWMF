// MyThread.cpp : implementation file
//

#include "stdafx.h"
#include "MyThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MyThread
IMPLEMENT_DYNCREATE(MyThread, CWinThread)

BEGIN_MESSAGE_MAP(MyThread, CWinThread)
	//{{AFX_MSG_MAP(VoltmeterThread)
	ON_THREAD_MESSAGE(UM_EXIT_THREAD,OnExitThread)	
	ON_THREAD_MESSAGE(UM_START,OnStart)			
	//}}AFX_MSG_MAP
	// Standard file based document commands	
END_MESSAGE_MAP()

//---------------------------------------------
MyThread::MyThread(): ThreadLock_Run(&RunMutex)
{	
	ThreadStatus=Stoped; Stop=Done;		
	Name="MyThread"; ThreadState=Suspended; 	
	TerminateEvent=0;  WAIT_TIME=1000; pass_In=pass_Out=0;
}
MyThread::~MyThread() {}

void MyThread::Create(CWinThread* parent,int ID, CEvent* evnt) 
{
	Parent=parent; PersonalID=ID; TerminateEvent=evnt;	
}

BOOL MyThread::InitInstance()
{	
	ThreadState=Resumed; Terminate=None;
	m_bAutoDelete=true; ThreadStatus=Stoped; Stop=Done;		
	CSingleLock T(TerminateEvent);	T.Lock(WAIT_TIME);			
	return TRUE;
}

int MyThread::ExitInstance()
{
	Terminate=Done;
	int t=CWinThread::ExitInstance();
	TerminateEvent->SetEvent();
	return t;
}

void MyThread::MySuspend()
{
	if(ThreadState==Suspended) return;
	ThreadState=Suspended;
	CWinThread::SuspendThread();
}

void MyThread::MyResume()
{
	if(ThreadState==Resumed) return;
	ThreadState=Resumed;
	CWinThread::ResumeThread();
}
//-------------------------------------------------
int MyThread::TerminateThread()
{
	CEvent *TE=TerminateEvent; int ret;
	{
		Stop=ToDo;
		CSingleLock TempLock(&RunMutex); 		
		if(TempLock.Lock(WAIT_TIME)) ret=0;
		else ret=1;
	}
	Terminate=ToDo;
	PostThreadMessage(UM_EXIT_THREAD,0,0);		

	CSingleLock T(TE);
	if(T.Lock(WAIT_TIME)) ret=0;	
	else ret=1;
	return ret;
}


int MyThread::BeforeExecute()
{
	ThreadLock_Run.Lock();	pass_In++;
	ThreadStatus=Running;  Stop=None;	
	Timer1.Start(); 
	Parent->PostThreadMessage(UM_START,PersonalID,ERR_OK);	
	return ERR_OK;
}
int MyThread::Main(WPARAM wParam, LPARAM lParam) {return ret;}

void MyThread::AfterExecute(int ret)
{
	Timer1.Stop(); 
	ThreadStatus=Stoped;
	ThreadLock_Run.Unlock(); pass_Out++;		
	Parent->PostThreadMessage(UM_STOP,PersonalID,ret);		
}

void MyThread::PauseThread()
{	
	ThreadStatus=Paused;	
	Parent->PostThreadMessage(UM_PAUSE,PersonalID,0);	
	MySuspend();	
}

void MyThread::ContinueThread()
{
	MyResume();
	ThreadStatus=Running;  Stop=None;	
	Parent->PostThreadMessage(UM_CONTINUE,PersonalID,0);	
}


void MyThread::StopThread() {Stop=ToDo; AfxBeginThread(StopThreadFunction,this);}

UINT StopThreadFunction( LPVOID pParam )
{
	MyThread *T=(MyThread*)pParam; int ret=ERR_OK;
	
	CSingleLock TempLock(&T->RunMutex);
	if(!(TempLock.Lock(T->WAIT_TIME))) 
	{
		ret|=ERR_THREAD_STOP;
		T->Parent->PostThreadMessage(UM_STOP,AssistantID,ret);
	}
	else
	{
		ret|=T->ret;	
	}
    return ret;
}

void MyThread::StartThread(WPARAM wParam,LPARAM lParam) 
{
	//startParam1.thread=this; startParam1.wParam=wParam; startParam1.lParam=lParam;
	StartParams *t=new StartParams(wParam,lParam,this);
	AfxBeginThread(StartThreadFunction,t);
}

UINT StartThreadFunction( LPVOID pParam )
{
	StartParams *T=(StartParams*)pParam; int ret=ERR_OK;
	MyThread *thread=(MyThread *)(T->dest);
	CSingleLock TempLock(&thread->RunMutex);
	if(!(TempLock.Lock(thread->WAIT_TIME))) 
	{
		ret|=ERR_THRD_ALRD_RUNNING;
		T->src->PostThreadMessage(UM_START,AssistantID,ret);
	}	
	else
	{
		TempLock.Unlock();
		T->dest->PostThreadMessage(UM_START,T->wParam,T->lParam);
	}
	delete T;
    return ret;
}

void MyThread::OnStart(WPARAM wParam, LPARAM lParam )
{	
	if(!(ret=BeforeExecute())) ret|=Main(wParam, lParam); 
	else 
	{
        ASSERT(0);	
	}
	AfterExecute(ret);		
}

void MyThread::OnExitThread(WPARAM wParam, LPARAM lParam ) {PostQuitMessage(PersonalID);}

CString GetString(enum MyThread::_ThreadState a)
{
	CString T;
	switch (a)
	{
	case MyThread::Resumed: T="Resumed"; break;
	case MyThread::Suspended: T="Suspended"; break;	
	default: T="Unknown";
	}
	return T;
}

CString GetString(enum MyThread::_ThreadStatus a)
{
	CString T;
	switch (a)
	{
	case MyThread::Stoped: T="Stoped"; break;
	case MyThread::Paused: T="Paused"; break;
	case MyThread::Running: T="Running"; break;
	case MyThread::Error: T="Error"; break;
	default: T="Unknown";
	}
	return T;
}

CWnd* MyThread::FromHandle(HWND handle)
{
	if(handle==NULL) return NULL;
	return CWnd::FromHandlePermanent(handle);
}

CDC* MyThread::FromHandle(HDC handle)
{
	if(handle==NULL) return NULL;
	return CDC::FromHandle(handle);
}

CBitmap* MyThread::FromHandle(HBITMAP handle)
{
	if(handle==NULL) return NULL;
	return CBitmap::FromHandle(handle);
}
