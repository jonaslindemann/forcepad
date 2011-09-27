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
#include "ImageGrid2.h"
#include "FemGrid2.h"
#include "Clipboard.h"
#include "ForcePadClipboard.h"
#include "ScreenImage.h"
#include "Ruler.h"

#include "FemGridSolver2.h"
#include "CGIndicator.h"
#include "Fl_Cursor_Shape.H"

class CPVModeChangeEvent;
class CPVViewModeChangeEvent;
class CPVViewModeErrorEvent;
class CPVModelChangedEvent;
class CPVRulerChangedEvent;
class CPVVisualisationModeChangedEvent;
class CPVModelLoadedEvent;
class CPVNewModelEvent;

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
		EM_RESULT,
		EM_RULER
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

	enum TVisualisationMode {
		VM_PRINCIPAL_STRESS,
		VM_MISES_STRESS,
		VM_DISPLACEMENTS,
		VM_STRUCTURE
	};
private:
	/*
	 *    Command line arguments
	 */

	int m_argc;
	char** m_argv;

	TEditMode m_editMode;
	TViewMode m_viewMode;
	TVisualisationMode m_visualisationMode;

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

	bool m_runOnce;

	/*
	 * Optimisation constraint definition
	 */

	bool m_optLayerActive;


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

#ifndef __APPLE__
	Fl_Cursor_Shape* m_cursors[20];
#endif

	float m_brushColor[3];
	float m_optConstraintColor[3];
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
	bool m_moveLoad;

	double m_optVolfrac;
	double m_optRmin;
	double m_optMinChange;
	int m_optMaxLoops;
	CFemGridSolver2::TFilterType m_optFilterType;

	CFemGridSolver2Ptr m_solver;

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
	CImagePtr m_optConstraintImage;
	CImagePtr m_buffer;
	CScreenImagePtr m_screenImage;

	CFemGrid2Ptr m_femGrid;

	CRulerPtr m_ruler;

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

	void* m_mainFrame;

	/*
	 *    Misc
	 */

	bool m_resized;
	bool m_drawImage;

	int m_lastSize[2];

	bool m_zoomResults;
	double m_zoomFactor;
	int m_zoomPos[2];
	int m_zoomStart[2];

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
	CPVRulerChangedEvent* m_rulerChangedEvent;
	CPVVisualisationModeChangedEvent* m_visualisationModeChangedEvent;
	CPVModelLoadedEvent* m_modelLoadedEvent;
	CPVNewModelEvent* m_newModelEvent;

	CGSStatusMessageEvent* m_statusMessageEvent;
	CGSLogMessageEvent* m_logMessageEvent;
	CGSContinueCalcEvent* m_continueCalcEvent;

	/*
	 *    Private event handlers
	 */

	/** Handles mouse wheel.  */
	void onMouseWheel(int dx, int dy);

	/** Handles mouse dragging. (Mouse move with button down) */
	void onDrag(int x, int y);

	/** Handles mouse click. */
	void onPush(int x, int y);

	/** Handles mouse up. */
	void onRelease(int x, int y);

	/** Handles passive mouse movement */
	void onMove(int x, int y);

	/** Handles passive mouse movement */
	void onResize(int w, int h);

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
	bool executeOpt();

	void undoToDrawing();

	void newModel();
	void saveModel();
	void saveModelAs();
	void openImage();
	void openModel();
	void openModel(const std::string filename);
	void expandImageToWindow();

	void showAbout();
	void showHelp();
	void updateModel();
	void pasteFromWindows();
	void copyToWindows();
	void transferViewToImage();
	void undo();
	void cut();
	void copy();
	void lockScaleFactor();
	void unlockScaleFactor();
	void applyElementScale();
	void setRulerLength(double actualLength);
	double getRulerLength();
	void setOptLayer(bool active);

	void zoomIn();
	void zoomOut();

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

	// ExecutablePath

	const std::string getApplicationPath();

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

	void setStressMode(CFemGrid2::TStressMode mode);
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

	void setZoomResults(bool flag);
	bool getZoomResults();

	void setDrawDisplacements(bool flag);
	bool getDrawDisplacements();

	void setDrawForcesAndConstraints(bool flag);

	void setStressType(CFemGrid2::TStressType stressType);
	void setColorMap(int index);
	void setInvertColorMap(bool flag);

	void setUpperMisesTreshold(double upper);

	void setVisualisationMode(TVisualisationMode mode);
	TVisualisationMode getVisualisationMode();

	// Calculation settings

	void setUseWeight(bool flag);
	bool getUseWeight();

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

	void setMoveLoad(bool flag);
	bool getMoveLoad();

	// Optmisation properties

	void setOptVolumeFraction(double fraction);
	double getOptVolumeFraction();

	void setOptRmin(double rmin);
	double getOptRmin();

	void setOptMinChange(double minChange);
	double getOptMinChange();

	void setOptMaxLoops(int loops);
	int getOptMaxLoops();

	void setOptFilterType(CFemGridSolver2::TFilterType filterType);
	CFemGridSolver2::TFilterType getOptFilterType();

	// User interface properties

	void setUiLineThickness(double thickness);
	double getUiLineThickness();

	void setUiSymbolLength(double length);
	double getUiSymbolLength();

	// Events

	void setModeChangeEvent(CPVModeChangeEvent* eventMethod);
	void setViewModeChangeEvent(CPVViewModeChangeEvent* eventMethod);
	void setViewModeErrorEvent(CPVViewModeErrorEvent* eventMethod);
	void setStatusMessageEvent(CGSStatusMessageEvent* eventMethod);
	void setLogMessageEvent(CGSLogMessageEvent* eventMethod);
	void setContinueCalcEvent(CGSContinueCalcEvent* eventMethod);
	void setModelChangedEvent(CPVModelChangedEvent* eventMethod);
	void setRulerChangedEvent(CPVRulerChangedEvent* eventMethod);
	void setVisualisationModeChangedEvent(CPVVisualisationModeChangedEvent* eventMethod);
	void setModelLoadedEvent(CPVModelLoadedEvent* eventMethod);
	void setNewModelEvent(CPVNewModelEvent* eventMethod);
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

class CPVRulerChangedEvent {
public:
	virtual ~CPVRulerChangedEvent() {};
	virtual void onRulerChanged(CRuler* ruler) = 0;
};

class CPVVisualisationModeChangedEvent {
public:
	virtual ~CPVVisualisationModeChangedEvent() {};
	virtual void onVisualisationModeChanged(CPaintView::TVisualisationMode oldMode, CPaintView::TVisualisationMode newMode) = 0;
};

class CPVModelLoadedEvent {
public:
	virtual ~CPVModelLoadedEvent() {};
	virtual void onModelLoaded() = 0;
};

class CPVNewModelEvent {
public:
	virtual ~CPVNewModelEvent() {};
	virtual void onNewModel() = 0;
};

#endif 
