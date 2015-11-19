// MessageContainers.h: interface for the MessageContainers class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESSAGECONTAINERS_H__12619758_5BBC_4889_86E0_A56158C60436__INCLUDED_)
#define AFX_MESSAGECONTAINERS_H__12619758_5BBC_4889_86E0_A56158C60436__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>

class AbstractMessage
{
	friend class MessagesInspector;
protected:
	AbstractMessage* Prev,*Next;
	int *counter;

public:
	int status;
	AbstractMessage();
	virtual ~AbstractMessage();		
	virtual void Clear();
};

template <class T>
class MsgArrayTemplate:public AbstractMessage
{
public:
	CArray<T,const T&> Values;
	int Add(T t) {return Values.Add(t);};
	int GetSize() {return Values.GetSize();};
	T& operator[](int i) {return Values[i];};
	virtual ~MsgArrayTemplate() {};
};

class MessagesInspector
{
	int counter; AbstractMessage NullMsg;
public:
	MessagesInspector();
	int RegisterMsg(AbstractMessage *);
	int CleanUp();
	~MessagesInspector();
};
#endif // !defined(AFX_MESSAGECONTAINERS_H__12619758_5BBC_4889_86E0_A56158C60436__INCLUDED_)
