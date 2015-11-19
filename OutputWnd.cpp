// OutputWnd.cpp : implementation file
//

#include "stdafx.h"
#include "testWMF.h"
#include "OutputWnd.h"
#include "bmpanvas.h"
#include "testwmfdlg.h"
#include "math.h"
#include "rotatefilter.h"
#include ".\outputwnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// OutputWnd

OutputWnd::OutputWnd(): LM0(0,0), OutputWndLock(true,false)
{
	zoom=0;zoom_min=1; buffer=0;
	DrawMode.Rst(ALL); SetMode(IDLE);
	CurPic=0; 
	busy=false;
	RotateLine.enable=false; RotateLine.cur_color=RGB(255,255,0); RotateLine.ActiveMask=1;
	ChangeCursor=false;
	counter=0; ZMode=FitAll;
	LastViewRgn=CRect(0,0,0,0);
	BckgBrush.CreateSolidBrush(RGB(133,133,133));
	BckgPen.CreatePen(PS_SOLID,1,RGB(0,0,0));
}

OutputWnd::~OutputWnd()
{
	if(buffer) {delete buffer; buffer=0;}
}


BEGIN_MESSAGE_MAP(OutputWnd, CWnd)
	//{{AFX_MSG_MAP(OutputWnd)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETCURSOR()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_POPUPMENU_ATTACH,OnZoneAttach)
	ON_COMMAND(ID_POPUPMENU_DETACH,OnZoneDetach)

	ON_COMMAND(ID_SELECT_ALL,OnSelectAll)
	ON_COMMAND(ID_PRINT,OnPrint)
	ON_COMMAND(ID_ORIGINAL,OnOriginal)
	ON_COMMAND(ID_ZOOM_IN,OnZoomIn)
	ON_COMMAND(ID_ZOOM_OUT,OnZoomOut)

	//}}AFX_MSG_MAP
	ON_WM_CONTEXTMENU()
	ON_MESSAGE(UM_STOP,OnStop)
	ON_WM_MBUTTONDOWN()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// OutputWnd message handlers

void OutputWnd::OnPaint() 
{	
	CPaintDC dc(this); // device context for painting
	
	HDC hdc=dc.GetSafeHdc();
	
	if(CurPic)
	{
		if(UpdateWndBckg) 
		{
			HGDIOBJ old_brsh=SelectObject(hdc,BckgBrush);
			HGDIOBJ old_pen=SelectObject(hdc,BckgPen);
			Rectangle(hdc,0,0,Wnd.Width(),Wnd.Height());
			SelectObject(hdc,old_brsh); SelectObject(hdc,old_pen);		
			UpdateWndBckg=false;
		}
		if(zoom==1.)
		{
			BitBlt(hdc,ActiveWnd.left,ActiveWnd.top,ActiveWnd.Width(),ActiveWnd.Height(),buffer->GetDC(), 0, 0,SRCCOPY);		            			
		}
		else
		{
			MyRect ViewRgn=CurPic->GetViewRgn();
			StretchBlt(hdc,ActiveWnd.left,ActiveWnd.top,ActiveWnd.Width(),ActiveWnd.Height(),buffer->GetDC(), 0, 0, ViewRgn.Width(),ViewRgn.Height(),SRCCOPY);	
		}
	}
	else
	{
		HGDIOBJ old_brsh=SelectObject(hdc,BckgBrush);
		HGDIOBJ old_pen=SelectObject(hdc,BckgPen);
		Rectangle(hdc,0,0,Wnd.Width(),Wnd.Height());
		SelectObject(hdc,old_brsh); SelectObject(hdc,old_pen);
	}
}

BOOL OutputWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{	
	BOOL ret=CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
	Wnd=MyRect(rect.left,rect.top,rect.right,rect.bottom); 

	buffer=new BMPanvas(GetDC());
	CoordCanvas.CreateCompatibleDC(0);
	CoordCanvas.SetMapMode(MM_ISOTROPIC);
	CoordCanvas.SetWindowExt(Wnd.Width(),Wnd.Height());
	CoordCanvas.SetViewportExt(Wnd.Width(),Wnd.Height());
	Timer1.Start();
	return ret;
}

void OutputWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(!CurPic) {CWnd::OnMouseMove(nFlags, point); return;}		
	CurPoint=ScreenToPic(point);
	if(CurPoint.x<0 || CurPoint.y<0) {CWnd::OnMouseMove(nFlags, point); return;}		

	BMPanvas &Buffer=*(CurPic->Buffer); BckgZone* ActiveZone;// Message *msg;
	MyRect t; 
	GraphicThread* parent=(GraphicThread* )AfxGetThread();
	ArrayOfZones& Zones=*(CurPic->GetZones());

	RGBQUAD col1; COLORREF *tr=(COLORREF*)&col1; 
	*tr=GetPixel(CurPic->Buffer->GetDC(),CurPoint.x,CurPoint.y);
	*tr=GetPixel(CurPic->Buffer->GetDC(),CurPoint.x,CurPoint.y);
	CString ttt; CString Caption; Caption="";
//	CoordCanvas.SetWindowExt(ActiveWnd.Width(),ActiveWnd.Height());
//	CoordCanvas.SetWindowOrg(ActiveWnd.TopLeft());

	MyRect vr=CurPic->GetViewRgn();
	CoordCanvas.SetWindowOrg(vr.TopLeft());
	CoordCanvas.SetWindowExt(vr.Width(),vr.Height());
	CoordCanvas.SetViewportExt(ActiveWnd.Width(),ActiveWnd.Height());
	CoordCanvas.SetViewportOrg(ActiveWnd.TopLeft());

	CPoint newCoord; newCoord=point; CoordCanvas.DPtoLP(&newCoord);

	ttt.Format("XY={%d:%d} <> {%d:%d}", CurPoint.x,CurPoint.y,newCoord.x,newCoord.y); Caption+=ttt;
	//ttt.Format(", RGB={%d:%d:%d}",col1.rgbBlue,col1.rgbGreen,col1.rgbRed); Caption+=ttt;
	Parent->ParentWnd->SetWindowText(Caption); 
	double dx,dy; CRect tt,tt0; CPoint diff;
	
	if(nFlags==MK_RBUTTON) SetMode(PIC_MOVE);

	switch(mode)
	{
	case ZONE_SET:
	case FREE_SPACE_CLICK:
		if(nFlags==MK_LBUTTON && CurPic->IsBckgFile) 
		{			
			if(LM0.x!=CurPoint.x && LM0.y!=CurPoint.y)
			{
				CRect t(LM0,CurPoint); t.NormalizeRect();
				BckgZone *t1=new BckgZone(CurPic); t1->Create(t);
				CurPic->AddZone(t1);  BYTE modeZ=0;

				modeZ|=CurPoint.x==LM0.x;
				modeZ|=(CurPoint.x>LM0.x)<<1;
				modeZ|=(CurPoint.y==LM0.y)<<2;
				modeZ|=(CurPoint.y>LM0.y)<<3;
                
				ActiveZone=CurPic->GetActiveZone();
				if(ActiveZone)
				{
					ActiveZone->size_mode=modeZ;
					if(modeZ==1) ActiveZone->size_mode=2;
					if(modeZ==4) ActiveZone->size_mode=0;
					if(modeZ==9) ActiveZone->size_mode=8;
					if(modeZ==6) ActiveZone->size_mode=10;
				}
				
				SetMode(ZONE_SIZE);
				parent->Parent->PostThreadMessage(UM_UPDATE,0,0);	
			}			
		}				
	case ZONE_SIZE:
		if(nFlags==(MK_LBUTTON))
		{
			if(!(ActiveZone=CurPic->GetActiveZone())) break;
			if(ActiveZone->SizeZone(LM0,CurPoint)) 
			{
				DrawMode.Set(FRG);Draw();
			}
		}		
		break;
	case ZONE_MOVE:
		if(nFlags==(MK_LBUTTON))
		{
			ActiveZone=CurPic->GetActiveZone();				
			if(ActiveZone->MoveZone(LM0,CurPoint)) {DrawMode.Set(FRG);Draw();}
		}		
		break;
	case CROP_MOVE:
		break;
	case PIC_MOVE:
		if(nFlags==MK_RBUTTON)
		{
			MyRect t=CurPic->GetViewRgn(); CPoint dd=CurPoint-LM0; 
			dx=dd.x*zoom*1.0; dy=dd.y*zoom*1.0; dd.x=(int)dx; dd.y=(int)dy;
			PicMove+=dd; counter++;
			if(counter>=3)
			{
				counter=0;
				PicMove.x=(int)(PicMove.x*0.7); PicMove.y=(int)(PicMove.y*0.7);
				if(PicMove.x || PicMove.y)
				{
					t+=PicMove; 
					CurPic->SetViewRgn(t); buffer->Rgn=CurPic->GetViewRgn();
					DrawMode.Set(BCKG);Draw();							
				}
				PicMove.x=PicMove.y=0;
			}
		}
		break;
	case ZOOM_OUT: SetMode(PIC_MOVE); break;
	case ROTATE_LINE:		
		RotateLine.End=CurPoint; 
		//RotateLine.Erase(buffer,RotateLine.Rgn);
		//RotateLine.Draw(buffer,1.); 
		DrawMode.Set(FRG);Draw();
		UpdateNow();					
		break;
	default: 
		SetMode(IDLE);		
		break;		
	}
	if(mode!=ZONE_SET) {LM0=MM0=CurPoint;}
	if(!nFlags) SetMode(IDLE);		
	
	SetFocus();

	CWnd::OnMouseMove(nFlags, point);
}


void OutputWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(!CurPic) 
	{
		CWnd::OnLButtonDown(nFlags, point);
		return;	
	}		
	CurPoint=ScreenToPic(point); 	
	GraphicThread* parent=(GraphicThread* )AfxGetThread();
	bool upd=false;  LM1=CurPoint; Message *msg; int i;
	
	switch(nFlags)
	{
	case MK_LBUTTON:
		//if(mode==ZONE_SET) break;	
		if((msg=CurPic->Lock()))
		{
			CurPic->IsInZone(CurPoint); BckgZone* ActiveZone=CurPic->GetActiveZone();
			if(ActiveZone)
			{
				if((i=ActiveZone->IsSizeZone(CurPoint))>=0) 
				{
					ActiveZone->size_mode=i;
					SetMode(ZONE_SIZE);
				}
				else SetMode(ZONE_MOVE);
			}	
			else SetMode(FREE_SPACE_CLICK);
			delete msg;
		}
		else SetMode(IDLE);
		LM0=CurPoint; 
		
		break;
	/*
	case (MK_LBUTTON|MK_CONTROL):
		if(mode==ZONE_SET) 
		{
			CRect t(LM0,CurPoint); t.NormalizeRect();
			BckgZone *t1=new BckgZone(CurPic); t1->Create(t);
			CurPic->AddZone(t1); 
			DrawMode.Set(FRG); Draw();	
			SetMode(IDLE);	
		}
		else
		{
			SetMode(ZONE_SET); LM0=CurPoint;
		}	
		break;
		*/
	case (MK_LBUTTON|MK_SHIFT|MK_CONTROL):	
		SetMode(ZOOM_IN); break;		
	case (MK_LBUTTON|MK_SHIFT):
		parent->bckg=CurPic->GetBckg(CurPoint);
		break;		
	}    	
	parent->Parent->PostThreadMessage(UM_UPDATE,0,0);	
	CWnd::OnLButtonDown(nFlags, point);
}


void OutputWnd::Clear() 
{
	PatBlt(buffer->GetDC(),0,0,Wnd.Width(),Wnd.Height(),BLACKNESS);
	//buffer->BitBlt(0,0,Wnd.Width(),Wnd.Height(),buffer,0,0,BLACKNESS);
}

void OutputWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if(!CurPic) 
	{
		CWnd::OnRButtonDown(nFlags, point);
		return;	
	}		
	CurPoint=ScreenToPic(point); 
	GraphicThread* parent=(GraphicThread* )AfxGetThread();
	bool upd=false; BckgZone* ActiveZone;
	
	switch(nFlags)
	{
	case MK_RBUTTON:
		CurPic->IsInZone(CurPoint); ActiveZone=CurPic->GetActiveZone();
		if(ActiveZone) SetMode(ZONE_MENU); 
		else SetMode(FREE_SPACE_CLICK);
		DrawMode.Set(FRG); Draw();	
		break;
	case (MK_RBUTTON|MK_CONTROL):		
		break;
	case (MK_RBUTTON|MK_SHIFT|MK_CONTROL):	
		SetMode(ZOOM_OUT); break;	
	case (MK_RBUTTON|MK_SHIFT):		
		if(mode==ROTATE_LINE)
		{
			SetMode(IDLE);
			RotateLine.enable=false;
			double t;
			if(RotateLine.Beg.x==RotateLine.End.x) t=0.;
			else
			{
				if(RotateLine.End.y>RotateLine.Beg.y) 
				{
					MyPoint t; t=RotateLine.End; RotateLine.End=RotateLine.Beg; RotateLine.Beg=t;
				}
				t=atan((double)(-RotateLine.End.y+RotateLine.Beg.y)/(RotateLine.End.x-RotateLine.Beg.x));
				if(t>M_PI/4) t=M_PI/2-t;
			}

			CurPic->CenterRotate.angle+=t;
			//CurPic->CenterRotate.Q=RotateFilterParam::HI;
			CurPic->Rotate(); 
			DrawMode.Set(BCKG); Draw();
			int err=0; 
		}
		else
		{
			SetMode(ROTATE_LINE); 
			RotateLine.Beg=CurPoint;RotateLine.End=CurPoint;
			RotateLine.enable=true;
			//RotateLine.Draw(buffer,1.); UpdateNow();
			LM0=CurPoint;		
		}		
		break;	

	}
	parent->Parent->PostThreadMessage(UM_UPDATE,0,0);	
	CWnd::OnRButtonDown(nFlags, point);
}

void OutputWnd::LoadPic(DjvuPic* pic)
{	
	GraphicThread* parent=(GraphicThread* )AfxGetThread();
	MyTimer Timer2; DjvuPic *CurPicT; double z1,z2;

	if(CurPic) UnLoadPic();
	
	CurPicT=pic; 
	CurPicT->Create(DjvuPic::NORMAL);			
    
	z1=(double)CurPicT->Buffer->w/Wnd.Width();
	z2=(double)CurPicT->Buffer->h/Wnd.Height();

	zoom_max=(z1>=z2 ? z1:z2);
	switch(ZMode)
	{
	case FitAll: zoom=zoom_max; break;
	case Normal: 
		if(zoom<zoom_min) zoom=zoom_min; 
		if(zoom>zoom_max) {zoom=zoom_max;} break;
	default: break;
	}
    
	SetZoom(zoom,ZMode,CurPicT);
	CurPic=CurPicT;
	DrawMode.Set(BCKG);	Draw();		
	parent->Parent->PostThreadMessage(UM_UPDATE,0,0);	
}

void OutputWnd::UnLoadPic()
{
	if(!CurPic) return;
	//GraphicThread* parent=(GraphicThread* )AfxGetThread();
	LastViewRgn=CurPic->GetViewRgn();
	CurPic->Destroy(); CurPic=0;
	buffer->Destroy();	
	Parent->Parent->PostThreadMessage(UM_UPDATE,0,0);		
}

void OutputWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(!CurPic) 
	{
		CWnd::OnLButtonUp(nFlags, point);
		return;	
	}		
	BMPanvas &Buffer=*(CurPic->Buffer);
	CurPoint=ScreenToPic(point); 
	GraphicThread* parent=(GraphicThread* )AfxGetThread();

	switch (mode)
	{
	case ZOOM_IN:
		if(zoom>zoom_min)
		{
			zoom=zoom_min;

//			MyRect tRgn=Wnd*zoom;
//			CPoint dd=CurPoint-tRgn.CenterPoint();
//			tRgn.OffsetRect(dd);				
//			CurPic->SetViewRgn(tRgn);				
//			buffer->Create(CurPic->Buffer,CurPic->GetViewRgn());
			SetZoom(zoom,Normal,CurPic);
			DrawMode.Set(BCKG); Draw();				
		}		
		SetMode(IDLE);	break;
	case ZONE_MOVE:		
	case FREE_SPACE_CLICK:
		if(nFlags==MK_RBUTTON)
		{
			parent->Parent->PostThreadMessage(UM_NEXT_PIC,0,0);
		}		
		else
		{
			DrawMode.Set(FRG); Draw();
		}
		SetMode(IDLE); break;
	case DBLCLICK:
		SetMode(IDLE); break;
	case ZONE_SET: break;
	default: 
		SetMode(IDLE);
	}		

	LM1=CurPoint;
	parent->Parent->PostThreadMessage(UM_UPDATE,0,0);	
	CWnd::OnLButtonUp(nFlags, point);
}

void OutputWnd::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if(!CurPic) 
	{
		CWnd::OnRButtonUp(nFlags, point);
		return;	
	}		
	BMPanvas &Buffer=*(CurPic->Buffer); 
	GraphicThread* parent=(GraphicThread* )AfxGetThread();

	CurPoint=ScreenToPic(point); if(!CurPic) return;	
	switch (mode)
	{
	case ZOOM_OUT:		
		if(zoom<zoom_max)
		{
			zoom=zoom_max; 
			//MyRect tRgn=Wnd*zoom;
			//CPoint dd=CurPoint-tRgn.CenterPoint();
			//tRgn.OffsetRect(dd);				
			//CurPic->SetViewRgn(tRgn);				
			//buffer->Create(CurPic->Buffer,CurPic->GetViewRgn());
			SetZoom(zoom,FitAll,CurPic);
			DrawMode.Set(BCKG); Draw(); 
		}			
	case FREE_SPACE_CLICK:
		if(nFlags==MK_LBUTTON)
		{
			parent->Parent->PostThreadMessage(UM_PREV_PIC,0,0);		
		}
		break;
	case PIC_MOVE: SetMode(IDLE); break;
	}	
	CWnd::OnRButtonUp(nFlags, point);
}

void OutputWnd::Draw() 
{	
	int mode;
	if((mode=DrawMode.Get()))
	{
		GraphicThread *thread=(GraphicThread *)AfxGetThread(); 
		thread->WAIT_TIME=500;		
		thread->Stop=None; thread->Main(mode,0); 
		RedrawWindow(0,0,RDW_INVALIDATE | RDW_NOERASE | RDW_NOFRAME | RDW_ALLCHILDREN);			
	}	
}


void OutputWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if(CurPic) 
	{
		if(CurPic->IsBckgFile) 
		{
			BMPanvas &Buffer=*(CurPic->Buffer); 
			GraphicThread* parent=(GraphicThread* )AfxGetThread();
			CurPoint=ScreenToPic(point); 	
			int t; SetMode(DBLCLICK);
			if((t=CurPic->IsInZone(CurPoint))>=0)
			{
				CurPic->DeleteZone(t);
				DrawMode.Eql(FRG); Draw();       
			}
			else
			{
				int w=100; CRect tempRect=CRect(CurPoint.x-w,CurPoint.y-w,CurPoint.x+w,CurPoint.y+w);		
				int bckg1=(int)(parent->bckg);		
				CurPic->Buffer->LoadBitmapArray();
				CurPic->FitZone1(tempRect,bckg1);	
				BckgZone *t1=new BckgZone(CurPic); 
				if(t1->Create(tempRect))
				{
					CurPic->AddZone(t1);			
					DrawMode.Eql(FRG); Draw();
				}
				else delete t1;
			}		

			parent->Parent->PostThreadMessage(UM_UPDATE,0,0);
		}
	}	
	CWnd::OnLButtonDblClk(nFlags, point);
}

MyPoint OutputWnd::ScreenToPic(CPoint& point)
{
	MyPoint CurPoint(point); 
	CurPoint-=ActiveWnd.TopLeft();
	CurPoint=CurPoint*zoom; CurPoint+=(CurPic->GetViewRgn()).TopLeft();
	if(CurPoint.x>=CurPic->Buffer->w) CurPoint.x=-1;
	if(CurPoint.y>=CurPic->Buffer->h) CurPoint.y=-1;
	return CurPoint;
}

/*
CPoint OutputWnd::FixViewRgn(CPoint &efPoint, CPoint &urPoint)
{
//	BMPanvas &frg=*(CurPic->frg), &Original=*(CurPic->Original);
	BMPanvas &Buffer=*(CurPic->Buffer);

	int dx=0,dy=0;
	if(ViewRgn.left<0) dx=-ViewRgn.left;
	if(ViewRgn.top<0) dy=-ViewRgn.top;
	if(ViewRgn.right>Buffer.w) dx=Buffer.w-ViewRgn.right;
	if(ViewRgn.bottom>Buffer.h) dy=Buffer.h-ViewRgn.bottom;
	return CPoint(dx,dy);
}
*/


/*
void OutputWnd::UpdateBackground()
{
	/*
	BMPanvas &frg=*(CurPic->frg), &Original=*(CurPic->Original);
	if(bckg_upd)
	{			
		buffer.StretchBlt(0,0,buffer.w,buffer.h,&frg,0,0,frg.w,frg.h,SRCCOPY);
		for(int i=0;i<CurPic->Zones.GetSize();i++) CurPic->Zones[i]->visible=false;
		Cross1.visible=false;
		bckg_upd=false;
	}
	
	DrawMode.Set(BCKG);
}
*/

int Cross::Draw(CDC* canvas,double zoom)
{
	/*
	if(visible || !enable) return ERR_GRAPHICS;
	COLORREF tempColor=cur_color;
	MyPoint temp=coord; temp=temp/zoom; 

	CPen pen; pen.CreatePen(PS_SOLID,1,tempColor); canvas->SelectObject(&pen);	
	CBrush brsh; brsh.CreateSolidBrush(tempColor); canvas->SelectObject(&brsh);
	canvas->SetROP2(R2_XORPEN);
	canvas->MoveTo(temp.x,temp.y);canvas->LineTo(temp.x+5,temp.y);
	canvas->LineTo(temp.x+5,temp.y+5);canvas->LineTo(temp.x,temp.y+5);
	canvas->LineTo(temp.x,temp.y);

	//last_canvas=canvas; 
	last_coord=temp; last_color=tempColor;
	visible=true;
	*/
	return ERR_OK;
}

void Cross::Erase(BMPanvas* canvas,CRect)
{
	/*
	if(!visible) return;
	COLORREF tempColor=last_color;	
	MyPoint temp=last_coord; //CDC* canvas=last_canvas;
	
	CPen pen; pen.CreatePen(PS_SOLID,1,tempColor); canvas->SelectObject(&pen);	
	CBrush brsh; brsh.CreateSolidBrush(tempColor); canvas->SelectObject(&brsh);
	canvas->SetROP2(R2_XORPEN);

	canvas->MoveTo(temp.x,temp.y);canvas->LineTo(temp.x+5,temp.y);
	canvas->LineTo(temp.x+5,temp.y+5);canvas->LineTo(temp.x,temp.y+5);
	canvas->LineTo(temp.x,temp.y);

	visible=false;
	*/
}

int Line::Draw(BMPanvas* src,double zoom)
{		
	if(enable)
	{
		HDC hdc=src->GetDC(); BOOL ret;
		CPoint tBeg=Beg-src->Rgn.TopLeft(), tEnd=End-src->Rgn.TopLeft();
		CPen pen; pen.CreatePen(PS_SOLID,1,cur_color); 
		CBrush brsh; brsh.CreateSolidBrush(cur_color); 
		SelectObject(hdc,HBRUSH(brsh));SelectObject(hdc,HPEN(pen));	
		ret=SetROP2(hdc,R2_XORPEN);
		ret=MoveToEx(hdc,tBeg.x,tBeg.y,NULL); 
		ret=LineTo(hdc,tEnd.x,tEnd.y);	
		Rgn=CRect(Beg,End);	
		return ERR_OK;
	}
	else return 1;	
}

void Line::Erase(BMPanvas *src, CRect rect)
{		
	HDC hdc=src->GetDC();
	CPoint tBeg=rect.TopLeft()-src->Rgn.TopLeft(), tEnd=rect.BottomRight()-src->Rgn.TopLeft();
	CPen pen; pen.CreatePen(PS_SOLID,1,cur_color); 
	CBrush brsh; brsh.CreateSolidBrush(cur_color); 
	SelectObject(hdc,HBRUSH(brsh));SelectObject(hdc,HPEN(pen));	
	SetROP2(hdc,R2_XORPEN);
	
	MoveToEx(hdc,tBeg.x,tBeg.y,NULL); LineTo(hdc,tEnd.x,tEnd.y);	
}

int MyRectangle::Draw(BMPanvas* src,double zoom)
{		
	if(enable)
	{
		HDC hdc=src->GetDC(); 
		SetROP2(hdc,R2_XORPEN);
		//SetROP2(hdc,R2_WHITE);
		

		Rgn=Rect-src->Rgn.TopLeft();
		//FrameRect(hdc,Rgn,brsh);		
		HGDIOBJ last_pen=SelectObject(hdc,HPEN(Pen1));
		MoveToEx(hdc,Rgn.left,Rgn.top,NULL); 
		LineTo(hdc,Rgn.left,Rgn.bottom);
		LineTo(hdc,Rgn.right,Rgn.bottom); LineTo(hdc,Rgn.right,Rgn.top);
		LineTo(hdc,Rgn.left,Rgn.top);
		//HGDIOBJ last_brsh=SelectObject(hdc,GetStockObject(NULL_BRUSH));
		//Rectangle(hdc,Rgn.left,Rgn.top,Rgn.right,Rgn.bottom);

		SelectObject(hdc,last_pen);
		//SelectObject(hdc,last_brsh);
		return ERR_OK;
	}
	else return 1;	
}

void MyRectangle::Erase(BMPanvas *src, CRect rect)
{		
	HDC hdc=src->GetDC(); 
	SetROP2(hdc,R2_XORPEN);	
	
	//FrameRect(hdc,rect,brsh);		
	HGDIOBJ last_pen=SelectObject(hdc,HPEN(Pen1));
//	HGDIOBJ last_brsh=SelectObject(hdc,GetStockObject(NULL_BRUSH));
//	Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom);
	MoveToEx(hdc,rect.left,rect.top,NULL); LineTo(hdc,rect.left,rect.bottom);
	LineTo(hdc,rect.right,rect.bottom); LineTo(hdc,rect.right,rect.top);
	LineTo(hdc,rect.left,rect.top);
	SelectObject(hdc,last_pen);
//	SelectObject(hdc,last_brsh);
}

void MyRectangle::Serialize( CArchive& ar )
{	
		if( ar.IsStoring() )
		{
			ar << Rect << enable;
		}
		else
		{
			ar >> Rect >> enable;
		}
};


CorrelationData Correlator::GetCorrelation(BYTE* _Y)
{
	int sum3=0, t;double meanXY; CorrelationData tr;
	YData.sum=YData.sum2=0;
	for(int i=0;i<N;i++)
	{
		t=_Y[i]; YData.sum+=t; YData.sum2+=t*t; sum3+=X[i]*t;			
	}
	YData.mean=YData.sum/N; YData.mean2=YData.sum2/N; meanXY=sum3/N;

	//cor=(meanXY-XData.mean*YData.mean)/(sqrt((XData.mean2-XData.mean*XData.mean)*(YData.mean2-YData.mean*YData.mean)));
	double tr1=sqrt(N*XData.sum2-XData.sum*XData.sum)*sqrt(N*YData.sum2-YData.sum*YData.sum);
	tr.r=(N*sum3-YData.sum*XData.sum)/((tr1));
	tr.s=fabs(sqrt((double)(N-1))*tr.r/(1-tr.r*tr.r));

	return tr;
}

void Correlator::SetX(BYTE* _X, int app)
{
	if(Apperture!=app)
	{
		Apperture=app; N=Apperture*Apperture;
		if(X) {delete[] X; X=0;}
		X=new BYTE[N];
	}	
	memcpy(X,_X,N); int t;
	XData.sum=XData.sum2=0;
	for(int i=0;i<N;i++)
	{
		t=X[i]; XData.sum+=t; XData.sum2+=t*t;			
	}
	XData.mean=XData.sum/N; XData.mean2=XData.sum2/N;
}

int OutputWnd::SetMode(modes m)
{
	int ret=mode;
	mode=m; ChangeCursor=true;
	return ret;
}

BOOL OutputWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(!ChangeCursor) return true;
	HCURSOR cursor;
	switch(mode)
	{
	case ROTATE_LINE:
		cursor=AfxGetApp()->LoadStandardCursor(IDC_CROSS); break;			
	case ZONE_SET:
		cursor=AfxGetApp()->LoadStandardCursor(IDC_SIZENESW); break;			
	case ZONE_MOVE:
		cursor=AfxGetApp()->LoadStandardCursor(IDC_SIZEALL); break;			
	case ZONE_SIZE:
		cursor=AfxGetApp()->LoadStandardCursor(IDC_SIZENWSE); break;				
	default:
		cursor=AfxGetApp()->LoadStandardCursor(IDC_ARROW); break;	
	}
	
	::SetCursor(cursor);
	ChangeCursor=false;
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void OutputWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	UINT a=nChar;
	
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void OutputWnd::OnContextMenu(CWnd* pWnd, CPoint point)
{
	switch(mode)
	{
	case ZONE_MENU:
		CMenu menu1; menu1.LoadMenu(IDR_MENU1);
		CMenu *menu2=menu1.GetSubMenu(0);
		menu2->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,point.x,point.y,pWnd);
		break;	
	}	
}

void OutputWnd::OnZoneAttach()
{
//	Message *msg;
	CString fltr="BMP Files (*.bmp)|*.bmp|All Files (*.*)|*.*||";
	CFileDialog dlg1(true,NULL,NULL,OFN_FILEMUSTEXIST,fltr);
	if(dlg1.DoModal()==IDOK)
	{
		int n;
		CRect tempRect=(CurPic->GetActiveZone(&n))->Rgn; 
		AttachedBckgZone *t=new AttachedBckgZone(dlg1.GetFileName(),CurPic);						
		if(t->Create(tempRect))
		{
			//if((msg=CurPic->Lock()))		
			{
				CurPic->AddZone(t);
				CurPic->DeleteZone(n);
				DrawMode.Set(FRG); Draw();
			}		
		}
		else delete t;
	}
}

void OutputWnd::OnZoneDetach()
{
	int n; //Message *msg;
	CRect tempRect=(CurPic->GetActiveZone(&n))->Rgn; 
	BckgZone *t=new BckgZone(CurPic);
	if(t->Create(tempRect))
	{
		//if((msg=CurPic->Lock()))	
		{
			CurPic->AddZone(t);
			CurPic->DeleteZone(n);
			DrawMode.Set(FRG); Draw();
		}	
	}
	else delete t;
}


LRESULT OutputWnd::OnStop(WPARAM wParam,LPARAM lParam)
{	
    return 0;
}

void OutputWnd::UpdateNow(void)
{
	RedrawWindow(0,0,RDW_INVALIDATE | RDW_NOERASE | RDW_NOFRAME | RDW_ALLCHILDREN);					
}


BOOL OutputWnd::OnMouseWheel(UINT nFlags,short zDelta,CPoint pt)
{
	if(!CurPic) 
	{
		return CWnd::OnMouseWheel(nFlags,zDelta,pt);		
	}		
//	double dx,dy;
	//MyRect t=CurPic->GetViewRgn(); CPoint dd(0,-zDelta); 
	//dx=dd.x*zoom; dy=dd.y*zoom; dd.x=(int)dx; dd.y=(int)dy;
	//t+=dd; 
	//CurPic->SetViewRgn(t); buffer->Rgn=CurPic->GetViewRgn();
	//DrawMode.Set(BCKG);Draw();							
	SetCursorPos(pt.x,pt.y-zDelta/2);
	return true;
}


void OutputWnd::OnDestroy( )
{
	if(buffer) {delete buffer; buffer=0;}
}

void OutputWnd::OnSelectAll()
{
	Parent->Parent->PostThreadMessage(WM_COMMAND,ID_SELECT_ALL,0);
}

void OutputWnd::OnPrint()
{
	Parent->Parent->PostThreadMessage(WM_COMMAND,ID_PRINT,0);
}

void OutputWnd::OnOriginal()
{
	Parent->Parent->PostThreadMessage(WM_COMMAND,ID_ORIGINAL,0);
}

void OutputWnd::SetZoom(double _zoom,ZoomMode _ZoomMode, DjvuPic *CurPicT)
{
	zoom=_zoom; ZMode=_ZoomMode;
	MyRect ViewRgn, Diff;
	
	CurPicT->SetViewRgn(Wnd*zoom); ViewRgn=CurPicT->GetViewRgn();
	if(!(LastViewRgn.IsRectEmpty())) 
	{
		ViewRgn.OffsetRect(LastViewRgn.GetCenter()-ViewRgn.GetCenter());
		CurPicT->SetViewRgn(ViewRgn); ViewRgn=CurPicT->GetViewRgn();
	}

//	HDC t=buffer->GetDC(); 
//	SetWindowExtEx(t,ViewRgn.Width(),ViewRgn.Height(),0); //SetWindowOrgEx(t,0,0,0);
//	SetViewportExtEx(t,Original->Rgn.Width(),Original->Rgn.Height(),0);
//	SetWindowOrgEx(t,0,0,0);

	buffer->Create(CurPicT->Buffer,ViewRgn);	
	ActiveWnd=ViewRgn; ActiveWnd/=zoom; 
	ActiveWnd.OffsetRect(Wnd.GetCenter()-ActiveWnd.GetCenter());

	if(ActiveWnd!=Wnd) UpdateWndBckg=true;
}

void OutputWnd::OnZoomOut()
{
	if(!CurPic) return;
	SetZoom(zoom_max, FitAll, CurPic);
	DrawMode.Set(BCKG); Draw(); 
}

void OutputWnd::OnZoomIn()
{
	if(!CurPic) return;
	SetZoom(zoom_min, Normal, CurPic);
	DrawMode.Set(BCKG); Draw();				
}


void OutputWnd::OnMButtonDown(UINT nFlags, CPoint point)
{
	if(!CurPic) 
	{
		CWnd::OnMButtonDown(nFlags, point);
		return;	
	}		
	CurPoint=ScreenToPic(point); 
	GraphicThread* parent=(GraphicThread* )AfxGetThread();

    SetMode(CROP_MOVE); MM0=CurPoint;	
}

void OutputWnd::OnMButtonUp( UINT nFlags, CPoint point)
{
	if(!CurPic) 
	{
		CWnd::OnMButtonUp(nFlags, point);
		return;	
	}		
	//CurPoint=ScreenToPic(point); 
	GraphicThread* parent=(GraphicThread* )AfxGetThread();

    SetMode(IDLE); //MM0=CurPoint;	

}

BOOL OutputWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	UpdateWndBckg=true;

	return CWnd::OnEraseBkgnd(pDC);
}
