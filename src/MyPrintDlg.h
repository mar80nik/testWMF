#if !defined(AFX_MYPRINTDLG_H__77D14B8E_D254_44BA_9D96_890E10BDEF44__INCLUDED_)
#define AFX_MYPRINTDLG_H__77D14B8E_D254_44BA_9D96_890E10BDEF44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyPrintDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// MyPrintDlg dialog

class MyPrintDlg : public CPrintDialog
{
	DECLARE_DYNAMIC(MyPrintDlg)

public:
	MyPrintDlg(BOOL bPrintSetupOnly,
		// TRUE for Print Setup, FALSE for Print Dialog
		DWORD dwFlags = PD_ALLPAGES | PD_USEDEVMODECOPIES | PD_NOPAGENUMS
			| PD_HIDEPRINTTOFILE | PD_NOSELECTION,
		CWnd* pParentWnd = NULL);

protected:
	//{{AFX_MSG(MyPrintDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYPRINTDLG_H__77D14B8E_D254_44BA_9D96_890E10BDEF44__INCLUDED_)
