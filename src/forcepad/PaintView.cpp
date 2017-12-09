//
// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2009 Division of Structural Mechanics, Lund University
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

#include "PaintView.h"

#include "FemGridSolver2.h"
#include "MainFrame2.h"
#include "LogWindow.h"
#include "JpegImage.h"
#include "PngImage.h"

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

//#include <FL/filename.H>
//#include <FL/fl_draw.H>

#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

#include <newmat.h>
#include <newmatio.h>
#include <newmatap.h>

#include "UiSettings.h"

#ifdef use_namespace
using namespace NEWMAT;
#endif

/////////////////////////////////////////////////////////////
// Structures used for copy and paste in Windows
/////////////////////////////////////////////////////////////

#ifdef WIN32

typedef struct _FPGLRGBTRIPLE {
	BYTE rgbRed ;
	BYTE rgbGreen ;
	BYTE rgbBlue ;
} FPGLRGBTRIPLE ;

typedef struct _FPRGBTRIPLE { //rgbt
	BYTE rgbtBlue ;
	BYTE rgbtGreen ;
	BYTE rgbtRed ;
} FPRGBTRIPLE ;

#endif

void splitPath(const std::string& fullPathAndFilename, std::string& path, std::string& filename)
{
	using namespace std;
	string fullPath(fullPathAndFilename);
	replace(fullPath.begin(), fullPath.end(), '\\', '/');
	string::size_type lastSlashPos = fullPath.find_last_of("/");
	if (lastSlashPos==std::string::npos)
	{
		path = "";
		filename = fullPath;
	}
	else
	{
		path=fullPath.substr(0,lastSlashPos);
		filename=fullPath.substr(lastSlashPos+1,fullPath.size()-lastSlashPos-1);
	}
}

class CPaintView;

/////////////////////////////////////////////////////////////
// CPaintView constructor destructor
/////////////////////////////////////////////////////////////

CPaintView::CPaintView(int x,int y,int w,int h,const char *l)
{
	so_print("CPaintView","CPaintView(...)");

	m_lastSize[0] = 0;
	m_lastSize[1] = 0;

	// Initialize general state variables
	
	m_drawingOffsetX = 0;
	m_drawingOffsetY = 0;
	m_gridSpacing = 8;
	m_editMode = EM_DIRECT_BRUSH;
	m_viewMode = VM_PHYSICS;
	m_constraintType = CConstraint::CT_XY;
	m_mainFrame = NULL;
	
	m_lockDrawing = false;
	m_danglingRelease = false;
	
	m_cg[0] = -1;
	m_cg[1] = -1;
	
	m_dirty = false;
	m_showMesh = false;
	m_snapToGrid = false;
	m_useWeight = false;
	m_useBlendingExtension = false;
	m_calcCG = false;
	m_resized = true;
	m_drawImage = false;
	m_importMode = IM_NEW_MODEL;

	// new Zoom mode

	m_zoomResults = false;
	m_zoomFactor = 0.1;
	m_zoomPos[0] = w/2;
	m_zoomPos[1] = h/2;

	// Calculation settings

	m_forceMagnitude = 1000.0;
	m_weight = 1000.0;
	m_useWeight = false;
	m_outputMatlab = false;
	m_stiffnessScalefactor = 1000.0;
	m_elasticModulus = 2.1e9;
	m_youngsModulus = 0.35;
	m_thickness = 0.1;
	m_constraintStiffnessScale = 1e3;
	m_moveLoad = false;

	// Optimisation parameters

	m_optVolfrac = 0.5;
	m_optMinChange = 0.01;
	m_optMaxLoops = 100;
	m_optRmin = 2.75;
	m_optFilterType = CFemGridSolver2::FT_SHARP_CONTOURING;

	// Test OpenGL version

	m_checkOpenGL = true;
	
	// Initialize mouse variables
	
	m_oldPos[0] = -1;
	m_oldPos[1] = -1;
	m_leftMouseDown = false;
		
	// Create drawing tools
	
	CColorPtr selectionColor = new CColor();
	selectionColor->setColor(1.0f, 0.0f, 0.0f, 1.0f);
	
	m_selectionBox = new CRectangle();
	m_selectionBox->setRectangleType(CRectangle::RT_OUTLINE);
	m_selectionBox->setLineColor(selectionColor);
	m_selectionBox->setLineWidth(1.0);
	m_selectionBox->setLineType(CRectangle::LT_DASHED);
	m_selectionBox->setSize(200.0,100.0);
	m_selectionBox->setPosition(50.0, 50.0);
	
	CColorPtr color = new CColor();
	color->setColor(0.0f, 0.0f, 0.0f, 0.5f);
	
	m_rectangle = new CRectangle();
	m_rectangle->setColor(color);
	
	m_ellipse = new CEllipse();
	m_ellipse->setColor(color);
	m_ellipse->setSectors(36);
	
	m_line = new CLine();
	m_line->setColor(color);
	m_line->setWidth(4);

    m_arch = new CArch();
    m_arch->setColor(color);
    m_arch->setLineWidth(4);
	
	color = new CColor();
	color->setColor(0.5f, 0.0f, 0.0f, 1.0f);

	color = new CColor();
	color->setColor(0.0f, 0.5f, 0.0f, 1.0f);

	// Rigid body variables
	
	m_cgIndicator = new CCGIndicator();
	m_cgIndicator->setColor(color);
	m_relativeForceSize = 0.05;
	
	// Optimisation constraints

	m_optLayerActive = false;
	m_optConstraintColor[0] = 1.0;
	m_optConstraintColor[1] = 0.0;
	m_optConstraintColor[2] = 0.0;
	
	// Create drawing area
	
	m_drawing = new CImage(2);
	m_drawing->setChannels(4);
	m_drawing->setSize(640,480);
	m_drawing->fillColor(255,255,255);

	m_drawing->setLayer(1);
	m_drawing->fillColor(255,255,255);
	m_drawing->fillAlpha(128);
	m_drawing->setAlpha(128);
	m_drawing->setLayer(0);

	m_ruler = new CRuler();
	m_ruler->setStartPos(0,0);
    m_ruler->setEndPos(m_drawing->getWidth(), 0);
	m_ruler->setActualLength(1.0);
	m_ruler->setColor(color);

	// Create a buffer for reading pixels back
	
	m_buffer = new CImage();
	m_buffer->setChannels(4);
	m_buffer->setSize(64,64);
	
	// Create image grid
	
	m_femGrid = new CFemGrid2();
	m_femGrid->setImage(m_drawing);
	m_femGrid->setStride(6);
	m_femGrid->setAverageStress(true);
	
	// Create clipboard
		
	m_clipboard = new CForcePadClipboard();
	m_clipboard->setImage(m_drawing);
	m_clipboard->setFemGrid(m_femGrid);
	
	// Create clipboard used in undo operations
	
	m_undoClipboard = new CClipboard();
	m_undoClipboard->setImage(m_drawing);
	m_undoClipboard->setPasteMode(CClipboard::PM_REPLACE);
	
	m_screenImage = new CScreenImage();
	m_screenImage->setImage(m_drawing);
	
	resetUndoArea();
	
	// Create dialogs
	
	m_modeChangeEvent = NULL;
	m_viewModeChangeEvent = NULL;
	m_viewModeErrorEvent = NULL;
	m_statusMessageEvent = NULL;
	m_logMessageEvent = NULL;
	m_modelChangedEvent = NULL;
	m_rulerChangedEvent = NULL;
	m_visualisationModeChangedEvent = NULL;
	m_modelLoadedEvent = NULL;
	m_newModelEvent = NULL;

	// Set initial model name
	
	setModelName("noname.fp2");

	m_runOnce = true;
}


CPaintView::~CPaintView()
{
    so_print("CPaintView","~CPaintViewidth()");
	
	// Do the usual cleanup
	
	deleteBrushes();	
	deleteCursors();
}

int CPaintView::height()
{
    return -1;
}

int CPaintView::width()
{
    return -1;
}

void CPaintView::doRedraw()
{
}

void CPaintView::doFlush()
{
}

void CPaintView::doInvalidate()
{
}

void CPaintView::doMakeCurrent()
{
}

const std::string CPaintView::doSaveDialog(const string title, const string filter, const string defaultFilename)
{
    return "";
}

bool CPaintView::doNewModel(int &width, int &height, int& initialStiffness)
{
    return false;
}

void CPaintView::doInfoMessage(const string message)
{

}

bool CPaintView::doAskYesNo(const string question)
{
    return false;
}

const std::string CPaintView::doOpenDialog(const string title, const string filter)
{
    return "";
}

void CPaintView::doCreateCursors()
{

}

void CPaintView::doUpdateCursor(TEditMode mode)
{

}

void CPaintView::doDeleteCursors()
{

}

void CPaintView::doShowAbout()
{

}

void CPaintView::doShowHelp()
{

}

/////////////////////////////////////////////////////////////
// CPaintView event methods
/////////////////////////////////////////////////////////////

void CPaintView::onMouseWheel(int dx, int dy)
{
	if (m_zoomResults && (m_viewMode == VM_ACTION))
	{
		if (dy>0)
		{
			this->zoomIn();
		}
		else
		{
			this->zoomOut();
		}
	}
}

void CPaintView::onPush(int x, int y)
{
	//
	// Handle FLTK push event (MouseDown)
	//

	int ww, hh;

	int sx, sy;

	if (m_snapToGrid)
	{
		sx = x - m_drawingOffsetX;
		sy = y - m_drawingOffsetY;
		m_femGrid->snapToGrid(sx, sy);
		x = sx + m_drawingOffsetX;
		y = sy + m_drawingOffsetY;
	}
	
	m_zoomStart[0] = m_zoomPos[0];
	m_zoomStart[1] = m_zoomPos[1];
	m_start[0] = x;
	m_start[1] = y;
	m_oldPos[0] = -1;
	m_leftMouseDown = true;
	CConstraintPtr constraint;

	updateCursor();
	resetUndoArea();
	
	switch (m_editMode) {
	case EM_DYNAMIC_FORCE_UPDATE:
		
		// Rotate existing force

		if (!m_zoomResults)
            m_selectedForce = m_femGrid->getNearestForce(x-m_drawingOffsetX, height()-y-m_drawingOffsetY);
		break;
	case EM_CONSTRAINT:
		
		// Create a constraint and add it to the grid
		
		constraint = new CConstraint();
        constraint->setPosition(x-m_drawingOffsetX, height()-y-m_drawingOffsetY);
		constraint->setConstraintType(m_constraintType);
		m_femGrid->addConstraint(constraint);
        this->doRedraw();
		break;
	case EM_FORCE:
		
		// Create a new force. The position is set here.
		// Direction is is done in the onDrag() method.
		
		m_newForce = new CForce();
        m_newForce->setPosition(x-m_drawingOffsetX, height()-y-m_drawingOffsetY);
		m_femGrid->addForce(m_newForce);
		break;
	case EM_CONSTRAINT_VECTOR:
		
		// Create a directional constraint (ForcePAD/Rigid)
		
		m_newConstraint = new CConstraint();
		m_newConstraint->setConstraintType(CConstraint::CT_VECTOR);
        m_newConstraint->setPosition(x-m_drawingOffsetX, height()-y-m_drawingOffsetY);
		m_newConstraint->setDirection(0.0, 1.0);
		m_femGrid->addConstraint(m_newConstraint);

		break;
	case EM_CONSTRAINT_HINGE:
		
		// Create a directional constraint (ForcePAD/Rigid)
		
		m_newConstraint = new CConstraint();
		m_newConstraint->setConstraintType(CConstraint::CT_HINGE);
        m_newConstraint->setPosition(x-m_drawingOffsetX, height()-y-m_drawingOffsetY);
		m_newConstraint->setDirection(0.0, 1.0);
		m_femGrid->addConstraint(m_newConstraint);

		break;
	case EM_ERASE_CONSTRAINTS_FORCES:
		
		// Erase constraints and forces
		
        m_femGrid->erasePointLoad(x-m_drawingOffsetX, height()-y-m_drawingOffsetY,16);
        m_femGrid->erasePointConstraint(x-m_drawingOffsetX, height()-y-m_drawingOffsetY,16);
		break;
	case EM_SELECT_BOX:
		
		// Set starting point for selection box
		
		m_selectionStart[0] = x-m_drawingOffsetX;
        m_selectionStart[1] = height()-y-m_drawingOffsetY;
		m_selectionEnd[0] = m_selectionStart[0];
		m_selectionEnd[1] = m_selectionStart[1];
		break;
	case EM_PASTE:
		
		// Paste from clipboard
		
        ww = m_clipboard->getClipboard()->getWidth()*m_brushScale;
		hh = m_clipboard->getClipboard()->getHeight()*m_brushScale;
		resetUndoArea();
		updateUndoArea(
            m_current[0]-m_drawingOffsetX-m_clipboard->getClipboard()->getWidth()/2,
            height() - m_current[1]-m_drawingOffsetY-m_clipboard->getClipboard()->getHeight()/2,
			0
			);
		updateUndoArea(
            m_current[0]-m_drawingOffsetX+m_clipboard->getClipboard()->getWidth()/2,
            height() - m_current[1]-m_drawingOffsetY+m_clipboard->getClipboard()->getHeight()/2,
			0
			);
		updateUndo();
        m_clipboard->paste(x-m_drawingOffsetX-ww/2, height()-y-m_drawingOffsetY-hh/2);
        this->doRedraw();
		break;
	case EM_DIRECT_BRUSH:
		
		// Update undo area
		
		undoToDrawing();
		updateUndo();
		break;
	default:
		break;
	}
	
	// We have to redraw the image
	
	if (m_editMode!=EM_RESULT)
        this->doRedraw();

	/*
	if (m_viewMode == VM_ACTION)
	{
		cout << "Zoom results:" << endl;
		if (m_zoomResults)
			m_zoomResults = false;
		else
			m_zoomResults = true;
        this->flush();
        this->doRedraw();
	}
	*/
}


void CPaintView::onDrag(int x, int y)
{
	//
	// Handle FLTK drag event
	//

	// Store current position

	int prevPos[2];
	prevPos[0] = m_current[0];
	prevPos[1] = m_current[1];

	float eraseColor[3];
	eraseColor[0] = eraseColor[1] = eraseColor[2] = 1.0;

	double moveDist = sqrt(pow((double)prevPos[0]-(double)x,2)+pow((double)prevPos[1]-(double)y,2));

	m_current[0] = x;
	m_current[1] = y;
	
	updateCursor();
	
	switch (m_editMode) {
	case EM_DYNAMIC_FORCE_UPDATE:

		// Modify selected force if any.

		if (m_selectedForce!=NULL)
		{	
			if (Fl::event_key(FL_Alt_L)||m_moveLoad)
			{
				// Update the position of the force

				//m_femGrid->removePointForce(m_selectedForce);
                //m_selectedForce->setPosition(x-m_drawingOffsetX, height()-y-m_drawingOffsetY);
				//m_femGrid->addForce(m_selectedForce);

                m_femGrid->moveForce(m_selectedForce, x-m_drawingOffsetX, height()-y-m_drawingOffsetY);

			}
			else
			{
				// Update the direction of the force created in onPush
				
                m_selectedForce->setDirection(m_start[0]-m_current[0], (height() - m_start[1]) - (height() - m_current[1]) );
			}

			m_solver->executeUpdate();
            this->doFlush();
            this->doRedraw();
		}

		break;
	case EM_DIRECT_ERASE:
	case EM_DIRECT_BRUSH:
		if (moveDist>1)
		{
			if (m_optLayerActive)
			{
				if (m_editMode == EM_DIRECT_ERASE)
					m_drawing->drawImageLine(
						m_currentBrush, 
                        prevPos[0]-m_drawingOffsetX-m_currentBrush->getWidth()/2,
                        height()-prevPos[1]-m_drawingOffsetY-m_currentBrush->getHeight()/2,
                        m_current[0]-m_drawingOffsetX-m_currentBrush->getWidth()/2,
                        height()-m_current[1]-m_drawingOffsetY-m_currentBrush->getHeight()/2,
						eraseColor
					);		
				else
					m_drawing->drawImageLine(
						m_currentBrush, 
                        prevPos[0]-m_drawingOffsetX-m_currentBrush->getWidth()/2,
                        height()-prevPos[1]-m_drawingOffsetY-m_currentBrush->getHeight()/2,
                        m_current[0]-m_drawingOffsetX-m_currentBrush->getWidth()/2,
                        height()-m_current[1]-m_drawingOffsetY-m_currentBrush->getHeight()/2,
						m_optConstraintColor
					);		
			}
			else
			{
				if (m_editMode == EM_DIRECT_ERASE)
					m_drawing->drawImageLine(
						m_currentBrush, 
                        prevPos[0]-m_drawingOffsetX-m_currentBrush->getWidth()/2,
                        height()-prevPos[1]-m_drawingOffsetY-m_currentBrush->getHeight()/2,
                        m_current[0]-m_drawingOffsetX-m_currentBrush->getWidth()/2,
                        height()-m_current[1]-m_drawingOffsetY-m_currentBrush->getHeight()/2,
						eraseColor
					);		
				else
					m_drawing->drawImageLine(
						m_currentBrush, 
                        prevPos[0]-m_drawingOffsetX-m_currentBrush->getWidth()/2,
                        height()-prevPos[1]-m_drawingOffsetY-m_currentBrush->getHeight()/2,
                        m_current[0]-m_drawingOffsetX-m_currentBrush->getWidth()/2,
                        height()-m_current[1]-m_drawingOffsetY-m_currentBrush->getHeight()/2,
						m_brushColor
                    );
			}
		}
		else
		{
			if (m_optLayerActive)
				if (m_editMode == EM_DIRECT_ERASE)
                    m_drawing->copyFrom(m_currentBrush, m_current[0]-m_drawingOffsetX-m_currentBrush->getWidth()/2, height()-m_current[1]-m_drawingOffsetY-m_currentBrush->getHeight()/2, eraseColor);
				else 
                    m_drawing->copyFrom(m_currentBrush, m_current[0]-m_drawingOffsetX-m_currentBrush->getWidth()/2, height()-m_current[1]-m_drawingOffsetY-m_currentBrush->getHeight()/2, m_optConstraintColor);
			else
				if (m_editMode == EM_DIRECT_ERASE)
                    m_drawing->copyFrom(m_currentBrush, m_current[0]-m_drawingOffsetX-m_currentBrush->getWidth()/2, height()-m_current[1]-m_drawingOffsetY-m_currentBrush->getHeight()/2, eraseColor);
				else
                    m_drawing->copyFrom(m_currentBrush, m_current[0]-m_drawingOffsetX-m_currentBrush->getWidth()/2, height()-m_current[1]-m_drawingOffsetY-m_currentBrush->getHeight()/2, m_brushColor);

		}
        this->doRedraw();
		break;
    case EM_ARCH:
        resetUndoArea();

        // Update the rectangle primitive

        if (this->m_optLayerActive)
            m_arch->getColor()->setColor(m_optConstraintColor);
        else
            m_arch->getColor()->setColor(m_brushColor);

        m_arch->setPosition(m_start[0], height() - m_start[1]);
        m_arch->setSize(m_current[0]-m_start[0], -m_current[1]+m_start[1]);

        // Update the affected area. We do this twice because
        // updateUndoArea only takes a position as input

        updateUndoArea(
            m_start[0] - m_drawingOffsetX,
            height() - m_start[1] - m_drawingOffsetY,
            (int)m_arch->getLineWidth()
            );
        updateUndoArea(
            m_current[0] - m_drawingOffsetX,
            height() - m_current[1] - m_drawingOffsetY,
            (int)m_arch->getLineWidth()
            );
        this->doRedraw();
        break;
    case EM_RECTANGLE:
		resetUndoArea();
		
		// Update the rectangle primitive

		if (this->m_optLayerActive)
			m_rectangle->getColor()->setColor(m_optConstraintColor);
		else
			m_rectangle->getColor()->setColor(m_brushColor);
		
        m_rectangle->setPosition(m_start[0], height() - m_start[1]);
		m_rectangle->setSize(m_current[0]-m_start[0], -m_current[1]+m_start[1]);
		
		// Update the affected area. We do this twice because
		// updateUndoArea only takes a position as input
		
		updateUndoArea(
			m_start[0] - m_drawingOffsetX,
            height() - m_start[1] - m_drawingOffsetY,
			0
			);
		updateUndoArea(
			m_current[0] - m_drawingOffsetX,
            height() - m_current[1] - m_drawingOffsetY,
			0
			);
        this->doRedraw();
		break;
	case EM_ELLIPSE:
		resetUndoArea();
		
		// Update the ellipse primitive
		
		if (this->m_optLayerActive)
			m_ellipse->getColor()->setColor(m_optConstraintColor);
		else
			m_ellipse->getColor()->setColor(m_brushColor);

        m_ellipse->setPosition(m_start[0], height() - m_start[1]);
		m_ellipse->setSize(m_current[0]-m_start[0], -m_current[1]+m_start[1]);
		
		// Update the affected area. We do this three times because
		// updateUndoArea only takes a position as input
		//    - ( m_current - m_start )
		//   + -------------- +      
		//   |                | 
		//   |         m_start|
		//   |       +        |
		//   |                |
		//   |                | m_current
		//   + -------------- +

		double p0[2];
		double p1[2];
		double p2[2];

		p0[0] = m_start[0];
		p0[1] = m_start[1];
		p1[0] = m_current[0];
		p1[1] = m_current[1];
		p2[0] = m_start[0] - m_current[0] + m_start[0];
		p2[1] = m_start[1] - m_current[1] + m_start[1];

		updateUndoArea(
			p1[0] - m_drawingOffsetX,
            height() - p1[1] - m_drawingOffsetY,
			0
			);
		updateUndoArea(
			p2[0] - m_drawingOffsetX,
            height() - p2[1] - m_drawingOffsetY,
			0
			);
		//updateUndoArea(
		//	-m_current[0] - m_drawingOffsetX,
        //	height() + m_current[1] - m_drawingOffsetY,
		//	0
		//	);
        this->doRedraw();
		break;
	case EM_LINE:
		resetUndoArea();
		
		// Update line primitive
		
		if (this->m_optLayerActive)
			m_line->getColor()->setColor(m_optConstraintColor);
		else
			m_line->getColor()->setColor(m_brushColor);

        m_line->setStartPos(m_start[0], height() - m_start[1]);
        m_line->setEndPos(m_current[0], height() - m_current[1]);
		
		// Update the affected area. We do this three times because
		// updateUndoArea only takes a position as input
		
		updateUndoArea(
			m_start[0] - m_drawingOffsetX,
            height() - m_start[1] - m_drawingOffsetY,
            m_line->getWidth()
			);
		updateUndoArea(
			m_current[0] - m_drawingOffsetX,
            height() - m_current[1] - m_drawingOffsetY,
            m_line->getWidth()
			);
        this->doRedraw();
		break;
	case EM_RULER:
		m_ruler->setStartPos(
			m_start[0] - m_drawingOffsetX, 
            height() - m_start[1] - m_drawingOffsetY
			);
		m_ruler->setEndPos(
			m_current[0] - m_drawingOffsetX, 
            height() - m_current[1] - m_drawingOffsetY
			);
		if (m_rulerChangedEvent!=NULL)
			m_rulerChangedEvent->onRulerChanged(m_ruler);
        this->doRedraw();
		break;
	case EM_FORCE:
		
		// Update the direction of the force created in onPush
		
        m_newForce->setDirection(m_start[0]-m_current[0], (height() - m_start[1]) - (height() - m_current[1]) );
        this->doRedraw();
		break;
	case EM_CONSTRAINT_VECTOR:
		
		// Update the constraint direction (Rigid) for the new constraint 
		// created in onPush
		
		if (m_newConstraint!=NULL)
		{
            m_newConstraint->setDirection(m_start[0]-m_current[0], (height() - m_start[1]) - (height() - m_current[1]) );
            this->doRedraw();
		}

		break;
	case EM_CONSTRAINT_HINGE:
		
		// Update the constraint direction (Rigid) for the new constraint 
		// created in onPush
		
		if (m_newConstraint!=NULL)
		{
            m_newConstraint->setDirection(m_start[0]-m_current[0], (height() - m_start[1]) - (height() - m_current[1]) );
            this->doRedraw();
		}
		break;
	case EM_CONSTRAINT:
		
		// Create additional constraints (normal ForcePAD)
		
		break;
	case EM_ERASE_CONSTRAINTS_FORCES:
		
		// Erase constraints and forces
		
        m_femGrid->erasePointLoad(x-m_drawingOffsetX, height()-y-m_drawingOffsetY,16);
        m_femGrid->erasePointConstraint(x-m_drawingOffsetX, height()-y-m_drawingOffsetY,16);
        this->doRedraw();
	case EM_SELECT_BOX:
		
		// Update the selection box to new cursor position
		
		m_selectionEnd[0] = x - m_drawingOffsetX;
        m_selectionEnd[1] = height() - y - m_drawingOffsetY;
		updateSelectionBox();
		
		// Here we need to redraw the image (Tiled rendering perhaps??)
		
        this->doRedraw();
		break;
	default:
		
		break;
	}

	if (m_zoomResults)
	{
		double dx = ( (double)m_current[0] - (double)m_start[0] );// * (2*m_zoomFactor);
		double dy = ( (double)m_current[1] - (double)m_start[1] );// * (2*m_zoomFactor);

		m_zoomPos[0] = m_zoomStart[0]-dx;
		m_zoomPos[1] = m_zoomStart[1]-dy;

        this->doFlush();
		//this->invalidate();
        this->doRedraw();
	}
}


void CPaintView::onRelease(int x, int y)
{
	//
	// Handle FLTK release event (MouseUp).
	// Read entire workspace into image m_drawing.
	//

	m_leftMouseDown = false;
	//m_zoomResults = false;
	m_selectedForce = 0;
	
	updateCursor();
	
	switch (m_editMode) {
	case EM_FLOODFILL:
		
		// Do floodfill of area under cursor
		
        m_drawing->floodFill(x-m_drawingOffsetX, (height()-y)-m_drawingOffsetY); //, 255, 0, 0);
        this->doRedraw();
		break;
	case EM_FORCE:
	case EM_CONSTRAINT:
		break;
	case EM_CONSTRAINT_VECTOR:
		m_newConstraint = NULL;
		break;
	case EM_CONSTRAINT_HINGE:
		m_newConstraint = NULL;
		break;
		
		// For the following modes the image has to be read
		// back from video memory using glReadPixels
		
	case EM_ELLIPSE:
	case EM_RECTANGLE:
	case EM_LINE:
    case EM_ARCH:
		
		updateUndo();
		m_screenImage->update(m_undoStart[0], m_undoStart[1], m_undoEnd[0], m_undoEnd[1]);
		
		break;
	default:
		break;
	}
	
	// Center of gravity(Rigid)/Center of stiffness calculations
	
	if (m_calcCG)
	{
		m_femGrid->calcCenterOfGravity(m_cg[0], m_cg[1]);
		m_cgIndicator->setPosition(m_drawingOffsetX+m_cg[0], m_drawingOffsetY+m_cg[1]);
        this->doRedraw();
	}
	else
		m_femGrid->updatePixelArea();
	
	this->updateModel();
	
	m_oldPos[0] = -1;
	m_oldPos[1] = -1;

}

void CPaintView::onClear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void CPaintView::onInitContext()
{
	so_print("CPaintView", "onInitContext()");

	if (m_checkOpenGL)
	{
		this->checkOpenGLVersion();
	}

	// Initialize OpenGL context
	
    m_drawingOffsetX = (width()-m_drawing->getWidth())/2;
    m_drawingOffsetY = (height()-m_drawing->getHeight())/2;

	if (m_drawingOffsetX<0)
		m_drawingOffsetX = 0;
	
	if (m_drawingOffsetY<0)
		m_drawingOffsetY = 0;
	
	m_clipboard->setDrawingOffset(m_drawingOffsetX, m_drawingOffsetY);
	
	this->updateSelectionBox();
	
	// Do center of mass/stiffness calculations
	
	if (m_calcCG)
	{
		m_femGrid->calcCenterOfGravity(m_cg[0], m_cg[1]);
		m_cgIndicator->setPosition(m_drawingOffsetX+m_cg[0], m_drawingOffsetY+m_cg[1]);
	}
	else
		m_femGrid->updatePixelArea();
	
	// Update model and reset stress drawing
	
	this->updateModel();
	
	m_femGrid->resetStressDrawing();
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_DITHER);
	glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	
	// Setup projection (ortho)
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    glViewport(0,0,width(),height());
    gluOrtho2D(0,width(),0,height());
	glMatrixMode(GL_MODELVIEW);
	
	
	// Define drawing area using the scissor function
	
    glScissor(m_drawingOffsetX, m_drawingOffsetY, m_drawing->getWidth(), m_drawing->getHeight());
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Implement a resize method

    if ((m_lastSize[0]!=width())||(m_lastSize[1]!=height()))
	{
        m_lastSize[0] = width();
        m_lastSize[1] = height();

        this->onResize(width(), height());
	}
}

void CPaintView::onResize(int w, int h)
{
	m_zoomPos[0] = w/2;
	m_zoomPos[1] = h/2;
}

void CPaintView::onDraw()
{
	glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_POLYGON_SMOOTH);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    glViewport(0,0,width(),height());
    gluOrtho2D(0,width(),0,height());
	glMatrixMode(GL_MODELVIEW);

	// Background properties

	int shadowWidth = 6;
	int shadowWidth2 = 3;

	float tColor[] = {0.6f, 0.6f, 0.6f};
	float bColor[] = {0.7f, 0.7f, 0.7f};
	float sColor[] = {0.3f, 0.3f, 0.3f};


	// Draw background
	
	glDisable(GL_SCISSOR_TEST);
	glBegin(GL_QUADS);
	glColor3fv(tColor);
    glVertex2i(0,height());
    glVertex2i(width(),height());
	glColor3fv(bColor);
    glVertex2i(width(),0);
	glVertex2i(0,0);
	glEnd();

	// Top frame shadow

	glBegin(GL_QUADS);
	glColor3fv(sColor);
    glVertex2i(0,height());
    glVertex2i(width(),height());
	glColor3fv(tColor);
    glVertex2i(width()-shadowWidth2,height()-shadowWidth);
    glVertex2i(shadowWidth,height()-shadowWidth);
	glEnd();

	// Left frame shadow

	glBegin(GL_QUADS);
	glColor3fv(sColor);
    glVertex2i(0,height());
	glVertex2i(0,0);
	glColor3fv(bColor);
	glVertex2i(shadowWidth,0);
	glColor3fv(tColor);
    glVertex2i(shadowWidth,height()-shadowWidth);
	glEnd();

	// Right frame shadow

	glBegin(GL_QUADS);
	glColor3fv(sColor);
    glVertex2i(width(),height());
    glVertex2i(width(),0);
	glColor3fv(bColor);
    glVertex2i(width()-shadowWidth2,0);
	glColor3fv(tColor);
    glVertex2i(width()-shadowWidth2,height()-shadowWidth);
	glEnd();

	// Bottom frame shadow

	glBegin(GL_QUADS);
	glColor3fv(sColor);
	glVertex2i(0,0);
    glVertex2i(width(),0);
	glColor3fv(bColor);
    glVertex2i(width()-shadowWidth2,shadowWidth2);
	glVertex2i(shadowWidth,shadowWidth2);
	glEnd();

	// Disable drawing outside the image.

	glEnable(GL_SCISSOR_TEST);

	if (m_zoomResults)
        glScissor(15, 15, width()-30, height()-30);
	else
        glScissor(m_drawingOffsetX, m_drawingOffsetY, m_drawing->getWidth(), m_drawing->getHeight());

	
	if (m_lockDrawing)
		return;
	
	glPushAttrib(GL_COLOR_BUFFER_BIT);
	
	if (!m_femGrid->getShowGrid())
	{
		m_screenImage->setPosition((double)m_drawingOffsetX, (double)m_drawingOffsetY);
		if (m_optLayerActive)
		{
			m_drawing->setLayer(0);
			m_screenImage->render();
			m_drawing->setLayer(1);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			m_screenImage->render();
			glDisable(GL_BLEND);
		}
		else
		{
			m_drawing->setLayer(0);
			m_screenImage->render();
		}
		
		switch (m_editMode) {
		case EM_PASTE:
			
			// Xor clipboard
			
			glEnable(GL_COLOR_LOGIC_OP);
			glPixelZoom(m_brushScale, m_brushScale);
			glLogicOp(GL_AND);
            glRasterPos2i(m_current[0]-m_clipboard->getClipboard()->getWidth()*m_brushScale/2, height()-m_current[1]-m_clipboard->getClipboard()->getHeight()*m_brushScale/2);
            glDrawPixels(m_clipboard->getClipboard()->getWidth(), m_clipboard->getClipboard()->getHeight(), GL_RGB, GL_UNSIGNED_BYTE, m_clipboard->getClipboard()->getImageMap());
			glDisable(GL_COLOR_LOGIC_OP);
            m_clipboard->render(m_current[0]-m_clipboard->getClipboard()->getWidth()*m_brushScale/2, height()-m_current[1]-m_clipboard->getClipboard()->getHeight()*m_brushScale/2);
			glPixelZoom(1.0, 1.0);
			break;
		case EM_RECTANGLE:
			if (m_leftMouseDown)
			{
				m_rectangle->render();
			}
            break;
        case EM_ARCH:
            if (m_leftMouseDown)
            {
                m_arch->render();
            }
            break;
		case EM_ELLIPSE:
			if (m_leftMouseDown)
			{
				m_ellipse->render();
			}
			break;
		case EM_LINE:
			if (m_leftMouseDown)
			{
				m_line->render();
			}
			break;
		case EM_FLOODFILL:
		case EM_FORCE:
		case EM_CONSTRAINT:
		case EM_CONSTRAINT_VECTOR:
		case EM_ERASE_CONSTRAINTS_FORCES:
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glPixelZoom(1.0, 1.0);
			glRasterPos2i(m_drawingOffsetX, m_drawingOffsetY);
            glDrawPixels(m_drawing->getWidth(), m_drawing->getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, m_drawing->getImageMap());
			glDisable(GL_BLEND);
			break;
		default:
			
			break;
		}
	}
	else
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}
	
	glPopAttrib();
	
	glPushAttrib(GL_LINE_BIT);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	if (m_leftMouseDown)
	{
		m_femGrid->resetStressDrawing();
		switch (m_editMode) {
		case EM_ELLIPSE:
		case EM_RECTANGLE:
        case EM_ARCH:
		case EM_LINE:
			break;
		default:
			if (m_zoomResults)
			{
				//m_femGrid->setPosition(m_drawingOffsetX+m_current[0]-m_drawingOffsetX, m_drawingOffsetY-m_current[1]+m_drawingOffsetY);
				//glPushMatrix();
				//glScalef(2.0f, 2.0f, 0.0f);

				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
                glViewport(0,0,width(),height());
                gluOrtho2D(m_zoomPos[0]-width()*m_zoomFactor,m_zoomPos[0]+width()*m_zoomFactor,(height()-m_zoomPos[1])-height()*m_zoomFactor,(height()-m_zoomPos[1])+height()*m_zoomFactor);
				glMatrixMode(GL_MODELVIEW);
			}
			else
			{
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
                glViewport(0,0,width(),height());
                gluOrtho2D(0,width(),0,height());
				glMatrixMode(GL_MODELVIEW);
			}

			m_femGrid->render();
			break;
		}
	}
	else
	{
		m_femGrid->setPosition(m_drawingOffsetX, m_drawingOffsetY);

		if (m_calcCG)
			m_cgIndicator->render();
		
		if (m_zoomResults)
		{
			//m_femGrid->setPosition(m_drawingOffsetX+m_current[0]-m_drawingOffsetX, m_drawingOffsetY-m_current[1]+m_drawingOffsetY);
			//glPushMatrix();
			//glScalef(2.0f, 2.0f, 0.0f);

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
            glViewport(0,0,width(),height());
            gluOrtho2D(m_zoomPos[0]-width()*m_zoomFactor,m_zoomPos[0]+width()*m_zoomFactor,(height()-m_zoomPos[1])-height()*m_zoomFactor,(height()-m_zoomPos[1])+height()*m_zoomFactor);
			glMatrixMode(GL_MODELVIEW);
		}
		else
		{
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
            glViewport(0,0,width(),height());
            gluOrtho2D(0,width(),0,height());
			glMatrixMode(GL_MODELVIEW);
		}

		m_femGrid->render();
	}
	if (m_editMode==EM_SELECT_BOX)
	{
		m_selectionBox->render();
	}

	if (m_editMode==EM_RULER)
	{
		glPushMatrix();
		glTranslated((double)m_drawingOffsetX, (double)m_drawingOffsetY, 0.0);
		m_ruler->render();
		glPopMatrix();
	}

	glDisable(GL_BLEND);
	glPopAttrib();
}

void CPaintView::onMove(int x, int y)
{
	//
	// Handle passive mouse movement
	//
	
	// Store current position
	
	m_current[0] = x;
	m_current[1] = y;

	updateCursor();
	
	switch (m_editMode) {
	case EM_DIRECT_BRUSH:
        this->doRedraw();
		break;
	case EM_PASTE:
        this->doRedraw();
		break;
	default:
		
		break;
	}

	if (m_zoomResults&&m_femGrid->getShowGrid())
        this->doRedraw();
}

/////////////////////////////////////////////////////////////
// CPaintView private methods
/////////////////////////////////////////////////////////////

void CPaintView::loadBrushes()
{
	so_print("CPaintView","loadBrushes()");
	
	CSgiImagePtr brush;
	CSgiImagePtr invertedBrush;
	
	// Test file locations
	
	string brushPath = "";
	string brushName = "";

#ifdef __APPLE__
	string exePath = m_argv[0];
	int lastSlash = exePath.rfind("/");
	brushPath = exePath.substr(0,lastSlash)+"/brushes/";
#endif

#ifdef WIN32
	string exePath = getApplicationPath();
	string fullPath = exePath + "\\brushes";
	brushPath = fullPath + "\\";
#else
    FILE* f = fopen("/usr/share/forcepad/brushes/rbrush4.rgb", "rb");
	if (!f)
	{
		f = fopen("/usr/local/share/forcepad/brushes/rbrush4.rgb", "rb");
		if (!f)
		{
			f = fopen("brushes/rbrush4.rgb", "rb");
			
			if (f)
			{
				brushPath = "brushes/";
				fclose(f);
			}
		}
		else
		{
			fclose(f);
			brushPath = "/usr/local/share/forcepad/brushes/";
		}
		
	}
	else
	{
		fclose(f);
		brushPath = "/usr/share/forcepad/brushes/";
	}
#endif
	
	brushName = brushPath+"rbrush4.rgb";	
	
	brush = new CSgiImage();
	brush->setFileName(brushName.c_str());
	brush->setAlphaChannel(true);
	brush->read();
	brush->createAlphaMask(0,m_blendFactor);
	m_brushes.push_back(CSgiImagePtr(brush));
	
	invertedBrush = new CSgiImage();
	invertedBrush->setFileName(brushName.c_str());
	invertedBrush->read();
	invertedBrush->invert();
	m_invertedBrushes.push_back(CSgiImagePtr(invertedBrush));
	
	brushName = brushPath+"rbrush8.rgb";	
	
	brush = new CSgiImage();
	brush->setFileName(brushName.c_str());
	brush->setAlphaChannel(true);
	brush->read();
	brush->createAlphaMask(0,m_blendFactor);
	m_brushes.push_back(CSgiImagePtr(brush));
	
	invertedBrush = new CSgiImage();
	invertedBrush->setFileName(brushName.c_str());
	invertedBrush->read();
	invertedBrush->invert();
	m_invertedBrushes.push_back(CSgiImagePtr(invertedBrush));
	
	brushName = brushPath+"rbrush16.rgb";	
	
	brush = new CSgiImage();
	brush->setFileName(brushName.c_str());
	brush->setAlphaChannel(true);
	brush->read();
	brush->createAlphaMask(0,m_blendFactor);
	m_brushes.push_back(CSgiImagePtr(brush));
	
	invertedBrush = new CSgiImage();
	invertedBrush->setFileName(brushName.c_str());
	invertedBrush->read();
	invertedBrush->invert();
	m_invertedBrushes.push_back(CSgiImagePtr(invertedBrush));
	
	brushName = brushPath+"rbrush32.rgb";
	
	brush = new CSgiImage();
	brush->setFileName(brushName.c_str());
	brush->setAlphaChannel(true);
	brush->read();
	brush->createAlphaMask(0,m_blendFactor);
	m_brushes.push_back(CSgiImagePtr(brush));
	
	invertedBrush = new CSgiImage();
	invertedBrush->setFileName(brushName.c_str());
	invertedBrush->read();
	invertedBrush->invert();
	m_invertedBrushes.push_back(CSgiImagePtr(invertedBrush));
	
	brushName = brushPath+"rbrush64.rgb";	
	
	brush = new CSgiImage();
	brush->setFileName(brushName.c_str());
	brush->setAlphaChannel(true);
	brush->read();
	brush->createAlphaMask(0,m_blendFactor);
	m_brushes.push_back(CSgiImagePtr(brush));
	
	invertedBrush = new CSgiImage();
	invertedBrush->setFileName(brushName.c_str());
	invertedBrush->read();
	invertedBrush->invert();
	m_invertedBrushes.push_back(CSgiImagePtr(invertedBrush));
	
	brushName = brushPath+"sbrush4.rgb";	
	
	brush = new CSgiImage();
	brush->setFileName(brushName.c_str());
	brush->setAlphaChannel(true);
	brush->read();
	brush->createAlphaMask(0,m_blendFactor);
	m_brushes.push_back(CSgiImagePtr(brush));
	
	invertedBrush = new CSgiImage();
	invertedBrush->setFileName(brushName.c_str());
	invertedBrush->read();
	invertedBrush->invert();
	m_invertedBrushes.push_back(CSgiImagePtr(invertedBrush));
	
	brushName = brushPath+"sbrush8.rgb";	
	
	brush = new CSgiImage();
	brush->setFileName(brushName.c_str());
	brush->setAlphaChannel(true);
	brush->read();
	brush->createAlphaMask(0,m_blendFactor);
	m_brushes.push_back(CSgiImagePtr(brush));
	
	invertedBrush = new CSgiImage();
	invertedBrush->setFileName(brushName.c_str());
	invertedBrush->read();
	invertedBrush->invert();
	m_invertedBrushes.push_back(CSgiImagePtr(invertedBrush));
	
	brushName = brushPath+"sbrush16.rgb";	
	
	brush = new CSgiImage();
	brush->setFileName(brushName.c_str());
	brush->setAlphaChannel(true);
	brush->read();
	brush->createAlphaMask(0,m_blendFactor);
	m_brushes.push_back(CSgiImagePtr(brush));
	
	invertedBrush = new CSgiImage();
	invertedBrush->setFileName(brushName.c_str());
	invertedBrush->read();
	invertedBrush->invert();
	m_invertedBrushes.push_back(CSgiImagePtr(invertedBrush));
	
	brushName = brushPath+"sbrush32.rgb";	
	
	brush = new CSgiImage();
	brush->setFileName(brushName.c_str());
	brush->setAlphaChannel(true);
	brush->read();
	brush->createAlphaMask(0,m_blendFactor);
	m_brushes.push_back(CSgiImagePtr(brush));
	
	invertedBrush = new CSgiImage();
	invertedBrush->setFileName(brushName.c_str());
	invertedBrush->read();
	invertedBrush->invert();
	m_invertedBrushes.push_back(CSgiImagePtr(invertedBrush));
	
	brushName = brushPath+"sbrush64.rgb";	
	
	brush = new CSgiImage();
	brush->setFileName(brushName.c_str());
	brush->setAlphaChannel(true);
	brush->read();
	brush->createAlphaMask(0,m_blendFactor);
	m_brushes.push_back(CSgiImagePtr(brush));
	
	invertedBrush = new CSgiImage();
	invertedBrush->setFileName(brushName.c_str());
	invertedBrush->read();
	invertedBrush->invert();
	m_invertedBrushes.push_back(CSgiImagePtr(invertedBrush));
	
	m_currentBrush = m_brushes[0];
	m_currentInvertedBrush = m_invertedBrushes[0];
}

void CPaintView::deleteBrushes()
{
	so_print("CPaintView","deleteBrushes()");
	
	m_brushes.clear();
	m_invertedBrushes.clear();
	
	m_currentBrush = NULL;
	m_currentInvertedBrush = NULL;
}

void CPaintView::clearMesh()
{
    so_print("CPaintView","clearMesh()");
	m_femGrid->setShowGrid(false);
    this->doRedraw();
}

void CPaintView::clearResults()
{
	so_print("CPaintView", "clearResults()");
	m_femGrid->clearResults();
    this->doRedraw();
}

void CPaintView::updateSelectionBox()
{
	so_print("CPaintView", "updateSelectionBox()");
	m_selectionBox->setPosition(m_selectionStart[0]+m_drawingOffsetX, m_selectionStart[1]+m_drawingOffsetY);
	m_selectionBox->setSize(m_selectionEnd[0]-m_selectionStart[0], m_selectionEnd[1]-m_selectionStart[1]);
}

void CPaintView::resetUndoArea()
{
	so_print("CPaintView", "resetUndoArea()");
	m_undoStart[0] = 65000;
	m_undoStart[1] = 65000;
	m_undoEnd[0] = -65000;
	m_undoEnd[1] = -65000;
}

void CPaintView::updateUndo()
{
	so_print("CPaintView", "updateUndo()");
	int x1, y1, x2, y2;
	
	if (m_undoStart[0]==65000)
		return;
	
	if (m_undoStart[0]>m_undoEnd[0])
	{
		x1 = m_undoEnd[0];
		x2 = m_undoStart[0];
	}
	else
	{
		x1 = m_undoStart[0];
		x2 = m_undoEnd[0];
	}
	
	if (m_undoStart[1]>m_undoEnd[1])
	{
		y1 = m_undoEnd[1];
		y2 = m_undoStart[1];
	}
	else
	{
		y1 = m_undoStart[1];
		y2 = m_undoEnd[1];
	}
	
	m_undoClipboard->copy(x1, y1, x2, y2);
}

void CPaintView::disableDrawing()
{
	m_lockDrawing = true;
}

void CPaintView::enableDrawing()
{
	m_lockDrawing = false;
}

void CPaintView::updateModel()
{
	so_print("CPaintView","updateModel()");
	if (m_mainFrame!=NULL)
	{
		((CMainFrame*)m_mainFrame)->setPixelWeight(m_femGrid->getPixelArea()*1e-3);
		((CMainFrame*)m_mainFrame)->setExternalForce(m_femGrid->getPixelArea()*m_relativeForceSize*1e-3);
        this->doRedraw();
	}
}

void CPaintView::updateCursor()
{
    doUpdateCursor(m_editMode);
}

void CPaintView::createCursors()
{
    doCreateCursors();
}

void CPaintView::deleteCursors()
{
    doDeleteCursors();
}

void CPaintView::checkOpenGLVersion()
{
	m_checkOpenGL = false;
	so_print("checkOpenGLVersion", (const char *)glGetString(GL_VENDOR));
	so_print("checkOpenGLVersion", (const char *)glGetString(GL_RENDERER));
	so_print("checkOpenGLVersion", (const char *)glGetString(GL_VERSION));
}

/////////////////////////////////////////////////////////////
// CPaintView public methods
/////////////////////////////////////////////////////////////

void CPaintView::zoomIn()
{
	m_zoomFactor -= 0.01;
	if (m_zoomFactor < 0.01)
		m_zoomFactor = 0.01;

    this->doFlush();
    this->doRedraw();
}

void CPaintView::zoomOut()
{
	m_zoomFactor += 0.01;
    this->doFlush();
    this->doRedraw();
}


bool CPaintView::execute()
{
	bool errors = true;
	
	so_print("CPaintView","execute()");
	
	//
	// Initialize grid
	//
	
	m_femGrid->initGrid();
	
	//
	// Check for long computational times >10000 dofs. (today...)
	//
	
	if (m_femGrid->enumerateDofs(ED_BOTTOM_TOP)>10000)
        if (!doAskYesNo("Model contains >10000 degrees of freedom.\nCalculation can take a long time.\nContinue?"))
			return false;
		
	//
	// Initiate solver
	//
	
	so_print("CPaintView","\tInitiating solver.");
	
	m_solver = new CFemGridSolver2();
	m_solver->setStatusMessageEvent(m_statusMessageEvent);
	m_solver->setLogMessageEvent(m_logMessageEvent);
	m_solver->setUseWeight(m_useWeight);
	m_solver->setConstraintStiffnessScale(m_constraintStiffnessScale);
	m_solver->setForceMagnitude(m_forceMagnitude);

	m_solver->setThickness(this->getThickness());
	m_solver->setElasticModulus(this->getElasticModulus());
	m_solver->setYoungsModulus(this->getYoungsModulus());

	if (m_useWeight)
		m_solver->setWeight(m_weight);

	m_solver->setFemGrid(m_femGrid);
	
	//
	// Execute calculation
	//
	
	so_print("CPaintView","\tExecuting solver.");
	
	m_solver->execute();
	
	//
	// Check for errors
	//
	
	so_print("CPaintView","\tChecking for errors.");
	
	switch (m_solver->getLastError()) {
	case CFemGridSolver2::ET_NO_ERROR:
		errors = false;
		break;
	case CFemGridSolver2::ET_NO_ELEMENTS:
        doInfoMessage("No structure to solve.");
		break;
	case CFemGridSolver2::ET_NO_BCS:
        doInfoMessage("Add locks to structure.");
		break;
	case CFemGridSolver2::ET_NO_LOADS:
        doInfoMessage("No loads defined on structure.");
		break;
	case CFemGridSolver2::ET_UNSTABLE:
        doInfoMessage("Structure unstable. Try adding locks.");
		break;
	case CFemGridSolver2::ET_INVALID_MODEL:
        doInfoMessage("Model invalid.");
		break;
	case CFemGridSolver2::ET_LOAD_OUTSIDE_AE:
        doInfoMessage("Loads defined outside structure.");
		break;
	case CFemGridSolver2::ET_BC_OUTSIDE_AE:
        doInfoMessage("Locks defined outside structure.");
		break;
	default:
		
		break;
	}
	
	//
	// Clean up and redraw
	//
	
	so_print("CPaintView","\tDestroying solver.");
	
	// delete solver;
	
	if (errors)
	{
		m_femGrid->setShowGrid(false);
        this->doInvalidate();
	}
	else
		m_femGrid->setShowGrid(true);
	
	so_print("CPaintView","\tRedraw.");
	
    this->doRedraw();

	return !errors;
}

bool CPaintView::executeOpt()
{
	bool errors = true;
	
	so_print("CPaintView","execute()");
	
	//
	// Initialize grid
	//
	
	m_femGrid->initGrid();
	
	//
	// Check for long computational times >10000 dofs. (today...)
	//
	
	if (m_femGrid->enumerateDofs(ED_BOTTOM_TOP)>10000)
        if (!doAskYesNo("Model contains >10000 degrees of freedom.\nCalculation can take a long time.\nContinue?"))
			return false;
		
	//
	// Initiate solver
	//
	
	so_print("CPaintView","\tInitiating solver.");
	
	m_solver = new CFemGridSolver2();
	m_solver->setStatusMessageEvent(m_statusMessageEvent);
	m_solver->setLogMessageEvent(m_logMessageEvent);
	m_solver->setContinueCalcEvent(m_continueCalcEvent);
	m_solver->setUseWeight(m_useWeight);
	m_solver->setConstraintStiffnessScale(m_constraintStiffnessScale);
	m_solver->setForceMagnitude(m_forceMagnitude);

	m_solver->setOptVolumeFraction(m_optVolfrac);
	m_solver->setOptRmin(m_optRmin);
	m_solver->setOptMinChange(m_optMinChange);
	m_solver->setOptMaxLoops(m_optMaxLoops);
	m_solver->setOptFilterType(m_optFilterType);

	m_solver->setThickness(this->getThickness());
	m_solver->setElasticModulus(this->getElasticModulus());
	m_solver->setYoungsModulus(this->getYoungsModulus());

	if (m_useWeight)
		m_solver->setWeight(m_weight);

	m_solver->setFemGrid(m_femGrid);
	m_femGrid->setShowDensity(true);
	
	//
	// Execute calculation
	//
	
	so_print("CPaintView","\tExecuting solver.");
	
	m_solver->executeOptimizer();
	
	//
	// Check for errors
	//
	
	so_print("CPaintView","\tChecking for errors.");
	
	switch (m_solver->getLastError()) {
	case CFemGridSolver2::ET_NO_ERROR:
		errors = false;
		break;
	case CFemGridSolver2::ET_NO_ELEMENTS:
        doInfoMessage("No structure to solve.");
		break;
	case CFemGridSolver2::ET_NO_BCS:
        doInfoMessage("Add locks to structure.");
		break;
	case CFemGridSolver2::ET_NO_LOADS:
        doInfoMessage("No loads defined on structure.");
		break;
	case CFemGridSolver2::ET_UNSTABLE:
        doInfoMessage("Structure unstable. Try adding locks.");
		break;
	case CFemGridSolver2::ET_INVALID_MODEL:
        doInfoMessage("Model invalid.");
		break;
	case CFemGridSolver2::ET_LOAD_OUTSIDE_AE:
        doInfoMessage("Loads defined outside structure.");
		break;
	case CFemGridSolver2::ET_BC_OUTSIDE_AE:
        doInfoMessage("Locks defined outside structure.");
		break;
	default:
		
		break;
	}
	
	//
	// Clean up and redraw
	//
	
	so_print("CPaintView","\tDestroying solver.");
	
	// delete solver;
	
	if (errors)
	{
		m_femGrid->setShowGrid(false);
		m_femGrid->setShowDensity(false);
        this->doInvalidate();
	}
	else
		m_femGrid->setShowDensity(false);
	
	so_print("CPaintView","\tRedraw.");
	
    this->doRedraw();

	return !errors;
}

void CPaintView::newModel()
{
	disableDrawing();

	so_print("CPaintView","newModel()");
	
    int width = 640;
    int height = 480;
    int initialStiffness = 0;
	
    if (this->doNewModel(width, height, initialStiffness))
	{		
		this->setModelName("noname.fp2");
		
		// Create drawing area

		int i;

		for (i=width; i>0; i--)
		{
			if (i % 8 == 0)
			{
				width = i;
				break;
			}
		}

		for (i=height; i>0; i--)
		{
			if (i % 8 == 0)
			{
				height = i;
				break;
			}
		}

		m_drawing = new CImage(2);
		m_drawing->setChannels(4);
		m_drawing->setSize(width, height);
		m_drawing->fillColor(255-initialStiffness,255-initialStiffness,255-initialStiffness);

		m_drawing->setLayer(1);
		m_drawing->fillColor(255,255,255);
		m_drawing->fillAlpha(128);
		m_drawing->setAlpha(128);
		m_drawing->setLayer(0);

		// Create image grid
		
		m_femGrid->setImage(m_drawing);
		m_femGrid->setShowGrid(false);
		m_clipboard->setImage(m_drawing);
		m_undoClipboard->setImage(m_drawing);
		m_screenImage->setImage(m_drawing);
		
		m_showMesh = false;

		m_useWeight = false;
		m_femGrid->setUseWeight(m_useWeight);
		
        this->doInvalidate();
        this->doRedraw();

		if (m_newModelEvent!=NULL)
			m_newModelEvent->onNewModel();
	}

	this->setViewMode(CPaintView::VM_SKETCH);	
	enableDrawing();
}

void CPaintView::executeCorba()
{
}

void CPaintView::openImage()
{
	disableDrawing();
	
	so_print("CPaintView","openImage()");
	
    std::string fname = this->doOpenDialog("Open image file", "*.*");

    m_danglingRelease = true;
	
    if (fname!="")
	{
		bool jpegFile = false;
		bool pngFile = false;
		bool rgbFile = false;

        int lastIndex = fname.find_last_of(".");
        std::string extension = fname.substr(lastIndex+1);

        if (extension == ".jpg")
			jpegFile = true;
		
        if (extension == ".jpeg")
			jpegFile = true;
		
        if (extension == ".png")
			pngFile = true;
		
        if (extension == ".rgb")
			rgbFile = true;
		
		if ((!jpegFile)&&(!pngFile)&&(!rgbFile))
			return;
		
		this->setModelName("noname.fp2");
		
		CJpegImagePtr jpegImage;
		CPngImagePtr pngImage;
		CSgiImagePtr rgbImage;
		CImagePtr image;
		
		if (jpegFile)
		{
			jpegImage = new CJpegImage();
            jpegImage->setFileName(fname.c_str());
			jpegImage->read();
			image = jpegImage;
		}
		
		if (pngFile)
		{
			pngImage = new CPngImage();
            pngImage->setFileName(fname.c_str());
			pngImage->read();
			image = pngImage;
		}
		
		if (rgbFile)
		{
			rgbImage = new CSgiImage();
            rgbImage->setFileName(fname.c_str());
			rgbImage->read();
			image = rgbImage;
		}
		
		image->grayscale();
		
		if (m_importMode==IM_NEW_MODEL)
		{
			m_drawing = image;
			
			// Create image grid
			
			m_femGrid->setImage(m_drawing);
			m_femGrid->setShowGrid(false);
			m_clipboard->setImage(m_drawing);
			m_undoClipboard->setImage(m_drawing);
			m_screenImage->setImage(m_drawing);
			
			m_showMesh = false;
		}
		else
		{
            m_clipboard->copyImage(image->getWidth(), image->getHeight(), image->getImageMap());
			setEditMode(EM_PASTE);
		}
		
        this->doInvalidate();
        this->doRedraw();
	}
	
	enableDrawing();
}

void CPaintView::saveModelAs()
{
	so_print("CPaintView", "saveModelAs()");
	
	//
	// Ask for file name
	//

    std::string fname = this->doSaveDialog("Save forcepad model", "*.fp2", m_modelName);

    if (fname!="")
        this->setModelName(fname);
    else
        return;
	
	//
	// Save file
	//
	
	using namespace std;

	m_femGrid->setUseWeight(m_useWeight);
	
	fstream f;
	f.open(m_modelName.c_str(), ios::out);
	m_femGrid->saveToStream(f);
	f.close();
}

void CPaintView::saveModel()
{
	so_print("CPaintView", "saveModel()");
	
	//
	// Ask for file name
	//

	if (m_modelName=="noname.fp2")
	{
        std::string fname = this->doSaveDialog("Save forcepad model", "*.fp2", m_modelName);
		
        if (fname!="")
		{
			setModelName(fname);
		}
		else
			return;
	}
	
	//
	// Save file
	//
	
	using namespace std;

	m_femGrid->setUseWeight(m_useWeight);
	
	fstream f;
	f.open(m_modelName.c_str(), ios::out);
	m_femGrid->saveToStream(f);
	f.close();
}

void CPaintView::expandImage()
{
    disableDrawing();

    int i, width, height, maxWidth, maxHeight;
    double dWidth, dHeight, dAspect;

    width = m_drawing->getWidth();
    height = m_drawing->getHeight();

    maxWidth = this->width()-50;
    maxHeight = this->height()-50;

    dAspect = (double)width/(double)height;
    dWidth = (double)width  * 1.20;
    dHeight = dWidth/dAspect;

    width = (int)dWidth;
    height = (int)dHeight;

    m_drawing = new CImage(2);
    m_drawing->setChannels(4);

    for (i=width; i>0; i--)
    {
        if (i % 8 == 0)
        {
            width = i;
            break;
        }
    }

    for (i=height; i>0; i--)
    {
        if (i % 8 == 0)
        {
            height = i;
            break;
        }
    }

    if (width>maxWidth)
        width = maxWidth;
    if (height>maxHeight)
        height = maxHeight;

    m_drawing->setSize(width, height);
    m_drawing->fillColor(255, 255 ,255);

    m_drawing->setLayer(1);
    m_drawing->fillColor(255,255,255);
    m_drawing->fillAlpha(128);
    m_drawing->setAlpha(128);
    m_drawing->setLayer(0);

    // Copy previous image to new image.

    m_clipboard->copy(0,0, m_femGrid->getImage()->getWidth(), m_femGrid->getImage()->getHeight());
    width = m_femGrid->getImage()->getWidth();
    height = m_femGrid->getImage()->getHeight();

    // Create image grid

    m_femGrid->setImage(m_drawing);
    m_femGrid->setShowGrid(false);
    m_clipboard->setImage(m_drawing);
    m_undoClipboard->setImage(m_drawing);
    m_screenImage->setImage(m_drawing);

    m_clipboard->paste(m_drawing->getWidth()/2-width/2, m_drawing->getHeight()/2-height/2);

    this->doInvalidate();
    this->doRedraw();

    enableDrawing();
}

void CPaintView::expandImageToWindow()
{
	disableDrawing();

	m_drawing = new CImage(2);
	m_drawing->setChannels(4);

	int i, width, height;

    width = this->width()-50;
    height = this->height()-50;

	for (i=width; i>0; i--)
	{
		if (i % 8 == 0)
		{
			width = i;
			break;
		}
	}

	for (i=height; i>0; i--)
	{
		if (i % 8 == 0)
		{
			height = i;
			break;
		}
	}

	m_drawing->setSize(width, height);
	m_drawing->fillColor(255, 255 ,255);

	m_drawing->setLayer(1);
	m_drawing->fillColor(255,255,255);
	m_drawing->fillAlpha(128);
	m_drawing->setAlpha(128);
	m_drawing->setLayer(0);

	// Copy previous image to new image.

    m_clipboard->copy(0,0, m_femGrid->getImage()->getWidth(), m_femGrid->getImage()->getHeight());
	
	// Create image grid
	
	m_femGrid->setImage(m_drawing);
	m_femGrid->setShowGrid(false);
	m_clipboard->setImage(m_drawing);
	m_undoClipboard->setImage(m_drawing);
	m_screenImage->setImage(m_drawing);

	m_clipboard->paste(0,0);

    this->doInvalidate();
    this->doRedraw();

	enableDrawing();
}

void CPaintView::openModel(const std::string filename)
{
	using namespace std;
	
	disableDrawing();
	
	so_print("CPaintView", "openModel()");
	so_print("CPatinView", filename.c_str());
	
	m_danglingRelease = true;
	
	setModelName(filename);
		
	m_drawing = new CImage(2);
	m_drawing->setSize(640,480);
	m_drawing->setChannels(4);
	m_femGrid->setImage(m_drawing);

	fstream f;
	f.open(m_modelName.c_str(), ios::in);
	m_femGrid->readFromStream(f);
	f.close();

	m_useWeight = m_femGrid->getUseWeight();

	m_drawing->setLayer(1);
	m_drawing->fillColor(255,255,255);
	m_drawing->fillAlpha(128);
	m_drawing->setAlpha(128);
	m_drawing->setLayer(0);
		
	m_clipboard->setImage(m_drawing);
	m_undoClipboard->setImage(m_drawing);
	m_screenImage->setImage(m_drawing);

	m_femGrid->setShowGrid(false);
	m_showMesh = false;
		
    this->doInvalidate();
    this->doRedraw();

	enableDrawing();

	this->setViewMode(CPaintView::VM_SKETCH);	

	if (m_modelLoadedEvent!=NULL)
		m_modelLoadedEvent->onModelLoaded();
}

void CPaintView::openModel()
{
	using namespace std;
	
	disableDrawing();
	
	so_print("CPaintView", "openModel()");
	
    std::string fname = doOpenDialog("Open forcepad model", "*.fp2");
	m_danglingRelease = true;
	
    if (fname!="")
	{
		setModelName(fname);
		
		m_drawing = new CImage(2);
		m_drawing->setSize(640,480);
		m_drawing->setChannels(4);
		m_femGrid->setImage(m_drawing);

		fstream f;
		f.open(m_modelName.c_str(), ios::in);
		m_femGrid->readFromStream(f);
		f.close();

		m_useWeight = m_femGrid->getUseWeight();

		m_drawing->setLayer(1);
		m_drawing->fillColor(255,255,255);
		m_drawing->fillAlpha(128);
		m_drawing->setAlpha(128);
		m_drawing->setLayer(0);
		
		m_clipboard->setImage(m_drawing);
		m_undoClipboard->setImage(m_drawing);
		m_screenImage->setImage(m_drawing);

		m_femGrid->setShowGrid(false);
		m_showMesh = false;
		
        this->doInvalidate();
        this->doRedraw();

		if (m_modelLoadedEvent!=NULL)
			m_modelLoadedEvent->onModelLoaded();

		this->setViewMode(CPaintView::VM_SKETCH);	

	}
	enableDrawing();
}

void CPaintView::copy()
{
	so_print("CPaintView", "copy()");
	int x1, y1, x2, y2;
	
	if (m_selectionStart[0]>m_selectionEnd[0])
	{
		x1 = m_selectionEnd[0];
		x2 = m_selectionStart[0];
	}
	else
	{
		x1 = m_selectionStart[0];
		x2 = m_selectionEnd[0];
	}
	
	if (m_selectionStart[1]>m_selectionEnd[1])
	{
		y1 = m_selectionEnd[1];
		y2 = m_selectionStart[1];
	}
	else
	{
		y1 = m_selectionStart[1];
		y2 = m_selectionEnd[1];
	}
	
	m_clipboard->copy(x1, y1, x2, y2);
	copyToWindows();
	
    this->doRedraw();
}

void CPaintView::cut()
{
	so_print("CPaintView", "cut()");
	int x1, y1, x2, y2;
	
	if (m_selectionStart[0]>m_selectionEnd[0])
	{
		x1 = m_selectionEnd[0];
		x2 = m_selectionStart[0];
	}
	else
	{
		x1 = m_selectionStart[0];
		x2 = m_selectionEnd[0];
	}
	
	if (m_selectionStart[1]>m_selectionEnd[1])
	{
		y1 = m_selectionEnd[1];
		y2 = m_selectionStart[1];
	}
	else
	{
		y1 = m_selectionStart[1];
		y2 = m_selectionEnd[1];
	}
	
	m_clipboard->cut(x1, y1, x2, y2);
	
    this->doRedraw();
}

void CPaintView::undo()
{
	so_print("CPaintView", "undo()");
	
	int x1, y1;
	
	if (m_undoStart[0]==65000)
		return;
	
	if (m_undoStart[0]>m_undoEnd[0])
		x1 = m_undoEnd[0];
	else
		x1 = m_undoStart[0];
	
	if (m_undoStart[1]>m_undoEnd[1])
		y1 = m_undoEnd[1];
	else
		y1 = m_undoStart[1];
	
	m_undoClipboard->paste(x1, y1);
	
    this->doRedraw();
}

void CPaintView::updateUndoArea(int x, int y, int brushSize)
{
	so_print("CPaintView", "updateUndoArea()");

	int xx, yy;

	xx = x;
	yy = y;
	
	if (xx<0)
		xx = 0;
	if (yy<0)
		yy = 0;

	if ((xx-brushSize/2)<m_undoStart[0])
		m_undoStart[0] = xx-brushSize/2;
	if ((yy-brushSize/2)<m_undoStart[1])
		m_undoStart[1] = yy-brushSize/2;
	if ((xx+brushSize/2)>m_undoEnd[0])
		m_undoEnd[0] = xx+brushSize/2;
	if ((yy+brushSize/2)>m_undoEnd[1])
		m_undoEnd[1] = yy+brushSize/2;
}

void CPaintView::undoToDrawing()
{
	updateUndoArea(0,0,0);
    updateUndoArea(m_drawing->getWidth(), m_drawing->getHeight(),0);
}

void CPaintView::transferViewToImage()
{
    int w = m_drawing->getWidth();
	int h = m_drawing->getHeight();
	int storageWidth = w*3;
	GLubyte *pPixelData = new GLubyte[storageWidth*h];
	
	// Make OpenGL context current
	
    this->doMakeCurrent();
	
	// Read pixels from screen
	
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	
	glReadPixels(
		m_drawingOffsetX,
		m_drawingOffsetY,
		w,
		h,
		GL_RGB,GL_UNSIGNED_BYTE,
		pPixelData
		);

	// Write pixels back into drawing.

	int i, j;

	for (i=0; i<h; i++)
		for (j=0; j<w; j++)
			m_drawing->setPixel(j, i, pPixelData[storageWidth*i + j*3], pPixelData[storageWidth*i + j*3 + 1], pPixelData[storageWidth*i + j*3 + 2]);

	// Clean up

	delete [] pPixelData;
}

void CPaintView::copyToWindows()
{
	so_print("CPaintView", "copyToWindows()");
#ifdef WIN32
	
	// Extract copy area from selection 
	
	int x1, y1, x2, y2;
	
	if (m_selectionStart[0]>m_selectionEnd[0])
	{
		x1 = m_selectionEnd[0];
		x2 = m_selectionStart[0];
	}
	else
	{
		x1 = m_selectionStart[0];
		x2 = m_selectionEnd[0];
	}
	
	if (m_selectionStart[1]>m_selectionEnd[1])
	{
		y1 = m_selectionEnd[1];
		y2 = m_selectionStart[1];
	}
	else
	{
		y1 = m_selectionStart[1];
		y2 = m_selectionEnd[1];
	}
	
	// Reduce selection to prevent reading the
	// selection box
	
	x1++;
	y1++; 
	x2--;
	y2--;
	
	//
	// Code based on original code by Pierre Alliez.
	// http://www.codeguru.com/opengl/snap.shtml
	//
	
	int w, h;
	
	w = x2 - x1;
	h = y2 - y1;
	
	// Lines must be word aligned (Cropping area)
	
	w -= w % 4;
	
	BITMAPINFOHEADER* header; 
	int nbBytes = 3 * w * h;
	int i, j;
	
	
	//GLubyte *pPixelData = new GLubyte[nbBytes];
	//int storageWidth = w*3 - (w*3)%sizeof(DWORD) + sizeof(DWORD);
	int storageWidth = w*3;
	GLubyte *pPixelDataPadded = new GLubyte[storageWidth*h];
	GLubyte red, green, blue;

	
	// Make OpenGL context current
	
    this->doMakeCurrent();
    //make_current();
	
	// Read pixels from screen
	
	/*
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	
	glReadPixels(
		x1+m_drawingOffsetX,
		y1+m_drawingOffsetY,
		w,
		h,
		GL_RGBA,GL_UNSIGNED_BYTE,
		pPixelData
		);
	*/

	// Create and fill header 
	
	header = new BITMAPINFOHEADER; 
	header->biWidth = w; 
	header->biHeight = h;
	header->biSizeImage = storageWidth*h; 
	header->biSize = sizeof(BITMAPINFOHEADER); 
	header->biPlanes = 1; 
	header->biBitCount =  24; // RGB 
	header->biCompression = 0; 
	header->biXPelsPerMeter = 0; 
	header->biYPelsPerMeter = 0; 
	header->biClrUsed = 0; 
	header->biClrImportant = 0; 
	
	// Pad and convert pixeldata to DIB format
	
	for (i=0; i<w; i++)
		for (j=0; j<h; j++)
		{
			m_clipboard->getClipboardImage()->getPixel(i,j,red,green,blue);

			pPixelDataPadded[j*storageWidth+i*3] = blue;
			pPixelDataPadded[j*storageWidth+i*3+1] = green;
			pPixelDataPadded[j*storageWidth+i*3+2] = red;
		}
		
		// Generate handle 
		
		HANDLE handle = (HANDLE)::GlobalAlloc (GHND,sizeof(BITMAPINFOHEADER) + storageWidth*h); 
		
		if(handle != NULL) 
		{ 
			// Lock handle 
			
			char *pData = (char *) ::GlobalLock((HGLOBAL)handle); 
			
			// Copy header and data 
			
			memcpy(pData,header,sizeof(BITMAPINFOHEADER)); 
			memcpy(pData+sizeof(BITMAPINFOHEADER),pPixelDataPadded,storageWidth*h); 
			
			// Unlock 
			
			::GlobalUnlock((HGLOBAL)handle); 
			
			// Push DIB in clipboard 
			
			OpenClipboard(NULL); 
			EmptyClipboard(); 
			SetClipboardData(CF_DIB,handle); 
			CloseClipboard(); 
		} 
		
		// Clean up
		
		delete header;
		//delete [] pPixelData;
		delete [] pPixelDataPadded;
#endif
}

void CPaintView::pasteFromWindows()
{
	so_print("CPaintView", "pasteFromWindows()");
#ifdef WIN32
	
	HGLOBAL   hglb; 
	
	// Check if valid formats
	
	if (!IsClipboardFormatAvailable(CF_DIB)) 
	{
        doInfoMessage("Clipboard format not supported.");
		return; 
	}
	
	// Try to open clipboard
	
	if (!OpenClipboard(NULL)) 
	{
        doInfoMessage("Could not open clipboard.");
		return; 
	}
	
	// Get clipboard data
	
	hglb = GetClipboardData(CF_DIB); 
	
	if (hglb != NULL) 
	{ 
		// Lock handle 
		
		char *pData = (char *) GlobalLock(hglb); 
		
		if (pData==NULL)
            doInfoMessage("This should not normally happen. (GlobalLock failed).");
		
		// Create header
		
		BITMAPINFOHEADER* header; 
		header = new BITMAPINFOHEADER; 
		
		// Copy header 
		
		memcpy(header,pData,sizeof(BITMAPINFOHEADER)); 
		
		// Check if we can handle the bitmap
		
		if (header->biBitCount!=24)
            doInfoMessage("Image must be 24 bit color.");
		
		if (header->biCompression!=0)
            doInfoMessage("Compression not supported.");
		
		GLubyte* pPixelDataPadded;
		GLubyte* pPixelData;
		
		if ((header->biBitCount==24)&&(header->biCompression==0)) 
		{
			// Create memory for image
			
			int nbBytes = 3 * header->biWidth * header->biHeight;
			pPixelData = new GLubyte[nbBytes];
			
			// Copy image data
			
			pPixelDataPadded = new GLubyte[header->biSizeImage];
			memcpy(pPixelDataPadded, pData+sizeof(BITMAPINFOHEADER), header->biSizeImage); 
			
			// Translate to OpenGL compatible bitmap
			// From "OpenGL V: Translating Windows DIBs", 1995 
			// Dale Rogerson, Microsoft Developer Technology Group
			
			FPRGBTRIPLE* pSrc = (FPRGBTRIPLE*) pPixelDataPadded;
			FPGLRGBTRIPLE* pDest = (FPGLRGBTRIPLE*) pPixelData;
			
			int w = header->biWidth;
			int storageWidth;
			
			if ((w*3)%sizeof(DWORD)!=0)
				storageWidth = w*3 - (w*3)%sizeof(DWORD) + sizeof(DWORD);
			else
				storageWidth = w*3;
			
			int widthDiff = storageWidth - header->biWidth*sizeof(FPRGBTRIPLE) ;
			for(int j = 0 ; j < header->biHeight ; j++)
			{
				for(int i = 0 ; i < header->biWidth ; i++)
				{
					pDest->rgbRed = pSrc->rgbtRed ;
					pDest->rgbGreen = pSrc->rgbtGreen ;
					pDest->rgbBlue = pSrc->rgbtBlue ;
					pDest++ ;
					pSrc++ ;
				}
				pSrc = (FPRGBTRIPLE*)( (BYTE*)pSrc + widthDiff) ;
			}
			
			// Copy to clipboard
			
			m_clipboard->setCopyImageMode(CClipboard::IM_GRAYSCALE);
			m_clipboard->copyImage(header->biWidth, header->biHeight, pPixelData);
			
			// Delete image data
			
			delete [] pPixelData;
			delete [] pPixelDataPadded;
		}
		
		// Unlock handle
		
		GlobalUnlock(hglb); 
		
		// Clean up
		
		delete header;
	} 
	else
        doInfoMessage("Could not get clipboard data.");
	
	CloseClipboard(); 
	
	return; 
	
#else
    doInfoMessage("Hmm. I am currently having difficulties pasting from the Windows Clipboard...");
#endif
}

void CPaintView::lockScaleFactor()
{
	m_femGrid->setLockScale(true);
}

void CPaintView::unlockScaleFactor()
{
	m_femGrid->setLockScale(false);
}

/////////////////////////////////////////////////////////////
// Get/set methods
/////////////////////////////////////////////////////////////

void CPaintView::setRelativeForceSize(double size)
{
	m_relativeForceSize = size;
	updateModel();
}

void CPaintView::setMainFrame(void *frame)
{
	m_mainFrame = frame;
}

void CPaintView::setStressStep(int step)
{
	m_femGrid->setStressStep(step);
    this->doRedraw();
}

int CPaintView::getStressStep()
{
	return m_femGrid->getStressStep();
}

void CPaintView::setUseWeight(bool flag)
{
	m_useWeight = flag;
}

bool CPaintView::getUseWeight()
{
	return m_useWeight;
}

void CPaintView::setStressMode(CFemGrid2::TStressMode mode)
{
	m_femGrid->setStressMode(mode);
    this->doRedraw();
}

void CPaintView::setBrushMagnification(int factor)
{
	m_brushScale = factor;
}

void CPaintView::setForegroundColor(float red, float green, float blue)
{
	m_brushColor[0] = red;
	m_brushColor[1] = green;
	m_brushColor[2] = blue;
	m_rectangle->getColor()->setColor(red, green, blue, (float)m_blendFactor/255.0f);
	m_drawing->setFillColor((GLubyte)(red*255),(GLubyte)(green*255),(GLubyte)(blue*255));
}

void CPaintView::setBackgroundColor(float red, float green, float blue)
{
	m_backColor[0] = red;
	m_backColor[0] = green;
	m_backColor[0] = blue;
}

void CPaintView::getForegroundColor(float &red, float &green, float &blue)
{
	red = m_brushColor[0];
	green = m_brushColor[1];
	blue = m_brushColor[2];
}

void CPaintView::setBlendFactor(int factor)
{
	m_blendFactor = factor;
	m_currentBrush->createAlphaMask(0,m_blendFactor);
	m_rectangle->getColor()->setAlpha((float)m_blendFactor/255.0f);
}

void CPaintView::setCurrentBrush(int idx)
{
	if ((idx>=0)||(idx<(int)m_brushes.size()))
	{
		m_currentBrush = m_brushes[idx];
		m_currentBrush->createAlphaMask(0, m_blendFactor);
		m_currentInvertedBrush = m_invertedBrushes[idx];
		m_currentBrushIdx = idx;
	}
}

void CPaintView::setGridStride(int stride)
{
	m_femGrid->setStride(stride);
	this->clearResults();
    this->clearMesh();
	m_gridSpacing = stride;
}

int CPaintView::getGridStride()
{
	return m_femGrid->getStride();
}


void CPaintView::setEditMode(TEditMode mode)
{
	TEditMode oldMode = m_editMode;
	m_editMode = mode;
	
	if ((m_editMode!=EM_SELECT_BOX)&&(m_editMode!=EM_RESULT)&&(m_editMode!=EM_DYNAMIC_FORCE_UPDATE))
	{
        this->clearMesh();
		m_femGrid->resetStressDrawing();
	}
	
	updateSelectionBox();
	
    this->doInvalidate(); // Fixed redrawing problems
    this->doRedraw();

	if (m_modeChangeEvent!=NULL)
	{
		m_modeChangeEvent->onModeChange(oldMode, m_editMode);
	}
}

CPaintView::TEditMode CPaintView::getEditMode()
{
	return m_editMode;
}

void CPaintView::setLineWidth(int width)
{
	m_line->setWidth(width);
    m_arch->setLineWidth(width);
}

int CPaintView::getBlendFactor()
{
	return m_blendFactor;
}

int CPaintView::getCurrentBrushIdx()
{
	return m_currentBrushIdx;
}

int CPaintView::getLineWidth()
{
    return 	m_line->getWidth();
}

void CPaintView::setStiffness(double stiffness)
{
	this->setForegroundColor(1.0f-stiffness, 1.0f-stiffness, 1.0f-stiffness);
}

double CPaintView::getStiffness()
{
	return 1.0-m_brushColor[0];
}

void CPaintView::setDisplacementScale(double value)
{
	m_femGrid->setDisplacementScale(value);
    this->doRedraw();
}

void CPaintView::setStressAlpha(double alpha)
{
	m_femGrid->setStressAlpha(alpha);
    this->doRedraw();
}

void CPaintView::setStressSize(double size)
{
	m_femGrid->setStressSize(size);
    this->doRedraw();
}

void CPaintView::setStressWidth(double width)
{
	m_femGrid->setStressWidth(width);
    this->doRedraw();
}

void CPaintView::setStressTreshold(double lower, double upper)
{
	m_femGrid->setStressTreshold(lower, upper);
    this->doRedraw();
}

void CPaintView::setLockScaling(bool flag)
{
	m_femGrid->setLockScale(flag);
}

void CPaintView::setMaxIntensity(float intensity)
{
	m_maxIntensity = intensity;
	m_femGrid->setMaxIntensity(intensity);
    this->doRedraw();
}

void CPaintView::applyElementScale()
{
    this->m_femGrid->setElementScaleFactor(m_ruler->getActualLength()/m_ruler->getPixelLength());
}

void CPaintView::setRulerLength(double actualLength)
{
	if (actualLength>0.0)
	{
		m_ruler->setActualLength(actualLength);
        this->m_femGrid->setElementScaleFactor(m_ruler->getActualLength()/m_ruler->getPixelLength());
	}
}

double CPaintView::getRulerLength()
{
    return m_ruler->getActualLength();
}

void CPaintView::setVisualisationMode(TVisualisationMode mode)
{
	TVisualisationMode oldMode = m_visualisationMode;
	m_visualisationMode = mode;

	switch (m_visualisationMode) {
	case VM_PRINCIPAL_STRESS:
		m_femGrid->setDrawStructure(false);
		this->setDrawDisplacements(false);
		this->setStressType(CFemGrid2::ST_PRINCIPAL);
		this->setDrawStress(true);
		this->setDrawForcesAndConstraints(true);
		break;
	case VM_MISES_STRESS:
		m_femGrid->setDrawStructure(false);
		this->setDrawDisplacements(false);
		this->setStressType(CFemGrid2::ST_MISES_SMOOTH);
		this->setDrawStress(true);
		this->setDrawForcesAndConstraints(true);
		break;
	case VM_DISPLACEMENTS:
		m_femGrid->setDrawStructure(false);
		this->setDrawDisplacements(true);
		this->setDrawStress(false);
		this->setDrawForcesAndConstraints(true);
		break;
	case VM_STRUCTURE:
		m_femGrid->setDrawStructure(true);
		this->setDrawDisplacements(false);
		this->setDrawStress(false);
		this->setDrawForcesAndConstraints(false);
		break;
	default:
		break;
	}

	if (m_visualisationModeChangedEvent!=NULL)
		this->m_visualisationModeChangedEvent->onVisualisationModeChanged(oldMode, mode);

}

CPaintView::TVisualisationMode CPaintView::getVisualisationMode()
{
	return m_visualisationMode;
}

void CPaintView::setCommandLine(int argc, char **argv)
{
	m_argc = argc;
	m_argv = argv;

	// Initialize brush lists and variables
	
	loadBrushes();
	m_brushScale = 1;
	m_brushColor[0] = 0.0f;
	m_brushColor[1] = 0.0f;
	m_brushColor[2] = 0.0f;
	m_blendFactor = 255;
	
	setCurrentBrush(3);
}

const std::string CPaintView::getApplicationPath()
{
	std::string exePath = m_argv[0];
#ifdef WIN32
	int lastSlash = exePath.rfind("\\");
#else
	int lastSlash = exePath.rfind("/");
#endif
	return exePath.substr(0,lastSlash);
}


void CPaintView::setConstraintType(CConstraint::TConstraintType constraintType)
{
	m_constraintType = constraintType;
}

void CPaintView::setModelName(const std::string& modelName)
{
	m_modelName = modelName;

	if (m_modelChangedEvent!=NULL)
	{
		string filePath;
		string filename;

		::splitPath(m_modelName, filePath, filename);
		if (m_modelChangedEvent!=NULL)
			m_modelChangedEvent->onModelChanged(filename);
	}
}

//const char* CPaintView::getModelName()
//{
//	return m_modelName;
//}

void CPaintView::setZoomResults(bool flag)
{
	m_zoomResults = flag;
    this->doRedraw();
}

bool CPaintView::getZoomResults()
{
	return m_zoomResults;
}

void CPaintView::setCalcCG(bool flag)
{
	m_calcCG = flag;
	int cgx, cgy;
	
	
	if (m_calcCG)
	{
		m_femGrid->calcCenterOfGravity(cgx, cgy);
		
		m_cgIndicator->setPosition(m_drawingOffsetX+cgx, m_drawingOffsetY+cgy);
		m_cg[0] = cgx;
		m_cg[1] = cgy;
	}
	else
		m_femGrid->updatePixelArea();
	
	this->updateModel();
    this->doRedraw();
}

bool CPaintView::getCalcCG()
{
	return m_calcCG;
}

void CPaintView::showAbout()
{
    this->doShowAbout();
}

void CPaintView::showHelp() 
{
    this->doShowHelp();
}

void CPaintView::setImportMode(TImportMode mode)
{
	m_importMode = mode;
}

CPaintView::TImportMode CPaintView::getImportMode()
{
	return m_importMode;
}

void CPaintView::setViewMode(TViewMode mode)
{
	TViewMode oldMode = m_viewMode;

	switch (mode) {
	case VM_ACTION:
		if (this->execute())
		{
			m_viewMode = VM_ACTION;
			m_femGrid->setDimmedConstraints(false);
			if (m_viewModeChangeEvent!=NULL)
			{
				m_viewModeChangeEvent->onViewModeChange(oldMode, m_viewMode);
			}
		}
		else
		{
			if (m_viewModeErrorEvent!=NULL)
				m_viewModeErrorEvent->onViewModeError(oldMode, mode);
		}
		break;
	case VM_SKETCH:
		m_viewMode = VM_SKETCH;
		m_femGrid->setShowGrid(false);
		m_femGrid->setDrawStress(false);
		m_femGrid->setDimmedConstraints(true);
		m_femGrid->setDrawForcesAndConstraints(true);
		m_femGrid->clearResults();
        this->doInvalidate();
        this->doRedraw();

		if (m_viewModeChangeEvent!=NULL)
		{
			m_viewModeChangeEvent->onViewModeChange(oldMode, m_viewMode);
		}

		break;
	case VM_PHYSICS:
		m_viewMode = VM_PHYSICS;
		m_femGrid->setShowGrid(false);
		m_femGrid->setDrawStress(false);
		m_femGrid->setDimmedConstraints(false);
		m_femGrid->setDrawForcesAndConstraints(true);
		m_femGrid->clearResults();
        this->doInvalidate();
        this->doRedraw();

		if (m_viewModeChangeEvent!=NULL)
		{
			m_viewModeChangeEvent->onViewModeChange(oldMode, m_viewMode);
		}

		break;
	default:

		break;
	}
}

void CPaintView::setDrawStress(bool flag)
{
	m_femGrid->setDrawStress(flag);
    this->doRedraw();
}

bool CPaintView::getDrawStress()
{
	return m_femGrid->getDrawStress();
}

void CPaintView::setDrawDisplacements(bool flag)
{
	m_femGrid->setDrawDisplacements(flag);
	m_femGrid->setUndeformedGrid(flag);
    this->doRedraw();
}

bool CPaintView::getDrawDisplacements()
{
	return m_femGrid->getDrawDisplacements();
}

void CPaintView::setDrawForcesAndConstraints(bool flag)
{
	m_femGrid->setDrawForcesAndConstraints(flag);
    this->doRedraw();
}


void CPaintView::setStressType(CFemGrid2::TStressType stressType)
{
	// Make sure a colormap is loaded

	if ((stressType == CFemGrid2::ST_MISES)||(stressType == CFemGrid2::ST_MISES_SMOOTH))
		this->setColorMap(1);

	m_femGrid->setStressType(stressType);
    this->doRedraw();
}

void CPaintView::setColorMap(int index)
{
	so_print("CPaintView", "setColorMap()");
	if ((index>=1)&&(index<=17))
	{
		string filename = "";
		string filenameIndex = "";
#ifdef WIN32
		char cfilename[512];
		::GetModuleFileName(NULL, cfilename, sizeof(cfilename));
		string applicationExeLocation = cfilename;
#else
		string applicationExeLocation = this->m_argv[0];
#endif
		cout << "applicationExeLocation = " << applicationExeLocation << endl;
#ifndef __APPLE__
		int pos = applicationExeLocation.find_last_of("\\");
		string applicationDir = applicationExeLocation.substr(0,pos);
#else
		int pos = applicationExeLocation.find_last_of("/");
		string applicationDir = applicationExeLocation.substr(0,pos);
#endif

		ostringstream oss;
		oss << index;
		filenameIndex = oss.str();
		if (pos>=0)
#ifdef WIN32
			filename = applicationDir + "\\colormaps\\colormap" + filenameIndex + ".map";
#else
			filename = applicationDir + "/colormaps/colormap" + filenameIndex + ".map";
#endif
		else
			filename = "colormaps/colormap" + filenameIndex + ".map";
		
		cout << "colormap filename = " << filename << endl;
		m_femGrid->getColorMap()->open(filename.c_str());
		m_femGrid->updateColorMapTexture();
        this->doRedraw();
	}
}

void CPaintView::setInvertColorMap(bool flag)
{
	m_femGrid->getColorMap()->setInvert(flag);
    this->doRedraw();
}

void CPaintView::setUpperMisesTreshold(double upper)
{
	m_femGrid->setUpperMisesTreshold(upper);
    this->doRedraw();
}

void CPaintView::setElementTreshold(double value)
{
	m_femGrid->setElementTreshold(value);
}

double CPaintView::getElementTreshold()
{
	return m_femGrid->getElementTreshold();
}

void CPaintView::setForceMagnitude(double value)
{
	m_forceMagnitude = value;
}

double CPaintView::getForceMagnitude()
{
	return m_forceMagnitude;
}

void CPaintView::setOutputMatlab(bool flag)
{
	m_outputMatlab = flag;
}

bool CPaintView::getOutputMatlab()
{
	return m_outputMatlab;
}

void CPaintView::setStiffnessScaleFactor(double value)
{
	m_stiffnessScalefactor = value;
}

double CPaintView::getStiffnessScaleFactor()
{
	return m_stiffnessScalefactor;
}

void CPaintView::setElasticModulus(double value)
{
	m_elasticModulus = value;
}

double CPaintView::getElasticModulus()
{
	return m_elasticModulus;
}

void CPaintView::setYoungsModulus(double value)
{
	if (value>0.0)
		m_youngsModulus = value;
}

double CPaintView::getYoungsModulus()
{
	return m_youngsModulus;
}

void CPaintView::setThickness(double value)
{
	if (value>0.0)
		m_thickness = value;
}

double CPaintView::getThickness()
{
	return m_thickness;
}

void CPaintView::setConstraintStiffnessScale(double value)
{
	if (value>0.0)
		m_constraintStiffnessScale = value;
}

double CPaintView::getConstraintStiffnessScale()
{
	return m_constraintStiffnessScale;
}

void CPaintView::setWeight(double value)
{
	if (value>0.0)
		m_weight = value;
}

double CPaintView::getWeight()
{
	return m_weight;
}

void CPaintView::setSnapToGrid(bool flag)
{
	m_snapToGrid = flag;
}

bool getSnapToGrid();

void CPaintView::setMoveLoad(bool flag)
{
	m_moveLoad = flag;
}

bool CPaintView::getMoveLoad()
{
	return m_moveLoad;
}

void CPaintView::setOptLayer(bool active)
{
	m_optLayerActive = active;
	if (active)
		m_drawing->setLayer(1);
	else
		m_drawing->setLayer(0);
    this->doRedraw();
}

void CPaintView::setOptVolumeFraction(double fraction)
{
	m_optVolfrac = fraction;
}

double CPaintView::getOptVolumeFraction()
{
	return m_optVolfrac;
}

void CPaintView::setOptRmin(double rmin)
{
	m_optRmin = rmin;
}

double CPaintView::getOptRmin()
{
	return m_optRmin;
}

void CPaintView::setOptMinChange(double minChange)
{
	m_optMinChange = minChange;
}

double CPaintView::getOptMinChange()
{
	return m_optMinChange;
}

void CPaintView::setOptMaxLoops(int loops)
{
	m_optMaxLoops = loops;
}

int CPaintView::getOptMaxLoops()
{
	return m_optMaxLoops;
}

void CPaintView::setUiLineThickness(double thickness)
{
	CUiSettings::getInstance()->setLineThickness(thickness);
    this->doRedraw();
}

double CPaintView::getUiLineThickness()
{
	return CUiSettings::getInstance()->getLineThickness();
}

void CPaintView::setUiSymbolLength(double length)
{
	CUiSettings::getInstance()->setSymbolLength(length);
    this->doRedraw();
}

double CPaintView::getUiSymbolLength()
{
    return CUiSettings::getInstance()->getSymbolLength();
}

void CPaintView::setOptFilterType(CFemGridSolver2::TFilterType filterType)
{
	m_optFilterType = filterType;
}

CFemGridSolver2::TFilterType CPaintView::getOptFilterType()
{
	return m_optFilterType;
}

void CPaintView::setModeChangeEvent(CPVModeChangeEvent* eventMethod)
{
	m_modeChangeEvent = eventMethod;
}

void CPaintView::setViewModeChangeEvent(CPVViewModeChangeEvent* eventMethod)
{
	m_viewModeChangeEvent = eventMethod;
}

void CPaintView::setViewModeErrorEvent(CPVViewModeErrorEvent* eventMethod)
{
	m_viewModeErrorEvent = eventMethod;
}

void CPaintView::setStatusMessageEvent(CGSStatusMessageEvent* eventMethod)
{
	m_statusMessageEvent = eventMethod;
}

void CPaintView::setLogMessageEvent(CGSLogMessageEvent* eventMethod)
{
	m_logMessageEvent = eventMethod;
}

void CPaintView::setContinueCalcEvent(CGSContinueCalcEvent* eventMethod)
{
	m_continueCalcEvent = eventMethod;
}

void CPaintView::setModelChangedEvent(CPVModelChangedEvent* eventMethod)
{
	m_modelChangedEvent = eventMethod;
}

void CPaintView::setRulerChangedEvent(CPVRulerChangedEvent* eventMethod)
{
	m_rulerChangedEvent = eventMethod;
}

void CPaintView::setVisualisationModeChangedEvent(CPVVisualisationModeChangedEvent* eventMethod)
{
	m_visualisationModeChangedEvent = eventMethod;
}

void CPaintView::setModelLoadedEvent(CPVModelLoadedEvent* eventMethod) 
{
	m_modelLoadedEvent = eventMethod;
}

void CPaintView::setNewModelEvent(CPVNewModelEvent* eventMethod)
{
	m_newModelEvent = eventMethod;
}

