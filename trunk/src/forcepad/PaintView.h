//
// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2007 Division of Structural Mecahnics, Lund University
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

#ifndef _CPaintView_h_
#define _CPaintView_h_

#include "forcepad_config.h"

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include <vector>

#include "Rectangle.h"
#include "Ellipse.h"
#include "Line.h"
#include "SgiImage.h"
#include "ImageGrid.h"
#include "FemGrid.h"
#include "Clipboard.h"
#include "ForcePadClipboard.h"
#include "ScreenImage.h"

#include "BrushProps.h"
#include "DrawingProps.h"
#include "DrawingTools.h"
#include "StiffnessProps.h"
#include "BcTypes.h"
#include "CalcProps.h"
#include "ImportProps.h"
#include "FemGridSolver.h"

#include "CGIndicator.h"

#include "Fl_Cursor_Shape.H"


#ifdef FORCEPAD_RIGID
#include "ReactionForce.h"
#endif

class CPVModeChangeEvent;
class CPVViewModeChangeEvent;
class CPVViewModeErrorEvent;

class CPaintView : public Fl_Gl_Window {
public:
	/*
	 *    Edit modes
	 */
	enum TEditMode { 
		EM_BRUSH, 
		EM_DIRECT_BRUSH,
		EM_ERASE, 
		EM_DIRECT_ERASE,
		EM_FORCE, 
		EM_CONSTRAINT,
		EM_CONSTRAINT_VECTOR,
		EM_CONSTRAINT_HINGE,
		EM_DYNAMIC_FORCE_UPDATE,
		EM_ROTATE_CONSTRAINT_VECTOR,
		EM_LINE,
		EM_RECTANGLE,
		EM_ELLIPSE,
		EM_FLOODFILL,
		EM_ERASE_CONSTRAINTS_FORCES,
		EM_SELECT_BOX,
		EM_PASTE,
		EM_RESULT
	};

	enum TImportMode {
		IM_NEW_MODEL,
		IM_PASTE
	};

	enum TViewMode {
		VM_SKETCH,
		VM_PHYSICS, 
		VM_ACTION
	};
private:
	/*
	 *    Command line arguments
	 */

	int m_argc;
	char** m_argv;

	TEditMode m_editMode;
	TViewMode m_viewMode;

	/*
	 *    Mouse state variables
	 */

	bool m_leftMouseDown;
	int m_current[2];
	int m_start[2];
	int m_oldPos[2];

	int m_selectionStart[2];
	int m_selectionEnd[2];

	int m_undoStart[2];
	int m_undoEnd[2];

	/*
	 *    Drawing surface and computational grid
	 */

	int m_drawingOffsetX;
	int m_drawingOffsetY;
	bool m_dirty;
	bool m_showMesh;
	float m_maxIntensity;

	bool m_snapToGrid;
	int m_gridSpacing;

	char* m_modelName;

	bool m_useWeight;

	int m_cg[2];

	/*
	 *    Brush management
	 */

	std::vector<CSgiImagePtr> m_brushes;
	std::vector<CSgiImagePtr> m_invertedBrushes;
	
	CSgiImagePtr m_currentBrush;
	CSgiImagePtr m_currentInvertedBrush;

	Fl_Cursor_Shape* m_cursors[20];

	float m_brushColor[3];
	float m_backColor[3];
	int m_brushScale;
	int m_blendFactor;
	int m_currentBrushIdx;
	bool m_useBlendingExtension;
	bool m_lockDrawing;

	TImportMode m_importMode;
	
	bool m_danglingRelease;

	/*
	 *    Calculation settings
	 */

	double m_forceMagnitude;
	double m_weight;
	bool m_outputMatlab;
	double m_stiffnessScalefactor;
	double m_elasticModulus;
	double m_youngsModulus;
	double m_thickness;
	double m_constraintStiffnessScale;

	CFemGridSolverPtr m_solver;

	/*
	 *    Drawing tools
	 */

	CRectanglePtr m_rectangle;
	CEllipsePtr m_ellipse;
	CLinePtr m_line;
	CForcePtr m_newForce;
	CConstraintPtr m_newConstraint;
	CConstraint::TConstraintType m_constraintType;

	CRectanglePtr m_selectionBox;
	CForcePadClipboardPtr m_clipboard;
	CClipboardPtr m_undoClipboard;

	CImagePtr m_drawing;
	CFemGridPtr m_femGrid;
	CScreenImagePtr m_screenImage;
	CImagePtr m_buffer;

	/*
	 *    Dynamic force update variables
	 */

	CForcePtr m_selectedForce;

	/*
	 *    Rigid body tools
	 */

	CCGIndicatorPtr m_cgIndicator;
	bool m_calcCG;
	double m_relativeForceSize;
	bool m_checkOpenGL;

#ifdef FORCEPAD_RIGID
	bool m_validReactions;
	CReactionForce* m_reactionForces[3];
#endif

	/*
	 *    Dialogs
	 */

	bool m_dialogsVisible;
	CBrushProps* m_brushPropsDlg;
	CDrawingProps* m_drawingPropsDlg;
	CDrawingTools* m_drawingToolsDlg;
	CStiffnessProps* m_stiffnessPropsDlg;
	CBcTypes* m_bcTypesDlg;
	CCalcProps* m_calcPropsDlg;
	CImportProps* m_importPropsDlg;

	void* m_mainFrame;

	/*
	 *    Misc
	 */

	bool m_resized;
	bool m_drawImage;

	/*
	 *    Private methods
	 */

	/*
	 *    Events
	 */

	CPVModeChangeEvent* m_modeChangeEvent;
	CPVViewModeChangeEvent* m_viewModeChangeEvent;
	CPVViewModeErrorEvent* m_viewModeErrorEvent;

	CGSStatusMessageEvent* m_statusMessageEvent;
	CGSLogMessageEvent* m_logMessageEvent;

	/** Loads brushes from /brushes */
	void loadBrushes();

	/** Clean up brush lists */
	void deleteBrushes();

	/*
	 *    Private event handlers
	 */

	/** Handles mouse dragging. (Mouse move with button down) */
	void onDrag(int x, int y);

	/** Handles mouse click. */
	void onPush(int x, int y);

	/** Handles mouse up. */
	void onRelease(int x, int y);

	/** Handles passive mouse movement */
	void onMove(int x, int y);

	/** Handles OpenGL drawing */
	void onDraw();

	void onClear();
	void onInitContext();

	void checkOpenGLVersion();

public:
	void undoToDrawing();

	void showBcTypes(int x, int y);
	void showStiffnessProps(int x, int y);
	void showDrawingTools(int x, int y);
	void showDrawingProps(int x, int y);
	void showCalcProps(int x, int y);
	void showBrushProps(int x, int y);
	void showImportProps(int x, int y);
	void hideDialogs();

	double getStiffness();
	void setStiffness(double stiffness);
	int getLineWidth();
	int getCurrentBrushIdx();
	int getBlendFactor();
	void clearResults();
	void openModel();
	const char* getModelName();
	void setModelName(const char* name);
	/*
	 *   Constructors 
	 */

	/** 
	 * PaintView class constructor.
	 *
	 * This is constructor corresponds to a standard FLTK
	 * window constructor.
	 */
	CPaintView(int x,int y,int w,int h,const char *l=0);

	/** ForcePadView class destructor. */
	virtual ~CPaintView();

	/*
	 *    Virtual overrides from FLTK 
	 */

	/** FLTK draw() method virtual override. */
    void draw();    

	/** FLTK handle() method virtual override. */
	int handle(int event);

	/*
	 *    Methods
	 */

	void clearMesh();
	void clearImage();
	void executeCorba();
	bool execute();

	void setViewMode(TViewMode mode);

	void newModel();
	void saveModel();
	void openImage();

	void deleteCursors();
	void createCursors();
	void updateCursor();

	void showAbout();
	void enableDrawing();
	void disableDrawing();
	void updateModel();
	void calcRigidReactions();
	void pasteFromWindows();
	void copyToWindows();
	void updateUndo();
	void resetUndoArea();
	void updateUndoArea(int x, int y, int brushSize);
	void undo();
	void cut();
	void copy();
	void updateSelectionBox();
	void lockScaleFactor();
	void unlockScaleFactor();

	/*
	 *    Properties
	 */

	void setGridStride(int stride);
	int getGridStride();
	void setCurrentBrush(int idx);
	void setBlendFactor(int factor);
	void getForegroundColor(float &red, float &green, float &blue);
	void setBackgroundColor(float red, float green, float blue);
	void setForegroundColor(float red, float green, float blue);
	void setBrushMagnification(int factor);
	void setMaxIntensity(float intensity);
	void setLockScaling(bool flag);
	void setStressTreshold(double lower, double upper);
	void setStressWidth(double width);
	void setStressSize(double size);
	void setStressAlpha(double alpha);
	void setDisplacementScale(double value);
	void setLineWidth(int width);
	void setEditMode(TEditMode mode);
	TEditMode getEditMode();
	void setCommandLine(int argc, char** argv);
	void setConstraintType(CConstraint::TConstraintType constraintType);
	void setImportMode(TImportMode mode);
	TImportMode getImportMode();
	void setUseWeight(bool flag);
	void setStressStep(int step);
	int getStressStep();
	void setMainFrame(void* frame);
	void setRelativeForceSize(double size);
	void setStressMode(CFemGrid::TStressMode mode);
	void setCalcCG(bool flag);
	bool getCalcCG();

	void setDrawStress(bool flag);
	bool getDrawStress();

	void setDrawDisplacements(bool flag);
	bool getDrawDisplacements();

	void setStressType(CFemGrid::TStressType stressType);
	void setColorMap(int index);
	void setInvertColorMap(bool flag);

	void setUpperMisesTreshold(double upper);

	// Calculation settings

	void setElementTreshold(double value);
	double getElementTreshold();
	
	void setForceMagnitude(double value);
	double getForceMagnitude();
	
	void setOutputMatlab(bool flag);
	bool getOutputMatlab();

	void setStiffnessScaleFactor(double value);
	double getStiffnessScaleFactor();

	void setElasticModulus(double value);
	double getElasticModulus();

	void setYoungsModulus(double value);
	double getYoungsModulus();

	void setThickness(double value);
	double getThickness();

	void setConstraintStiffnessScale(double value);
	double getConstraintStiffnessScale();

	void setWeight(double value);
	double getWeight();

	void setSnapToGrid(bool flag);
	bool getSnapToGrid();

	// Events

	void setModeChangeEvent(CPVModeChangeEvent* eventMethod);
	void setViewModeChangeEvent(CPVViewModeChangeEvent* eventMethod);
	void setViewModeErrorEvent(CPVViewModeErrorEvent* eventMethod);
	void setStatusMessageEvent(CGSStatusMessageEvent* eventMethod);
	void setLogMessageEvent(CGSLogMessageEvent* eventMethod);
};

class CPVModeChangeEvent {
public:
	virtual ~CPVModeChangeEvent() {};
	virtual void onModeChange(CPaintView::TEditMode oldMode, CPaintView::TEditMode newMode) = 0;
};

class CPVViewModeChangeEvent {
public:
	virtual ~CPVViewModeChangeEvent() {};
	virtual void onViewModeChange(CPaintView::TViewMode oldMode, CPaintView::TViewMode newMode) = 0;
};

class CPVViewModeErrorEvent {
public:
	virtual ~CPVViewModeErrorEvent() {};
	virtual void onViewModeError(CPaintView::TViewMode oldMode, CPaintView::TViewMode newMode) = 0;
};

#endif 
