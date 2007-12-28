#include "Mesh2d.h"

// ------------------------------------------------------------
CMesh2d::CMesh2d(unsigned int rows, unsigned int cols, double width, double height, unsigned int nodeDofs)
		:CBase()
{
	m_rows = rows;
	m_cols = cols;
	m_width = width;
	m_height = height;
	m_createElementMatrixEvent = NULL;
	m_nodeDofs = nodeDofs;
	m_bandWidth = 0;
	this->initialize();
}

// ------------------------------------------------------------
CMesh2d::~CMesh2d()
{

}

// ------------------------------------------------------------
void CMesh2d::initialize()
{
	unsigned int i, j;

	m_relativeStiffness.ReSize(m_rows, m_cols);
	m_relativeStiffness = 1.0;

	double dx = m_width / m_cols;
	double dy = m_height / m_rows;

	double x = 0.0;
	double y = 0.0;

	// Create node array

	m_nodeArray.resize(m_rows+1);

	for (i=0; i<m_rows+1; i++)
	{
		for (j=0; j<m_cols+1; j++)
		{
			CNode3dPtr node = new CNode3d();
			node->setComponents(x, y);
			node->getDofs()->setSize(m_nodeDofs);
			x += dx;

			m_nodeArray[i].push_back(node);
			m_nodeList.push_back(node);
		}

		x = 0.0;

		y+=dy;
	}
}

// ------------------------------------------------------------
void CMesh2d::setMesh(int rows, int cols)
{
	m_rows = rows;
	m_cols = cols;
	this->initialize();
}

// ------------------------------------------------------------
void CMesh2d::setSize(double width, double height)
{
	m_width = width;
	m_height = height;
	this->initialize();
}

// ------------------------------------------------------------
//void CMesh2d::setCreateStiffnessMatrixEvent(CCreateStiffnessMatrixEvent* event)
//{
//	m_createStiffnessMatrixEvent = event;
//}

//
//   m_rows = 3, m_cols = 4
//
//    31,32  33,34  35,36  37,38  39,40
//   o------o------o------o------o
//   |      |      |      |      |
//   |21,22 |23,24 |25,26 |27,28 |29,30
//   o------o------o------o------o
//   |      |      |      |      |
//   |11,12 |13,14 |15,16 |17,18 |19,20
//   o------o------o------o------o
//   |      |      |      |      |
//   |1,2   |3,4   |5,6   |7,8   |9,10
//   o------o------o------o------o
//
//   m_bandwidthHorisontal = m_nodeDofs*(m_cols+1) = 2*(4+1) = 10;
//   m_bandwidthVertical   = m_nodeDofs*(m_rows+1) = 2*(3+1) = 1;
//

int CMesh2d::enumerateDofs(int start)
{
	unsigned int bwHor = m_nodeDofs*(m_cols+1);
	unsigned int bwVert = m_nodeDofs*(m_rows+1);

	if (bwHor>bwVert)
		return enumerateDofsHorisontal(start);
	else
		return enumerateDofsVertical(start);
}


// -----------------------------------------------------------
int CMesh2d::enumerateDofsHorisontal(int start)
{
	unsigned int i, j;

	int dofCount = start;

	for (i=0; i<m_rows+1; i++)
		for (j=0; j<m_cols+1; j++)
			dofCount = m_nodeArray[i][j]->getDofs()->enumerate(dofCount);

	return dofCount;
}

// ------------------------------------------------------------
int CMesh2d::enumerateDofsVertical(int start)
{
	unsigned int i, j;

	int dofCount = start;

	for (i=0; i<m_cols+1; i++)
		for (j=0; j<m_rows+1; j++)
			dofCount = m_nodeArray[j][i]->getDofs()->enumerate(dofCount);

	m_bandWidth = m_nodeDofs*(m_rows+1);

	return dofCount;
}

// ------------------------------------------------------------
void CMesh2d::print(std::ostream& out)
{
	out << "Mesh2d = [" << std::endl;
	out << std::endl;
	out << "# -------- Nodes ---------" << std::endl;
	out << std::endl;

	//for (i=0; i<m_nodeList.size(); i++)
	//	m_nodeList[i]->print(out);

	out << "# -------- Constraints ---------" << std::endl;
	out << std::endl;

	TConstraintMapIterator mi;
	
	for (mi = m_constraintMap.begin(); mi!=m_constraintMap.end(); mi++)
		out << mi->first << " = " << mi->second << std::endl;

}

// ------------------------------------------------------------
void CMesh2d::constrainNode(unsigned int row, unsigned int col, unsigned int dof, double value)
{
	if ((row>=0)&&(row<=m_rows)&&(col>=0)&&(col<=m_rows))
	{
		CNode3dPtr node = m_nodeArray[row][col];
		m_constraintMap[node->getDofs()->getDof(dof)] = value;
	}
}

// ------------------------------------------------------------
void CMesh2d::constrainNodeRow(unsigned int row, unsigned int dof, double value)
{
	unsigned int i;

	for (i=0; i<m_cols; i++)
		this->constrainNode(row, i, dof, value);
}

// ------------------------------------------------------------
void CMesh2d::constrainNodeCol(unsigned int col, unsigned int dof, double value)
{
	unsigned int i;

	for (i=0; i<m_rows; i++)
		this->constrainNode(i, col, dof, value);
}

// ------------------------------------------------------------
void CMesh2d::constrainHorisontal(unsigned int row, unsigned int start, unsigned int end, unsigned int dof, double value)
{
	unsigned int i;

	for (i=start; i<=end; i++)
		this->constrainNode(row, i, dof, value);
}

// ------------------------------------------------------------
void CMesh2d::constrainVertical(unsigned int col, unsigned int start, unsigned int end, unsigned int dof, double value)
{
	unsigned int i;

	for (i=start; i<=end; i++)
		this->constrainNode(i, col, dof, value);
}

// ------------------------------------------------------------
void CMesh2d::applyForceNode(unsigned int row, unsigned int col, double fx, double fy)
{
	if ((row>=0)&&(row<=m_rows)&&(col>=0)&&(col<=m_rows))
	{
		CNode3dPtr node = m_nodeArray[row][col];
		if (fabs(fx)>0.0)
			this->m_forceMap[node->getDofs()->getDof(1)] = fx;
		if (fabs(fy)>0.0)
			this->m_forceMap[node->getDofs()->getDof(2)] = fy;
	}	
}

// ------------------------------------------------------------
void CMesh2d::applyForceRow(unsigned int row, double fx, double fy)
{
	unsigned int i;

	for (i=0; i<m_cols; i++)
		this->applyForceNode(row, i, fx, fy);
}

// ------------------------------------------------------------
void CMesh2d::applyForceCol(unsigned int col, double fx, double fy)
{
	unsigned int i;

	for (i=0; i<m_rows; i++)
		this->applyForceNode(i, col, fx, fy);
}

// ------------------------------------------------------------
void CMesh2d::applyForceHorisontal(unsigned int row, unsigned int start, unsigned int end, double fx, double fy)
{
	unsigned int i;

	for (i=start; i<=end; i++)
		this->applyForceNode(row, i, fx, fy);
}

// ------------------------------------------------------------
void CMesh2d::applyForceVertical(unsigned int col, unsigned int start, unsigned int end, double fx, double fy)
{
	unsigned int i;

	for (i=start; i<=end; i++)
		this->applyForceNode(i, col, fx, fy);
}

// ------------------------------------------------------------
unsigned int CMesh2d::getRows()
{
	return m_rows;
}

// ------------------------------------------------------------
unsigned int CMesh2d::getCols()
{
	return m_cols;
}

// ------------------------------------------------------------
unsigned int CMesh2d::getTotalDofs()
{
	return (m_rows+1)*(m_cols+1)*m_nodeDofs;
}

// ------------------------------------------------------------
TConstraintMap& CMesh2d::getConstraints()
{
	return m_constraintMap;
}

// ------------------------------------------------------------
TForceMap& CMesh2d::getForces()
{
	return m_forceMap;
}

double CMesh2d::getWidth()
{
	return m_width;
}

double CMesh2d::getHeight()
{
	return m_height;
}

double CMesh2d::getElementWidth()
{
	return m_width/(double)m_cols;
}

double CMesh2d::getElementHeight()
{
	return m_height/(double)m_rows;
}

// ------------------------------------------------------------
//  
//    1 2 3 0 4 5 0 6 7
//  1 x x x | x x | x x 
//  2 x x x | x x | x x 
//  3 x x x | x x | x x 
//  0 ------|-----|---- 
//  4 x x x | x x | x x 
//  5 x x x | x x | x x 
//  0 ------|-----|---- 
//  6 x x x | x x | x x 
//  7 x x x | x x | x x 
//
void CMesh2d::assembleGlobalStiffnessMatrix()
{
	// Determine size of reduced system matrix

	unsigned int nDofConstraints = m_constraintMap.size();
	unsigned int nTotalDofs = this->getTotalDofs();
	unsigned int nSystemDofs = nTotalDofs - nDofConstraints;

	m_K.resize(nSystemDofs,m_bandWidth);
	m_K = 0.0;

	int i, j;

	for (i=0; i<m_rows; i++)
	{
		for (j=0; j<m_cols; j++)
		{

		}
	}
}

void CMesh2d::setCreateElementMatrixEvent(CCreateElementMatrixEvent* event)
{
	m_createElementMatrixEvent = event;
}

