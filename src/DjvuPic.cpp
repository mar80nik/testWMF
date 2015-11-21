// DjvuPic.cpp: implementation of the DjvuPic class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "testWMF.h"
#include "DjvuPic.h"
#include "rotatefilter.h"
#include ".\djvupic.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DjvuPic::DjvuPic(CString name,OutputWnd *parent)//: PicLock(true,false)
{
	Buffer=Background=Foreground=0; 
	BckgRgn=MyRect(0,0,2,2); 
	IsBckgFile=IsFrgFile=IsDjvuFile=false;
	FullName=Path=Name=Ext=FrgFile=BckgFile=DjvuFile="None";
	ZonesActive.Eql(0);
	Zones.RemoveAll();	

	Parent=parent; FullName=name; 
	ProcessFullName();
	GetPicDim();

	Buffer=new BMPanvas();	
	Foreground=new BMPanvas();	
	Background=new BMPanvas();	

	Param.dd=CPoint(2,2); 
}

void DjvuPic::Create( CreateMode mode )
{
	Destroy();
	if(mode==IN_MEMORY || mode==NORMAL)
	{
		SetForeground(); 	
		SetBackground(); 	
		if(mode==NORMAL) SetBuffer(FullName);
		if(CenterRotate.angle) Rotate(); 
		ViewRgn=MyRect(0,0,Parent->Wnd.Width(),Parent->Wnd.Height());
		for(int i=0;i<Zones.GetSize();i++)
		{
			if(Zones[i]) Zones[i]->Create();
		}
	}
    return;
}

void DjvuPic::Destroy()
{
	if(Buffer) Buffer->Destroy();
	if(Background) Background->Destroy();
	if(Foreground) Foreground->Destroy();
	for(int i=0;i<Zones.GetSize();i++)
	{
        if(Zones[i]) Zones[i]->Destroy();
	}	
}

DjvuPic::~DjvuPic()
{
	Destroy();
	for(int i=0;i<Zones.GetSize();i++)
	{
        if(Zones[i]) delete Zones[i];
	}
	if(Buffer) delete Buffer;
	if(Background) delete Background;
	if(Foreground) delete Foreground;
}

int DjvuPic::IsInZone(CPoint &CurPoint)
{
	int i, upd=-1;
	for(i=0;i<Zones.GetSize();i++)
	{
		if(Zones[i]->Rgn.PtInRect(CurPoint) && Zones[i]->enable) upd=i;		
	}
	SetActiveZone(upd);
	return upd;
}

int DjvuPic::AddZone(BckgZone* z)
{
	int t=Zones.Add(z);
	z->ActiveMask=1<<t;
	ZonesActive.Eql(z->ActiveMask);
	z->enable=true; //z->Create(this);	
	return t;
}

void DjvuPic::DeleteZone(int i)
{
	if(i<0 || i>=Zones.GetSize()) return;

	AbstractGraphics* temp=Zones[i]; Zones.RemoveAt(i);
	int j; temp->ActiveMask=0;
	for(j=Zones.GetSize()-1;j>=i;j--) 
		Zones[j]->ActiveMask=Zones[j]->ActiveMask>>1;

	int mask=0; for(j=0;j<=i;j++) mask|=1<<j;
	int flag=ZonesActive.All();
	int flag1=flag&(~mask); 
	flag1=flag1>>1;	
	flag=flag&(mask>>1);
	flag=flag|flag1; ZonesActive.Eql(flag);
	if(!flag)
	{
		if((j=Zones.GetSize()))
		{
			if(i>=j) i=j-1;
			ZonesActive.Set(1<<i);                        			
		}		
	}	
}


int DjvuPic::GetBckg(CPoint &CurPoint)
{
	int i,j,h=BckgRgn.Width(), sum=0; RGBQUAD col;
	Buffer->LoadBitmapArray(CurPoint.y,CurPoint.y+BckgRgn.Height());
	
	for(j=0;j<BckgRgn.Height();j++)
	{		
		for(i=CurPoint.x;i<CurPoint.x+BckgRgn.Width();i++) 
		{			
			h=Buffer->arr[j*Buffer->wbyte+i];
			col=Buffer->INFO.bmiColors[h];			
			sum+=col.rgbBlue;
		}
	}	
	return sum;	
}

void DjvuPic::GetPoints(CPoint &CurPoint, BYTE* arr1, int app)
{/*
	BMPanvas &frg=*(this->frg), &Original=*(this->Original);

	BMPanvas &canvas=frg;
	HDC hdc2=canvas.GetSafeHdc(); int i,j, sum=0,k=0; RGBQUAD col;
	HBITMAP hbmp=HBITMAP(canvas.bmp);
	int size=(canvas.wbyte)+1;
	BYTE *arr=new BYTE[size];

	GetDIBColorTable(hdc2,0,256,canvas.INFO.bmiColors);
		
	for(j=CurPoint.y;j<CurPoint.y+app;j++)
	{
		i=GetDIBits(hdc2,hbmp,canvas.h-j,1,arr,canvas.lpbi,DIB_RGB_COLORS);
		for(i=CurPoint.x;i<CurPoint.x+app;i++) 
		{
			sum=arr[i];
			col=canvas.INFO.bmiColors[sum];			
			arr1[k++]=col.rgbBlue;
		}
	}
	delete[] arr;	
	*/
}

int DjvuPic::GetBckg(MyRect& Rect)
{
	int i,j,k, sum=0; RGBQUAD col; int h=Buffer->h, h1;
	if(Rect.left<0 || Rect.top<0 || Rect.right>Buffer->w || Rect.bottom>h) return 0xFFFFFFF;
	
	for(i=h-1-Rect.top,k=i*Buffer->wbyte;i>h-1-Rect.bottom;i--)
	{
		for(j=Rect.left;j<Rect.right;j++) 
		{
			h1=Buffer->arr[k+j];
			col=Buffer->INFO.bmiColors[h1];			
			sum+=col.rgbBlue;			
			//sum+=frg->arr[i*w+j];	
		}			
		k-=Buffer->wbyte;
	}
	return sum;
}


void DjvuPic::FitZone1(CRect& Rgn, int bckg1)
{
	if(!(Buffer->arr)) return;
	MyRect Rect; 
	Param.bckg=bckg1; Param.www=BckgRgn.Width();

////////////Shift Right Side/////////////////			
	Rect=CRect(CPoint(Rgn.right,Rgn.top),CSize(Param.www,Param.www));	
	Param.zz=Rgn.Height(); 
	if(Rect.right<PicDim.cx) {ShiftRight(Rect,Param);Rgn.right=Rect.right;}
	
//////////////Shift Left Side//////////////		
	Rect=CRect(CPoint(Rgn.left-Param.www,Rgn.top),CSize(Param.www,Param.www));	
	Param.zz=Rgn.Height(); 
	if(Rect.left>0) {ShiftLeft(Rect,Param);Rgn.left=Rect.left;}
////////////////Shift Top Side///////////////		
	Rect=CRect(CPoint(Rgn.left,Rgn.top-Param.www),CSize(Param.www,Param.www));	
	Param.zz=Rgn.Width(); 
	if(Rect.top>0) {ShiftTop(Rect,Param);Rgn.top=Rect.top;}
//////////////////Shift Bottom Side////////////		
	Rect=CRect(CPoint(Rgn.left,Rgn.bottom),CSize(Param.www,Param.www));	
	Param.zz=Rgn.Width(); 
	if(Rect.bottom<PicDim.cy) {ShiftBottom(Rect,Param);Rgn.bottom=Rect.bottom;}

////////////Shift Right Side/////////////////			
	Rect=CRect(CPoint(Rgn.right,Rgn.top),CSize(Param.www,Param.www));	
	Param.zz=Rgn.Height(); 
	if(Rect.right<PicDim.cx) {ShiftRight(Rect,Param);Rgn.right=Rect.right;}		
//////////////Shift Left Side//////////////		
	Rect=CRect(CPoint(Rgn.left-Param.www,Rgn.top),CSize(Param.www,Param.www));	
	Param.zz=Rgn.Height(); 
	if(Rect.left>0) {ShiftLeft(Rect,Param);Rgn.left=Rect.left;}
////////////////Shift Top Side///////////////		
	Rect=CRect(CPoint(Rgn.left,Rgn.top-Param.www),CSize(Param.www,Param.www));	
	Param.zz=Rgn.Width(); 
	if(Rect.top>0) {ShiftTop(Rect,Param);Rgn.top=Rect.top;}
//////////////////Shift Bottom Side////////////		
	Rect=CRect(CPoint(Rgn.left,Rgn.bottom),CSize(Param.www,Param.www));	
	Param.zz=Rgn.Width(); 
	if(Rect.bottom<PicDim.cy) {ShiftBottom(Rect,Param);Rgn.bottom=Rect.bottom;}
}

void DjvuPic::Serialize( CArchive& ar )
{	
	if( ar.IsStoring() )
	{
		GBparam.Serialize(ar); CenterRotate.Serialize(ar); //CropZone.Serialize(ar);
		ar << FullName;
		int n=Zones.GetSize(); ar << n;
		for(int i=0;i<n;i++) Zones[i]->Serialize(ar);
	}
	else
	{
		GBparam.Serialize(ar); CenterRotate.Serialize(ar); //CropZone.Serialize(ar);
		ar >> FullName;
		ProcessFullName();
		int n, ZoneType; ar >> n; BckgZone* temp; 
		Zones.RemoveAll();		
		for(int i=0;i<n;i++) 
		{
			ar >> ZoneType;
			switch(ZoneType)
			{
			case BckgZone::NormalZone: 
				temp=new BckgZone(this);				
				break;
			case BckgZone::AttachedZone: 
				temp=new AttachedBckgZone("",this);								
				break;			
			}			
			temp->Serialize(ar);
			//temp->Create();
			Zones.Add(temp);
		}
		GetPicDim();
	}
};

int DjvuPic::ProcessFullName()
{
	int t; HANDLE SearchHandle; WIN32_FIND_DATA FindFileData; CString SearchPattern;
	if((t=FullName.ReverseFind('\\'))<0)
	{
		Name=FullName; Path="";	
	}
	else
	{
		Path=FullName.Left(++t);	
		Name=FullName.Right(FullName.GetLength()-t);
	}
	if((t=Name.ReverseFind('.'))<0)
	{
		Ext="";	
	}
	else
	{
		Ext=Name.Right(Name.GetLength()-(t));
		Name=Name.Left(t);			
	}

	IsFrgFile=true; FrgFile=FullName;

	SearchPattern=Path+Name+".jpg";	
	if( (SearchHandle=FindFirstFile(LPCSTR(SearchPattern),&FindFileData) )!=INVALID_HANDLE_VALUE)
	{			
		IsBckgFile=true; BckgFile=SearchPattern;
	}
	SearchPattern=Path+Name+".djvu";	
	if( (SearchHandle=FindFirstFile(LPCSTR(SearchPattern),&FindFileData) )!=INVALID_HANDLE_VALUE)
	{			
		IsDjvuFile=true; DjvuFile=SearchPattern;
	}
	return 0;
}

void DjvuPic::operator =(DjvuPic& src) 
	{
		GBparam=src.GBparam;
		FullName=src.FullName;
		Path=src.Path;
		Name=src.Name;
		Ext=src.Name;
		BckgFile=src.BckgFile;
		IsBckgFile=src.IsBckgFile;
		CenterRotate=src.CenterRotate;
	};

int DjvuPic::SetBuffer(CString& fullName,bool check)
{	
	int err=ERR_OK;

	Buffer->Destroy(); 
	Buffer->Create(Parent, Foreground->INFO.bmiHeader.biWidth, Foreground->INFO.bmiHeader.biHeight, 8); 
	Buffer->CreateGrayPallete();			
	CImage TT; TT.Load(FrgFile); TT.BitBlt(Buffer->GetDC(),0,0);
	return err;
}

int DjvuPic::SetForeground()
{
	Foreground->LoadImage(FrgFile); 	
	return 0;
}


int DjvuPic::SetBackground()
{
	if(IsBckgFile) Background->LoadImage(BckgFile); 	
	else
	{
//		temp.bmiHeader.biHeight/=10; temp.bmiHeader.biWidth/=10;
		Background->Destroy(); 
		Buffer->Create(Parent, Foreground->INFO.bmiHeader.biWidth, Foreground->INFO.bmiHeader.biHeight, 8); 
		Background->CreateGrayPallete();		  
	}
	return 0;
}


void DjvuPic::SetActiveZone(int n)
{
    if(n<0) ZonesActive.Eql(0);
	else ZonesActive.Eql(1<<n);
	return;
}

BckgZone* DjvuPic::GetActiveZone(int* N)
{
	int n=0,t;
	if((t=ZonesActive.Get()))
	{		
		while(t>1) {t=t>>1; n++;}
		if(N) *N=n;
		return Zones[n];
	}	
	if(N) *N=-1;
	return 0;
}


MyRect DjvuPic::GetViewRgn(void)
{
	return ViewRgn;
}

void DjvuPic::SetViewRgn(MyRect rgn)
{	
	CSize size=rgn.Size(); CPoint topleft=rgn.TopLeft();
	
	if(size.cx>Buffer->w) size.cx=Buffer->w;
    if(size.cy>Buffer->h) size.cy=Buffer->h;
	
	if(rgn.left<0) topleft.x=0;
	if(rgn.right>Buffer->w) topleft.x=Buffer->w-size.cx;

	if(rgn.top<0) topleft.y=0;	
	if(rgn.bottom>Buffer->h) topleft.y=Buffer->h-size.cy;

    ViewRgn=CRect(topleft,size);
	return;
}

ArrayOfZones* DjvuPic::GetZones(void)
{
	return &Zones;
}

Message* DjvuPic::Lock(int wt)
{
    Message* t=new Message(this);
	if(wt<=0) wt=100;
	if(t->Lock(wt)) return t;
	else return NULL;
}
int DjvuPic::Rotate(void)
{
	RotateFilter Filter1(CenterRotate); 
	Filter1.SetFilter(CenterRotate.angle,CPoint(Buffer->w/2,Buffer->h/2));	

	Filter1.Main(Buffer); 

	if(IsBckgFile) 
	{
		Filter1.Main(Background);
		for(int i=0;i<Zones.GetSize();i++)
		{
			if(Zones[i]) Zones[i]->Create();
		}
	}

	return ERR_OK;
}


void DjvuPic::ShiftRight(MyRect& Rect, ShiftParams Param)
{
	int j,sum;
	bool move; int top=Rect.top, bottom=Rect.bottom;
	int bckg=Param.bckg, www=Param.www, zh=Param.zz, dy=Param.dd.y,dx=Param.dd.x;

	do
	{			
		move=false; Rect.top=top; Rect.bottom=bottom;						
		for(j=0;j<zh && !move;j+=www)
		{			
			sum=GetBckg(Rect);
			if(sum<bckg) 
			{
				if(Rect.right+dx<=Buffer->w) {move=true; Rect+=CPoint(dx,0);}
				else {Rect.right=Buffer->w; move=false;}
			}
			else Rect+=CPoint(0,www);
		}			
	}	
	while(move);
}

void DjvuPic::ShiftLeft(MyRect& Rect, ShiftParams Param)
{
	int j,sum;
	bool move; int top=Rect.top, bottom=Rect.bottom;
	int bckg=Param.bckg, www=Param.www, zh=Param.zz, dy=Param.dd.y,dx=Param.dd.x;

	do
	{
		move=false; Rect.top=top; Rect.bottom=bottom;						
		for(j=0;j<zh && !move;j+=www)
		{				
			sum=GetBckg(Rect);
			if(sum<bckg) 
			{
				if(Rect.left-dx>=0) {move=true; Rect+=CPoint(-dx,0);}
				else {Rect.left=0; move=false;}
			}
			else Rect+=CPoint(0,www);
		}		
	}	
	while(move);
}

void DjvuPic::ShiftTop(MyRect& Rect, ShiftParams Param)
{
	int j,sum;
	bool move; int left=Rect.left, right=Rect.right;
	int bckg=Param.bckg, www=Param.www, zw=Param.zz, dy=Param.dd.y,dx=Param.dd.x;

	do
	{			
		move=false; Rect.left=left; Rect.right=right;						
		for(j=0;j<zw && !move;j+=www)
		{			
			sum=GetBckg(Rect);
			if(sum<bckg) 
			{
				if(Rect.top-dy>=0) {move=true; Rect+=CPoint(0,-dy);}
				else {Rect.top=0; move=false;}
			}
			else Rect+=CPoint(www,0);
		}			
	}	
	while(move);


}

void DjvuPic::ShiftBottom(MyRect& Rect, ShiftParams Param)
{
	int j,sum;
	bool move; int left=Rect.left, right=Rect.right;
	int bckg=Param.bckg, www=Param.www, zw=Param.zz, dy=Param.dd.y,dx=Param.dd.x;

	do
	{			
		move=false; Rect.left=left; Rect.right=right;						
		for(j=0;j<zw && !move;j+=www)
		{			
			sum=GetBckg(Rect);
			if(sum<bckg) 
			{
				if(Rect.bottom+dy<=Buffer->h) {move=true; Rect+=CPoint(0,dy);}
				else {Rect.bottom=Buffer->h; move=false;}
			}
			else Rect+=CPoint(www,0);
		}			
	}	
	while(move);
}


void DjvuPic::NormalizeRect(CRect& rect)
{
	if(rect.left>rect.right) {int t=rect.left; rect.left=rect.right; rect.right=t;}
	if(rect.bottom<rect.top) {int t=rect.top; rect.top=rect.bottom; rect.bottom=t;}
	if(rect.left<0) rect.left=0;
	if(rect.top<0) rect.top=0;
	if(rect.right>PicDim.cx) rect.right=PicDim.cx;
	if(rect.bottom>PicDim.cy) rect.bottom=PicDim.cy;
}

void DjvuPic::GetPicDim()
{
	BMPINFO temp;
	if(Ext=="None") {ASSERT(0); return;}
	if(Ext==".jpg") temp=MyJPEG::ReadHeader(FrgFile); 
	if(Ext==".tif") temp=MyTIFF::ReadHeader(FrgFile); 
	if(Ext==".png") temp=MyPNG::ReadHeader(FrgFile); 
	if(Ext==".bmp") temp=MyBMP::ReadHeader(FrgFile); 	
	PicDim.SetSize(temp.bmiHeader.biWidth,temp.bmiHeader.biHeight);	
}

int DjvuPic::CheckDjvu(void)
{
	int ret; WIN32_FIND_DATA FindFileData; ; HANDLE FFF;
	CString SearchPattern; SearchPattern.Format("%s\\%s*.djvu",Path,Name);
	if((FFF=FindFirstFile(LPCSTR(SearchPattern),&FindFileData))!=INVALID_HANDLE_VALUE) ret=1;
	else ret=0;
	return ret;
}
