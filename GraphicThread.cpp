#include "stdafx.h"
#include ".\graphicthread.h"
#include "resource.h"
#include "testWMFDlg.h"
#include "rotatefilter.h"

IMPLEMENT_DYNCREATE(GraphicThread, MyThread)

BEGIN_MESSAGE_MAP(GraphicThread, MyThread)
	//{{AFX_MSG_MAP(VoltmeterThread)
	ON_THREAD_MESSAGE(UM_LOAD_PIC,OnLoadPic)		
	ON_THREAD_MESSAGE(UM_UNLOAD_PIC,OnUnLoadPic)		
	ON_THREAD_MESSAGE(UM_ORIGINAL,OnOriginal)		
	ON_THREAD_MESSAGE(UM_BLUR,OnBlur)		
	ON_THREAD_MESSAGE(UM_BW,OnBW)		
	ON_THREAD_MESSAGE(UM_ROTATE,OnRotate)		
	ON_THREAD_MESSAGE(UM_UPDATE,OnUpdate)		
	ON_THREAD_MESSAGE(UM_PRINT,OnPrint)	

	//}}AFX_MSG_MAP
	// Standard file based document commands	
END_MESSAGE_MAP()


GraphicThread::GraphicThread(void)
{
    bckg=0;	
}

GraphicThread::~GraphicThread(void)
{
}


BOOL GraphicThread::InitInstance()
{	
	ParentWnd=CWnd::FromHandle(ParentHWND);
	OWnd1.Create(NULL,"Output",WS_CHILD | WS_VISIBLE | WS_BORDER ,CRect(0,0,700,980),ParentWnd,IDS_OWND1,0);	
	m_pMainWnd=&OWnd1;

	OWnd1.Parent=this;
	accel=LoadAccelerators(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_ACCELERATOR1));
	return MyThread::InitInstance();
}

int GraphicThread::ExitInstance()
{	
	return MyThread::ExitInstance();
}


int GraphicThread::Main(WPARAM wParam, LPARAM lParam)
{
	int ret=ERR_OK;
	int (GraphicThread::* func)(Message*)=0;
	switch(wParam)
	{
	case BCKG:	func=&GraphicThread::MainBCKG; break;
	case FRG:	func=&GraphicThread::MainFRG; break;
	case BW:	func=&GraphicThread::MainBW; break;	
	case BLUR:  func=&GraphicThread::MainBlur; break;	
	case ROTLINE:  func=&GraphicThread::MainRotLine; break;	
	}
    if(func) 
	{	
		ret=(this->*func)((Message*)lParam);
	}
    return ret;
}
//------------------------------------------------
void GraphicThread::DrawShape(AbstractGraphics *shape,BMPanvas* dest,double zoom)
{
	if(!(shape->Draw(dest,zoom)))
	{		
		shape->zOrder=Shapes.Add(shape)+1;								
		Eraser.Add(shape->Rgn);
	}	
}
void GraphicThread::EraseShape(AbstractGraphics *shape,BMPanvas* dest,CRect rect)
{	
	shape->Erase(dest,rect); 
	if(!shape->ActiveMask) delete shape;								
}
//------------------------------------------------
int GraphicThread::MainBlur(Message* msg)
{	
	int i,ret=ERR_OK;

	BMPanvas* dest=OWnd1.buffer; DjvuPic* CurPic=OWnd1.CurPic;	
	GausianBlur filter1; filter1.SetKernel(CurPic->GBparam.sigma,CurPic->GBparam.KernelSize);
	
	for(i=0; i<Shapes.GetSize() && !Stop && !ret; i++) EraseShape(Shapes[i],dest,Eraser[i]);
	Shapes.RemoveAll(); Eraser.RemoveAll();      

	for(i=0;i<OWnd1.CurPic->Zones.GetSize() && !Stop;i++)
	{
		BckgZone* Zone=OWnd1.CurPic->Zones[i];	
		CRect rect=Zone->GetFillRect();
		Zone->CopyOriginal(); 
		filter1.Main(Zone->buffer,rect);
		DrawShape(Zone,dest,1.);
	}

    OWnd1.DrawMode.Rst(BLUR);			
	if(msg) delete msg;	
	
    return ret;
}
//====================================================

int GraphicThread::MainFRG(Message* msg)
{	
	int i,ret=ERR_OK;
	
	BMPanvas* dest=OWnd1.buffer; 
	///ERASE////
	for(i=0; i<Shapes.GetSize() && !Stop && !ret; i++) EraseShape(Shapes[i],dest,Eraser[i]);
	Shapes.RemoveAll(); Eraser.RemoveAll();        	
    ///DRAW///
	for(i=0;i<OWnd1.CurPic->Zones.GetSize() && !Stop;i++) DrawShape(OWnd1.CurPic->Zones[i],dest,1.);

	DrawShape(&OWnd1.RotateLine,dest,1.);
//	DrawShape(&OWnd1.CurPic->CropZone,dest,1.);

	OWnd1.DrawMode.Rst(FRG);			
	if(msg) delete msg;	
	
    return ret;
}
int GraphicThread::MainBCKG(Message* msg)
{
	int i,ret=ERR_OK;
	
	BMPanvas* dest=OWnd1.buffer; AbstractGraphics *shape;
	BMPanvas* src=OWnd1.CurPic->Buffer;	
	 
	src->CopyTo(dest,CPoint(0,0),OWnd1.CurPic->ViewRgn);
	for(i=Shapes.GetSize()-1; i>=0 && !Stop; i--) {shape=Shapes[i]; if(!shape->ActiveMask) delete shape;}
	Shapes.RemoveAll(); Eraser.RemoveAll();

	for(i=0;i<OWnd1.CurPic->Zones.GetSize() && !Stop;i++) DrawShape(OWnd1.CurPic->Zones[i],dest,1.);

//	DrawShape(&OWnd1.CurPic->CropZone,dest,1.);

	OWnd1.DrawMode.Rst(BCKG);
    if(msg) delete msg;	
	
    return ret;
}
int GraphicThread::MainBW(Message* msg)
{
	int ret;
	BW_FloydSteinberg filter2; filter2.threshold=160;	
	filter2.Main(OWnd1.CurPic->Buffer);	
	ret=MainBCKG(msg);	
	OWnd1.DrawMode.Rst(BW);
    return ret;
}

int GraphicThread::MainRotLine(Message*)
{
	int ret=0;
    return ret;
}


void GraphicThread::OnLoadPic(WPARAM wParam, LPARAM lParam )
{	
	DjvuPic* t=(DjvuPic*)wParam;	
	OWnd1.LoadPic(t);					    	
}

void GraphicThread::OnUnLoadPic(WPARAM wParam, LPARAM lParam )
{		
	OWnd1.UnLoadPic();					    	
}

void GraphicThread::OnOriginal(WPARAM wParam, LPARAM lParam )
{
	OWnd1.CurPic->SetBuffer(OWnd1.CurPic->FullName,!wParam);
	OWnd1.DrawMode.Set(BCKG);	OWnd1.Draw();
}

void GraphicThread::OnBlur(WPARAM wParam, LPARAM lParam )
{
	OWnd1.DrawMode.Set(BLUR); OWnd1.Draw();		
}

void GraphicThread::OnBW(WPARAM wParam, LPARAM lParam )
{
	OWnd1.DrawMode.Set(BW); OWnd1.Draw();		
}

void GraphicThread::OnRotate(WPARAM wParam, LPARAM lParam )
{
	OWnd1.CurPic->Rotate();
	OWnd1.DrawMode.Set(BCKG); OWnd1.Draw();		
}

void GraphicThread::OnUpdate(WPARAM wParam, LPARAM lParam )
{
	OWnd1.UpdateNow();
}

void GraphicThread::OnPrint(WPARAM wParam, LPARAM lParam )
{	
	/*
	//CTestWMFDlg *dlg1=(CTestWMFDlg *)(wParam);	
	OWnd1.UnLoadPic();	
	
	CPrintDialog *dlg2=(CPrintDialog *)(lParam);
	HDC PrinterDC=dlg2->GetPrinterDC();	
	CDC PrintDC1; 	PrintDC1.Attach(PrinterDC);

	int i,j,num=0;
	DOCINFO infStru;		
	ZeroMemory (&infStru, sizeof (DOCINFO));
	CString title="step 45 ";
	infStru.cbSize = sizeof (DOCINFO);		
	infStru.lpszOutput=NULL; CPoint offset;

	DjvuPicArray *Pics=(DjvuPicArray *)wParam; DjvuPicArray &Pics1=*(Pics);
	int *ind=0; 

	RotateFilter Filter1; BWFilter Filter2; GausianBlur Filter3; 
	for(j=0;j<Pics1.GetSize();j++)
	{
		DjvuPic &CurPic=*(Pics1[j]);
		ArrayOfZones& Zones=*(CurPic.GetZones());
		
		CString name; name.Format("%d",j);//"=CurPic.Name;
		infStru.lpszDocName=name; PrintDC1.StartDoc(&infStru); PrintDC1.StartPage();		

		CRect tr; PrintDC1.GetClipBox(&tr);
        
		CurPic.CenterRotate.Q=RotateFilterParam::HI; CurPic.Create(); 
				
		for(i=0;i<Zones.GetSize();i++)
		{
			BckgZone* Zone=Zones[i];
			Filter3.SetKernel(CurPic.GBparam.sigma,CurPic.GBparam.KernelSize);
			Filter3.Main(Zone->buffer);			
		}		

		for(i=0;i<Zones.GetSize();i++)
		{
			BckgZone* Zone=Zones[i];			
			PatBlt(CurPic.Buffer->GetDC(),Zone->Rgn.left,Zone->Rgn.top,Zone->Rgn.Width(),Zone->Rgn.Height(),WHITENESS);
		}
		offset.x=(tr.Width()-CurPic.Buffer->w)/2; offset.y=(tr.Height()-CurPic.Buffer->h)/2; 		
		BitBlt(PrinterDC,offset.x,offset.y,CurPic.Buffer->w,CurPic.Buffer->h,CurPic.Buffer->GetDC(),0,0,SRCCOPY);				

        for(i=0;i<Zones.GetSize();i++)
		{			
			CRect t4=Zones[i]->Rgn; t4.OffsetRect(offset);	
			BitBlt(PrinterDC,t4.left,t4.top,t4.Width(),t4.Height(),Zones[i]->buffer->GetDC(),0,0,SRCCOPY);	
		}
			
		PrintDC1.EndPage(); PrintDC1.EndDoc();	
		CurPic.Destroy();		
	}
	PrintDC1.Detach();	
	if(!ind) delete[] ind;

	StopWaiting=false;
	WIN32_FIND_DATA FindFileData;
	CString SearchPattern; 

	Parent->PostThreadMessage(UM_WAITING,wParam,lParam);	
	while(!StopWaiting)
	{
		SearchPattern.Format("%s\\%d.djvu",DJVUOutFldr,j-1);
		if(FindFirstFile(LPCSTR(SearchPattern),&FindFileData)!=INVALID_HANDLE_VALUE) break;
		Parent->PostThreadMessage(UM_GET_PATH,wParam,lParam);
		Sleep(1000);
	}
	Parent->PostThreadMessage(UM_PRINT,wParam,lParam);
	*/
}


BOOL GraphicThread::PreTranslateMessage(MSG* pMsg) 
{
	TranslateAccelerator( m_pMainWnd->m_hWnd, accel,pMsg);
	
	return MyThread::PreTranslateMessage(pMsg);
}

int GraphicThread::TerminateThread()
{
    StopWaiting=true;
	OWnd1.DestroyWindow();
	return MyThread::TerminateThread();
}