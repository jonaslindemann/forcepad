//
// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2008 Division of Structural Mechanics, Lund University
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

#include "NewModelDlg.h"
#include "FemGridSolver2.h"
#include "MainFrame2.h"
#include "LogWindow.h"
#include "JpegImage.h"
#include "PngImage.h"

#define WIN32_COMMON_DIALOGS

#ifdef WIN32
#ifdef WIN32_COMMON_DIALOGS
#include <windows.h>
#include "fl_ask_win.h"
#include "fl_message_win.h"
#include "fl_file_chooser_win.H"
#else
#include <FL/fl_ask.h>
#include <FL/fl_message.h>
#include <FL/fl_file_chooser.h>
#endif
#else
#include <FL/fl_ask.h>
#include <FL/fl_message.h>
#include <FL/fl_file_chooser.h>
#endif

#include <FL/filename.H>
#include <FL/fl_draw.H>

#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

#include <newmat.h>
#include <newmatio.h>
#include <newmatap.h>

#include "Fl_Cursor_Shape.H"
#include "Cursors.h"

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
: Fl_Gl_Window(x,y,w,h,l)
{
	so_print("CPaintView","CPaintView(...)");

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

	// Calculation settings

	m_forceMagnitude = 1000.0;
	m_weight = 1000.0;
	m_useWeight = false;
	m_outputMatlab = false;
	m_stiffnessScalefactor = 1000.0;
	m_elasticModulus = 2.1e9;
	m_youngsModulus = 0.35;
	m_thickness = 1.0;
	m_constraintStiffnessScale = 1e3;
	m_moveLoad = false;

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
	
	color = new CColor();
	color->setColor(0.5f, 0.0f, 0.0f, 1.0f);

	m_ruler = new CRuler();
	m_ruler->setStartPos(100, 100);
	m_ruler->setEndPos(150, 150);
	m_ruler->setColor(color);

	color = new CColor();
	color->setColor(0.0f, 0.5f, 0.0f, 1.0f);

	// Rigid body variables
	
	m_cgIndicator = new CCGIndicator();
	m_cgIndicator->setColor(color);
	m_relativeForceSize = 0.05;
	
#ifdef FORCEPAD_RIGID
	m_cgIndicator->setShowGravityArrow(true);
	m_cgIndicator->setArrowLength(50.0);
	
	color = new CColor();
	color->setColor(1.0f, 0.5f, 0.0f, 1.0f);
	
	int i;
	
	for (i=0; i<3; i++)
	{
		m_reactionForces[i] = new CReactionForce();
		m_reactionForces[i]->setColor(color);
		m_reactionForces[i]->setPosition(100.0 + (double)i*20.0, 100.0);
	}
	
	m_validReactions = false;
	m_calcCG = true;
#endif

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
	m_drawing->fillRectAlpha(50,50,200,200, 128);
	m_drawing->setAlpha(128);
	m_drawing->setLayer(0);

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
	
	createCursors();

	m_modeChangeEvent = NULL;
	m_viewModeChangeEvent = NULL;
	m_viewModeErrorEvent = NULL;
	m_statusMessageEvent = NULL;
	m_logMessageEvent = NULL;
	m_modelChangedEvent = NULL;
	m_rulerChangedEvent = NULL;

	// Set initial model name
	
	setModelName("noname.fp2");

	m_runOnce = true;
}


CPaintView::~CPaintView()
{
	so_print("CPaintView","~CPaintView()");
	
	// Do the usual cleanup
	
	deleteBrushes();
	
	
#ifdef FORCEPAD_RIGID
	int i;
	
	for (i=0; i<3; i++)
		delete m_reactionForces[i];
#endif
	
	deleteCursors();
}

/////////////////////////////////////////////////////////////
// CPaintView FLTK overrides
/////////////////////////////////////////////////////////////

void CPaintView::draw()
{
	// Clear screen

	if (m_runOnce)
	{
		//
		// Initialise GLEW
		//

		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		}
		fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
		m_runOnce = false;
	}
	
	onClear(); 
	
	if (!valid())
		onInitContext();
	
	glPushMatrix();
	onDraw();
	glPopMatrix();
}


int CPaintView::handle(int event)
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
		fl_cursor( FL_CURSOR_DEFAULT );
		return 1;
		break;
	default:
		//so_print("CPaintView","unhandled event");
		return 0;
	}
}

/////////////////////////////////////////////////////////////
// CPaintView event methods
/////////////////////////////////////////////////////////////

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

		m_selectedForce = m_femGrid->getNearestForce(x-m_drawingOffsetX, h()-y-m_drawingOffsetY);
		break;
	case EM_CONSTRAINT:
		
		// Create a constraint and add it to the grid
		
		constraint = new CConstraint();
		constraint->setPosition(x-m_drawingOffsetX, h()-y-m_drawingOffsetY);
		constraint->setConstraintType(m_constraintType);
		m_femGrid->addConstraint(constraint);
		this->redraw();
		break;
	case EM_FORCE:
		
		// Create a new force. The position is set here.
		// Direction is is done in the onDrag() method.
		
		m_newForce = new CForce();
		m_newForce->setPosition(x-m_drawingOffsetX, h()-y-m_drawingOffsetY);
		m_femGrid->addForce(m_newForce);
		break;
	case EM_CONSTRAINT_VECTOR:
		
		// Create a directional constraint (ForcePAD/Rigid)
		
#ifdef FORCEPAD_RIGID
		if (m_femGrid->getConstraintsSize()<3)
		{
			m_newConstraint = new CConstraint();
			m_newConstraint->setConstraintType(CConstraint::CT_VECTOR);
			m_newConstraint->setPosition(x-m_drawingOffsetX, h()-y-m_drawingOffsetY);
			m_femGrid->addConstraint(m_newConstraint);
		}
#else
		m_newConstraint = new CConstraint();
		m_newConstraint->setConstraintType(CConstraint::CT_VECTOR);
		m_newConstraint->setPosition(x-m_drawingOffsetX, h()-y-m_drawingOffsetY);
		m_newConstraint->setDirection(0.0, 1.0);
		m_femGrid->addConstraint(m_newConstraint);
#endif

		break;
	case EM_CONSTRAINT_HINGE:
		
		// Create a directional constraint (ForcePAD/Rigid)
		
		m_newConstraint = new CConstraint();
		m_newConstraint->setConstraintType(CConstraint::CT_HINGE);
		m_newConstraint->setPosition(x-m_drawingOffsetX, h()-y-m_drawingOffsetY);
		m_newConstraint->setDirection(0.0, 1.0);
		m_femGrid->addConstraint(m_newConstraint);

		break;
	case EM_ERASE_CONSTRAINTS_FORCES:
		
		// Erase constraints and forces
		
		m_femGrid->erasePointLoad(x-m_drawingOffsetX, h()-y-m_drawingOffsetY,16);
		m_femGrid->erasePointConstraint(x-m_drawingOffsetX, h()-y-m_drawingOffsetY,16);
		break;
	case EM_SELECT_BOX:
		
		// Set starting point for selection box
		
		m_selectionStart[0] = x-m_drawingOffsetX;
		m_selectionStart[1] = h()-y-m_drawingOffsetY;
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
			h() - m_current[1]-m_drawingOffsetY-m_clipboard->getClipboard()->getHeight()/2,
			0
			);
		updateUndoArea(
			m_current[0]-m_drawingOffsetX+m_clipboard->getClipboard()->getWidth()/2, 
			h() - m_current[1]-m_drawingOffsetY+m_clipboard->getClipboard()->getHeight()/2,
			0
			);
		updateUndo();
		m_clipboard->paste(x-m_drawingOffsetX-ww/2, h()-y-m_drawingOffsetY-hh/2);
		this->redraw();
		break;
	case EM_DIRECT_BRUSH:
		
		// Update undo area
		
		undoToDrawing();
		updateUndo();
		break;
	default:
		break;
	}
	
	// We have to redraw then image
	
	if (m_editMode!=EM_RESULT)
		this->redraw();
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
	
#ifndef FORCEPAD_RIGID
	CConstraintPtr constraint;
#endif

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
				//m_selectedForce->setPosition(x-m_drawingOffsetX, h()-y-m_drawingOffsetY);
				//m_femGrid->addForce(m_selectedForce);

				m_femGrid->moveForce(m_selectedForce, x-m_drawingOffsetX, h()-y-m_drawingOffsetY);

			}
			else
			{
				// Update the direction of the force created in onPush
				
				m_selectedForce->setDirection(m_start[0]-m_current[0], (h() - m_start[1]) - (h() - m_current[1]) );			
			}

			m_solver->executeUpdate();
			this->flush();
			this->redraw();
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
						h()-prevPos[1]-m_drawingOffsetY-m_currentBrush->getHeight()/2, 
						m_current[0]-m_drawingOffsetX-m_currentBrush->getWidth()/2, 
						h()-m_current[1]-m_drawingOffsetY-m_currentBrush->getHeight()/2, 
						eraseColor
					);		
				else
					m_drawing->drawImageLine(
						m_currentBrush, 
						prevPos[0]-m_drawingOffsetX-m_currentBrush->getWidth()/2, 
						h()-prevPos[1]-m_drawingOffsetY-m_currentBrush->getHeight()/2, 
						m_current[0]-m_drawingOffsetX-m_currentBrush->getWidth()/2, 
						h()-m_current[1]-m_drawingOffsetY-m_currentBrush->getHeight()/2, 
						m_optConstraintColor
					);		
			}
			else
			{
				if (m_editMode == EM_DIRECT_ERASE)
					m_drawing->drawImageLine(
						m_currentBrush, 
						prevPos[0]-m_drawingOffsetX-m_currentBrush->getWidth()/2, 
						h()-prevPos[1]-m_drawingOffsetY-m_currentBrush->getHeight()/2, 
						m_current[0]-m_drawingOffsetX-m_currentBrush->getWidth()/2, 
						h()-m_current[1]-m_drawingOffsetY-m_currentBrush->getHeight()/2, 
						eraseColor
					);		
				else
					m_drawing->drawImageLine(
						m_currentBrush, 
						prevPos[0]-m_drawingOffsetX-m_currentBrush->getWidth()/2, 
						h()-prevPos[1]-m_drawingOffsetY-m_currentBrush->getHeight()/2, 
						m_current[0]-m_drawingOffsetX-m_currentBrush->getWidth()/2, 
						h()-m_current[1]-m_drawingOffsetY-m_currentBrush->getHeight()/2, 
						m_brushColor
					);		
			}
		}
		else
		{
			if (m_optLayerActive)
				m_drawing->copyFrom(m_currentBrush, m_current[0]-m_drawingOffsetX-m_currentBrush->getWidth()/2, h()-m_current[1]-m_drawingOffsetY-m_currentBrush->getHeight()/2, m_optConstraintColor);
			else
				m_drawing->copyFrom(m_currentBrush, m_current[0]-m_drawingOffsetX-m_currentBrush->getWidth()/2, h()-m_current[1]-m_drawingOffsetY-m_currentBrush->getHeight()/2, m_brushColor);
		}
		this->redraw();
		break;
	case EM_RECTANGLE:
		resetUndoArea();
		
		// Update the rectangle primitive

		if (this->m_optLayerActive)
			m_rectangle->getColor()->setColor(m_optConstraintColor);
		else
			m_rectangle->getColor()->setColor(m_brushColor);
		
		m_rectangle->setPosition(m_start[0], h() - m_start[1]);
		m_rectangle->setSize(m_current[0]-m_start[0], -m_current[1]+m_start[1]);
		
		// Update the affected area. We do this twice because
		// updateUndoArea only takes a position as input
		
		updateUndoArea(
			m_start[0] - m_drawingOffsetX,
			h() - m_start[1] - m_drawingOffsetY,
			0
			);
		updateUndoArea(
			m_current[0] - m_drawingOffsetX,
			h() - m_current[1] - m_drawingOffsetY,
			0
			);
		this->redraw();
		break;
	case EM_ELLIPSE:
		resetUndoArea();
		
		// Update the ellipse primitive
		
		if (this->m_optLayerActive)
			m_ellipse->getColor()->setColor(m_optConstraintColor);
		else
			m_ellipse->getColor()->setColor(m_brushColor);

		m_ellipse->setPosition(m_start[0], h() - m_start[1]);
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
			h() - p1[1] - m_drawingOffsetY,
			0
			);
		updateUndoArea(
			p2[0] - m_drawingOffsetX,
			h() - p2[1] - m_drawingOffsetY,
			0
			);
		//updateUndoArea(
		//	-m_current[0] - m_drawingOffsetX,
		//	h() + m_current[1] - m_drawingOffsetY,
		//	0
		//	);
		this->redraw();
		break;
	case EM_LINE:
		resetUndoArea();
		
		// Update line primitive
		
		if (this->m_optLayerActive)
			m_line->getColor()->setColor(m_optConstraintColor);
		else
			m_line->getColor()->setColor(m_brushColor);

		m_line->setStartPos(m_start[0], h() - m_start[1]);
		m_line->setEndPos(m_current[0], h() - m_current[1]);
		
		// Update the affected area. We do this three times because
		// updateUndoArea only takes a position as input
		
		updateUndoArea(
			m_start[0] - m_drawingOffsetX,
			h() - m_start[1] - m_drawingOffsetY,
			m_line->getWidth()
			);
		updateUndoArea(
			m_current[0] - m_drawingOffsetX,
			h() - m_current[1] - m_drawingOffsetY,
			m_line->getWidth()
			);
		this->redraw();
		break;
	case EM_RULER:
		m_ruler->setStartPos(
			m_start[0] - m_drawingOffsetX, 
			h() - m_start[1] - m_drawingOffsetY
			);
		m_ruler->setEndPos(
			m_current[0] - m_drawingOffsetX, 
			h() - m_current[1] - m_drawingOffsetY
			);
		if (m_rulerChangedEvent!=NULL)
			m_rulerChangedEvent->onRulerChanged(m_ruler);
		this->redraw();
		break;
	case EM_FORCE:
		
		// Update the direction of the force created in onPush
		
		m_newForce->setDirection(m_start[0]-m_current[0], (h() - m_start[1]) - (h() - m_current[1]) );
		this->redraw();
		break;
	case EM_CONSTRAINT_VECTOR:
		
		// Update the constraint direction (Rigid) for the new constraint 
		// created in onPush
		
#ifdef FORCEPAD_RIGID
		if ((m_femGrid->getConstraintsSize()<=3)&&(m_newConstraint!=NULL))
		{
			m_newConstraint->setDirection(m_start[0]-m_current[0], (h() - m_start[1]) - (h() - m_current[1]) );
			this->redraw();
		}
#else
		if (m_newConstraint!=NULL)
		{
			m_newConstraint->setDirection(m_start[0]-m_current[0], (h() - m_start[1]) - (h() - m_current[1]) );
			this->redraw();
		}
#endif
		break;
	case EM_CONSTRAINT_HINGE:
		
		// Update the constraint direction (Rigid) for the new constraint 
		// created in onPush
		
		if (m_newConstraint!=NULL)
		{
			m_newConstraint->setDirection(m_start[0]-m_current[0], (h() - m_start[1]) - (h() - m_current[1]) );
			this->redraw();
		}
		break;
	case EM_CONSTRAINT:
		
		// Create additional constraints (normal ForcePAD)
		
#ifndef FORCEPAD_RIGID
		constraint = new CConstraint();
		constraint->setPosition(x-m_drawingOffsetX, h()-y-m_drawingOffsetY);
		constraint->setConstraintType(m_constraintType);
		m_femGrid->addConstraint(constraint);
		this->redraw();
#endif
		break;
	case EM_ERASE_CONSTRAINTS_FORCES:
		
		// Erase constraints and forces
		
		m_femGrid->erasePointLoad(x-m_drawingOffsetX, h()-y-m_drawingOffsetY,16);
		m_femGrid->erasePointConstraint(x-m_drawingOffsetX, h()-y-m_drawingOffsetY,16);
		this->redraw();
	case EM_SELECT_BOX:
		
		// Update the selection box to new cursor position
		
		m_selectionEnd[0] = x - m_drawingOffsetX;
		m_selectionEnd[1] = h() - y - m_drawingOffsetY;
		updateSelectionBox();
		
		// Here we need to redraw the image (Tiled rendering perhaps??)
		
		this->redraw();
		break;
	default:
		
		break;
	}
}


void CPaintView::onRelease(int x, int y)
{
	//
	// Handle FLTK release event (MouseUp).
	// Read entire workspace into image m_drawing.
	//
	
	m_leftMouseDown = false;
	
	updateCursor();
	
	switch (m_editMode) {
	case EM_FLOODFILL:
		
		// Do floodfill of area under cursor
		
		m_drawing->floodFill(x-m_drawingOffsetX, (h()-y)-m_drawingOffsetY); //, 255, 0, 0);
		this->redraw();
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
		
		updateUndo();
		m_screenImage->update(m_undoStart[0], m_undoStart[1], m_undoEnd[0], m_undoEnd[1]);
		
		break;
	default:
		break;
	}
	
	// Center of gravity(Rigid)/Center of stiffness calculations
	
	if (m_calcCG)
	{
#ifndef FORCEPAD_RIGID
		m_femGrid->calcCenterOfGravity(m_cg[0], m_cg[1]);
#else
		m_femGrid->calcCenterOfStiffness(m_cg[0], m_cg[1]);
#endif
		m_cgIndicator->setPosition(m_drawingOffsetX+m_cg[0], m_drawingOffsetY+m_cg[1]);
		this->redraw();
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
	
	m_drawingOffsetX = (w()-m_drawing->getWidth())/2;
	m_drawingOffsetY = (h()-m_drawing->getHeight())/2;
	
	if (m_drawingOffsetX<0)
		m_drawingOffsetX = 0;
	
	if (m_drawingOffsetY<0)
		m_drawingOffsetY = 0;
	
	m_clipboard->setDrawingOffset(m_drawingOffsetX, m_drawingOffsetY);
	
	this->updateSelectionBox();
	
	// Do center of mass/stiffness calculations
	
	if (m_calcCG)
	{
#ifndef FORCEPAD_RIGID
		m_femGrid->calcCenterOfGravity(m_cg[0], m_cg[1]);
#else
		m_femGrid->calcCenterOfStiffness(m_cg[0], m_cg[1]);
#endif
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
	glViewport(0,0,w(),h());
	gluOrtho2D(0,w(),0,h());
	glMatrixMode(GL_MODELVIEW);
	
	
	// Define drawing area using the scissor function
	
	glScissor(m_drawingOffsetX, m_drawingOffsetY, m_drawing->getWidth(), m_drawing->getHeight());
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void CPaintView::onDraw()
{
	glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_POLYGON_SMOOTH);

	// Background properties

	int shadowWidth = 10;
	int shadowWidth2 = 5;

	// Draw background
	
	glDisable(GL_SCISSOR_TEST);
	glBegin(GL_QUADS);
	glColor3f(0.7f, 0.7f, 0.7f);
	glVertex2i(0,h());
	glVertex2i(w(),h());
	glColor3f(0.8f, 0.8f, 0.8f);
	glVertex2i(w(),0);
	glVertex2i(0,0);
	glEnd();

	// Top frame shadow

	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.5f, 0.5f);
	glVertex2i(0,h());
	glVertex2i(w(),h());
	glColor3f(0.7f, 0.7f, 0.7f);
	glVertex2i(w()-shadowWidth2,h()-shadowWidth);
	glVertex2i(shadowWidth,h()-shadowWidth);
	glEnd();

	// Left frame shadow

	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.5f, 0.5f);
	glVertex2i(0,h());
	glVertex2i(0,0);
	glColor3f(0.8f, 0.8f, 0.8f);
	glVertex2i(shadowWidth,0);
	glColor3f(0.7f, 0.7f, 0.7f);
	glVertex2i(shadowWidth,h()-shadowWidth);
	glEnd();

	// Right frame shadow

	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.5f, 0.5f);
	glVertex2i(w(),h());
	glVertex2i(w(),0);
	glColor3f(0.8f, 0.8f, 0.8f);
	glVertex2i(w()-shadowWidth2,0);
	glColor3f(0.7f, 0.7f, 0.7f);
	glVertex2i(w()-shadowWidth2,h()-shadowWidth);
	glEnd();

	// Bottom frame shadow

	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.5f, 0.5f);
	glVertex2i(0,0);
	glVertex2i(w(),0);
	glColor3f(0.8f, 0.8f, 0.8f);
	glVertex2i(w()-shadowWidth2,shadowWidth2);
	glVertex2i(shadowWidth,shadowWidth2);
	glEnd();

	// Disable drawing outside the image.

	glEnable(GL_SCISSOR_TEST);
	
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
			glRasterPos2i(m_current[0]-m_clipboard->getClipboard()->getWidth()*m_brushScale/2, h()-m_current[1]-m_clipboard->getClipboard()->getHeight()*m_brushScale/2);
			glDrawPixels(m_clipboard->getClipboard()->getWidth(), m_clipboard->getClipboard()->getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, m_clipboard->getClipboard()->getImageMap());
			glDisable(GL_COLOR_LOGIC_OP);
			m_clipboard->render(m_current[0]-m_clipboard->getClipboard()->getWidth()*m_brushScale/2, h()-m_current[1]-m_clipboard->getClipboard()->getHeight()*m_brushScale/2);
			glPixelZoom(1.0, 1.0);
			break;
		case EM_RECTANGLE:
			if (m_leftMouseDown)
			{
				m_rectangle->render();
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
		case EM_LINE:
			break;
		default:
			m_femGrid->setPosition(m_drawingOffsetX, m_drawingOffsetY);
			m_femGrid->render();
			break;
		}
	}
	else
	{
		m_femGrid->setPosition(m_drawingOffsetX, m_drawingOffsetY);
		
		if (m_calcCG)
			m_cgIndicator->render();
		
		m_femGrid->render();
		
#ifdef FORCEPAD_RIGID
		if (m_validReactions)
		{
			int i;
			glPushMatrix();
			glTranslated((double)m_drawingOffsetX, (double)m_drawingOffsetY, 0.0);
			for (i=0; i<3; i++)
				m_reactionForces[i]->render();
			glPopMatrix();
		}
#endif
		
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
		this->redraw();
		break;
	case EM_PASTE:
		this->redraw();
		break;
	default:
		
		break;
	}
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
	
	FILE* f;
	string brushPath = "";
	string brushName = "";

#ifdef __APPLE__
	string exePath = m_argv[0];
	int lastSlash = exePath.rfind("/");
	brushPath = exePath.substr(0,lastSlash)+"/brushes/";
#else
	f = fopen("/usr/share/forcepad/brushes/rbrush4.rgb", "rb");
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
	this->redraw();
}

void CPaintView::clearResults()
{
	so_print("CPaintView", "clearResults()");
	m_femGrid->clearResults();
	this->redraw();
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
		calcRigidReactions();
		this->redraw();
	}
}

void CPaintView::updateCursor()
{
	switch (m_editMode) {
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
}

void CPaintView::createCursors()
{
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
	
}

void CPaintView::deleteCursors()
{
	int i;
	
	for (i=0; i<20; i++)
		delete m_cursors[i];
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
		if (fl_ask("Model contains >10000 degrees of freedom.\nCalculation can take a long time.\nContinue?")==0)	
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
		fl_message("No structure to solve.");
		break;
	case CFemGridSolver2::ET_NO_BCS:
		fl_message("Add locks to structure.");
		break;
	case CFemGridSolver2::ET_NO_LOADS:
		fl_message("No loads defined on structure.");
		break;
	case CFemGridSolver2::ET_UNSTABLE:
		fl_message("Structure unstable. Try adding locks.");
		break;
	case CFemGridSolver2::ET_INVALID_MODEL:
		fl_message("Model invalid.");
		break;
	case CFemGridSolver2::ET_LOAD_OUTSIDE_AE:
		fl_message("Loads defined outside structure.");
		break;
	case CFemGridSolver2::ET_BC_OUTSIDE_AE:
		fl_message("Locks defined outside structure.");
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
		this->invalidate();
	}
	else
		m_femGrid->setShowGrid(true);
	
	so_print("CPaintView","\tRedraw.");
	
	this->redraw();

	return !errors;
}

void CPaintView::newModel()
{
	disableDrawing();
	
	so_print("CPaintView","newModel()");
	
	CNewModelDlg* dlg = new CNewModelDlg();
	dlg->setSize(640, 480);
	dlg->centerWindow(this->window());
	dlg->show();
	
	if (dlg->getModalResult()!=MR_CANCEL)
	{
		int width, height, initialStiffness;
		
		dlg->getSize(width, height);
		initialStiffness = dlg->getInitialStiffness();
		
		this->setModelName("noname.fp2");
		
		// Create drawing area
		
		m_drawing = new CSgiImage();
		m_drawing->setChannels(3);
		m_drawing->setSize(width, height);
		m_drawing->fillColor(255-initialStiffness,255-initialStiffness,255-initialStiffness);
		
		// Create image grid
		
		m_femGrid->setImage(m_drawing);
		m_femGrid->setShowGrid(false);
		m_clipboard->setImage(m_drawing);
		m_undoClipboard->setImage(m_drawing);
		m_screenImage->setImage(m_drawing);
		
		m_showMesh = false;
		
#ifdef FORCEPAD_RIGID
		m_validReactions = false;
#endif
		this->invalidate();
		this->redraw();
	}
	
	enableDrawing();
}

void CPaintView::executeCorba()
{
}

void CPaintView::openImage()
{
	disableDrawing();
	
	so_print("CPaintView","openImage()");
	
#ifdef WIN32
	char* fname = fl_file_chooser_image("Open image file", "");
#else
	char* fname = fl_file_chooser("Open image file", "*.*", "");
#endif
	m_danglingRelease = true;
	
	if (fname!=NULL)
	{
		bool jpegFile = false;
		bool pngFile = false;
		bool rgbFile = false;
		
		if (strcmp(fl_filename_ext(fname),".jpg")==0)
			jpegFile = true;
		
		if (strcmp(fl_filename_ext(fname),".jpeg")==0)
			jpegFile = true;
		
		if (strcmp(fl_filename_ext(fname),".png")==0)
			pngFile = true;
		
		if (strcmp(fl_filename_ext(fname),".rgb")==0)
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
			jpegImage->setFileName(fname);
			jpegImage->read();
			image = jpegImage;
		}
		
		if (pngFile)
		{
			pngImage = new CPngImage();
			pngImage->setFileName(fname);
			pngImage->read();
			image = pngImage;
		}
		
		if (rgbFile)
		{
			rgbImage = new CSgiImage();
			rgbImage->setFileName(fname);
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
			
#ifdef FORCEPAD_RIGID
			m_validReactions = false;
#endif
		}
		else
		{
			m_clipboard->copyImage(image->getWidth(), image->getHeight(), image->getImageMap());			
			setEditMode(EM_PASTE);
		}
		
		this->invalidate();
		this->redraw();
	}
	
	enableDrawing();
}

void CPaintView::saveModelAs()
{
	so_print("CPaintView", "saveModelAs()");
	
	//
	// Ask for file name
	//

	char* fname = fl_file_chooser("Save forcepad model", "*.fp2", m_modelName.c_str());
	
	if (fname!=NULL)
	{
		setModelName(fname);
	}
	else
		return;
	
	//
	// Save file
	//
	
	using namespace std;
	
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
		char* fname = fl_file_chooser("Save forcepad model", "*.fp2", m_modelName.c_str());
		
		if (fname!=NULL)
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
	
	fstream f;
	f.open(m_modelName.c_str(), ios::out);
	m_femGrid->saveToStream(f);
	f.close();
}

void CPaintView::expandImageToWindow()
{
	disableDrawing();

	m_drawing = new CSgiImage();
	m_drawing->setChannels(3);

	int i, width, height;

	width = this->w()-50;
	height = this->h()-50;

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
	
	// Create image grid
	
	m_femGrid->setImage(m_drawing);
	m_femGrid->setShowGrid(false);
	m_clipboard->setImage(m_drawing);
	m_undoClipboard->setImage(m_drawing);
	m_screenImage->setImage(m_drawing);

	this->invalidate();
	this->redraw();

	enableDrawing();
}

void CPaintView::openModel()
{
	using namespace std;
	
	disableDrawing();
	
	so_print("CPaintView", "openModel()");
	
	char* fname = fl_file_chooser("Open forcepad model", "*.fp2", "");
	m_danglingRelease = true;
	
	if (fname!=NULL)
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
		
		this->invalidate();
		this->redraw();
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
	
	copyToWindows();
	m_clipboard->copy(x1, y1, x2, y2);
	
	this->redraw();
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
	
	this->redraw();
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
	
	this->redraw();
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
	
	
	GLubyte *pPixelData = new GLubyte[nbBytes];
	//int storageWidth = w*3 - (w*3)%sizeof(DWORD) + sizeof(DWORD);
	int storageWidth = w*3;
	GLubyte *pPixelDataPadded = new GLubyte[storageWidth*h];
	
	// Make OpenGL context current
	
	make_current(); 
	
	// Read pixels from screen
	
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	
	glReadPixels(
		x1+m_drawingOffsetX,
		y1+m_drawingOffsetY,
		w,
		h,
		GL_RGBA,GL_UNSIGNED_BYTE,
		pPixelData
		);
	
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
			pPixelDataPadded[j*storageWidth+i*3] = pPixelData[j*w*3+i*3+2];
			pPixelDataPadded[j*storageWidth+i*3+1] = pPixelData[j*w*3+i*3+1];
			pPixelDataPadded[j*storageWidth+i*3+2] = pPixelData[j*w*3+i*3];
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
		delete [] pPixelData;
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
		fl_message("Clipboard format not supported.");
		return; 
	}
	
	// Try to open clipboard
	
	if (!OpenClipboard(NULL)) 
	{
		fl_message("Could not open clipboard.");
		return; 
	}
	
	// Get clipboard data
	
	hglb = GetClipboardData(CF_DIB); 
	
	if (hglb != NULL) 
	{ 
		// Lock handle 
		
		char *pData = (char *) GlobalLock(hglb); 
		
		if (pData==NULL)
			fl_message("This should not normally happen. (GlobalLock failed).");
		
		// Create header
		
		BITMAPINFOHEADER* header; 
		header = new BITMAPINFOHEADER; 
		
		// Copy header 
		
		memcpy(header,pData,sizeof(BITMAPINFOHEADER)); 
		
		// Check if we can handle the bitmap
		
		if (header->biBitCount!=24)
			fl_message("Image must be 24 bit color.");
		
		if (header->biCompression!=0)
			fl_message("Compression not supported.");
		
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
		fl_message("Could not get clipboard data.");
	
	CloseClipboard(); 
	
	return; 
	
#else
	fl_message("Hmm. I am currently having difficulties pasting from the Windows Clipboard...");
#endif
}

void CPaintView::calcRigidReactions()
{
	so_print("CPaintView", "calcRigidReactions()");
#ifdef FORCEPAD_RIGID
	if (m_calcCG)
	{
		int i;
		
		double pixelArea = m_femGrid->getPixelArea();
		
		if (pixelArea>0.0)
		{
			
			CConstraintSelectionPtr constraintSelection = m_femGrid->getConstraints();
			CForceSelectionPtr forceSelection = m_femGrid->getForces();
			
			if (constraintSelection->getSize()!=3)
				return;
			
			double Fx = 0.0;
			double Fy = 0.0;
			double M = 0.0;
			
			Matrix K(3,3);
			K = 0.0;
			
			ColumnVector R(3);
			R = 0.0;
			
			ColumnVector Fext(3);
			R = 0.0;
			
			for (i=0; i<3; i++)
			{
				CConstraintPtr constraint = constraintSelection->getConstraint(i);
				
				double x, y;
				double ex, ey;
				
				constraint->getPosition(x, y);
				constraint->getDirection(ex, ey);
				
				switch (constraint->getConstraintType()) {
				case CConstraint::CT_X:
					K(1,i+1) = 1.0;
					K(3,i+1) = y - (double)m_cg[1];
					m_reactionForces[i]->setDirection(1.0, 0.0);
					break;
				case CConstraint::CT_Y:
					K(2,i+1) = 1.0;
					K(3,i+1) = (double)m_cg[0] - x;
					m_reactionForces[i]->setDirection(0.0, 1.0);
					break;
				case CConstraint::CT_VECTOR:
					K(1,i+1) = ex;
					K(2,i+1) = ey;
					//K(3,i+1) = ey*((double)m_cg[0] - x) + ex*(y - (double)m_cg[1]);
					K(3,i+1) = ey*x - ex*y;
					m_reactionForces[i]->setDirection(ex, ey);
					break;
				default:
					
					break;
				}
				m_reactionForces[i]->setPosition(x, y);
			}
			
			for (i=0; i<forceSelection->getSize(); i++)
			{
				CForcePtr force = forceSelection->getForce(i);
				
				double x, y;
				
				force->getPosition(x, y);
				
				double ex, ey;
				
				force->getDirection(ex, ey);
				
				Fx += m_relativeForceSize*pixelArea*ex;
				Fy += m_relativeForceSize*pixelArea*ey;
				
				//M += Fx * (y - (double)m_cg[1]);
				//M += Fy * ((double)m_cg[0] - x);
				M -= m_relativeForceSize*pixelArea*ex * y;
				M += m_relativeForceSize*pixelArea*ey * x;
			}
			
			M -= pixelArea*m_cg[0];
			
			Fy -= pixelArea;
			
			Fext(1) = -Fx;
			Fext(2) = -Fy;
			Fext(3) = -M;
			
			Try 
			{
				LinearEquationSolver A = K;
				R = A.i() * Fext;
			}
			CatchAll 
			{	
				so_print("CPaintView","\tSomethings wrong...");
			}
			
			m_reactionForces[0]->setLength(m_cgIndicator->getArrowLength()*R(1)/pixelArea);
			m_reactionForces[1]->setLength(m_cgIndicator->getArrowLength()*R(2)/pixelArea);
			m_reactionForces[2]->setLength(m_cgIndicator->getArrowLength()*R(3)/pixelArea);
			
			m_validReactions = true;
		}
	}
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
	this->redraw();
}

int CPaintView::getStressStep()
{
	return m_femGrid->getStressStep();
}

void CPaintView::setUseWeight(bool flag)
{
	m_useWeight = flag;
}

void CPaintView::setStressMode(CFemGrid2::TStressMode mode)
{
	m_femGrid->setStressMode(mode);
	this->redraw();
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
	
	this->invalidate(); // Fixed redrawing problems
	this->redraw();

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
	this->redraw();
}

void CPaintView::setStressAlpha(double alpha)
{
	m_femGrid->setStressAlpha(alpha);
	this->redraw();
}

void CPaintView::setStressSize(double size)
{
	m_femGrid->setStressSize(size);
	this->redraw();
}

void CPaintView::setStressWidth(double width)
{
	m_femGrid->setStressWidth(width);
	this->redraw();
}

void CPaintView::setStressTreshold(double lower, double upper)
{
	m_femGrid->setStressTreshold(lower, upper);
	this->redraw();
}

void CPaintView::setLockScaling(bool flag)
{
	m_femGrid->setLockScale(flag);
}

void CPaintView::setMaxIntensity(float intensity)
{
	m_maxIntensity = intensity;
	m_femGrid->setMaxIntensity(intensity);
	this->redraw();
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
		this->calcRigidReactions();
	}
	else
		m_femGrid->updatePixelArea();
	
	this->updateModel();
	this->redraw();
}

bool CPaintView::getCalcCG()
{
	return m_calcCG;
}

void CPaintView::showAbout()
{
#ifdef WIN32
	ShellExecute(0, "open", "http://www.byggmek.lth.se/resources/forcepad/forcepad.htm", NULL, NULL, SW_SHOWNORMAL);
#endif
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
		m_femGrid->clearResults();
		this->invalidate();
		this->redraw();

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
		m_femGrid->clearResults();
		this->invalidate();
		this->redraw();

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
	this->redraw();
}

bool CPaintView::getDrawStress()
{
	return m_femGrid->getDrawStress();
}

void CPaintView::setDrawDisplacements(bool flag)
{
	m_femGrid->setDrawDisplacements(flag);
	m_femGrid->setUndeformedGrid(flag);
	this->redraw();
}

bool CPaintView::getDrawDisplacements()
{
	return m_femGrid->getDrawDisplacements();
}

void CPaintView::setStressType(CFemGrid2::TStressType stressType)
{
	// Make sure a colormap is loaded

	if ((stressType == CFemGrid2::ST_MISES)||(stressType == CFemGrid2::ST_MISES_SMOOTH))
		this->setColorMap(1);

	m_femGrid->setStressType(stressType);
	this->redraw();
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
		int pos = applicationExeLocation.find_last_of("\\");
		string applicationDir = applicationExeLocation.substr(0,pos);

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
		
		m_femGrid->getColorMap()->open(filename.c_str());
		this->redraw();
	}
}

void CPaintView::setInvertColorMap(bool flag)
{
	m_femGrid->getColorMap()->setInvert(flag);
	this->redraw();
}

void CPaintView::setUpperMisesTreshold(double upper)
{
	m_femGrid->setUpperMisesTreshold(upper);
	this->redraw();
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
	this->redraw();
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

void CPaintView::setModelChangedEvent(CPVModelChangedEvent* eventMethod)
{
	m_modelChangedEvent = eventMethod;
}

void CPaintView::setRulerChangedEvent(CPVRulerChangedEvent* eventMethod)
{
	m_rulerChangedEvent = eventMethod;
}
