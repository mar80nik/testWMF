#pragma once
#include "generic.h"


class DjvuPic;

class BckgZone :
	public AbstractGraphics
{
protected:
	//CRect last_Rgn; COLORREF last_color;
public:		
	DjvuPic *Parent;
	BMPanvas *buffer;
	COLORREF active_color,inactive_color;		
	int size_mode;	
	enum {NormalZone,AttachedZone} ZoneType;

	void operator = (BckgZone& src);

	int IsSizeZone(MyPoint&);
	virtual int SizeZone(MyPoint&,MyPoint&);	
	virtual int MoveZone(MyPoint &RefPoint, MyPoint &CurPoint);
	
	BckgZone(DjvuPic* parent);	
	virtual int Draw(BMPanvas*,double);
	virtual void CopyOriginal();	
	virtual void Erase(BMPanvas*,CRect);	
	virtual void Serialize( CArchive& ar );
	virtual int Create(CRect rect=CRect(0,0,0,0));	
	virtual void Destroy();	
	
	virtual ~BckgZone(void);
	CRect GetFillRect(void);
};

class AttachedBckgZone: public BckgZone
{
public:	
	CString Name;

	AttachedBckgZone(CString name, DjvuPic* parent);
	virtual int Draw(BMPanvas*,double);	
	virtual int Create(CRect rect=CRect(0,0,0,0));	
	virtual void CopyOriginal();
	virtual int SizeZone(MyPoint&,MyPoint&);	
	
	virtual int MoveZone(MyPoint &RefPoint, MyPoint &CurPoint);
	virtual void Serialize( CArchive& ar );
	//virtual void Create(DjvuPic* parent);		
};
