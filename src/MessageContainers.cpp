// MessageContainers.cpp: implementation of the MessageContainers class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MessageContainers.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AbstractMessage::AbstractMessage() {Prev=Next=0; counter=0;}
AbstractMessage::~AbstractMessage() 
{
	if(counter) 
		(*counter)-=1;
	if(Prev) Prev->Next=Next;
	if(Next) Next->Prev=Prev;
	
}		
void AbstractMessage::Clear() {;}


MessagesInspector::MessagesInspector()
{
	counter=0; 
}

int MessagesInspector::RegisterMsg(AbstractMessage *msg)
{
	int ret=-1;
	AbstractMessage* Null=&NullMsg, *First=Null->Next;

	Null->Next=msg; msg->Prev=Null;
	if(First) First->Prev=msg; 
	msg->Next=First;
	counter++; ret=counter;
	msg->counter=&counter;
	return ret;
}

int MessagesInspector::CleanUp()
{
	if(counter!=0)
	{
		AbstractMessage *msg=NullMsg.Next,*t;		
		while(msg)
		{
			t=msg;            
			delete t;
			msg=NullMsg.Next;		
		}	
	}
	return counter;
}

MessagesInspector::~MessagesInspector()
{
	CleanUp();
}



