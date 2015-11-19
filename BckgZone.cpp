#include "stdafx.h"
#include ".\bckgzone.h"

#include "djvupic.h"
#include "graphicthread.h"

BckgZone::BckgZone(DjvuPic* parent)
{
	Parent=parent; buffer=0;
	Rgn=CRect(0,0,0,0);
	size_mode=-1; 
	active_color=RGB(50,50,50); inactive_color=RGB(200,200,200);	
	ZoneType=NormalZone;

	buffer=new BMPanvas(parent->Buffer);
}

int BckgZone::Create(CRect rect)
{
	int ret;
	if(rect.IsRectNull()) {rect=Rgn;}

	Destroy();
	if((ret=buffer->Create(Parent->Background,rect)))
	{
		Rgn=rect;
		CopyOriginal(); 
	}
	return ret;
}

void BckgZone::Destroy()
{	
	if(buffer) {buffer->Destroy();}	
}


BckgZone::~BckgZone(void)
{
	Destroy();
	if(buffer) {delete buffer;}	
}

void BckgZone::Serialize( CArchive& ar )
{	
		if( ar.IsStoring() )
		{
			ar << ZoneType << Rgn << active_color << inactive_color;
		}
		else
		{
			ar >> Rgn >> active_color >> inactive_color;
		}
};

int BckgZone::IsSizeZone(MyPoint &CurPoint)
{
	CRect t=Rgn; int w=(Rgn.Ratio()>1 ? Rgn.Height():Rgn.Width()); w=(int)(w*0.35); int ret=-1;
	CRect LTzone(Rgn.TopLeft(),CSize(w,w));
	CRect RBzone(Rgn.BottomRight(),CSize(-w,-w)); RBzone.NormalizeRect();
	CRect RTzone(CPoint(Rgn.right,Rgn.top),CSize(-w,w)); RTzone.NormalizeRect();
	CRect LBzone(CPoint(Rgn.left,Rgn.bottom),CSize(w,-w)); LBzone.NormalizeRect();
	
	if(LTzone.PtInRect(CurPoint)) ret=0;
	if(RBzone.PtInRect(CurPoint)) ret=10;
	if(RTzone.PtInRect(CurPoint)) ret=2;
	if(LBzone.PtInRect(CurPoint)) ret=8;	
	return ret;
}

int BckgZone::SizeZone(MyPoint &RefPoint, MyPoint &CurPoint)
{
	CRect t=Rgn;

	MyPoint dif=CurPoint-RefPoint;
	switch(size_mode)
	{
	case 0: t.left+=dif.x;t.top+=dif.y; break;
	case 10: t.right+=dif.x;t.bottom+=dif.y; break;
	case 2: t.right+=dif.x;t.top+=dif.y; break;
	case 8: t.left+=dif.x;t.bottom+=dif.y; break;	
	}

	if(t!=Rgn)
	{	
		Destroy();
		return Create(t);
	}
	else
		int a=4;
	return 0;
}

int BckgZone::MoveZone(MyPoint &RefPoint, MyPoint &CurPoint)
{
	bool move=false; CRect t=Rgn; 
	MyPoint dif=CurPoint-RefPoint;
	t+=dif;
	if(t!=Rgn)
	{	
		Rgn=t;
		CopyOriginal(); return 1;
	}
	else return 0;
}

int BckgZone::Draw(BMPanvas* canvas,double zoom)
{
	if(Rgn.IsRectEmpty()) return ERR_GRAPHICS;

	//CDC* dest=CDC::FromHandle(canvas->GetSafeHdc());
	COLORREF tempColor=(Parent->ZonesActive.Chk(ActiveMask) ? active_color:inactive_color);
	CBrush brsh; brsh.CreateSolidBrush(tempColor); 

	CRect t4=Rgn; t4.OffsetRect(-canvas->Rgn.TopLeft());
	buffer->CopyTo(canvas,t4,CPoint(0,0));	
	//if(enable) dest->FrameRect(t4,&brsh);
	if(enable) FrameRect(canvas->GetDC(),t4,HBRUSH(brsh));

	return ERR_OK;
}

void BckgZone::CopyOriginal()
{
	Parent->Background->CopyTo(buffer,CPoint(0,0),Rgn);
}

void BckgZone::Erase(BMPanvas* canvas,CRect Rgn)
{
//	CDC* dest=CDC::FromHandle(canvas->GetSafeHdc());
//	CDC* ers=CDC::FromHandle(eraser->GetSafeHdc());

	CRect t4=Rgn; t4.OffsetRect(-canvas->Rgn.TopLeft());
	Parent->Buffer->CopyTo(canvas,t4.TopLeft(),Rgn);
	
//	dest->BitBlt(temp.left,temp.top,temp.Width(),temp.Height(),ers,0,0,SRCCOPY);

}

void BckgZone::operator = (BckgZone& src)
{
    Rgn=src.Rgn;	
	Parent=src.Parent;
	inactive_color=src.inactive_color;
	active_color=src.active_color;
}

AttachedBckgZone::AttachedBckgZone(CString name, DjvuPic* parent):
	BckgZone(parent)
{
	Name=name;
	ZoneType=AttachedZone;		
}

int AttachedBckgZone::Create(CRect rgn)
{
	int ret;
	if(rgn.IsRectNull()) {ret=0;return ret;}
	
	BITMAPINFOHEADER info=GetBMPInfo(Name);
	int dx=10,dy=10,w=info.biWidth,h=info.biHeight;	
	CPoint mid=rgn.CenterPoint();
	rgn.left=mid.x-(w/2)-dx; rgn.right=mid.x+(w/2)+dx;
	rgn.top=mid.y-(h/2)-dy; rgn.bottom=mid.y+(h/2)+dy;
	rgn.NormalizeRect();	
	ret=BckgZone::Create(rgn);
	return ret;
}

int AttachedBckgZone::Draw(BMPanvas* canvas,double zoom)
{
	if(Rgn.IsRectEmpty()) return ERR_GRAPHICS;	

//	CDC* dest=CDC::FromHandle(canvas->GetSafeHdc());
	COLORREF tempColor=(Parent->ZonesActive.Chk(ActiveMask) ? active_color:inactive_color);
	CBrush brsh; brsh.CreateSolidBrush(tempColor); 

	MyPoint d; MyRect frgRect=Rgn, t4=Rgn;
	d.x=(Rgn.Width()-buffer->w)/2; d.y=(Rgn.Height()-buffer->h)/2; 
	frgRect=CRect(Rgn.TopLeft()+d,CSize(buffer->w,buffer->h));	
	t4.OffsetRect(-canvas->Rgn.TopLeft());	frgRect.OffsetRect(-canvas->Rgn.TopLeft());

	//dest->PatBlt(t4.left,t4.top,Rgn.Width(),Rgn.Height(),WHITENESS);
	PatBlt(canvas->GetDC(),t4.left,t4.top,Rgn.Width(),Rgn.Height(),WHITENESS);
	//canvas->BitBlt(frgRect.left, frgRect.top, frgRect.Width(),frgRect.Height(),buffer,0,0,SRCCOPY);		
	buffer->CopyTo(canvas,frgRect,CPoint(0,0));
	
	//if(enable) canvas->FrameRect(Rgn,&brsh);
	if(enable) FrameRect(canvas->GetDC(),t4,HBRUSH(brsh));
	return ERR_OK;
}

int AttachedBckgZone::SizeZone(MyPoint &RefPoint, MyPoint &CurPoint)
{
	CRect t=Rgn;

	MyPoint dif=CurPoint-RefPoint;
	switch(size_mode)
	{
	case 1: t.left+=dif.x;t.top+=dif.y; break;
	case 2: t.right+=dif.x;t.bottom+=dif.y; break;
	case 3: t.right+=dif.x;t.top+=dif.y; break;
	case 4: t.left+=dif.x;t.bottom+=dif.y; break;	
	}

	if(t!=Rgn && t.Width()>buffer->w && t.Height()>buffer->h)
	{	
        Rgn=t;	return 1;	
	}
	else return 0;
}


void AttachedBckgZone::CopyOriginal()
{	
	buffer->LoadImage(Name);
}

/*
CRect AttachedBckgZone::SizeZone(MyPoint &RefPoint, MyPoint &CurPoint)
{
	CRect t=BckgZone::SizeZone(RefPoint,CurPoint);
	if(t.Width()<=buffer->w || t.Height()<=buffer->h) t=Rgn;
	else
	{
		update_eraser=true;
	}
	return t;
}
*/
int AttachedBckgZone::MoveZone(MyPoint &RefPoint, MyPoint &CurPoint)
{
	bool move=false; CRect t=Rgn; 
	MyPoint dif=CurPoint-RefPoint;
	t+=dif;	
	if(t!=Rgn) 
	{
		Rgn=t;
		return 1;
	}
	else return 0;
}

void AttachedBckgZone::Serialize( CArchive& ar )
{	
		if( ar.IsStoring() )
		{
			BckgZone::Serialize(ar);
			ar << Name;			
		}
		else
		{
			BckgZone::Serialize(ar);
			ar >> Name;
		}
};

CRect BckgZone::GetFillRect(void)
{
	CRect ret=CRect(0,0,Rgn.Width(),Rgn.Height());
	if(Rgn.left<0) ret.left=-Rgn.left;
	if(Rgn.top<0) ret.top=-Rgn.top;
	if(Rgn.right>Parent->PicDim.cx) ret.right-=Rgn.right-Parent->PicDim.cx;
	if(Rgn.bottom>Parent->PicDim.cy) ret.bottom-=Rgn.bottom-Parent->PicDim.cy;
	return ret;
}
