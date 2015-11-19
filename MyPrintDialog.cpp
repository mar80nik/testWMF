// MyPrintDialog.cpp : implementation file
//

#include "stdafx.h"
#include "testWMF.h"
#include "MyPrintDialog.h"


// MyPrintDialog

IMPLEMENT_DYNAMIC(MyPrintDialog, CPrintDialog)
MyPrintDialog::MyPrintDialog(BOOL bPrintSetupOnly, DWORD dwFlags, CWnd* pParentWnd) :
	CPrintDialog(bPrintSetupOnly, dwFlags, pParentWnd)
{
}

MyPrintDialog::~MyPrintDialog()
{
}


BEGIN_MESSAGE_MAP(MyPrintDialog, CPrintDialog)
	ON_MESSAGE(UM_UPDATE_THREAD_LIST,OnThreadListUpdate)	
END_MESSAGE_MAP()


LRESULT MyPrintDialog::OnThreadListUpdate(WPARAM wParam, LPARAM lParam )
{	
	return 0;
}


// MyPrintDialog message handlers

