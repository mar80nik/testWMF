// DjvuPic.h: interface for the DjvuPic class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DJVUPIC_H__ECDC651B_060C_4614_82FE_D0B34CE9E4CA__INCLUDED_)
#define AFX_DJVUPIC_H__ECDC651B_060C_4614_82FE_D0B34CE9E4CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "generic.h"
#include "bmpanvas.h"
#include "Afxtempl.h"
#include "bckgzone.h"
#include "MyJPEG.h"
class OutputWnd;
class Message;

typedef CArray<BckgZone*,BckgZone*> ArrayOfZones;

struct ShiftParams
{	
	int bckg, www, zz;
	CPoint dd;
};

class DjvuPic: public CMutex
{
friend class BckgZone;
friend class AttachedBckgZone;
friend class GraphicThread;

void ShiftLeft(MyRect& Rect, ShiftParams Param);
void ShiftRight(MyRect& Rect, ShiftParams Param);
void ShiftTop(MyRect& Rect, ShiftParams Param);
void ShiftBottom(MyRect& Rect, ShiftParams Param);

protected:
	OutputWnd *Parent;
	MyRect ViewRgn;
	FLAG ZonesActive;	
	ArrayOfZones Zones;	
	ShiftParams Param;
	
	int ProcessFullName();

public:
	enum CreateMode {IN_MEMORY,NORMAL};

	MyRect BckgRgn; //MyRectangle CropZone; 	
	BMPanvas *Buffer, *Background, *Foreground;
	GausianBlurParam GBparam;
	CString FullName, Path,Name, Ext, FrgFile, BckgFile, DjvuFile;	bool IsFrgFile, IsBckgFile, IsDjvuFile;
	RotateFilterParam CenterRotate;
	CSize PicDim;

	DjvuPic(CString name,OutputWnd *parent);	
	virtual ~DjvuPic();

	void Create(CreateMode mode);
	void Destroy();
	Message* Lock(int wt=0);

	void FitZone1(CRect& , int thrsh);
	int GetBckg(CPoint&);
	void GetPoints(CPoint &CurPoint, BYTE* arr1, int app);	

	void DeleteZone(int i);	
	int AddZone(BckgZone*);
	
	void operator =(DjvuPic& src);
	int GetBckg(MyRect&);
	int IsInZone(CPoint&);

	void Serialize( CArchive& ar );
	
	int SetBuffer(CString& fullName,bool check=true);
	int SetForeground();
	int SetBackground();
	void SetActiveZone(int);
	BckgZone* GetActiveZone(int* N=0);
	MyRect GetViewRgn(void);
	void SetViewRgn(MyRect);
	ArrayOfZones* GetZones(void);
	int Rotate(void);	
	void NormalizeRect(CRect& rect);
	void GetPicDim();
	int CheckDjvu(void);
};

class DjvuPicArray: public CArray<DjvuPic*,DjvuPic*>
{
public:
    CMutex ArrayMutex;
};


#endif // !defined(AFX_DJVUPIC_H__ECDC651B_060C_4614_82FE_D0B34CE9E4CA__INCLUDED_)
