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

#ifndef _CFemGrid_h_
#define _CFemGrid_h_

#include "ImageGrid2.h"
#include "Color.h"
#include "Force.h"
#include "Constraint.h"
#include "ForceSelection.h"
#include "ConstraintSelection.h"
#include "ColorMap.h"

typedef deque<CForcePtr> CForceQue;
typedef deque<CForcePtr>::iterator CForceQueIter;

typedef deque<CConstraintPtr> CConstraintQue;
typedef deque<CConstraintPtr>::iterator CConstraintQueIter;

typedef vector< vector<int> > CElementList;
typedef vector< vector<int> >::iterator CElementListIter;

#define ED_LEFT_RIGHT 0
#define ED_BOTTOM_TOP 1

IvfSmartPointer(CFemGrid2);

class CFemGrid2 : public CImageGrid2 {
public:
	enum TStressMode {
		SM_ALL,
		SM_POSITIVE,
		SM_NEGATIVE
	};
	enum TStressType {
		ST_PRINCIPAL,
		ST_MISES, 
		ST_MISES_SMOOTH
	};
private:
	vector<CForceQue> m_pointForces;
	vector<CConstraintQue> m_pointConstraints;

	bool m_showGrid;
	bool m_stressDrawn;
	bool m_drawStressOnce;
	bool m_averageStress;

	bool m_drawStress;
	bool m_drawDisplacements;
	bool m_drawForcesAndConstraints;
	bool m_drawStructure;
	
	CColorPtr m_forceColor;
	CColorPtr m_constraintColor;

	CColorMapPtr m_colorMap;

	int*** m_dofs;

	int m_dofRows;
	int m_dofCols;

	int m_bandwidth;

	int m_currentPointForceRow;
	CForceQueIter m_currentPointForceIter;

	int m_currentPointConstraintRow;
	CConstraintQueIter m_currentPointConstraintIter;

	double m_elementTreshold;

	double* m_displacements;
	int m_displacementSize;
	double m_maxDisplacement;
	double m_displacementScale;
	double m_maxNodeValue;
	double m_maxStressValue;
	double m_maxPosStressValue;
	double m_maxNegStressValue;
	double m_maxMisesStressValue;
	double m_maxIntensity;
	double m_upperMisesTreshold;

	double m_stressAlpha;
	double m_stressSize;
	double m_stressWidth;
	double m_lowerStressTreshold;
	double m_upperStressTreshold;
	TStressMode m_stressMode;
	TStressType m_stressType;
	bool m_invertColorMap;
	bool m_lockScaleFactor;
	bool m_dimmedConstraints;
	bool m_undeformedGrid;
	bool m_drawDensity;

	bool m_useWeight;

	double m_maxStressFactor;
	double m_maxPosStressFactor;
	double m_maxNegStressFactor;

	int m_stressStep;

	double*** m_results;
	double** m_nodeResults;
	int** m_nodeUsage;

	double m_arrowSize;

	vector<int> m_xpoints;
	vector<int> m_ypoints;

	double m_pixelArea;

    unsigned char m_colorMapTex1D[384];

	void calcMaxMin(int* arr, int size, int &maxVal, int &minVal );

	void drawGrid();
	void drawUndeformedGrid();
	void drawStructure();
	void drawConstraints();
	void drawForces();
	void drawStress();
	void drawMisesStress();
	void drawMisesStressSmooth();
	void drawDebugPoints();
	void drawStressArrow(double x, double y, const double* values);
	void drawDoubleDofs();
	void drawGridPoints();
	void drawDensity();
public:
	CFemGrid2();
	virtual ~CFemGrid2();

	int getElementCount();
	void updatePixelArea();
	int getStressStep();
	void setStressStep(int step);
	bool getAverageStress();
	void setAverageStress(bool flag);
	int getConstraintsSize();
	CForceSelectionPtr getForces();
	CConstraintSelectionPtr getConstraints();
	double getPixelArea();
	void calcCenterOfStiffness(int &cgx, int &cgy);
	void removePointConstraint(CConstraint* constraint);
	void removePointForce(CForce* force);
	void moveForce(CForce* force, int x, int y);
	void getConstraints(int x1, int y1, int x2, int y2, CConstraintSelection* selection);
	void getForces(int x1, int y1, int x2, int y2, CForceSelection* selection);
	CForce* getNearestForce(int x, int y);
	void calcCenterOfGravity(int &x, int &y);
	void setMaxNegStressValue(double value);
	void setMaxPosStressValue(double value);
	double getMaxNegStressValue();
	double getMaxPosStressValue();
	void setStressMode(TStressMode mode);
	void readFromStream(istream &in);
	void saveToStream(ostream &out);
	void setDrawStressOnce(bool flag);
	void resetStressDrawing();
	void getElements(int x1, int y1, int x2, int y2, CElementList& list);

	// Functions for hinge constraints

	void getElementPos(double x, double y, int& r, int& c);
	double getStiffness(double x, double y);
	void setStiffness(double x, double y, double value);
	void setStiffness(double x, double y, double value, int offsetElements, bool special);
	void setStiffnessLine(double x1, double y1, double x2, double y2, double value, double width);

	IvfClassInfo("CFemGrid2",CImageGrid2);

	int enumerateDofs(int direction);
	void resetDofs();
	void clearForces();
	void clearConstraints();
	void deactivateDofs();
	void clearResults();
	void initResults();
	void initDofs();
	void clearDofs();

	void zeroNodeResults();
	void averageNodeResults();

	void addPoint(int x, int y);
	void clearPoints();

	void addForce(CForce* force);
	void addConstraint(CConstraint* constraint);

	void setShowGrid(bool flag);
	bool getShowGrid();

	void setShowReactionForces(bool flag);

	bool getElement(int row, int col, double &value, double* ex, double* ey, int* topo);
	int getBandwidth();
	void getNearestDofs(int x, int y, int* dofs);

	CForce* getNextPointLoad();
	CForce* getFirstPointLoad();
	void erasePointLoad(int x, int y, int brushSize);

	CConstraint* getFirstPointConstraint();
	CConstraint* getNextPointConstraint();
	void erasePointConstraint(int x, int y, int brushSize);
	int getPointConstraintsSize();

	void setDisplacementSize(int size);
	int getDisplacementSize();
	void setDisplacement(int dof, double value);
	double getDisplacement(int dof);
	void clearDisplacements();
	void setDisplacementScale(double scaleFactor);

	void setResult(int i, int j, const double* values);
	void setResult(int i, int j, int index, double value);
	void getResult(int i, int j, double* values);

	void setNodeResult(int i, int j, const double value);
	void addNodeResult(int i, int j, const double value);
	double getResult(int i, int j);

	double getMaxNodeValue();
	void setMaxNodeValue(double value);
	double getMaxStressValue();
	void setMaxStressValue(double value);

	void setMaxMisesStressValue(double value);

	void setLockScale(bool flag);
	void setStressWidth(double width);
	void setStressSize(double size);
	void setStressAlpha(double alpha);
	void setStressTreshold(double lower, double upper);

	void setUpperMisesTreshold(double upper);

	void setDrawStress(bool flag);
	bool getDrawStress();

	void setDrawDisplacements(bool flag);
	bool getDrawDisplacements();

	void setDrawForcesAndConstraints(bool flag);
	bool getDrawForcesAndConstraints();

	void setDimmedConstraints(bool flag);
	bool getDimmedConstraints();

	void setUndeformedGrid(bool flag);
	bool getUndeformedGrid();

	void setDrawStructure(bool flag);
	bool getDrawStructure();

	void setMaxIntensity(double intensity);

	void setColorMap(CColorMap* colorMap);
	CColorMap* getColorMap();

	void setStressType(TStressType stressType);

	void setElementTreshold(double value);
	double getElementTreshold();

	int getPointLoadSize();

	void setGridSpacing(int spacing);
	int getGridSpacing();

	void assignNonElements(Matrix& M, double value);

	void setShowDensity(bool flag);
	bool getShowDensity();

	void setUseWeight(bool flag);
	bool getUseWeight();

	void updateColorMapTexture();

	virtual void setImage(CImage* image);
	virtual void initGrid();
	virtual void doGeometry();
};

#endif 
