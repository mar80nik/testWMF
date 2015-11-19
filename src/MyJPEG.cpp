// MyJPEG.cpp: implementation of the MyJPEG class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyJPEG.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MyJPEG::MyJPEG(CString& n)
{
    Name=n; buffer=0;
	buffer=new BMPanvas((CDC*)0);
}

MyJPEG::~MyJPEG()
{
    if(buffer) delete buffer;
}


BMPINFO MyJPEG::ReadHeader(const CString& name)
{
	JPEG_CORE_PROPERTIES jcprops;
	BMPINFO temp; temp.bmiHeader.biWidth=temp.bmiHeader.biHeight=0;

	if ( ijlInit(&jcprops) == IJL_OK ) 
	{
		jcprops.JPGFile=name;
		if ( ijlRead(&jcprops,IJL_JFILE_READPARAMS) == IJL_OK )
		{			
			temp.bmiHeader.biWidth=jcprops.JPGWidth;
			temp.bmiHeader.biHeight=jcprops.JPGHeight;
			temp.bmiHeader.biBitCount=jcprops.JPGChannels*8;
		}
	}
	ijlFree(&jcprops);
	return temp;
}

// читаем картинку в этот буфер.
int MyJPEG::ReadImage()
{
	JPEG_CORE_PROPERTIES jcprops;
	if ( ijlInit(&jcprops) != IJL_OK ) {ijlFree(&jcprops); return 1;}

	jcprops.JPGFile=Name;

	if ( ijlRead(&jcprops,IJL_JFILE_READPARAMS) != IJL_OK ) {ijlFree(&jcprops); return 1;}
	
	BMPINFO temp=buffer->INFO;
	temp.bmiHeader.biWidth=jcprops.JPGWidth;
	temp.bmiHeader.biHeight=jcprops.JPGHeight;
	temp.bmiHeader.biBitCount=jcprops.JPGChannels*8;
	buffer->Create(&temp);
	buffer->CreateGrayPallete();
	buffer->LoadBitmapArray();

	jcprops.DIBWidth=jcprops.JPGWidth;
	jcprops.DIBHeight=-jcprops.JPGHeight;
	jcprops.DIBChannels=jcprops.JPGChannels;	
	jcprops.DIBPadBytes=IJL_DIB_PAD_BYTES(jcprops.JPGWidth,jcprops.JPGChannels);	
	jcprops.DIBBytes=buffer->arr;

	if ( jcprops.JPGChannels == 3 )
	{
		jcprops.DIBColor = IJL_BGR;
		jcprops.JPGColor = IJL_YCBCR;
	}
	else
	{
		jcprops.DIBColor = IJL_G;
		jcprops.JPGColor = IJL_G;
	}

	if ( ijlRead(&jcprops, IJL_JFILE_READWHOLEIMAGE) != IJL_OK ) 
	{
		buffer->UnloadBitmapArray();	
		ijlFree(&jcprops);return 1;
	}
	buffer->SetBitmapArray();
	buffer->UnloadBitmapArray();	

	if ( ijlFree(&jcprops) != IJL_OK ) return 1;
	return 0;
}


int MyJPEG::Compress(int quality)
{
	JPEG_CORE_PROPERTIES jcprops;
	
	if ( ijlInit(&jcprops) != IJL_OK ) {ijlFree(&jcprops); return 1;}

	buffer->LoadBitmapArray();	

	jcprops.JPGFile=Name;
	jcprops.DIBWidth = buffer->w;
	jcprops.DIBHeight = -buffer->h;
	jcprops.JPGWidth = buffer->w;
	jcprops.JPGHeight = buffer->h;
	jcprops.DIBBytes = buffer->arr;	
	jcprops.DIBChannels = buffer->INFO.bmiHeader.biBitCount/8;
	jcprops.JPGChannels = jcprops.DIBChannels;
	jcprops.DIBPadBytes = IJL_DIB_PAD_BYTES(buffer->w,jcprops.DIBChannels);
	jcprops.jquality = quality;

	if ( jcprops.DIBChannels == 3 )
	{
		jcprops.DIBColor = IJL_BGR;
		jcprops.JPGColor = IJL_YCBCR;
	}
	else
	{
		jcprops.DIBColor = IJL_G;
		jcprops.JPGColor = IJL_G;
	}

	if ( ijlWrite(&jcprops,IJL_JFILE_WRITEWHOLEIMAGE) != IJL_OK )
	{
		buffer->UnloadBitmapArray();	
		ijlFree(&jcprops); return 1;
	}
	
	buffer->UnloadBitmapArray();	
	
	if ( ijlFree(&jcprops) != IJL_OK ) {return 1;}	
	return 0;
}

BMPINFO MyTIFF::ReadHeader( const CString& name)
{
	TIFF *image; 
	BMPINFO temp; temp.bmiHeader.biWidth=temp.bmiHeader.biHeight=0;

	if((image = TIFFOpen(name, "r")) != NULL)
	{
		TIFFGetField(image, TIFFTAG_IMAGEWIDTH, &temp.bmiHeader.biWidth);
		TIFFGetField(image, TIFFTAG_IMAGELENGTH, &temp.bmiHeader.biHeight);	
		TIFFClose(image);
	}	
	return temp;
}

BMPINFO MyPNG::ReadHeader( const CString& name)
{
	BMPINFO temp; temp.bmiHeader.biWidth=temp.bmiHeader.biHeight=0;
	unsigned char sig[8];

	FILE *infile;
	fopen_s(&infile, name,"rb");

	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_uint_32 w,h; int bpp,clr_type;
	w=h=0; bpp=clr_type=0;

    fread(sig, 1, 8, infile);
    if (png_sig_cmp(sig, 0, 8)) return temp;   /* bad signature */


    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) return temp;   

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) 
	{
		png_destroy_read_struct(&png_ptr, NULL, NULL); 
		return temp; 
	}

    if (setjmp(png_jmpbuf(png_ptr))) 
	{
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return temp;
    }

    png_init_io(png_ptr, infile);
    png_set_sig_bytes(png_ptr, 8); 
    png_read_info(png_ptr, info_ptr);  /* read all PNG info up to image data */

    png_get_IHDR(png_ptr, info_ptr,&w,&h,&bpp , &clr_type,NULL, NULL, NULL);
	temp.bmiHeader.biWidth=w; temp.bmiHeader.biHeight=h; temp.bmiHeader.biBitCount=bpp;

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(infile);
	return temp;
}

BMPINFO MyBMP::ReadHeader( const CString& name)
{
	CFile ttt; BMPINFO ret;
	if(ttt.Open(name,CFile::modeRead))
	{
		BITMAPFILEHEADER tt;			
		ttt.Read(&tt,sizeof(BITMAPFILEHEADER));
		ttt.Read(&ret,sizeof(BMPINFO)); 	
		ttt.Close();		
	}
	return ret;
}
