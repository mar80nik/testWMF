#pragma once


// MyPrintDialog

class MyPrintDialog : public CPrintDialog
{
	DECLARE_DYNAMIC(MyPrintDialog)

public:
	MyPrintDialog(BOOL bPrintSetupOnly,
			// TRUE for Print Setup, FALSE for Print Dialog
			DWORD dwFlags = PD_ALLPAGES | PD_USEDEVMODECOPIES | PD_NOPAGENUMS
				| PD_HIDEPRINTTOFILE | PD_NOSELECTION,
			CWnd* pParentWnd = NULL);
	virtual ~MyPrintDialog();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg  LRESULT OnThreadListUpdate(WPARAM wParam, LPARAM lParam );
};


