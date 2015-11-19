// MyJPEG.h: interface for the MyJPEG class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYJPEG_H__21CA2E5B_78AF_4A77_81CC_E8766498B9F0__INCLUDED_)
#define AFX_MYJPEG_H__21CA2E5B_78AF_4A77_81CC_E8766498B9F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "bmpanvas.h"
#include "tiffio.h"
#include "ijl.h"
#include "png.h"

class MyJPEG
{
public:
	BMPanvas *buffer;
	CString Name;	

	MyJPEG(CString&);
	virtual ~MyJPEG();
	static BMPINFO ReadHeader(const CString&);

	int ReadImage();
//		int &width, // ширина загружаемой картинки.
//		int &height, // высота загружаемой картинки.
//		int &bpp, // БАЙТ (не бит) НА ПИКСЕЛЬ.
///		const void *buffer, // адрес памяти, в котором хранятся сжатые jpeg-ом данные.
//		int sizebytes); // размер сжатых jpeg-ом данных.

	int Compress(int quality=75);//(const void *buffer, // адрес картинки в памяти
//		int width, // ширина картинки в пикселах
//		int height, // высота картинки в пикселах.
//		int bpp, // *БАЙТ* на пиксель изображения 1 или 3
///		int &len, // возвращённая длина сжатых данных
//		int quality=75); // качество картинки в процентах
};

class MyTIFF
{
public:
	static BMPINFO ReadHeader(const CString&);
};


class MyPNG
{
public:
	static BMPINFO ReadHeader(const CString&);
};

class MyBMP
{
public:
	static BMPINFO ReadHeader(const CString&);
};

#endif // !defined(AFX_MYJPEG_H__21CA2E5B_78AF_4A77_81CC_E8766498B9F0__INCLUDED_)
