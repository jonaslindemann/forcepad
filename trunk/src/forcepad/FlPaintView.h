//
// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2008 Division of Structural Mecahnics, Lund University
//
// Written by Jonas Lindemann
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//
// Comments and suggestions to jonas.lindemann@byggmek.lth.se
//

#ifndef _CFlPaintView_h_
#define _CFlPaintView_h_

#include "forcepad_config.h"

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>

#include <FL/gl.h>

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include <FL/fl_draw.H>

#ifndef __APPLE__
#include "Fl_Cursor_Shape.H"
#endif

#include "PaintView.h"

class CFlPaintView : public Fl_Gl_Window, public CPaintView {
private:
    #ifndef __APPLE__
        Fl_Cursor_Shape* m_cursors[20];
    #endif
public:
    CFlPaintView(int x,int y,int w,int h,const char *l=0);
    virtual ~CFlPaintView();

    /*
     *    Virtual overrides from FLTK
     */

    /** FLTK draw() method virtual override. */
    void draw();

    /** FLTK handle() method virtual override. */
    int handle(int event);

protected:
    virtual int height();
    virtual int width();
    virtual void doRedraw();
    virtual void doFlush();
    virtual void doInvalidate();
    virtual void doMakeCurrent();
    virtual const std::string doSaveDialog(const string title, const string filter, const string defaultFilename);
    virtual bool doNewModel(int &width, int &height, int& initialStiffness);
    virtual void doInfoMessage(const string message);
    virtual bool doAskYesNo(const string question);
    virtual const std::string doOpenDialog(const string title, const string filter);
    virtual void doCreateCursors();
    virtual void doUpdateCursor(TEditMode mode);
    virtual void doDeleteCursors();
    virtual void doShowAbout();
    virtual void doShowHelp();
};

#endif
