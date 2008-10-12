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

#include "FemGridSolver.h"

#include "CGIndicator.h"

#include "Fl_Cursor_Shape.H"


#ifdef FORCEPAD_RIGID
#include "ReactionForce.h"
#endif

class CPVModeChangeEvent;
class CPVViewModeChangeEvent;
class CPVViewModeErrorEvent;
class CPVModelChangedEvent;

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

	std::string m_modelName;

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
	CPVModelChangedEvent* m_modelChangedEvent;

	CGSStatusMessageEvent* m_statusMessageEvent;
	CGSLogMessageEvent* m_logMessageEvent;

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

	/** Clear frame buffer **/
	void onClear();

	/** Handles initialisation of OpenGL context **/
	void onInitContext();

	/** Check for OpenGL version **/
	void checkOpenGLVersion();

	void deleteCursors();
	void createCursors();
	void updateCursor();
	void resetUndoArea();
	void updateUndoArea(int x, int y, int brushSize);
	void updateUndo();

	/** Loads brushes from /brushes */
	void loadBrushes();

	/** Clean up brush lists */
	void deleteBrushes();

	void clearMesh();
	void clearResults();
	void updateSelectionBox();
	void enableDrawing();
	void disableDrawing();

public:
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

	void executeCorba();
	bool execute();

	void undoToDrawing();

	void newModel();
	void saveModel();
	void saveModelAs();
	void openImage();
	void openModel();

	void showAbout();
	void updateModel();
	void calcRigidReactions();
	void pasteFromWindows();
	void copyToWindows();
	void undo();
	void cut();
	void copy();
	void lockScaleFactor();
	void unlockScaleFactor();

	/*
	 *    Properties
	 */

	// Application modes

	void setViewMode(TViewMode mode);
	void setEditMode(TEditMode mode);
	TEditMode getEditMode();
	void setImportMode(TImportMode mode);
	TImportMode getImportMode();

	// Child/Parent relationship

	void setMainFrame(void* frame);
	void setCommandLine(int argc, char** argv);

	// Model related settings

	void setModelName(const std::string& modelName);
	//const char* getModelName();

	// Drawing settings

	int getCurrentBrushIdx();
	int getBlendFactor();
	void setStiffness(double stiffness);
	double getStiffness();
	void setCurrentBrush(int idx);
	void setBlendFactor(int factor);
	void getForegroundColor(float &red, float &green, float &blue);
	void setBackgroundColor(float red, float green, float blue);
	void setForegroundColor(float red, float green, float blue);
	void setBrushMagnification(int factor);
	void setConstraintType(CConstraint::TConstraintType constraintType);
	void setRelativeForceSize(double size);
	void setCalcCG(bool flag);
	bool getCalcCG();
	void setSnapToGrid(bool flag);
	bool getSnapToGrid();
	void setGridStride(int stride);
	int getGridStride();

	// Visualisation settings

	void setStressMode(CFemGrid::TStressMode mode);
	void setStressStep(int step);
	int getStressStep();
	void setMaxIntensity(float intensity);
	void setLockScaling(bool flag);
	void setStressTreshold(double lower, double upper);
	void setStressWidth(double width);
	void setStressSize(double size);
	void setStressAlpha(double alpha);
	void setDisplacementScale(double value);
	void setLineWidth(int width);
	int getLineWidth();

	void setDrawStress(bool flag);
	bool getDrawStress();

	void setDrawDisplacements(bool flag);
	bool getDrawDisplacements();

	void setStressType(CFemGrid::TStressType stressType);
	void setColorMap(int index);
	void setInvertColorMap(bool flag);

	void setUpperMisesTreshold(double upper);

	// Calculation settings

	void setUseWeight(bool flag);

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

	// Events

	void setModeChangeEvent(CPVModeChangeEvent* eventMethod);
	void setViewModeChangeEvent(CPVViewModeChangeEvent* eventMethod);
	void setViewModeErrorEvent(CPVViewModeErrorEvent* eventMethod);
	void setStatusMessageEvent(CGSStatusMessageEvent* eventMethod);
	void setLogMessageEvent(CGSLogMessageEvent* eventMethod);
	void setModelChangedEvent(CPVModelChangedEvent* eventMethod);
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

class CPVModelChangedEvent {
public:
	virtual ~CPVModelChangedEvent() {};
	virtual void onModelChanged(const std::string& newModelName) = 0;
};

#endif 