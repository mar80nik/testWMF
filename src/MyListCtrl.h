#if !defined(AFX_MYLISTCTRL_H__24186151_56AC_4961_A811_98221F8B79AD__INCLUDED_)
#define AFX_MYLISTCTRL_H__24186151_56AC_4961_A811_98221F8B79AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyListCtrl.h : header file
//
#include "djvupic.h"

/////////////////////////////////////////////////////////////////////////////
// MyListCtrl window

int CALLBACK CompareFunc1(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

class CTestWMFDlg;

class MyListCtrl : public CListCtrl
{
// Construction
public:
	MyListCtrl();

// Attributes
public:
	int LastN;
	LV_ITEM _ms_lvi;
	int SortStatus, SortMask;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MyListCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	int HitItem; CTestWMFDlg *Parent;

	void UpdateList();
	virtual ~MyListCtrl();
	int HitTest(CPoint point);

	void SetState(int num, int state);
	int GetState(int num);
	void GetSelectedItems(DjvuPicArray*);

	// Generated message map functions
protected:
	//{{AFX_MSG(MyListCtrl)	
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);	
//	afx_msg void OnGetCropZone();
//	afx_msg void OnGetCropMaxW();
//	afx_msg void OnGetCropMaxH();
//	afx_msg void OnGetCropMaxWH();
//	afx_msg void OnClearCropZone();
	afx_msg void OnSaveNamesList();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	void SelectAll(void);
	void CreateColumns(void);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYLISTCTRL_H__24186151_56AC_4961_A811_98221F8B79AD__INCLUDED_)
