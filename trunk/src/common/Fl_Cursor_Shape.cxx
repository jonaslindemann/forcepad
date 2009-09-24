//|
//| Mouse cursor shapes for the Fast Light Tool Kit (FLTK).
//|
//| Copyright 1998-1999 by Vivality llc, Venice, CA, U.S.A. 
//|  and Matthias Melcher.
//|
//| This library is free software; you can redistribute it and/or
//| modify it under the terms of the GNU Library General Public
//| License as published by the Free Software Foundation; either
//| version 2 of the License, or (at your option) any later version.
//|
//| This library is distributed in the hope that it will be useful,
//| but WITHOUT ANY WARRANTY; without even the implied warranty of
//| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//| Library General Public License for more details.
//|
//| You should have received a copy of the GNU Library General Public
//| License along with this library; if not, write to the Free Software
//| Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
//| USA.
//|
//| Please report all bugs and problems to "matthias@mediaone.net".
//|

// create free shaped cursors. The cursor size can either be
// 16x16 or 32x32. Two colors and transparency are support.
// To stay compatible with prvious versions of FLTK, I did
// not chage the size of "enum Fl_Cursor" which limits us
// to 180 custom cursor shapes. The next version will get around
// that limit.
//
// There is no X Version yet due to the lack of a decent 
// Unix machine in my houshold. Anybody?

// The "and" and "xor" members shoul represent bit arrays with the bit meaning
// and xor
//-------------
//  0   0  outline color
//  0   1  fill color
//  1   0  transparent
//  1   1  don't use

// A new "shape" method that reads Pixmaps is planned

#define SWAP_BYTES

#include "Fl_Cursor_Shape.H"
#include <FL/fl_draw.H>

#ifdef WIN32
# include <malloc.h>
#elif defined(__APPLE__)
#else
# include <malloc.h>
# include <string.h>
#endif

#ifndef __APPLE__
#include <FL/x.H>
#endif

//|
//| activate this cursor shape for the given application
//|
void fl_cursor_ex(Fl_Cursor_Shape *c) {
  Fl_Window *win = Fl::first_window();
  if ( !win ) 
	  return;
#ifdef WIN32
  Fl_X::i( win )->cursor = c->handle();
  SetCursor( c->handle() );
#elif defined(__APPLE__)
  //SetCursor( c->handle() );
#else
  XDefineCursor( fl_display, fl_xid(win), c->handle() );
#endif
}

//|
//| we need to swap bytes if we are on a LSB-First machine. The only way to test this at
//| compile time is through preprocessor defines which are different for different compilers.
//| Runtime checks would slow these routines down pretty much... .
//| 

// Unix requires us to swap bits!
#ifdef WIN32
# define SB(x) x
#elif defined(__APPLE__)
# define SB(x) x
#else
      static uchar hiNibble[16] =
      { 0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
	0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0 };
      static uchar loNibble[16] =
      { 0x00, 0x08, 0x04, 0x0c, 0x02, 0x0a, 0x06, 0x0e,
	0x01, 0x09, 0x05, 0x0d, 0x03, 0x0b, 0x07, 0x0f };
# define SB(b) ( ( hiNibble[b&15] ) | ( loNibble[(b>>4)&15] ) )
#endif

#if defined(SWAP_BYTES) && !defined(__APPLE__)
  static unsigned int intelSwap32(unsigned int x) { 
    union { unsigned int vs; unsigned char ds[4]; };
    union { unsigned int vd; unsigned char dd[4]; };
    vs = x;
    dd[0] = SB( ds[3] );
    dd[1] = SB( ds[2] );
    dd[2] = SB( ds[1] );
    dd[3] = SB( ds[0] );
    return vd;
  } 
  static unsigned short intelSwap16(unsigned short x) { 
    union { unsigned short vs; unsigned char ds[2]; };
    union { unsigned short vd; unsigned char dd[2]; };
    vs = x;
    dd[0] = SB( ds[1] );
    dd[1] = SB( ds[0] );
    return vd;
  } 
#else
# ifdef WIN32
  static unsigned int intelSwap32(unsigned int x) { 
    union { unsigned int v; unsigned char d[4]; };
    d[0] = SB( d[0] );
    d[1] = SB( d[1] );
    d[2] = SB( d[2] );
    d[3] = SB( d[3] );
    return v;
  } 
  static unsigned short intelSwap16(unsigned short x) { 
    union { unsigned short v; unsigned char d[2]; };
    d[0] = SB( d[0] );
    d[1] = SB( d[1] );
    return v;
  } 
# elif defined(__APPLE__)
#  define intelSwap32(x) x
#  define intelSwap16(x) x
# else
#  define intelSwap32(x) x
#  define intelSwap16(x) x
# endif
#endif


//|________________________________________________________________
//| machine independent part

//|
//| initialize the cursor to 16x16 2 colors + mask
//|
Fl_Cursor_Shape::Fl_Cursor_Shape(int hotx, int hoty, unsigned short *andPattern, unsigned short *xorPattern, Fl_Color ol, Fl_Color fl) {
  init();
  shape(hotx, hoty, andPattern, xorPattern, ol, fl);
}


//|
//| initialize the cursor to 32x32 2 colors + mask
//|
Fl_Cursor_Shape::Fl_Cursor_Shape(int hotx, int hoty, unsigned int *andPattern, unsigned int *xorPattern, Fl_Color ol, Fl_Color fl) {
  init();
  shape(hotx, hoty, andPattern, xorPattern, ol, fl);
}


//|
//| preset everything nicely
//|
void Fl_Cursor_Shape::init() {
  handle_ = 0L;
  ol_ = FL_BLACK;
  fl_ = FL_WHITE;
}


//|________________________________________________________________
#ifdef WIN32
//| windows part

//|
//| Windows 95/98 supports only one single cursor size (usually 32x32)
//| Windows also supports full color cursors which I will ignore here for now
//|
int Fl_Cursor_Shape::wdt_ = GetSystemMetrics(SM_CXCURSOR);
int Fl_Cursor_Shape::hgt_ = GetSystemMetrics(SM_CYCURSOR);


//| WIN32:
//| set or change the cursor to 16x16 2 colors + mask
//|
void Fl_Cursor_Shape::shape(
		int hotx, int hoty, 
		unsigned short *andPattern, unsigned short *xorPattern, 
		Fl_Color ol, Fl_Color fl) 
{
  ol_ = ol; fl_ = fl;
  int x = (wdt_+31)>>5; x = x<<1;
  unsigned short *And = (unsigned short*)malloc(x*hgt_*2); memset(And, 0xff, x*hgt_*2);
  unsigned short *Xor = (unsigned short*)calloc(x*hgt_, 2);
  unsigned short *dAnd = And, *dXor = Xor, *sAnd = andPattern, *sXor = xorPattern;
  for (int i=0; i<16; i++) {
    *dAnd = intelSwap16(*sAnd++); dAnd += x;
    *dXor = intelSwap16(*sXor++); dXor += x;
  }
  handle_ = CreateCursor(fl_display, hotx, hoty, wdt_, hgt_, And, Xor);
  free(And);
  free(Xor);
  color(ol, fl); //: ignored on WIN32 anyway
}


//| WIN32:
//| set or change the cursor to 32x32 2 colors + mask
//|
void Fl_Cursor_Shape::shape(
		int hotx, int hoty, 
		unsigned int *andPattern, unsigned int *xorPattern, 
		Fl_Color ol, Fl_Color fl) 
{
  ol_ = ol; fl_ = fl;
  int x = (wdt_+31)>>5; x = x;	//: wdt_ can be any value divedable by int boundaries (32)
  unsigned int *And = (unsigned int*)malloc(x*hgt_*4); memset(And, 0xff, x*hgt_*4);
  unsigned int *Xor = (unsigned int*)calloc(x*hgt_, 4);
  unsigned int *dAnd = And, *dXor = Xor, *sAnd = andPattern, *sXor = xorPattern;
  for (int i=0; i<32; i++) {
    *dAnd = intelSwap32(*sAnd++); dAnd += x;
    *dXor = intelSwap32(*sXor++); dXor += x;
  }
  handle_ = CreateCursor(fl_display, hotx, hoty, wdt_, hgt_, And, Xor);
  free(And);
  free(Xor);
  color(ol, fl); //: ignored on Win32 anyway
}


//| WIN32:
//| destroy the cursor shape and remove us from the list
//|
Fl_Cursor_Shape::~Fl_Cursor_Shape() {
  if (handle_) DestroyCursor(handle_);
}


//| WIN32:
//| just copy the values... .
//|
void Fl_Cursor_Shape::color(Fl_Color ol, Fl_Color fl) { ol_ = ol; fl_ = fl; }

//|________________________________________________________________
#elif defined(__APPLE__)
//| Mac OS Carbon part

int Fl_Cursor_Shape::wdt_ = 16;	//: how do we determine the maximum size in X? Can we use any size? Up to some size?
int Fl_Cursor_Shape::hgt_ = 16;


//| Mac OS Carbon:
//| set or change the cursor to 16x16 2 colors + mask
//|
void Fl_Cursor_Shape::shape(
		int hotx, int hoty, 
		unsigned short *andPattern, unsigned short *xorPattern, 
		Fl_Color ol, Fl_Color fl) 
{
  if (!handle_)
    handle_ = (CursPtr)calloc(1,sizeof(Cursor));
  if (!handle_)
    handle_ = (CursPtr)calloc(1,sizeof(Cursor));
  ol_ = ol; fl_ = fl;
  unsigned short dA, dX;
  for (int i=0; i<16; i++) {
    dA = *andPattern++;
    dX = *xorPattern++;
    handle_->data[i] = ~( dA | dX);   // smart, huh? ;-)
    handle_->mask[i] = ~ dA;
  }
  handle_->hotSpot.h = hotx;
  handle_->hotSpot.v = hoty;
}


//| Mac OS Carbon:
//| set or change the cursor to 32x32 2 colors + mask
//|
void Fl_Cursor_Shape::shape(
		int hotx, int hoty, 
		unsigned int *andPattern, unsigned int *xorPattern, 
		Fl_Color ol, Fl_Color fl) 
{
  if (!handle_)
    handle_ = (CursPtr)calloc(1,sizeof(Cursor));
  ol_ = ol; fl_ = fl;
  unsigned int And[32];
  unsigned int Xor[32];
  unsigned int *dAnd = And, *dXor = Xor, dA, dX;
  for (int i=0; i<32; i++) {
    dA = *andPattern++;
    dX = *xorPattern++;
    *dAnd++ = ~( dA | dX);   // smart, huh? ;-)
    *dXor++ = ~ dA;
  }
  for (int i=0; i<16; i++) {
    handle_->data[i] = And[i]>>16;
    handle_->mask[i] = Xor[i]>>16;
  }
  handle_->hotSpot.h = hotx;
  handle_->hotSpot.v = hoty;
}


//| Mac OS Carbon: 
//| destroy the cursor shape and remove us from the list
//|
Fl_Cursor_Shape::~Fl_Cursor_Shape() {
  if (handle_) free(handle_);
}


//| Mac OS Carbon:
//| recolor the cursor
//|
void Fl_Cursor_Shape::color(Fl_Color fg, Fl_Color bg) { ol_ = fg; fl_ = bg; }

//|________________________________________________________________
#else 
//| Unix part

int Fl_Cursor_Shape::wdt_ = 32;	//: how do we determine the maximum size in X? Can we use any size? Up to some size?
int Fl_Cursor_Shape::hgt_ = 32;


//| XWindows:
//| set or change the cursor to 16x16 2 colors + mask
//|
void Fl_Cursor_Shape::shape(
		int hotx, int hoty, 
		unsigned short *andPattern, unsigned short *xorPattern, 
		Fl_Color ol, Fl_Color fl) 
{
  ol_ = ol; fl_ = fl; 
  unsigned short And[16];
  unsigned short Xor[16];
  unsigned short *dAnd = And, *dXor = Xor, dA, dX;
  for (int i=0; i<16; i++) { 
    dA = intelSwap16(*andPattern++);
    dX = intelSwap16(*xorPattern++);
    *dAnd++ = ~( dA | dX);   // smart, huh? ;-)
    *dXor++ = ~ dA;
  } 
  XColor dummy; 
  Pixmap andMap = XCreateBitmapFromData(fl_display, RootWindow(fl_display, fl_screen), 
                (char*)And, 16, 16); 
  Pixmap xorMap = XCreateBitmapFromData(fl_display, RootWindow(fl_display, fl_screen), 
                (char*)Xor, 16, 16); 
  if ( handle_ ) XFreeCursor( fl_display, handle_ );
  handle_ = XCreatePixmapCursor(fl_display, andMap, xorMap, &dummy, &dummy, hotx, hoty); 
  XFreePixmap(fl_display, andMap); 
  XFreePixmap(fl_display, xorMap); 
  color(ol, fl);
}


//| XWindows:
//| set or change the cursor to 32x32 2 colors + mask
//|
void Fl_Cursor_Shape::shape(
		int hotx, int hoty, 
		unsigned int *andPattern, unsigned int *xorPattern, 
		Fl_Color ol, Fl_Color fl) 
{
  ol_ = ol; fl_ = fl;
  unsigned int And[32];
  unsigned int Xor[32];
  unsigned int *dAnd = And, *dXor = Xor, dA, dX;
  for (int i=0; i<32; i++) {
    dA = intelSwap32(*andPattern++);
    dX = intelSwap32(*xorPattern++);
    *dAnd++ = ~( dA | dX);   // smart, huh? ;-)
    *dXor++ = ~ dA;
  }
  XColor dummy;
  Pixmap andMap = XCreateBitmapFromData(fl_display, RootWindow(fl_display, fl_screen),
                (char*)And, 32, 32);
  Pixmap xorMap = XCreateBitmapFromData(fl_display, RootWindow(fl_display, fl_screen),
                (char*)Xor, 32, 32);
  if ( handle_ ) XFreeCursor( fl_display, handle_ );
  handle_ = XCreatePixmapCursor(fl_display, andMap, xorMap, &dummy, &dummy, hotx, hoty);
  XFreePixmap(fl_display, andMap);
  XFreePixmap(fl_display, xorMap);
  color(ol, fl);
}


//| XWindows: 
//| destroy the cursor shape and remove us from the list
//|
Fl_Cursor_Shape::~Fl_Cursor_Shape() {
  if (handle_) XFreeCursor(fl_display, handle_);
}


//| XWindows:
//| recolor the cursor
//|
void Fl_Cursor_Shape::color(Fl_Color fg, Fl_Color bg) {
  if ( !handle_ ) return;
  XColor fgc;
  uchar r,g,b;
  Fl::get_color(fg,r,g,b);
  fgc.red = r<<8; fgc.green = g<<8; fgc.blue = b<<8;
  XColor bgc;
  Fl::get_color(bg,r,g,b);
  bgc.red = r<<8; bgc.green = g<<8; bgc.blue = b<<8;
  XRecolorCursor(fl_display, handle_, &fgc, &bgc);
}


#endif



#ifdef intelSwap16
#undef intelSwap16
#endif

#ifdef intelSwap32
#undef intelSwap32
#endif

