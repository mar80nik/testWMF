// testWMFDlg.cpp : implementation file
//

#include "stdafx.h"
#include "testWMF.h"
#include "testWMFDlg.h"
#include "deskewfilter.h"
#include "niblackfilter.h"
#include "myprintdlg.h"
#include "grayfilter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "gausianblur.h"
#include "my_timer.h"
#include "rotatefilter.h"
#include ".\testwmfdlg.h"
#include "Psapi.h"

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

MyTimer Timer1;

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestWMFDlg dialog

CTestWMFDlg::CTestWMFDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestWMFDlg::IDD, pParent)	
	, Errors(0)
	, LoadOriginal(FALSE), dlg1(false)	
{
	//{{AFX_DATA_INIT(CTestWMFDlg)
	threshold = 127;
	GBsigma = 3.0;
	GBn = 0;
	bckg = 1020;
//	level = 0.98;
	angle = 0.0;
	time = 0.0;
	interp = FALSE;
	edges = FALSE;
	edge_level = 50;
//	kk = 0.2;
	canvasW = 1263;
	canvasH = 1977;
	DJVUOutFldr="g:\\!tmp\\";	
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SaveListFile="";
	InputFldr=CString(""); ThreadsPID=1;
}

void CTestWMFDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestWMFDlg)
	DDX_Control(pDX, IDC_LIST3, FileList1);
	DDX_Control(pDX, IDC_LIST2, InfoList);
	DDX_Text(pDX, IDC_EDIT4, GBsigma);
	DDV_MinMaxDouble(pDX, GBsigma, 1.e-002, 101.);
	DDX_Text(pDX, IDC_EDIT5, GBn);
	DDV_MinMaxInt(pDX, GBn, 0, 101);
	DDX_Text(pDX, IDC_EDIT6, bckg);
	DDX_Text(pDX, IDC_EDIT8, angle);
	DDX_Check(pDX, IDC_CHECK2, edges);
	DDX_Text(pDX, IDC_EDIT1, edge_level);
	//	DDX_Text(pDX, IDC_EDIT2, kk);
	DDX_Text(pDX, IDC_EDIT10, DJVUOutFldr);	
	DDX_Check(pDX, IDC_CHECK3, LoadOriginal);
	DDX_Control(pDX, IDC_CHECK3, OriginalCheck);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT2, InputFldr);
	DDX_Control(pDX, IDC_LIST4, ThreadsList);
}

BEGIN_MESSAGE_MAP(CTestWMFDlg, CDialog)
	//{{AFX_MSG_MAP(CTestWMFDlg)	
	ON_WM_SYSCOMMAND()
//	ON_WM_MOUSEMOVE()
//	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnOpen)
	ON_WM_CREATE()
//	ON_BN_CLICKED(IDC_BUTTON4, OnApplyButton)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_BN_CLICKED(IDC_BUTTON5, OnBlurButton)
	ON_BN_CLICKED(IDC_BUTTON6, OnFixButton)
	ON_BN_CLICKED(IDC_BUTTON8, OnSaveList)
	ON_BN_CLICKED(IDC_BUTTON9, OnRestoreList)
	ON_BN_CLICKED(IDC_BUTTON10, OnDeskew)
//	ON_BN_CLICKED(IDC_BUTTON11, OnFitCanvas)
//	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST3, OnBegindragList3)

	ON_COMMAND(ID_NEXT123,OnCommandNext)
	ON_COMMAND(ID_PREV123,OnCommandPrev)
	ON_COMMAND(ID_PRINT,OnButton1)	
	ON_COMMAND(ID_SELECT_ALL,OnSelectAll)	
	ON_COMMAND(ID_ORIGINAL,OnOriginal)
	//}}AFX_MSG_MAP	
	ON_BN_CLICKED(IDC_BUTTON13, OnBnClickedButton13)
	
	ON_BN_CLICKED(IDC_BUTTON14, OnBnClickedButton14)
	ON_BN_CLICKED(IDC_CHECK3, OnBnClickedCheck3)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
	ON_EN_KILLFOCUS(IDC_EDIT4, OnEnKillfocusEdit4)
	ON_EN_KILLFOCUS(IDC_EDIT5, OnEnKillfocusEdit5)
	ON_BN_CLICKED(IDC_BUTTON15, OnBnClickedButton15)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	
	ON_MESSAGE(UM_PRINT,OnPrint)
	ON_MESSAGE(UM_PRINT_DIALOG,OnPrintDialog)
	
	ON_MESSAGE(UM_WAITING,OnWaiting)	
	ON_MESSAGE(UM_UPDATE,OnUpdate)	
	ON_MESSAGE(UM_PRINTERLOCK,OnPrinterLock)	
	ON_MESSAGE(UM_UPDATE_THREAD_LIST,OnThreadListUpdate)	
	
	ON_BN_CLICKED(IDC_BUTTON16, OnBnClickedButton16)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST3, OnLvnColumnclickList3)
	ON_BN_CLICKED(IDC_BUTTON7, OnBnClickedButton7)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestWMFDlg message handlers

BOOL CTestWMFDlg::OnInitDialog()
{
	CDialog::OnInitDialog();	
	Graph->ParentHWND=GetSafeHwnd();
	
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	FileList1.Parent=ThreadsList.Parent=this;
	FileList1.CreateColumns();
	DWORD ExStyle = FileList1.GetExtendedStyle();
	ExStyle |= LVS_EX_FULLROWSELECT;
	FileList1.SetExtendedStyle(ExStyle);
	FileList1.DragAcceptFiles(true);

	ExStyle = ThreadsList.GetExtendedStyle();
	ExStyle |= LVS_EX_FULLROWSELECT;
	ThreadsList.SetExtendedStyle(ExStyle);


	ThreadsList.CreateColumns();
	ThreadsList.UpdateList();

	Graph->bckg=bckg;
	Graph->ResumeThread();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestWMFDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}


UINT WorkThreadMain( LPVOID pParam )
{
	ThreadParams *Params=(ThreadParams *)pParam;
	Params->_Main();
	return 0;
}

LRESULT CTestWMFDlg::OnPrintDialog(WPARAM wParam, LPARAM lParam )
{
	for(int i=0;i<Pics.GetSize();i++) Pics[i]->Destroy();	

	DjvuPicArray *Pics, *Pics1; Pics1=new DjvuPicArray();

	FileList1.GetSelectedItems(Pics1);
	Graph->PostThreadMessage(UM_UNLOAD_PIC,0,0);
	CString T; T.Format("Completed 0%%"); SetWindowText(T);
	PrintThreadParams* Params;

	for(int i=0;i<4;i++)
	{
		Params=new PrintThreadParams(); Pics=new DjvuPicArray();
		Params->Name.Format("Print%d",i+1); Params->PID=ThreadsPID++; Params->Parent=AfxGetApp();
		Params->Path=&DJVUOutFldr; Params->Pics=Pics;
		for(int j=i;j<Pics1->GetSize();j+=4) Pics->Add((*Pics1)[j]);
		if(Pics->GetSize()!=0) Threads.Add(Params);
		else {delete Params; delete Pics;}
		int a=5;
	}

	ThreadsList.UpdateList();
	
	for(int i=0;i<Threads.GetSize();i++)
	{
		ThreadParams *Params=Threads[i];
		if(Params->Status==Preparing) AfxBeginThread(WorkThreadMain,Params,THREAD_PRIORITY_IDLE);	
		Sleep(1000);
	}
	
	delete Pics1; FileList1.SetFocus();
    return 0;
}

void CTestWMFDlg::OnButton1() 
{
	OnSaveList();	UpdateData(); 
	OutputWnd &OWnd1=Graph->OWnd1;
	OWnd1.UnLoadPic();

	OnPrintDialog(0,0);

//	if(dlg1.DoModal()==IDCANCEL) {FileList1.SetFocus(); return;}
//	PrintDialogThreadParams* Params=new PrintDialogThreadParams();
//	Params->Name="PrintDialog"; Params->PID=ThreadsPID++; Params->Parent=AfxGetApp();
//	Params->PrintDlg=&dlg1;	
//	Threads.Add(Params);

//	ThreadsList.UpdateList();
	
//	AfxBeginThread(WorkThreadMain,Params,THREAD_PRIORITY_BELOW_NORMAL);	
}


void CTestWMFDlg::OnOpen() 
{
	OutputWnd &OWnd1=Graph->OWnd1;UpdateData();

	if(InputFldr=="")
	{
		CString fltr="BMP Files (*.bmp)|*.bmp|All Files (*.*)|*.*||";
		CFileDialog dlg1(true,NULL,NULL,OFN_ALLOWMULTISELECT,fltr);
		char * buf=new char[10000]; memset(buf,0,10000); DjvuPic *temp;
		dlg1.m_ofn.lpstrFile=buf; dlg1.m_ofn.nMaxFile=10000;
		if(dlg1.DoModal()==IDOK)
		{
			POSITION pos=dlg1.GetStartPosition();
			while(pos)
			{
				temp=new DjvuPic(dlg1.GetNextPathName(pos),&OWnd1) ;					
				Pics.Add(temp);
			}
			FileList1.UpdateList();
		}
		delete[] buf;
	}
	else
	{
		CString T,SearchPattern=InputFldr+"\\*.jpg"; HANDLE SearchHandle; int n=0;
		WIN32_FIND_DATA FindFileData; DjvuPic *temp;

		if( (SearchHandle=FindFirstFile(LPCSTR(SearchPattern),&FindFileData) )!=INVALID_HANDLE_VALUE)
		{            
			T=InputFldr+"\\"+FindFileData.cFileName; 
			temp=new DjvuPic(T,&(OWnd1)); 
			Pics.Add(temp); 
			n++;

			while(FindNextFile(SearchHandle,&FindFileData))
			{
				T=InputFldr+"\\"+FindFileData.cFileName; 
				temp=new DjvuPic(T,&(OWnd1)); Pics.Add(temp); n++;								
			}
			FindClose(SearchHandle);
		}	
		if(n) SaveListFile=InputFldr+"\\list.dat"; 		
	
		T.Format("Found %d files at %s",n,InputFldr);
		InfoList.AddString(T);  
		FileList1.UpdateList();
	}		
}

int CTestWMFDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	ShowWindow(SW_SHOWMAXIMIZED);
	
	return 0;
}

void CTestWMFDlg::PostNcDestroy() 
{
	for(int i=0;i<Pics.GetSize();i++)
	{
		delete Pics[i];	
	}

	for(int i=0;i<Threads.GetSize();i++)
	{
		Threads[i]->StopWaiting=true;
		WaitForSingleObject(Threads[i]->Terminator,1000);
		delete Threads[i];
	}	
	
	CDialog::PostNcDestroy();
}

void CTestWMFDlg::OnButton3() 
{
	OutputWnd &OWnd1=Graph->OWnd1;
	for(int i=0;i<Pics.GetSize();i++)
	{
		Pics[i]->Destroy();
		delete Pics[i];	
	}
	Pics.RemoveAll();
	OWnd1.CurPic=0;
	FileList1.UpdateList();
	SaveListFile="";
}

void CTestWMFDlg::OnBlurButton() 
{
	DjvuPic* CurPic=Graph->OWnd1.CurPic;
	if(!CurPic) return; 
	UpdateData();
	
	CurPic->GBparam.sigma=GBsigma;
	CurPic->GBparam.KernelSize=GBn;
	
	FileList1.UpdateList();
	Graph->PostThreadMessage(UM_BLUR,0,0); 
}

void CTestWMFDlg::OnFixButton() 
{
	DjvuPic* CurPic=Graph->OWnd1.CurPic;
	if(!CurPic) return; 
	UpdateData(); 	
	
	CurPic->CenterRotate.angle=angle;
	CurPic->CenterRotate.Q=RotateFilterParam::LOW;		
	FileList1.UpdateList();
	
	Graph->PostThreadMessage(UM_ROTATE,0,0); 
	
	
}


void CTestWMFDlg::OnCommandNext()
{
	//CArray<DjvuPic*,DjvuPic*> &pics=*Pics;

	int &n=FileList1.HitItem;
	if(n>=0 && n<Pics.GetSize()-1) 
	{
		n++;			
		FileList1.SendMessage(WM_LBUTTONDBLCLK,1234,0);
	}

	//return true;
}

void CTestWMFDlg::OnCommandPrev()
{
	int &n=FileList1.HitItem;
	if(n>0) 
	{
		n--;		
		FileList1.SendMessage(WM_LBUTTONDBLCLK,1234,0);
	}
	//return true;
}


void CTestWMFDlg::OnOK() 
{
	// TODO: Add extra validation here
	
//	CDialog::OnOK();
}

void CTestWMFDlg::OnSaveList() 
{
	CFile f; CString ret="SaveList Error"; ;
	CString fltr="Dat Files (*.dat)|*.dat|All Files (*.*)|*.*||";
	CFileDialog dlg1(false,"dat","list",OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,fltr);
	if(SaveListFile=="")
	{
		if(dlg1.DoModal()==IDOK) SaveListFile=dlg1.GetFileName();
		else SaveListFile="";
	}
	else
	{
		if(f.Open(SaveListFile,CFile::modeCreate | CFile::modeWrite))
		{
			{
			CArchive ar(&f,CArchive::store);
			int n=Pics.GetSize(); ar << n;
			for(int i=0;i<n;i++) (Pics[i])->Serialize(ar);						
			ret="SaveList OK"; 
			}
			f.Close();	
		}			
	}	
	InfoList.AddString(ret);
}

void CTestWMFDlg::OnRestoreList() 
{
	SaveListFile="list.dat";
    RestoreList();    
}

void CTestWMFDlg::RestoreList(void)
{
	OutputWnd &OWnd1=Graph->OWnd1;
	int i,n;
	OWnd1.UnLoadPic();
	for(i=0;i<Pics.GetSize();i++)
	{
		Pics[i]->Destroy();
		delete Pics[i];	
	}
	Pics.RemoveAll();

	CFile f; 
	if(f.Open(SaveListFile,CFile::modeRead))
	{
		{
			CArchive ar(&f,CArchive::load);
			ar >> n;
			for(i=0;i<n;i++)
			{
				DjvuPic *temp=new DjvuPic("",&OWnd1);				
				temp->Serialize(ar);			
				Pics.Add(temp);
			}
		}
		f.Close();
	}	
	FileList1.UpdateList();
}

void CTestWMFDlg::OnDeskew() 
{
	OutputWnd &OWnd1=Graph->OWnd1;
	DeskewFilter Filter1; UpdateData();	
	Filter1.SetupFilter(70*DEGREE,110*DEGREE,.1*DEGREE,edge_level);
	Filter1.edges=edges;

	double ret=Filter1.Main(OWnd1.CurPic->Buffer);	
	angle=(90*DEGREE-ret); 
	UpdateData(0);

	CString T; T.Format("Deskew DJVU %s",Filter1.timer1.GetStringF()); InfoList.AddString(T);
}


//void CTestWMFDlg::OnFitCanvas() 
//{
//	/*
//	DjvuPic *CurPic=OWnd1.CurPic; CRect testRect, tempRect; MyTimer timer1; CString ttt; UpdateData();
//	if(CurPic)
//	{
//		timer1.Start();
//		testRect=CRect(0,0,canvasW,canvasH); 
//		CPoint dd((CurPic->Buffer->w-canvasW)/2,(CurPic->Buffer->h-canvasH)/2);
//		testRect+=dd;
//		CurPic->Buffer->LoadBitmapArray();unsigned int *Accumulator=0, max, mean; int i,k,x=0,y=0,N=10000;
//
//		CPoint *Pnts=new CPoint[N]; double f,r,Cosf,Sinf,a=canvasW/2,b=canvasH/2;
//		for(i=0,k=0;i<N;i++)
//		{
//			f=2*M_PI*i/N; Cosf=cos(f); Sinf=sin(f); r=a*b/sqrt(b*b*Cosf*Cosf+a*a*Sinf*Sinf);
//			r=r*0.1*(1+sin((double)i))/2+0.9*r;
//			Pnts[k].x=(CurPic->Original->w/2)+(int)(r*Cosf);Pnts[k++].y=(CurPic->Original->h/2)+(int)(r*Sinf);
//		}	
//
//		int DD=3,DD2=DD/2; unsigned int *first,*last,*tp;
//
//		//for(i=0,k=0;i<N;i++)
//		{
//		//	CurPic->frg->arr[Pnts[i].x+Pnts[i].y*CurPic->frg->wbyte]=0;
//			//Pnts[k].x=(CurPic->Original->w/2)+(int)(r*Cosf);Pnts[k++].y=(CurPic->Original->h/2)+(int)(r*Sinf);
//		}
//		//CurPic->frg->SetBitmapArray();
//		
//		if(Accumulator) delete[] Accumulator; Accumulator=0; Accumulator=new unsigned int[2*dd.y]; 
//		for(i=-dd.y,k=0;i<dd.y;i++)
//		{
//			Accumulator[k++]=CurPic->frg->GetSum3(Pnts,N,CPoint(0,i));					
//		}
//		if(k>=DD)
//		{
//			for(tp=first=Accumulator,mean=0,max=0;tp<Accumulator+DD;tp++) mean+=*tp;	last=tp;
//			for(i=DD2;i<k-DD2;i++)
//			{				
//				if(mean>max) {max=mean; y=i;}
//				//fprintf(f,"%d\t%lf\n",i-dd.y,(double)mean/DD);
//				mean+=(*last-*first); first++; last++;
//			}	
//		}
//		//fclose(f);
//		if(max) y+=-dd.y;
//
//		if(Accumulator) delete[] Accumulator; Accumulator=0; Accumulator=new unsigned int[2*dd.x]; 
//		for(i=-dd.x,k=0;i<dd.x;i++)
//		{
//			Accumulator[k++]=CurPic->frg->GetSum3(Pnts,N,CPoint(i,y));					
//		}
//		if(k>=DD)
//		{
//			for(tp=first=Accumulator,mean=0,max=0;tp<Accumulator+DD;tp++) mean+=*tp;	last=tp;
//			for(i=DD2;i<k-DD2;i++)
//			{				
//				if(mean>max) {max=mean; x=i;}				
//				mean+=(*last-*first); first++; last++;
//			}	
//		}		
//		if(max) x+=-dd.x;
//		
//		CurPic->AddZone(testRect+CPoint(x,y));
////		CurPic->AddZone(testRect);
//		//ttt.Format("dx=%d dy=%d",x,y); InfoList.AddString(ttt);
//
//
//		if(Accumulator) delete[] Accumulator;	
//		if(Pnts) delete[] Pnts;
//
//		timer1.Stop(); time=timer1.GetValue(); UpdateData(0);
//		OWnd1.DrawMode.Set(BCKG);; 
//		OWnd1.Draw();
//	}
//	*/
//}

//void CTestWMFDlg::OnBegindragList3(NMHDR* pNMHDR, LRESULT* pResult) 
//{
//	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
//	// TODO: Add your control notification handler code here
//	
//	*pResult = 0;
//}




void CTestWMFDlg::OnBnClickedButton13()
{
	OutputWnd &OWnd1=Graph->OWnd1;
//	GrayFilter Filter1;
//	Filter1.Main(OWnd1.CurPic);
	//OWnd1.CurPic->frg->ConvertToGray();

//	OWnd1.DrawMode.Set(BCKG);; 
//	OWnd1.Draw();	
	//CString T;T.Format("%s: %s",Filter1.Name,Filter1.Timer1.GetStringF());
	//InfoList.AddString(T);
}

//		DjvuPic NewBckg(CurPic.Original); NewBckg=CurPic; //NewBckg.Create(CurPic.Original);
		//NewBckg.Original->CreateCompatible(CurPic.frg); 
//		NewBckg.LoadBMP(NewBckg.BckgFile); //NewBckg.frg->CreateCompatible(CurPic.frg);		
//		NewBckg.frg->Copy(NewBckg.Original); //NewBckg.Original->CreateCompatible(CurPic.frg);
		//NewBckg.SwapBMP(&(NewBckg.Original),&(NewBckg.frg));
		/*
		if(!Filter1.Main(NewBckg)) //Rotate
		{
			CurPic.SwapBMP(&(NewBckg.frg),&(NewBckg.Original));
			CurPic.frg->Copy(NewBckg.Original);		
		}
		
		if(!Filter1.Main(CurPic)) //Rotate
		{
			CurPic.SwapBMP(&(CurPic.frg),&(CurPic.Original));
			CurPic.frg->Copy(CurPic.Original);		
		}
		*/
		//if(!CurPic.IsBckgFile) 
		//	Filter2.Main(&CurPic); // BW	
		//else 
			//CurPic.frg->Copy(NewBckg.frg);
//	NewBckg.frg->CopyTo(CurPic.frg);
void CTestWMFDlg::OnBnClickedButton14()
{
	OutputWnd &OWnd1=Graph->OWnd1;
	DjvuPic *CurPic=OWnd1.CurPic; CString T; HRESULT reslt; int i,j;
	CImage Img; GausianBlur Filter3; 
	
	for(i=0;i<Pics.GetSize();i++) Pics[i]->Destroy();
	CArray<DjvuPic*,DjvuPic*> Pics1; int *ind=0; Pics1.RemoveAll();
    
	POSITION pos=FileList1.GetFirstSelectedItemPosition();
	while(pos)
	{
		int n=FileList1.GetNextSelectedItem(pos); 
		Pics1.Add(Pics[n]); 
	}
		
		for(j=0;j<Pics1.GetSize();j++)
		{
			DjvuPic *CurPic=Pics1[j]; CurPic->Create(DjvuPic::IN_MEMORY);
			ArrayOfZones& Zones=*(CurPic->GetZones());

			for(int i=0;i<Zones.GetSize();i++)
			{
				BckgZone* Zone=Zones[i];
				Zone->CopyOriginal();
				Filter3.SetKernel(CurPic->GBparam.sigma,CurPic->GBparam.KernelSize);
				Filter3.Main(Zone->buffer);		
				//Img.Attach(HBITMAP(Zone->buffer->bmp));			
				Img.Attach(Zone->buffer->GetHBMP());			
				T.Format("%s\\%s_%d%s",CurPic->Path,CurPic->Name,i,CurPic->Ext);
				reslt=Img.Save(T,ImageFormatBMP);
				Img.Detach();
				T.Format("Saving %s_%d%s - ",CurPic->Name,i,CurPic->Ext);
				if(reslt==S_OK) T+="Ok";
				else T+="Err";
				InfoList.AddString(T);
				//InfoList.SetCurSel(InfoList.GetCount()-1);
			}
			CurPic->Destroy();		
		}	
		OWnd1.CurPic=0;        	
}

void CTestWMFDlg::OnBnClickedCheck3()
{
	UpdateData(); 
	Graph->PostThreadMessage(UM_ORIGINAL,(WPARAM)LoadOriginal,0);
}


void CTestWMFDlg::OnBnClickedCheck1()
{
	// TODO: Add your control notification handler code here
}



void CTestWMFDlg::OnEnKillfocusEdit4()
{
	UpdateData();
}

void CTestWMFDlg::OnEnKillfocusEdit5()
{
	UpdateData();
}

void CTestWMFDlg::OnBnClickedButton15()
{
	UpdateData();
	DjvuPicArray PicList;
	FileList1.GetSelectedItems(&PicList);
	for(int i=0;i<PicList.GetSize();i++) 
	{
		PicList[i]->GBparam.KernelSize=GBn;
		PicList[i]->GBparam.sigma=GBsigma;
	}
	FileList1.UpdateList();
}

LRESULT CTestWMFDlg::OnWaiting(WPARAM wParam, LPARAM lParam )
{
	CString T; T.Format("Waiting for files to rename");
	InfoList.AddString(T);    
	return 0;
}

LRESULT CTestWMFDlg::OnPrint(WPARAM wParam, LPARAM lParam )
{	
	Beep(2000,1000);
	CString T; T.Format("Waiting Completed");
	InfoList.AddString(T);    
	ThreadsList.UpdateList();
	return 0;
}

LRESULT CTestWMFDlg::OnUpdate(WPARAM wParam, LPARAM lParam )
{
	CString T; T.Format("Completed %d%%",100*wParam/lParam);
	SetWindowText(T);
	return 0;
}

LRESULT CTestWMFDlg::OnPrinterLock(WPARAM wParam, LPARAM lParam )
{
	CString T; 
	if(lParam)
	{
		T.Format("Locking for LtiVpd.exe");	
	}
	else
	{
		if(wParam) T.Format("LtiVpd.exe set IDLE Ok");
		else T.Format("LtiVpd.exe set priority ERROR");		
	}
	InfoList.AddString(T);    
	return 0;
}

void CTestWMFDlg::OnBnClickedButton4()
{
	UpdateData();
	DjvuPicArray PicList;
	FileList1.GetSelectedItems(&PicList);
	for(int i=0;i<PicList.GetSize();i++) PicList[i]->CenterRotate.angle=angle;
	FileList1.UpdateList();
}

void CTestWMFDlg::OnSelectAll()
{
	CString T; T="Select All"; InfoList.AddString(T);
	FileList1.SelectAll();
}


void CTestWMFDlg::OnOriginal()
{	
	LoadOriginal=(LoadOriginal ? false:true);
	UpdateData(false);
	OnBnClickedCheck3();	
}

void CTestWMFDlg::OnBnClickedButton16()
{
	for(int i=0;i<Threads.GetSize();i++)
	{
		Threads[i]->StopWaiting=true;
		WaitForSingleObject(Threads[i]->Terminator,1500);
	}	
	ThreadsList.UpdateList();
}

void CTestWMFDlg::OnLvnColumnclickList3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	int& mask=FileList1.SortMask; int &Status=FileList1.SortStatus;

	mask=1<<pNMLV->iSubItem; 
	if(Status&mask) Status&=(~mask);
	else Status|=mask;	

	FileList1.SortItems(CompareFunc1,(LPARAM) (&FileList1));


	*pResult = 0;
}

LRESULT CTestWMFDlg::OnThreadListUpdate(WPARAM wParam, LPARAM lParam )
{	
	StatMessage *msg=(StatMessage *)wParam; int ArrayN; int t=0;
	ThreadParams* Thrd=Threads.FindElement(msg->ThreadPID,ArrayN);
	if(Thrd) 
	{
		if(Thrd->Update(msg)) 
			ThreadsList.UpdateElement(ThreadsList.FindElement(msg->ThreadPID),ArrayN);
	}
	delete msg;
	return 0;
}

/*
		if(msg->FLAGS & DONE_MSK) {Thrd->Done=msg->Done; t++;}
		if(msg->FLAGS & TIME_MSK) {Thrd->Time=msg->Time; t++;}
		if(msg->FLAGS & TIME_LEFT_MSK) {Thrd->TimeLeft=msg->TimeLeft; t++;}
		if(msg->FLAGS & STATUS_MSK) {Thrd->Status=msg->Status; t++;}

*/


void CTestWMFDlg::OnBnClickedButton7()
{
	CString Name,TmpPBM,TmpRLE,TmpPPM,TmpSEP,T; 

	Name=CString("p0005"); 
	TmpPBM.Format("%s_PBM.pbm",Name); TmpRLE.Format("%_RLE.rle",Name);
	TmpPPM.Format("%_PPM.ppm",Name); TmpSEP.Format("%_SEP.sep",Name);


	T.Format("i_view32 %s.tif /convert=%s",Name,TmpPBM);
	STARTUPINFO si;  GetStartupInfo(&si); PROCESS_INFORMATION pi1;	

	if(CreateProcess(NULL, (char *)LPCSTR(T), NULL, NULL, FALSE, DETACHED_PROCESS | CREATE_SUSPENDED | IDLE_PRIORITY_CLASS, NULL, NULL, &si, &pi1)) 
	{
		ResumeThread(pi1.hThread);
		WaitForSingleObject(pi1.hThread,INFINITE);
	}

	T.Format("i_view32 %s.jpg /convert=%s",Name,TmpPPM);
	if(CreateProcess(NULL, (char *)LPCSTR(T), NULL, NULL, FALSE, DETACHED_PROCESS | CREATE_SUSPENDED | IDLE_PRIORITY_CLASS, NULL, NULL, &si, &pi1)) 
	{
		ResumeThread(pi1.hThread);
		WaitForSingleObject(pi1.hThread,INFINITE);
	}

	T.Format("pnmtodjvurle.exe"); 
	SECURITY_ATTRIBUTES sa;
	sa.nLength=sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor=NULL;
	sa.bInheritHandle=TRUE;

	HANDLE in=CreateFile(TmpPBM, GENERIC_READ, 0, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	HANDLE out=CreateFile(TmpRLE, GENERIC_WRITE, 0, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	si.hStdInput=in;
	si.hStdOutput=out;
	si.dwFlags|=STARTF_USESTDHANDLES;

	if(CreateProcess(NULL, (char *)LPCSTR(T), NULL, NULL, TRUE, 
		DETACHED_PROCESS | CREATE_SUSPENDED | IDLE_PRIORITY_CLASS, NULL, NULL, &si, &pi1)) 
	{
		ResumeThread(pi1.hThread);
		WaitForSingleObject(pi1.hThread,INFINITE);
	}
	CloseHandle(in); CloseHandle(out); DeleteFile(TmpPBM);


	HANDLE sep=CreateFile(TmpSEP, GENERIC_WRITE, 0, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	HANDLE frg=CreateFile(TmpRLE, GENERIC_READ, 0, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	HANDLE bkg=CreateFile(TmpPPM, GENERIC_READ, 0, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	
	int size; DWORD done; BYTE *arr=NULL;
	size=GetFileSize(frg,0); arr=new BYTE[size];
	ReadFile(frg,arr,size,&done,0);	WriteFile(sep,arr,size,&done,0);
	delete[] arr;
	SetEndOfFile(sep);
	size=GetFileSize(bkg,0); arr=new BYTE[size];
	ReadFile(bkg,arr,size,&done,0);	WriteFile(sep,arr,size,&done,0);
	delete[] arr;
	CloseHandle(sep); CloseHandle(frg); CloseHandle(bkg);
	DeleteFile(TmpRLE); DeleteFile(TmpPPM);

	GetStartupInfo(&si);
	T.Format("csepdjvu %s %s.djvu",TmpSEP,Name);
	if(CreateProcess(NULL, (char *)LPCSTR(T), NULL, NULL, FALSE, DETACHED_PROCESS | CREATE_SUSPENDED | IDLE_PRIORITY_CLASS, NULL, NULL, &si, &pi1)) 
	{
		ResumeThread(pi1.hThread);
		WaitForSingleObject(pi1.hThread,INFINITE);
	}
	DeleteFile(TmpSEP);	
}
