#ifndef _TGA_H_
#define _TGA_H_

typedef struct _TGAColMapSpec
{
	_TGAColMapSpec()
	{
		 firstentry = 0;
		 length = 256;
		 bitdeepth = 24;
	}
	short firstentry;
	short length;
	char  bitdeepth;
}TGAColMapSpec;

typedef struct _TGAImgSpec
{
	_TGAImgSpec()
	{
		xo = yo = 0;
		width = 32;
		height = 32;
		bitdeepth = 32;
		descriptor = 0;
	}
	short xo, yo;        // origin of image (lower left corner)
	short width, height;
	char  bitdeepth;
	char  descriptor;    // bit 0..3: alpha channel bits
	// bit 4..5: Image origin (0:dn-l, 1:dn-r, 2:t-l, 3:t-r)
}TGAImgSpec;

typedef struct _TGAFileHeader
{
	_TGAFileHeader()
	{
		 IDLength = 0;
		 ColorMapType = 0;
		 ImageType = 2;
	}
	char IDLength;
	char ColorMapType;  // 0: no color map, 1: colomap is included
	char ImageType;     //  0: no image data
	//  1: uncompressed color-mapped image
	//  2: uncompressed true color image
	//  3: uncompressed black and white image
	//  9: Run-length encoded, color-mapped image
	// 10: Run-length encoded, true color image
	// 11: Run-length encoded, black and white image
	TGAColMapSpec colmapspec;
	TGAImgSpec    imagespec;

}TGAHeader;

#define POW2N(_n) (1<<(_n))
#define GETSHORT(_l, _h) ((_l) | ((_h)<<8))
#define GETLO(_s) ((_s) & 0xff)
#define GETHI(_s) (((_s) >> 8) & 0xff)

unsigned char *GetTGA(unsigned char *buf, TGAHeader &tga)
{
	tga.IDLength     = buf[0];
	tga.ColorMapType = buf[1];
	tga.ImageType    = buf[2];

	tga.colmapspec.firstentry = GETSHORT(buf[3], buf[4]);
	tga.colmapspec.length     = GETSHORT(buf[5], buf[6]);
	tga.colmapspec.bitdeepth  = buf[7];

	tga.imagespec.xo          = GETSHORT(buf[8],  buf[9]);
	tga.imagespec.yo          = GETSHORT(buf[10], buf[11]);
	tga.imagespec.width       = GETSHORT(buf[12], buf[13]);
	tga.imagespec.height      = GETSHORT(buf[14], buf[15]);
	tga.imagespec.bitdeepth   = buf[16];
	tga.imagespec.descriptor  = buf[17];

	return buf + 18 + tga.IDLength;
}

#endif //_TGA_H_
