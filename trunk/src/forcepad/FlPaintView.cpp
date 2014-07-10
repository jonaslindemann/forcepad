#include "FlPaintView.h"

#include <string>

#include <FL/fl_ask.h>
#include <FL/fl_file_chooser.h>
#include <FL/Fl_Native_File_Chooser.H>


#include "NewModelDlg.h"

#ifndef __APPLE__
#include "Cursors.h"
#endif

#undef NATIVE_FS

using namespace std;

CFlPaintView::CFlPaintView(int x,int y,int w,int h,const char *l)
: Fl_Gl_Window(x,y,w,h,l), CPaintView(x, y, w, h, l)
{
    //so_print("CFlPaintView","CPaintView(...)");
    
    this->doCreateCursors();
}

CFlPaintView::~CFlPaintView()
{
    
}

/////////////////////////////////////////////////////////////
// CPaintView FLTK overrides
/////////////////////////////////////////////////////////////

int CFlPaintView::height()
{
    return this->h();
}

int CFlPaintView::width()
{
    return this->w();
}

void CFlPaintView::doRedraw()
{
    this->redraw();
}

void CFlPaintView::doFlush()
{
    this->flush();
}

void CFlPaintView::doInvalidate()
{
    this->invalidate();
}

void CFlPaintView::doMakeCurrent()
{
    this->make_current();
}

void CFlPaintView::doCreateCursors()
{
#ifndef __APPLE__
    int i;
    
    for (i=0; i<20; i++)
        m_cursors[i] = new Fl_Cursor_Shape();
    
    m_cursors[0]->shape( rshape4_hotX, rshape4_hotY, rshape4_and, rshape4_xor );
    m_cursors[1]->shape( rshape8_hotX, rshape8_hotY, rshape8_and, rshape8_xor );
    m_cursors[2]->shape( rshape16_hotX, rshape16_hotY, rshape16_and, rshape16_xor );
    m_cursors[3]->shape( rshape32_hotX, rshape32_hotY, rshape32_and, rshape32_xor );
    m_cursors[4]->shape( rshape32_hotX, rshape32_hotY, rshape32_and, rshape32_xor );
    m_cursors[5]->shape( sshape4_hotX, sshape4_hotY, sshape4_and, sshape4_xor );
    m_cursors[6]->shape( sshape8_hotX, sshape8_hotY, sshape8_and, sshape8_xor );
    m_cursors[7]->shape( sshape16_hotX, sshape16_hotY, sshape16_and, sshape16_xor );
    m_cursors[8]->shape( sshape32_hotX, sshape32_hotY, sshape32_and, sshape32_xor );
    m_cursors[9]->shape( sshape32_hotX, sshape32_hotY, sshape32_and, sshape32_xor );
    m_cursors[10]->shape( cross_select_hotX, cross_select_hotY, cross_select_and, cross_select_xor );
    m_cursors[11]->shape( cross_load_hotX, cross_load_hotY, cross_load_and, cross_load_xor );
    m_cursors[12]->shape( cross_bc_hotX, cross_bc_hotY, cross_bc_and, cross_bc_xor );
    m_cursors[13]->shape( cross_line_hotX, cross_line_hotY, cross_line_and, cross_line_xor );
    m_cursors[14]->shape( cross_rect_hotX, cross_rect_hotY, cross_rect_and, cross_rect_xor );
    m_cursors[15]->shape( cross_circle_hotX, cross_circle_hotY, cross_circle_and, cross_circle_xor );
    m_cursors[16]->shape( cross_bucket_hotX, cross_bucket_hotY, cross_bucket_and, cross_bucket_xor );
    m_cursors[17]->shape( erase_hotX, erase_hotX, erase_and, erase_xor );
#endif
}

void CFlPaintView::doUpdateCursor(TEditMode mode)
{
#ifndef __APPLE__
    switch (mode) {
            //case EM_BRUSH:
        case EM_DIRECT_BRUSH:
            //case EM_ERASE:
        case EM_DIRECT_ERASE:
            fl_cursor_ex( m_cursors[m_currentBrushIdx] );
            break;
        case EM_SELECT_BOX:
            fl_cursor_ex( m_cursors[10] );
            break;
        case EM_FORCE:
            fl_cursor_ex( m_cursors[11] );
            break;
        case EM_CONSTRAINT:
            fl_cursor_ex( m_cursors[12] );
            break;
        case EM_CONSTRAINT_VECTOR:
            fl_cursor_ex( m_cursors[12] );
            break;
        case EM_CONSTRAINT_HINGE:
            fl_cursor_ex( m_cursors[12] );
            break;
        case EM_LINE:
            fl_cursor_ex( m_cursors[13] );
            break;
        case EM_RECTANGLE:
            fl_cursor_ex( m_cursors[14] );
            break;
        case EM_ELLIPSE:
            fl_cursor_ex( m_cursors[15] );
            break;
        case EM_FLOODFILL:
            fl_cursor_ex( m_cursors[16] );
            break;
        case EM_ERASE_CONSTRAINTS_FORCES:
            fl_cursor_ex( m_cursors[17] );
            break;
        default:
            fl_cursor( FL_CURSOR_DEFAULT );
    }
#endif
}

void CFlPaintView::doDeleteCursors()
{
#ifndef __APPLE__
    int i;
    
    for (i=0; i<20; i++)
        delete m_cursors[i];
#endif
}

void CFlPaintView::draw()
{
    // Clear screen
    
    if (m_runOnce)
    {
        m_runOnce = false;
    }
    
    onClear();
    
    if (!Fl_Gl_Window::valid())
        onInitContext();
    
    glPushMatrix();
    onDraw();
    glPopMatrix();
}


int CFlPaintView::handle(int event)
{
    //
    // Overridden FLTK handle method for capturing
    // events.
    //
    
    int x = Fl::event_x();
    int y = Fl::event_y();
    
    int sx, sy;
    
    // If the SHIFT button is down coordinates are snapped
    // to a grid
    
    if ((Fl::event_state()&FL_SHIFT)>0)
    {
        sx = x - m_drawingOffsetX;
        sy = y - m_drawingOffsetY;
        m_femGrid->snapToGrid(sx, sy);
        x = sx + m_drawingOffsetX;
        y = sy + m_drawingOffsetY;
    }
    else
    {
        if (m_snapToGrid)
        {
            sx = x - m_drawingOffsetX;
            sy = y - m_drawingOffsetY;
            m_femGrid->snapToGrid(sx, sy);
            x = sx + m_drawingOffsetX;
            y = sy + m_drawingOffsetY;
        }
    }
    
    /*
     if ((Fl::event_state()&FL_CTRL)>0)
     m_zoomResults = true;
     else
     m_zoomResults = false;
     */
    
    // Call the different event methods
    
    switch (event) {
        case FL_PUSH:
            m_leftMouseDown = true;
            onPush(x, y);
            return 1;
            break;
        case FL_DRAG:
            onDrag(x, y);
            return 1;
        case FL_RELEASE:
            if (!m_danglingRelease)
                onRelease(x, y);
            else
                m_danglingRelease = false;
            return 1;
            break;
        case FL_MOVE:
            onMove(x, y);
            return 1;
            break;
        case FL_ENTER:
            updateCursor();
            return 1;
            break;
        case FL_LEAVE:
#ifndef __APPLE__
            fl_cursor( FL_CURSOR_DEFAULT );
#endif
            return 1;
            break;
        case FL_MOUSEWHEEL:
            onMouseWheel(Fl::event_dx(), Fl::event_dy());
            return 1;
            break;
        default:
            return Fl_Gl_Window::handle(event);
    }
    return Fl_Gl_Window::handle(event);
}

const std::string CFlPaintView::doSaveDialog(const string title, const string filter, const string defaultFilename)
{
#ifdef NATIVE_FS
    // Create native chooser
    Fl_Native_File_Chooser native;
    
    std::string fname;
    
    native.title("Save model");
    native.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
    native.filter("ForcePAD\t*.fp2\n");
    native.preset_file(defaultFilename.c_str());
    // Show native chooser
    switch ( native.show() ) {
        case -1: fprintf(stderr, "ERROR: %s\n", native.errmsg()); break;    // ERROR
        case  1: fprintf(stderr, "*** CANCEL\n"); fl_beep(); break;         // CANCEL
        default:                                                            // PICKED FILE
            if ( native.filename() ) {
                fname = native.filename();
            } else {
                fname = "";
            }
            break;
    }
#else
    std::string fname = fl_file_chooser(title.c_str(), filter.c_str(), defaultFilename.c_str());
#endif
    return fname;
}

bool CFlPaintView::doNewModel(int &width, int &height, int& initialStiffness)
{
    CNewModelDlg* dlg = new CNewModelDlg();
    dlg->setSize(640, 480);
    dlg->centerWindow(this->window());
    dlg->show();
    
    if (dlg->getModalResult()!=MR_CANCEL)
    {
        dlg->getSize(width, height);
        initialStiffness = dlg->getInitialStiffness();
        
        delete dlg;
        return true;
    }
    else
    {
        delete dlg;
        return false;
    }
}

void CFlPaintView::doInfoMessage(const string message)
{
    fl_message(message.c_str());
}

bool CFlPaintView::doAskYesNo(const string question)
{
    return fl_ask(question.c_str());
}

const std::string CFlPaintView::doOpenDialog(const string title, const string filter)
{
#ifdef NATIVE_FS
    // Create native chooser
    Fl_Native_File_Chooser native;
    
    std::string fname;
    
    native.title("Pick a file");
    native.type(Fl_Native_File_Chooser::BROWSE_FILE);
    native.filter("ForcePAD\t*.fp2\n");
    native.preset_file("noname.fp2");
    // Show native chooser
    switch ( native.show() ) {
        case -1: fprintf(stderr, "ERROR: %s\n", native.errmsg()); break;    // ERROR
        case  1: fprintf(stderr, "*** CANCEL\n"); fl_beep(); break;         // CANCEL
        default:                                                            // PICKED FILE
            if ( native.filename() ) {
                fname = native.filename();
            } else {
                fname = "";
            }
            break;
    }
    
#else
    const char *name;
    name = fl_file_chooser(title.c_str(), filter.c_str(), "");
    std::string fname = name;
#endif
    cout << "selected filename = " << fname << endl;
    return fname;
}

void CFlPaintView::doShowAbout()
{
#ifdef WIN32
    ShellExecute(0, "open", "http://forcepad.sourceforge.net", NULL, NULL, SW_SHOWNORMAL);
#endif
}

void CFlPaintView::doShowHelp()
{
#ifdef WIN32
#ifdef FORCEPAD_KIOSK
    ShellExecute(0, "open", ".\\kiosk\\ForcePAD.pdf", NULL, NULL, SW_SHOWNORMAL);
#endif
#endif
}
