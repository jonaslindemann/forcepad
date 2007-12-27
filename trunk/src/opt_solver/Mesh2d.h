#ifndef _CMesh2d_H_
#define _CMesh2d_H_

#include "RFemBase.h"
#include "Element.h"
#include "Node3d.h"
#include "Constraint.h"
#include "Force.h"

SmartPointer(CMesh2d);

typedef std::vector<CNode3dPtr> TNodeList;

typedef std::map<unsigned int,double> TConstraintMap;
typedef std::map<unsigned int,double>::const_iterator TConstraintMapIterator;
typedef std::map<unsigned int,double> TForceMap;
typedef std::map<unsigned int,double>::const_iterator TForceMapIterator;

class CCreateElementMatrixEvent {
public:
	virtual ReturnMatrix onCreateElementMatrix(int row, int col, TNodeList& nodes) = 0;
};

class CMesh2d : public CBase {
private:
	unsigned int m_rows;
	unsigned int m_cols;
	double m_width;
	double m_height;

	unsigned int m_bandwidth;
	unsigned int m_nodeDofs;

	CCreateElementMatrixEvent* m_createElementMatrixEvent;

	std::vector<CNode3dPtr> m_nodeList;
	std::vector<std::vector<CNode3dPtr>> m_nodeArray;

	TConstraintMap m_constraintMap;
	TForceMap m_forceMap;
	std::vector<CForcePtr> m_forceList;

	Matrix m_relativeStiffness;

	SymmetricBandMatrix m_K;

	void initialize();
public:
	CMesh2d(unsigned int rows, unsigned int cols, double width, double height, unsigned int nodeDofs);
	virtual ~CMesh2d ();

	void setCreateElementMatrixEvent(CCreateElementMatrixEvent* event);

	void setMesh(int rows, int cols);
	void setSize(double width, double height);
	double getWidth();
	double getHeight();
	double getElementWidth();
	double getElementHeight();

	unsigned int getRows();
	unsigned int getCols();
	unsigned int getTotalDofs();

	int enumerateDofs(int start);
	int enumerateDofsHorisontal(int start);
	int enumerateDofsVertical(int start);

	void constrainNode(unsigned int row, unsigned int col, unsigned int dof, double value);
	void constrainNodeRow(unsigned int row, unsigned int dof, double value);
	void constrainNodeCol(unsigned int col, unsigned int dof, double value);
	void constrainHorisontal(unsigned int row, unsigned int start, unsigned int end, unsigned int dof, double value);
	void constrainVertical(unsigned int col, unsigned int start, unsigned int end, unsigned int dof, double value);

	void applyForceNode(unsigned int row, unsigned int col, double fx, double fy);
	void applyForceRow(unsigned int row, double fx, double fy);
	void applyForceCol(unsigned int col, double fx, double fy);
	void applyForceHorisontal(unsigned int row, unsigned int start, unsigned int end, double fx, double fy);
	void applyForceVertical(unsigned int col, unsigned int start, unsigned int end, double fx, double fy);

	void assembleGlobalStiffnessMatrix();

	void print(std::ostream& out);

	TConstraintMap& getConstraints();
	TForceMap& getForces();

	ClassInfo("CMesh2d", CBase);
};

#endif
