#if !defined(AFX_OUTPUTWND_H__EF93FDD3_92FE_492D_8A9F_8B32ADD73397__INCLUDED_)
#define AFX_OUTPUTWND_H__EF93FDD3_92FE_492D_8A9F_8B32ADD73397__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OutputWnd.h : header file
//
#include "bmpanvas.h"
#include "generic.h"
#include "djvupic.h"
/////////////////////////////////////////////////////////////////////////////
// OutputWnd window

#define ALL 0xFFFFFFFF



class OutputWnd : public CWnd
{
friend class GraphicThread;
	MyPoint CurPoint;
	BYTE counter;
	CPoint PicMove;
public:
	CDC CoordCanvas;
	OutputWnd();

	GraphicThread * Parent;
// Attributes
public:
	BMPanvas *buffer;	
	MyPoint LM0,LM1, MM0,MM1;
	MyRect Wnd, LastViewRgn, ActiveWnd;
	double zoom_max, zoom_min;	
	bool busy;	MyTimer Timer1;
	CEvent OutputWndLock;
	CBrush BckgBrush; CPen BckgPen;
	
	DjvuPic *CurPic;
	enum modes {IDLE,ZONE_MOVE,ZONE_SIZE, ZONE_SET, PIC_MOVE, ZOOM_IN, ZOOM_OUT, ROTATE_LINE,
		ZONE_MENU, DBLCLICK, FREE_SPACE_CLICK, CROP_MOVE};
	int SetMode(modes); FLAG DrawMode;

	Cross Cross1; Line RotateLine;
protected:
	modes mode; bool ChangeCursor; bool UpdateWndBckg;
	enum ZoomMode {Normal,FitAll} ZMode;
	double zoom;

// Operations
public:	
	void Clear();
	void LoadPic(DjvuPic* );
	void UnLoadPic();
	void Draw();
	void SetZoom(double ,ZoomMode , DjvuPic *);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OutputWnd)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	MyPoint ScreenToPic(CPoint& point);
	virtual ~OutputWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(OutputWnd)
	afx_msg void OnPaint();
	afx_msg BOOL OnMouseWheel(UINT nFlags,short zDelta,CPoint pt);	
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnZoneAttach();
	afx_msg void OnZoneDetach();
	afx_msg LRESULT OnStop(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMButtonUp( UINT nFlags, CPoint point);
	afx_msg void OnDestroy( );

	afx_msg  void OnPrint();
	afx_msg void OnSelectAll();	
	afx_msg void  OnOriginal();
	afx_msg void  OnZoomIn();
	afx_msg void  OnZoomOut();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);	
	void UpdateNow(void);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OUTPUTWND_H__EF93FDD3_92FE_492D_8A9F_8B32ADD73397__INCLUDED_)
