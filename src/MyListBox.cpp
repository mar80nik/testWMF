// MyListBox.cpp : implementation file
//

#include "stdafx.h"
#include "MyListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MyListBox

MyListBox::MyListBox()
{
}

MyListBox::~MyListBox()
{
}


BEGIN_MESSAGE_MAP(MyListBox, CListBox)
	//{{AFX_MSG_MAP(MyListBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MyListBox message handlers
int MyListBox::AddString(LPCTSTR s)
{
	CString Message;
	CTime Time1; Time1=CTime::GetCurrentTime();	
	Message.Format("[%02d:%02d:%02d] %s",Time1.GetHour(),Time1.GetMinute(), Time1.GetSecond(),s);	
    int result = CListBox::AddString(Message);
    if(result < 0)
       return result;
    updateWidth(s);
	SetCurSel(GetCount()-1);
    return result;
   }

void MyListBox::updateWidth(LPCTSTR s)
    {
     CClientDC dc(this);

     CFont * f = CListBox::GetFont();
     dc.SelectObject(f);

     CSize sz = dc.GetTextExtent(s, _tcslen(s));
     sz.cx += 3 * ::GetSystemMetrics(SM_CXBORDER);
     if(sz.cx > width)
	 { /* extend */
	  width = sz.cx;
	  CListBox::SetHorizontalExtent(width);
	 } /* extend */
    }

void MyListBox::ResetContent()
    {
     CListBox::ResetContent();
     width = 0;
	 CListBox::SetHorizontalExtent(width);
    }