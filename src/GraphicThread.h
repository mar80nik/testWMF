#pragma once
#include "mythread.h"
#include "outputwnd.h"
#include "GausianBlur.h"

struct GraphModes 
{
	enum {NONE=0, FRG=1, BCKG=1<<1, BW=1<<2, BLUR=1<<3, ROTLINE=1<<4};
};

enum ErrorCodes {ERR_OK=0,ERR_BAD_STAGE=1<<1,ERR_MUTEX_LOCK=1<<2,ERR_MUTEX_UNLOCK=1<<3,
	ERR_SENSOR=1<<4, ERR_VOLT_VALUES_ARRAY=1<<5, ERR_THRD_ALRD_RUNNING=1<<6,
	ERR_THREAD_STOP=1<<7,ERR_STAGE_STOP=1<<8, ERR_GRAPHICS=1<<9};

class Message: public CSingleLock
{
public:	
    int m1,m2,m3;	
	//CMutex* obj;

	Message(CMutex* _obj): CSingleLock(_obj)  
	{
		m1=m2=m3=0; //obj=_obj;		
	};	
};

struct PrintMsg
{
    CArray<DjvuPic*,DjvuPic*> *Pics1;

};

enum GraphicMessages
{	
	UM_LOAD_PIC=WM_APP  + 176, UM_UPDATE,UM_UPDATE1, UM_NEXT_PIC, UM_PREV_PIC, UM_ORIGINAL,
	UM_UNLOAD_PIC, UM_BLUR, UM_ROTATE, UM_ON_INIT, UM_PRINT, UM_SELECT_ALL, UM_WAITING, UM_GET_PATH, UM_BW, 
	UM_PRINTERLOCK, UM_UPDATE_THREAD_LIST, UM_PRINT_DIALOG
};

class CTestWMFDlg;

class GraphicThread :	public MyThread
{
	friend class OutputWnd;
	DECLARE_DYNCREATE(GraphicThread)	
private:
	int MainFRG(Message*);
	int MainBCKG(Message*);
	int MainBW(Message*);
	int MainBlur(Message*);
	int MainRotLine(Message*);
	void DrawShape(AbstractGraphics *,BMPanvas* src,double zoom);
	void EraseShape(AbstractGraphics *,BMPanvas* src,CRect);
protected:
	CArray<AbstractGraphics*,AbstractGraphics*> Shapes;
	CArray<CRect,CRect> Eraser;

	virtual void Main(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual ~GraphicThread(void);
	virtual BOOL InitInstance();
	virtual int ExitInstance();	
public:
	OutputWnd OWnd1;	
	CWnd* ParentWnd;
	HWND ParentHWND;
	int bckg;
	HACCEL accel; bool StopWaiting; CString DJVUOutFldr;

	virtual int TerminateThread();
	GraphicThread(void);		

	afx_msg void OnLoadPic(WPARAM wParam, LPARAM lParam );	
	afx_msg void OnUnLoadPic(WPARAM wParam, LPARAM lParam );	
	afx_msg void OnOriginal(WPARAM wParam, LPARAM lParam );		
	afx_msg void OnBlur(WPARAM wParam, LPARAM lParam );		
	afx_msg void OnBW(WPARAM wParam, LPARAM lParam );		
	afx_msg void OnRotate(WPARAM wParam, LPARAM lParam );		
	afx_msg void OnUpdate(WPARAM wParam, LPARAM lParam );		
	afx_msg void OnPrint(WPARAM wParam, LPARAM lParam );	


	DECLARE_MESSAGE_MAP()
};
