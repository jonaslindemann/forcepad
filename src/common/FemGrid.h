//
// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2007 Division of Structural Mechanics, Lund University
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

#ifndef _FemGrid_h_
#define _FemGrid_h_

#include "ImageGrid.h"
#include "Color.h"
#include "Force.h"
#include "Constraint.h"
#include "ForceSelection.h"
#include "ConstraintSelection.h"
#include "ColorMap.h"

namespace fp {

typedef std::deque<ForcePtr> CForceQue;
typedef std::deque<ForcePtr>::iterator CForceQueIter;

typedef std::deque<ConstraintPtr> CConstraintQue;
typedef std::deque<ConstraintPtr>::iterator CConstraintQueIter;

typedef std::vector< std::vector<int> > CElementList;
typedef std::vector< std::vector<int> >::iterator CElementListIter;

#define ED_LEFT_RIGHT 0
#define ED_BOTTOM_TOP 1

IvfSmartPointer(FemGrid);

class FemGrid : public ImageGrid {
public:
	enum TStressMode {
		SM_ALL,
		SM_POSITIVE,
		SM_NEGATIVE
	};
	enum TStressType {
		ST_PRINCIPAL,
		ST_MISES
	};
private:
	std::vector<CForceQue> m_pointForces;
	std::vector<CConstraintQue> m_pointConstraints;

	bool m_showGrid;
	bool m_stressDrawn;
	bool m_drawStressOnce;
	bool m_averageStress;

	bool m_drawStress;
	bool m_drawDisplacements;
	bool m_drawForcesAndConstraints;
	
	ivf2d::ColorPtr m_forceColor;
	ivf2d::ColorPtr m_constraintColor;

	ColorMapPtr m_colorMap;

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

	double m_maxStressFactor;
	double m_maxPosStressFactor;
	double m_maxNegStressFactor;

	int m_stressStep;

	double*** m_results;

	double m_arrowSize;

	std::vector<int> m_xpoints;
	std::vector<int> m_ypoints;

	double m_pixelArea;

	void calcMaxMin(int* arr, int size, int &maxVal, int &minVal );

	void drawGrid();
	void drawGrid2();
	void drawUndeformedGrid();
	void drawConstraints();
	void drawForces();
	void drawStress();
	void drawMisesStress();
	void drawDebugPoints();
	void drawStressArrow(double x, double y, const double* values);
	void drawDoubleDofs();
	void drawGridPoints();
public:
	FemGrid();
	virtual ~FemGrid();

	static FemGridPtr create() { return std::make_shared<FemGrid>(); }

	int getElementCount();
	void updatePixelArea();
	int getStressStep();
	void setStressStep(int step);
	bool getAverageStress();
	void setAverageStress(bool flag);
	int getConstraintsSize();
	ForceSelectionPtr getForces();
	ConstraintSelectionPtr getConstraints();
	double getPixelArea();
	void calcCenterOfStiffness(int &cgx, int &cgy);
	void removePointConstraint(Constraint* constraint);
	void removePointForce(Force* force);
	void moveForce(Force* force, int x, int y);
	void getConstraints(int x1, int y1, int x2, int y2, ConstraintSelection* selection);
	void getForces(int x1, int y1, int x2, int y2, ForceSelection* selection);
	Force* getNearestForce(int x, int y);
	void calcCenterOfGravity(int &x, int &y);
	void setMaxNegStressValue(double value);
	void setMaxPosStressValue(double value);
	double getMaxNegStressValue();
	double getMaxPosStressValue();
	void setStressMode(TStressMode mode);
	void readFromStream(std::istream &in);
	void saveToStream(std::ostream &out);
	void setDrawStressOnce(bool flag);
	void resetStressDrawing();
	void getElements(int x1, int y1, int x2, int y2, CElementList& list);

	// Functions for hinge constraints

	void getElementPos(double x, double y, int& r, int& c);
	double getStiffness(double x, double y);
	void setStiffness(double x, double y, double value);
	void setStiffness(double x, double y, double value, int offsetElements, bool special);
	void setStiffnessLine(double x1, double y1, double x2, double y2, double value, double width);

	IvfClassInfo("FemGrid",ImageGrid);

	int enumerateDofs(int direction);
	void resetDofs();
	void clearForces();
	void clearConstraints();
	void deactivateDofs();
	void clearResults();
	void initResults();
	void initDofs();
	void clearDofs();

	void addPoint(int x, int y);
	void clearPoints();

	void addForce(ForcePtr force);
	void addConstraint(ConstraintPtr constraint);

	void setShowGrid(bool flag);
	bool getShowGrid();

	void setShowReactionForces(bool flag);

	bool getElement(int row, int col, int element, double &value, double* ex, double* ey, int* topo);
	int getBandwidth();
	void getNearestDofs(int x, int y, int* dofs);

	Force* getNextPointLoad();
	Force* getFirstPointLoad();
	void erasePointLoad(int x, int y, int brushSize);

	Constraint* getFirstPointConstraint();
	Constraint* getNextPointConstraint();
	void erasePointConstraint(int x, int y, int brushSize);
	int getPointConstraintsSize();

	void setDisplacementSize(int size);
	int getDisplacementSize();
	void setDisplacement(int dof, double value);
	double getDisplacement(int dof);
	void clearDisplacements();
	void setDisplacementScale(double scaleFactor);

	void setResult(int i, int j, int element, const double* values);
	void setResult(int i, int j, int element, int index, double value);
	void getResult(int i, int j, int element, double* values);

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

	void setMaxIntensity(double intensity);

	void setColorMap(ColorMapPtr colorMap);
	ColorMap* getColorMap();

	void setStressType(TStressType stressType);

	void setElementTreshold(double value);
	double getElementTreshold();

	int getPointLoadSize();

	void setGridSpacing(int spacing);
	int getGridSpacing();

	virtual void setImage(ivf2d::ImagePtr image);
	virtual void initGrid();
	virtual void doGeometry();
};


} // namespace fp

#endif 
