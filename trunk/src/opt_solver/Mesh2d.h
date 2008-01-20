#ifndef _CMesh2d_H_
#define _CMesh2d_H_

#include "RFemBase.h"
#include "Element.h"
#include "Node3d.h"
#include "Force.h"

SmartPointer(CMesh2d);

typedef std::vector<CNode3dPtr> TNodeList;

typedef std::map<CConstraintPtr, CNode3dPtr> TConstraintMap;
typedef std::map<CConstraintPtr, CNode3dPtr>::iterator TConstraintMapIterator;
typedef std::vector<CConstraintPtr> TConstraintList;

typedef std::map<CForcePtr, CNode3dPtr> TForceMap;
typedef std::map<CForcePtr, CNode3dPtr>::iterator TForceMapIterator;
typedef std::vector<CForcePtr> TForceList;

class CMesh2d : public CBase {
private:
	unsigned int m_rows;
	unsigned int m_cols;
	double m_width;
	double m_height;

	unsigned int m_bandWidth;
	unsigned int m_nodeDofs;

	unsigned int m_totalDofs;

	TNodeList m_nodeList;
	TNodeList m_tempNodeList;
	std::vector<std::vector<CNode3dPtr>> m_nodeArray;

	TConstraintMap m_constraintMap;
	TConstraintList m_constraintList;

	TForceMap m_forceMap;
	TForceList m_forceList;

	Matrix m_relativeStiffness;

	SymmetricBandMatrix m_K;
	ColumnVector m_f;

	void initialize();
public:
	CMesh2d(unsigned int rows, unsigned int cols, double width, double height, unsigned int nodeDofs);
	virtual ~CMesh2d ();

	void setMesh(int rows, int cols);
	void setSize(double width, double height);
	double getWidth();
	double getHeight();
	double getElementWidth();
	double getElementHeight();

	CNode3d* getNode(unsigned int row, unsigned int col);
	TNodeList& getNodeList(unsigned int row, unsigned int col);
	ReturnMatrix getTopo(unsigned int row, unsigned int col);

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

	virtual ReturnMatrix onCreateElementMatrix(TNodeList& nodes, double relativeStiffness) = 0;
	virtual int onGetElementMatrixSize() = 0;

	void assembleGlobalStiffnessMatrix();
	void assembleForceVector();

	void print(std::ostream& out);

	ReturnMatrix getGlobalStiffnessMatrix();
	ReturnMatrix getForceVector();

	ClassInfo("CMesh2d", CBase);
};

#endif
