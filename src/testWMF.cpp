// testWMF.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "testWMF.h"
#include "testWMFDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

GraphicThread *Graph;
MessagesInspector GlobalInspector;
MessagesInspector* MessagesInspectorSubject::GlobalInspector=&::GlobalInspector;
WindowAddress LogMessage::LogWindow;
WindowAddress MyThread::ConfigParentWindow;

/////////////////////////////////////////////////////////////////////////////
// CTestWMFApp
CEvent Terminator1(true);

BEGIN_MESSAGE_MAP(CTestWMFApp, CWinApp)
	//{{AFX_MSG_MAP(CTestWMFApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
	ON_THREAD_MESSAGE(UM_UPDATE,OnUpdate)
	ON_THREAD_MESSAGE(UM_NEXT_PIC,OnNextPic)
	ON_THREAD_MESSAGE(UM_PREV_PIC,OnPrevPic)
	ON_THREAD_MESSAGE(UM_ON_INIT,OnInit)
	ON_THREAD_MESSAGE(UM_PRINT,OnPrint)	
	ON_THREAD_MESSAGE(UM_WAITING,OnWaiting)	
	ON_THREAD_MESSAGE(UM_GET_PATH,OnGetPath)	
	ON_THREAD_MESSAGE(UM_UPDATE1,OnUpdate1)	
	ON_THREAD_MESSAGE(UM_PRINTERLOCK,OnPrinterLock)	
	ON_THREAD_MESSAGE(WM_COMMAND,OnCommand)		
	ON_THREAD_MESSAGE(UM_UPDATE_THREAD_LIST, OnThreadListUpdate)
	ON_THREAD_MESSAGE(UM_PRINT_DIALOG, OnPrinterDialog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestWMFApp construction

CTestWMFApp::CTestWMFApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	Graph=0;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTestWMFApp object

CTestWMFApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTestWMFApp initialization

BOOL CTestWMFApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
//	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
//	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CTestWMFDlg dlg;
	m_pMainWnd = &dlg;

	CRuntimeClass* prt = RUNTIME_CLASS(GraphicThread);
	Graph=(GraphicThread*)AfxBeginThread(prt,THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);		
	Graph->Create(this,789,&Terminator1);	

	LogMessage::LogWindow.pWND = &dlg;
	MyThread::ConfigParentWindow.pWND = &dlg;

	accel=LoadAccelerators(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_ACCELERATOR1));

	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	Graph->TerminateThread();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}


BOOL CTestWMFApp::PreTranslateMessage(MSG* pMsg) 
{	
	TranslateAccelerator( m_pMainWnd->m_hWnd, accel,pMsg);
	
	return CWinApp::PreTranslateMessage(pMsg);
}

void CTestWMFApp::OnUpdate(WPARAM wParam, LPARAM lParam )
{
	CTestWMFDlg *dlg1=(CTestWMFDlg *)m_pMainWnd;

	if(Graph->OWnd1.CurPic)
	{
		dlg1->angle=Graph->OWnd1.CurPic->CenterRotate.angle; 	
	}
	else
	{
		dlg1->angle=0;	
		dlg1->OriginalCheck.EnableWindow(false);		
	}

	dlg1->bckg=Graph->bckg;

	dlg1->UpdateData(0);
	//dlg1->FileList1.UpdateList();    	
}
 
void CTestWMFApp::OnNextPic(WPARAM wParam, LPARAM lParam )
{
	CTestWMFDlg *dlg1=(CTestWMFDlg *)m_pMainWnd;
	dlg1->OnCommandNext();	
}
void CTestWMFApp::OnPrevPic(WPARAM wParam, LPARAM lParam )
{
	CTestWMFDlg *dlg1=(CTestWMFDlg *)m_pMainWnd;
	dlg1->OnCommandPrev();	
}


void CTestWMFApp::OnInit(WPARAM wParam, LPARAM lParam )
{
}

void CTestWMFApp::OnPrint(WPARAM wParam, LPARAM lParam )
{
	CTestWMFDlg *dlg1=(CTestWMFDlg *)m_pMainWnd;
	dlg1->SendMessage(UM_PRINT,wParam,lParam);
}

void CTestWMFApp::OnWaiting(WPARAM wParam, LPARAM lParam )
{
	CTestWMFDlg *dlg1=(CTestWMFDlg *)m_pMainWnd;
	dlg1->SendMessage(UM_WAITING,wParam,lParam);
}

void CTestWMFApp::OnGetPath(WPARAM wParam, LPARAM lParam )
{
	CTestWMFDlg *dlg1=(CTestWMFDlg *)m_pMainWnd;
	dlg1->SendMessage(UM_GET_PATH,wParam,lParam);
}

void CTestWMFApp::OnUpdate1(WPARAM wParam, LPARAM lParam )
{
	CTestWMFDlg *dlg1=(CTestWMFDlg *)m_pMainWnd;
	dlg1->SendMessage(UM_UPDATE,wParam,lParam);
}

void CTestWMFApp::OnPrinterLock(WPARAM wParam, LPARAM lParam )
{
	CTestWMFDlg *dlg1=(CTestWMFDlg *)m_pMainWnd;
	dlg1->SendMessage(UM_PRINTERLOCK,wParam,lParam);
}

void CTestWMFApp::OnPrinterDialog(WPARAM wParam, LPARAM lParam )
{
	CTestWMFDlg *dlg1=(CTestWMFDlg *)m_pMainWnd;
	dlg1->SendMessage(UM_PRINT_DIALOG,wParam,lParam);
}


void CTestWMFApp::OnCommand(WPARAM wParam, LPARAM lParam )
{
	CTestWMFDlg *dlg1=(CTestWMFDlg *)m_pMainWnd;
    switch(wParam)
	{
	case ID_SELECT_ALL: dlg1->OnSelectAll(); break;
	case ID_ORIGINAL: dlg1->OnOriginal(); break;
	case ID_PRINT:	dlg1->OnButton1();break;
	}
}

void CTestWMFApp::OnThreadListUpdate(WPARAM wParam, LPARAM lParam )
{
	CTestWMFDlg *dlg1=(CTestWMFDlg *)m_pMainWnd;
	dlg1->PostMessage(UM_UPDATE_THREAD_LIST,wParam,lParam);
}