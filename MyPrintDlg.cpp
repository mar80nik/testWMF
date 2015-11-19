// MyPrintDlg.cpp : implementation file
//

#include "stdafx.h"
#include "testWMF.h"
#include "MyPrintDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MyPrintDlg

IMPLEMENT_DYNAMIC(MyPrintDlg, CPrintDialog)

MyPrintDlg::MyPrintDlg(BOOL bPrintSetupOnly, DWORD dwFlags, CWnd* pParentWnd) :
	CPrintDialog(bPrintSetupOnly, dwFlags, pParentWnd)
{
}


BEGIN_MESSAGE_MAP(MyPrintDlg, CPrintDialog)
	//{{AFX_MSG_MAP(MyPrintDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL MyPrintDlg::OnInitDialog() 
{
	CPrintDialog::OnInitDialog();

	LPDEVMODE tt=GetDevMode();
	//(*tt)=(*tt1);

	//dlg1.m_pd.hDevMode=tt;

	tt->dmPelsWidth=333;
	tt->dmPelsHeight=777;

	tt->dmPaperWidth=(short)(254*tt->dmPelsWidth/300);
	tt->dmPelsHeight=(short)(254*tt->dmPelsHeight/300);


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
