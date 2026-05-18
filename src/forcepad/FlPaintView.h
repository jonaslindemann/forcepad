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
    void draw() override;

    /** FLTK handle() method virtual override. */
    int handle(int event) override;

protected:
    virtual int height() override;
    virtual int width() override;
    virtual void doRedraw() override;
    virtual void doFlush() override;
    virtual void doInvalidate() override;
    virtual void doMakeCurrent() override;
    virtual const std::string doSaveDialog(const string title, const string filter, const string defaultFilename) override;
    virtual bool doNewModel(int &width, int &height, int& initialStiffness) override;
    virtual void doInfoMessage(const string message) override;
    virtual bool doAskYesNo(const string question) override;
    virtual const std::string doOpenDialog(const string title, const string filter) override;
    virtual void doCreateCursors() override;
    virtual void doUpdateCursor(TEditMode mode) override;
    virtual void doDeleteCursors() override;
    virtual void doShowAbout() override;
    virtual void doShowHelp() override;
};

#endif
