#if !defined(AFX_MYLISTBOX_H__6E4C072E_73DB_4FAE_B54A_B6DC0D3A2A37__INCLUDED_)
#define AFX_MYLISTBOX_H__6E4C072E_73DB_4FAE_B54A_B6DC0D3A2A37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// MyListBox window

class MyListBox : public CListBox
{
// Construction
public:
	MyListBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MyListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	int width;

	virtual ~MyListBox();
	int AddString(LPCTSTR s);
	void updateWidth(LPCTSTR s);
	void ResetContent();

	// Generated message map functions
protected:
	//{{AFX_MSG(MyListBox)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYLISTBOX_H__6E4C072E_73DB_4FAE_B54A_B6DC0D3A2A37__INCLUDED_)
