#include "stdafx.h"
#include "threadslist.h"
#include "graphicthread.h"
#include "testWMFDlg.h"
#include "Psapi.h"
#include "rotatefilter.h"

void GetRandomString(CString &T)
{
	T=""; CString T1; srand( (unsigned)time( NULL ) ); BYTE code, code1; double t, t1;
	for(int i=0;i<4;i++)
	{	
		t=rand()/((double)RAND_MAX); t1=rand()/((double)RAND_MAX);
		code=(BYTE)(97+25*t); code1=(BYTE)(65+25*t1);
		T1.Format("%c%c",code, code1); T+=T1;
	}		   
}

ThreadParams::ThreadParams(): Terminator(true,true) 
{
	Name="NoName"; StopWaiting=true; Parent=0;  PID=-1; Status=Preparing; 
	Time=TimeLeft=-1;
};

CString ThreadParams::GetStatusText()
{
	CString T;
	switch(Status)
	{
	case Preparing: T="Preparing"; break;
	case Working: T="Working"; break;
	case Finished: T="Finished"; break;
	case Waiting: T="Waiting"; break;
	case Rename: T="Rename"; break;
	case Error: 
	default: T="Error"; break; 	
	}
    return T;
}

void ThreadParams::PostParentMessage(UINT Msg, StatMessage* msg)
{
	Inspector1.RegisterMsg(msg);
	msg->ThreadPID=PID; 
	Parent->PostThreadMessage(Msg,(WPARAM)msg,0);	
}

void ThreadParams::FormatElement(int subItem, CString& T)
{
	switch (subItem)
	{
	case Name_COL: T=Name; break;
	case Start_COL: T.Format("%02d:%02d:%02d",
						StartTime.GetHour(),StartTime.GetMinute(), StartTime.GetSecond()); break;
	case Time_COL: T=ConvTimeToStr(Time); break;
	case Left_COL: T=ConvTimeToStr(TimeLeft); break;
	case Status_COL: T=GetStatusText(); break;
	default: T="???";
	}
}

void PrintThreadParams::FormatElement(int subItem, CString& T)
{
	switch (subItem)
	{
	case Pics_COL: T.Format("%d",PicsN); break;
	case Done_COL: T.Format("%d",Done); break;
	default: ThreadParams::FormatElement(subItem,T);
	}
}

void RenameThreadParams::FormatElement(int subItem, CString& T)
{
	switch (subItem)
	{	
	case Done_COL: 
		if(Renamed==0) T.Format("%d",Done); 
		else T.Format("%d (%d)",Done,Renamed); 
		break;
	default: PrintThreadParams::FormatElement(subItem,T);
	}
}

int ThreadParams::Update(StatMessage* msg)
{
	int t=0;
	if(msg->FLAGS & TIME_MSK) {Time=msg->Time; t++;}
	if(msg->FLAGS & TIME_LEFT_MSK) {TimeLeft=msg->TimeLeft; t++;}
	if(msg->FLAGS & STATUS_MSK) {Status=msg->Status; t++;}
	return t;
}

int PrintThreadParams::Update(StatMessage* msg)
{
	int t=0;
	if(msg->FLAGS & DONE_MSK) {Done=msg->Done; t++;}
	t+=ThreadParams::Update(msg);
	return t;
}

void ThreadParams::_Main()
{
	StatMessage* msg; 	
	Terminator.ResetEvent(); StopWaiting=false; Timer1.Start();

	msg=new StatMessage(); Timer1.Stop(); msg->Time=Timer1.GetValue(); StartTime=CTime::GetCurrentTime();	
	msg->Status=Working; msg->FLAGS=STATUS_MSK | TIME_MSK;
	PostParentMessage(UM_UPDATE_THREAD_LIST,msg);	

	Main();

	msg=new StatMessage(); Timer1.Stop(); msg->Time=Timer1.GetValue(); msg->TimeLeft=msg->Time;
	msg->Status=Finished; msg->FLAGS=STATUS_MSK | TIME_MSK | TIME_LEFT_MSK;
	PostParentMessage(UM_UPDATE_THREAD_LIST,msg);	

	Terminator.SetEvent();
}

void PrintThreadParams::Main()
{
	int i,j,num=0; int *ind=0; 	StatMessage* msg; 

	CString TmpName,TmpPBM,TmpPGM,TmpRLE,TmpPPM,TmpSEP,TmpDJVU,T,name,TmpFRG; 

	DjvuPicArray &Pics1=*(Pics); PicsN=Pics->GetSize();
	CSingleLock Lock1(&(Pics->ArrayMutex)); CImage Img;
	while(!(Lock1.Lock(500))) if(StopWaiting) return;
	CString RandomText; GetRandomString(RandomText);	
	CWinThread* thrd=AfxGetThread();
//*****************PRINTING************************	
	RotateFilter Filter1; BWFilter Filter2; GausianBlur Filter3; 

	for(j=0;j<Pics1.GetSize() && !StopWaiting;j++)
	{
		msg=new StatMessage();msg->Status=Working; msg->FLAGS=STATUS_MSK; PostParentMessage(UM_UPDATE_THREAD_LIST,msg);		
		DjvuPic &CurPic=*(Pics1[j]);
		ArrayOfZones& Zones=*(CurPic.GetZones());
		CurPic.CenterRotate.Q=RotateFilterParam::HI; 
		CurPic.Create(DjvuPic::IN_MEMORY); 
		
//		name.Format("%s%03d",RandomText,j+1); 
		T=CurPic.Name; 
//		T=name;
		T.Replace(' ','_'); TmpName=*Path+T;		
		TmpPBM=TmpName+".pbm"; TmpPGM=TmpName+".pgm"; TmpPPM=TmpName+".ppm"; 		
		TmpRLE=TmpName+".rle";		
		TmpSEP=TmpName+".sep";TmpDJVU=TmpName+".djvu";
		switch (CurPic.Foreground->ColorType)
		{
		case BMPanvas::TRUE_COLOR: ASSERT(0); break;
		case BMPanvas::RGBA:;
		case BMPanvas::COLOR_PAL: TmpFRG=TmpPPM; break;
		case BMPanvas::GRAY_PAL: TmpFRG=TmpPGM; break;
		case BMPanvas::BW: TmpFRG=TmpPBM; break;
		case BMPanvas::COL_ERROR: 
		default: ASSERT(0); 
		}
				
		for(i=0;i<Zones.GetSize();i++)
		{
			BckgZone* Zone=Zones[i];
			CRect rect=Zone->GetFillRect();
			Filter3.SetKernel(CurPic.GBparam.sigma,CurPic.GBparam.KernelSize);
			Filter3.Main(Zone->buffer,rect);			
		}		
// ******FOREGROUND**********
		for(i=0;i<Zones.GetSize();i++)
		{			
			CRect t5=Zones[i]->Rgn;
			PatBlt(CurPic.Foreground->GetDC(),t5.left,t5.top,t5.Width(),t5.Height(),WHITENESS);
		}
		Img.Attach(CurPic.Foreground->GetHBMP()); Img.Save(TmpName,ImageFormatPNG); Img.Detach();
// converting
		T.Format("i_view32 %s /convert=%s",TmpName,TmpFRG);
		STARTUPINFO si;  GetStartupInfo(&si); PROCESS_INFORMATION pi1;	
		if(CreateProcess(NULL, (char *)LPCSTR(T), NULL, NULL, FALSE, DETACHED_PROCESS | CREATE_SUSPENDED | NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi1)) 
		{
			ResumeThread(pi1.hThread);
			WaitForSingleObject(pi1.hThread,INFINITE);
		}
		T.Format("pnmtodjvurle.exe"); 
		SECURITY_ATTRIBUTES sa; sa.nLength=sizeof(SECURITY_ATTRIBUTES); sa.lpSecurityDescriptor=NULL; sa.bInheritHandle=TRUE;
		HANDLE in=CreateFile(TmpFRG, GENERIC_READ, 0, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		HANDLE out=CreateFile(TmpRLE, GENERIC_WRITE, 0, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		si.hStdInput=in; si.hStdOutput=out; si.dwFlags|=STARTF_USESTDHANDLES;
		if(CreateProcess(NULL, (char *)LPCSTR(T), NULL, NULL, TRUE, DETACHED_PROCESS | CREATE_SUSPENDED | NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi1)) 
		{
			ResumeThread(pi1.hThread);
			WaitForSingleObject(pi1.hThread,INFINITE);
		}
		CloseHandle(in); CloseHandle(out); DeleteFile(TmpFRG);
// **************************
// ******BACKGROUND**********
		CurPic.Background->PatBlt(WHITENESS);
		for(i=0;i<Zones.GetSize();i++)
		{			
			CRect t5=Zones[i]->Rgn; 
			BitBlt(CurPic.Background->GetDC(),t5.left,t5.top,t5.Width(),t5.Height(),Zones[i]->buffer->GetDC(),0,0,SRCCOPY);	
		}
		Img.Attach(CurPic.Background->GetHBMP()); 
		Img.Save(TmpName,ImageFormatPNG); Img.Detach();
// converting
		T.Format("i_view32 %s /convert=%s",TmpName,TmpPPM);
		if(CreateProcess(NULL, (char *)LPCSTR(T), NULL, NULL, FALSE, DETACHED_PROCESS | CREATE_SUSPENDED | NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi1)) 
		{
			ResumeThread(pi1.hThread);
			WaitForSingleObject(pi1.hThread,INFINITE);
		}
		DeleteFile(TmpName);
// **************************
// ******DJVU*******
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
		T.Format("csepdjvu %s %s",TmpSEP,TmpDJVU);

		if(CreateProcess(NULL, (char *)LPCSTR(T), NULL, NULL, FALSE, DETACHED_PROCESS | CREATE_SUSPENDED | NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi1)) 
		{
			ResumeThread(pi1.hThread);
			WaitForSingleObject(pi1.hThread,INFINITE);
		}
		Sleep(100); DeleteFile(TmpSEP);	
		Sleep(100);	MoveFile(TmpDJVU,CurPic.Path+CurPic.Name+".djvu");
// **************************

		CurPic.Destroy();		
		
		msg=new StatMessage();	Timer1.Stop(); msg->Time=Timer1.GetValue(); msg->Done=j+1;
		msg->TimeLeft=msg->Time*PicsN/(j+1); msg->FLAGS=TIME_MSK | DONE_MSK | TIME_LEFT_MSK;
		PostParentMessage(UM_UPDATE_THREAD_LIST,msg); Printed++;

	}
	if(!ind) delete[] ind; Lock1.Unlock();
	delete Pics; Pics=0;
}

void PriorityThreadParams::Main()
{
	DWORD procs[1000]; DWORD num,len; int t,ret=0; BYTE buf[1000];
	do
	{
		if(EnumProcesses(procs,sizeof(DWORD)*1000,&num))
		{
			HANDLE p; CString ExeName; 
			for(DWORD i=0;i<num;i++)
			{
				if((p=OpenProcess(PROCESS_ALL_ACCESS,false,procs[i])))
				{
					len=GetProcessImageFileName(p,(LPSTR)buf,1000);					
					CString FullName(buf),Name;
					if(len>0)
					{					
						if((t=FullName.ReverseFind('\\'))<0) Name=FullName;
						else Name=FullName.Right(FullName.GetLength()-t-1);
						if(Name=="LtiVpd.exe") ret=SetPriorityClass(p,IDLE_PRIORITY_CLASS);
					}
					CloseHandle(p);
				}
			}
		}
		if(!ret) Sleep(1000);
	}
	while(!StopWaiting && ret==0);
}

void PrintDialogThreadParams::Main()
{
	if(PrintDlg->DoModal()==IDOK)
	{
		Parent->PostThreadMessage(UM_PRINT_DIALOG, 0,0);
	}
}

void RenameThreadParams::Main()
{
//	WIN32_FIND_DATA FindFileData; ; HANDLE FFF;	StatMessage* msg; 
	PicsN=Pics->GetSize(); CString SearchPattern; CString OldFile, NewFile;
	int Total; Total=Moved=Renamed=0;
/*
	Parent->PostThreadMessage(UM_WAITING,0,0);		
//	SearchPattern.Format("%s\\%s*.djvu",*Path,RandomText);
	do
	{
		if((FFF=FindFirstFile(LPCSTR(SearchPattern),&FindFileData))!=INVALID_HANDLE_VALUE) 
		{	
			do
			{
				CString Name=FindFileData.cFileName; int ArrayN;
				Name=Name.Right(8); Name=Name.Left(3);
				ArrayN=atoi(Name);
				if(ArrayN>0 && ArrayN<=PicsN)
				{
					DjvuPic* CurPic=Pics->GetAt(--ArrayN);
					OldFile.Format("%s\\%s",*Path,FindFileData.cFileName);					
					NewFile=CurPic->Path+CurPic->Name+".djvu";
					if(MoveFile(OldFile,NewFile)) Moved++;
					else
					{
						NewFile.Format("%s\\%s.djvu",*Path,CurPic->Name);
						MoveFile(OldFile,NewFile); Renamed++;
					}				
				}
			}
			while(FindNextFile(FFF,&FindFileData) && !StopWaiting);
			if(Moved+Renamed!=Total)
			{
				Total=Moved+Renamed;
				msg=new StatMessage();	Timer1.Stop(); msg->Time=Timer1.GetValue(); msg->Done=Moved;
				msg->TimeLeft=msg->Time*PicsN/Total; msg->FLAGS=TIME_MSK | DONE_MSK | TIME_LEFT_MSK;
				PostParentMessage(UM_UPDATE_THREAD_LIST,msg);	

				Parent->PostThreadMessage(UM_UPDATE1,Total,PicsN);
			}
			Total=Moved+Renamed; 
            
			Limit1->Update(-1,Total);
		}
		else Sleep(100);		
		FindClose(FFF);
	}
	while(!StopWaiting && Total<PicsN); 
*/
//	delete Pics; Pics=0;
	Limit1->terminator.SetEvent();
	Parent->PostThreadMessage(UM_PRINT,0,0);	
}

PrintThreadParams::PrintThreadParams()
{
	Path=0; Done=0; PicsN=0; Pics=0;  Printed=0;
}

ThreadParams* ThreadsArray::FindElement(int pid,int &n)
{
	ThreadParams* ret=0,*t; n=-1;    
	for(int i=0;i<GetSize() && ret==0; i++) 
	{
		t=GetAt(i); if(t->PID==pid) {ret=t;	n=i;}
	}
    return ret;
}
//************************************************
BEGIN_MESSAGE_MAP(ThreadsList, CListCtrl)
END_MESSAGE_MAP()

ThreadsList::ThreadsList(void)
{
	LastN=-1;
}

ThreadsList::~ThreadsList(void)
{
}

void ThreadsList::CreateColumns(void)
{
	int i=0;
	InsertColumn(i++,"¹",LVCFMT_CENTER ,20);
	InsertColumn(i++,"Name",LVCFMT_CENTER ,80);
	InsertColumn(i++,"Pics",LVCFMT_LEFT ,40);
	InsertColumn(i++,"Done",LVCFMT_LEFT ,40);
	InsertColumn(i++,"Start",LVCFMT_CENTER ,75);
	InsertColumn(i++,"Time",LVCFMT_CENTER ,75);
	InsertColumn(i++,"Total",LVCFMT_CENTER ,75);
	InsertColumn(i++,"Status",LVCFMT_CENTER ,70);
}

void ThreadsList::UpdateList()
{
	LV_ITEM lvi; ThreadsArray& Threads=Parent->Threads;
	int n1=Threads.GetSize(); CString T; int i;

	DeleteAllItems(); 
	for(i=0;i<Threads.GetSize();i++)
	{
		ThreadParams &tt=*(Threads[i]); 
		lvi.mask = LVIF_PARAM; lvi.iItem = i;lvi.iSubItem = 0; lvi.lParam=tt.PID;			
		InsertItem(&lvi);
	} 

	for(i=0;i<GetItemCount();i++) 
	{
		UpdateElement(i,i);
	}

	/*
	LV_ITEM _ms_lvi;
	_ms_lvi.mask=LVIF_STATE;
	_ms_lvi.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
	_ms_lvi.state = LVIS_SELECTED | LVIS_FOCUSED;			

	LV_ITEM FAR * temp=&_ms_lvi;

	this->PostMessage(LVM_SETITEMSTATE ,(WPARAM)(i), (LPARAM)(temp));
//	EnsureVisible(n, false);	
*/
	EnsureVisible(i-1, false);	
}

void ThreadsList::UpdateElement(int ListN,int ArrayN)
{
	int i=ListN; CString T; LV_ITEM lvi;
	ThreadsArray& Threads=Parent->Threads;
	ThreadParams* Thrd;

	if(ListN<0 || ListN>=GetItemCount()) return;
	if(ArrayN<0 || ArrayN>=Threads.GetSize()) Thrd=Threads.FindElement(GetItemData(ListN),ArrayN);
	if(ArrayN<0) return;
	Thrd=Threads[ArrayN];
	
	lvi.mask = LVIF_TEXT; lvi.iItem = i; lvi.iSubItem=0;
	T.Format("%d",i+1); SetItemText(i,lvi.iSubItem++,(char*)LPCTSTR(T));
	for(;lvi.iSubItem<=8;lvi.iSubItem++)
	{
		Thrd->FormatElement(lvi.iSubItem,T); 
		SetItemText(i,lvi.iSubItem,(char*)LPCTSTR(T));			
	}
}

int ThreadsList::FindElement(int pid)
{
	int ret=-1;
	for(int i=0;i<GetItemCount() && ret<0; i++) if(GetItemData(i)==pid) ret=i;	
    return ret;
}
