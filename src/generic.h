#if !defined generic_h
#define generic_h

#define M_PI 3.14159
#define DEGREE (M_PI/180.)

#include "bmpanvas.h"
#include "afxmt.h"
#include "atlimage.h"
#include "afxtempl.h"

class MyMultiLock;

extern CArray<MyMultiLock*,MyMultiLock*> Locks;

class MyMultiLock
{
public:
	CMultiLock *lock;

	MyMultiLock() {lock=0;}
	~MyMultiLock() {if(lock) delete lock;}
    MyMultiLock(CSyncObject* ppObjects[ ], DWORD dwCount)
	{
        lock=new CMultiLock(ppObjects,dwCount);
	}    
};

class FLAG
{
	int flag;
public:
	int Set(int f)	{int i=flag; flag|=f; return i;};
	int Rst(int f)  {int i=flag; flag&=(~f); return i;};
	int Get() {return flag;}    
	int Chk(int f) {return flag&f;}
	int Eql(int f) {int i=flag; flag=f; return i;}
	int All() {return flag;}
};


class MyRect: public CRect
{
public:
	MyRect() {}
	MyRect(int l,int t,int r,int b):CRect(l,t,r,b) {}
	MyRect(CRect& s): CRect(s) {}
	void operator /=(double z)
	{
		left=(int)(left/z);	right=(int)(right/z);	
		top=(int)(top/z);	bottom=(int)(bottom/z);			
	}
	void operator *=(double z)
	{
		left=(int)(left*z);	right=(int)(right*z);	
		top=(int)(top*z);	bottom=(int)(bottom*z);	
	}
	MyRect operator *(double z)
	{
		MyRect t;
		t.left=(int)(left*z);	t.right=(int)(right*z);	
		t.top=(int)(top*z);	t.bottom=(int)(bottom*z);	return t;
	}
	CPoint BottomRight() {CPoint t; t.x=right-1; t.y=bottom-1; return t;}
	double Ratio() {return (double)Width()/Height();}
	CPoint GetCenter() {CPoint ret; ret.x=left+Width()/2; ret.y=top+Height()/2; return ret;}
};

class MyPoint: public CPoint
{
public:
	MyPoint(): CPoint() {}
	MyPoint(int x,int y): CPoint(x,y) {}
	MyPoint(CPoint&a): CPoint(a) {}
	MyPoint operator /(double z)
	{
		MyPoint t; t.x=(int)(x/z);	t.y=(int)(y/z); return t;
	}
	MyPoint operator *(double z)
	{
		MyPoint t; t.x=(int)(x*z);	t.y=(int)(y*z); return t;
	}
	MyPoint operator -(MyPoint &z)
	{
		MyPoint t; t.x=x-z.x;	t.y=y-z.y; return t;
	}	
	MyPoint operator -(CPoint &z)
	{
		MyPoint t; t.x=x-z.x;	t.y=y-z.y; return t;
	}	
	void operator *=(double z)
	{
		x=(int)(x*z);	y=(int)(y*z);			
	}
	
};

class RealPoint
{
public:
	double x,y;
};

class AbstractGraphics
{
protected:	
	AbstractGraphics() {enable=true; zOrder=-1;}
public:
	int enable, zOrder;
	int ActiveMask; MyRect Rgn;
	CMutex GraphLock;

	virtual int Draw(BMPanvas* canvas,double)
	{
//		CFont t; t.CreatePointFont(240,"arial"); canvas->SelectObject(&t);
//		CString T; T.Format("%d",ActiveMask);
//		canvas->TextOut(Rgn.left,Rgn.top,T);	
		return 0;
	};
	virtual void Erase(BMPanvas*,CRect)=0;	
	virtual ~AbstractGraphics() {};
	virtual void Destroy()=0;
};


class Cross: public AbstractGraphics
{
public:
	COLORREF last_color; MyPoint last_coord, coord;
	COLORREF cur_color;
	virtual int Draw(CDC*,double);
	virtual void Erase(BMPanvas*,CRect);	
	virtual void Destroy() {};
};

class Line: public AbstractGraphics
{
public:
	COLORREF cur_color; 	
	MyPoint Beg,End;

	virtual int Draw(BMPanvas*,double);
	virtual void Erase(BMPanvas*,CRect);
	virtual void Destroy() {};
};

class MyRectangle: public AbstractGraphics
{
public:
	COLORREF cur_color; 	
	CRect Rect;
	CBrush brsh; CPen Pen1;

	virtual int Draw(BMPanvas*,double);
	virtual void Erase(BMPanvas*,CRect);
	virtual void Destroy() {};
	void Serialize( CArchive& ar );
	//MyRectangle():AbstractGraphics() {}
};

class GausianBlurParam 
{
public:
	double sigma; int KernelSize;
	GausianBlurParam() {sigma=0;KernelSize=0;}
	void Serialize( CArchive& ar )
	{	
		if( ar.IsStoring() )
		{
			ar << sigma << KernelSize;
		}
		else
		{
			ar >> sigma >> KernelSize;
		}
	};
};

struct CorrData {double mean,mean2; double sum,sum2;};
struct CorrelationData {double r,s;};

class Correlator
{
public:
	int Apperture,N;
	BYTE *X;	
	CorrData XData, YData;

	Correlator() {X=0; Apperture=0;};
	~Correlator () 
	{
		if(X) 
		{
			delete[] X;
			X=0;
		} 
	}
	void SetX(BYTE*, int);
	CorrelationData GetCorrelation(BYTE *);
};


struct RotateFilterParam 
{
	double angle;
	enum Quality {HI=1,LOW=2} Q;
	RotateFilterParam() {angle=0.;}
	void Serialize( CArchive& ar )
	{	
		if( ar.IsStoring() )
		{
			ar << angle;
		}
		else
		{
			ar >> angle;
		}
	};
};

#endif
