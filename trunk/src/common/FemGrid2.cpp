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

#include "FemGrid2.h"

#include "Vec3d.h"

CFemGrid2::CFemGrid2()
{
	m_showGrid = false;

	m_forceColor = new CColor();
	m_forceColor->setColor(1.0f, 0.0f, 0.0f, 1.0f);

	m_constraintColor = new CColor();
	m_constraintColor->setColor(0.0f, 0.3f, 1.0f, 1.0f);

	m_colorMap = new CColorMap();
	m_invertColorMap = false;

	m_dofs = NULL;
	m_bandwidth = 0;
	m_displacementScale = 1.0;
	m_displacements = NULL;
	m_results = NULL;
	m_nodeResults = NULL;

	m_elementTreshold = 0.02;

	m_stressSize = 50.0;
	m_stressAlpha = 0.5;
	m_stressWidth = 1.0;
	m_lowerStressTreshold = 0.0;
	m_upperStressTreshold = 1.0;
	m_stressMode = CFemGrid2::SM_ALL;
	m_stressType = CFemGrid2::ST_PRINCIPAL;
	m_lockScaleFactor = false;
	m_stressStep = 1;

	m_maxIntensity = 0.4;

	m_stressDrawn = false;
	m_drawStressOnce = true;
	m_averageStress = false;
	m_drawStress = true;
	m_drawDisplacements = true;
	m_drawForcesAndConstraints = true;
	m_dimmedConstraints = false;
	m_undeformedGrid = false;
	m_drawDensity = false;
	m_drawStructure = false;

	m_pixelArea = -1.0;

	m_upperMisesTreshold = 0.8; // Remove stress spikes.

	m_pointConstraints.clear();
	m_pointForces.clear();
}

CFemGrid2::~CFemGrid2()
{
	clearForces();
	clearConstraints();
	clearResults();
	clearDofs();

	if (m_displacements!=NULL)
		delete [] m_displacements;
}

void CFemGrid2::addForce(CForce* force)
{
	double x, y;
	int iy;

	force->getPosition(x, y);
	force->setColor(m_forceColor);

	iy = (int) y;

	if ((iy>=0)&&(iy<this->getImage()->getHeight()))
	{
		m_pointForces[iy].push_back(CForcePtr(force));
		//force->addReference();
	}
}

void CFemGrid2::doGeometry()
{
	// Draw grid

	//drawGridPoints();
	if (m_showGrid)
	{
		if (m_drawDensity)
			drawDensity();
		else
		{
			if (m_undeformedGrid)
				drawUndeformedGrid();

			if (m_drawStructure)
				drawStructure();
			drawGrid();
		}
	}

	// Draw forces

	if (m_drawStress)
	{
		if (m_stressType == ST_PRINCIPAL)
			drawStress();
		else if (m_stressType == ST_MISES)
			drawMisesStress();
		else
			drawMisesStressSmooth();
	}

	if (m_drawForcesAndConstraints)
	{
		if (m_dimmedConstraints)
		{
			m_constraintColor->setColor(0.7f, 0.7f, 0.7f, 0.7f);
			m_forceColor->setColor(0.7f, 0.7f, 0.7f, 0.7f);
		}
		else
		{
			m_constraintColor->setColor(0.0f, 0.3f, 1.0f, 1.0f);
			m_forceColor->setColor(1.0f, 0.0f, 0.0f, 1.0f);
		}

		// Draw forces

		drawForces();

		// Draw constraints

		drawConstraints();

	}

	// Draw debug points

	drawDebugPoints();
	//drawDoubleDofs();

}

void CFemGrid2::setShowGrid(bool flag)
{
	m_showGrid = flag;
	this->setShowReactionForces(flag);
}

bool CFemGrid2::getShowGrid()
{
	return m_showGrid;
}

void CFemGrid2::setImage(CImage *image)
{
	CImageGrid2::setImage(image);

	int height = image->getHeight();

	clearForces();
	clearConstraints();
	clearResults();
	clearDofs();
}

void CFemGrid2::clearForces()
{
	m_pointForces.clear();

	if (this->getImage()!=NULL)
		m_pointForces.resize(this->getImage()->getHeight());
}

void CFemGrid2::addConstraint(CConstraint *constraint)
{
	double x, y;
	int iy;

	constraint->getPosition(x, y);
	constraint->setColor(m_constraintColor);

	iy = (int) y;
	m_pointConstraints[iy].push_back(CConstraintPtr(constraint));
}

void CFemGrid2::clearConstraints()
{
	m_pointConstraints.clear();

	if (this->getImage()!=NULL)
		m_pointConstraints.resize(this->getImage()->getHeight());
}

void CFemGrid2::initDofs()
{
	int rows, cols;
	int i, j;

	this->getGridSize(rows, cols);

	rows++;
	cols++;

	// Delete old grid, if any.

	clearDofs();

	m_dofRows = rows;
	m_dofCols = cols;

	// Create new grid

	m_dofs = new int** [rows];

	for (i=0; i<rows; i++)
	{
		m_dofs[i] = new int* [cols];
		for (j=0; j<cols; j++)
		{
			m_dofs[i][j] = new int[4];
			m_dofs[i][j][0] = -1;
			m_dofs[i][j][1] = -1;
			m_dofs[i][j][2] = -1;
			m_dofs[i][j][3] = -1;
		}
	}
}

void CFemGrid2::clearDofs()
{
	int rows, cols;
	int i, j;

	this->getGridSize(rows, cols);

	rows++;
	cols++;

	// Delete old grid, if any.

	if (m_dofs!=NULL)
	{
		for (i=0; i<m_dofRows; i++)
		{
			for (j=0; j<m_dofCols; j++)
				delete [] m_dofs[i][j];

			delete [] m_dofs[i];
		}

		delete [] m_dofs;
	}
	m_dofs = NULL;
}


int CFemGrid2::enumerateDofs(int direction)
{
	//
	//    o----------------o---------------o o---------------o----------------o
	//    |              / |              / |              / |              / |
	//    |            /   |            /   |            /   |            /   |
	//    |          /     |          /     |          /     |          /     |
	//    |        /       |        /       |        /       |        /       |
	//    |      /         |      /         |      /         |      /         |
	//    |    /           |    /           |    /           |    /           |
	//    |  /             |  /             |  /             |  /             |
	//    |/               |/               |/               |/               |
	//    o----------------o---------------o o---------------o----------------o
	//    |              / |              / |              / |              / |
	//    |            /   |            /   |            /   |            /   |
	//    |          /     |          /     |          /     |          /     |
	//    |        /       |        /       |        /       |        /       |
	//    |      /         |      /         |      /         |      /         |
	//    |    /           |    /           |    /           |    /           |
	//    |  /             |  /             |  /             |  /             |
	//    |/               |/               |/               |/               |
	//    o----------------o---------------o o---------------o----------------o
	//    |              / |              / |              / |              / |
	//    |            /   |            /   |            /   |            /   |
	//    |          /     |          /     |          /     |          /     |
	//    |        /       |        /       |        /       |        /       |
	//    |      /         |      /         |      /         |      /         |
	//    |    /           |    /           |    /           |    /           |
	//    |  /             |  /             |  /             |  /             |
	//    |/               |/               |/               |/               |
	//    o----------------o---------------o o---------------o----------------o
	//    |              / |              / |              / |              / |
	//    |            /   |            /   |            /   |            /   |
	//    |          /     |          /     |          /     |          /     |
	//    |        /       |        /       |        /       |        /       |
	//    |      /         |      /         |      /         |      /         |
	//    |    /           |    /           |    /           |    /           |
	//    |  /             |  /             |  /             |  /             |
	//    |/               |/               |/               |/               |
	//    o----------------o---------------o o---------------o----------------o
	//
	//    * = 0 : Fully coupled 
	//        1 : Uncoupled in x-direction
	//        2 : Uncoupled in y-direction
	//        3 : Uncoupled in x and y-directions
	//

	int rows, cols;
	int i, j, m;
	int dof = 1;

	int dx[4];
	int dy[4];

	int elementDof;
	int elementTopo[8];
	int maxDof;
	int minDof;

	m_bandwidth = 0;
	this->getGridSize(rows, cols);
	if (m_dofs!=NULL)
	{
		switch (direction) {
		case ED_LEFT_RIGHT:
			for (i=0; i<rows; i++)
				for (j=0; j<cols; j++)
				{
					elementTopo[0] = -1;
					elementTopo[1] = -1;
					elementTopo[2] = -1;
					elementTopo[3] = -1;
					elementTopo[4] = -1;
					elementTopo[5] = -1;
					elementTopo[6] = -1;
					elementTopo[7] = -1;

					if (this->getGridValue(i,j)>m_elementTreshold)
					{
						this->getElementTopo(i, j, dx, dy);

						elementDof = 0;

						for (m=0; m<4; m++)
						{
							if (m_dofs[ i+dy[m] ][ j+dx[m] ][0]<0)
								m_dofs[ i+dy[m] ][ j+dx[m] ][0] = dof++;
							if (m_dofs[ i+dy[m] ][ j+dx[m] ][1]<0)
								m_dofs[ i+dy[m] ][ j+dx[m] ][1] = dof++;
							
							elementTopo[elementDof++] = m_dofs[ i+dy[m] ][ j+dx[m] ][0];
							elementTopo[elementDof++] = m_dofs[ i+dy[m] ][ j+dx[m] ][1];
						}

						calcMaxMin(elementTopo, 8, maxDof, minDof);

						if ( (maxDof-minDof+1)>m_bandwidth )
							m_bandwidth = maxDof-minDof+1;
					}
				}

				return dof-1;
				break;
		case ED_BOTTOM_TOP:
			for (j=0; j<cols; j++)
				for (i=0; i<rows; i++)
				{
					elementTopo[0] = -1;
					elementTopo[1] = -1;
					elementTopo[2] = -1;
					elementTopo[3] = -1;
					elementTopo[4] = -1;
					elementTopo[5] = -1;

					if (this->getGridValue(i,j)>m_elementTreshold)
					{
						this->getElementTopo(i, j, dx, dy);

						elementDof = 0;

						for (m=0; m<4; m++)
						{
							if (m_dofs[ i+dy[m] ][ j+dx[m] ][0]<0)
								m_dofs[ i+dy[m] ][ j+dx[m] ][0] = dof++;
							if (m_dofs[ i+dy[m] ][ j+dx[m] ][1]<0)
								m_dofs[ i+dy[m] ][ j+dx[m] ][1] = dof++;
							
							elementTopo[elementDof++] = m_dofs[ i+dy[m] ][ j+dx[m] ][0];
							elementTopo[elementDof++] = m_dofs[ i+dy[m] ][ j+dx[m] ][1];
						}

						calcMaxMin(elementTopo, 8, maxDof, minDof);

						if ( (maxDof-minDof+1)>m_bandwidth )
							m_bandwidth = maxDof-minDof+1;
					}
				}
				return dof-1;
				break;
		default:

			break;
		}
		return -1;
	}
	else
		return -1;
}

void CFemGrid2::resetDofs()
{
	int rows, cols;
	int i, j;

	this->getGridSize(rows, cols);

	rows++;
	cols++;
	if (m_dofs!=NULL)
	{
		for (j=0; j<cols; j++)
			for (i=0; i<rows; i++)
			{
				m_dofs[i][j][0] = -1;
				m_dofs[i][j][1] = -1;
			}
	}
}

void CFemGrid2::initGrid()
{
	clearDofs();
	clearResults();
	CImageGrid2::initGrid();
	initDofs();
	initResults();
}

bool CFemGrid2::getElement(int row, int col, double &value, double *ex, double *ey, int *topo)
{
	int rows, cols;
	int i;
	int dx[4];
	int dy[4];
	int dof = 0;
	bool active = true;

	this->getGridSize(rows, cols);

	if ((rows>0)&&(cols>0))
	{
		value = this->getGridValue(row, col);	
		if (value>m_elementTreshold)
		{
			this->getElementCoords(row, col, ex, ey);
			this->getElementTopo(row, col, dx, dy);

			for (i=0; i<4; i++)
			{
				topo[dof++] = m_dofs[ row+dy[i] ][ col+dx[i] ][0];
				topo[dof++] = m_dofs[ row+dy[i] ][ col+dx[i] ][1];
			}
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

void CFemGrid2::calcMaxMin(int *arr, int size, int &maxVal, int &minVal)
{
	int i;

	maxVal = -10000000;
	minVal =  10000000;


	for (i=0; i<size; i++)
	{
		if (arr[i]>maxVal)
			maxVal = arr[i];
		if (arr[i]<minVal)
			minVal = arr[i];
	}
}

int CFemGrid2::getBandwidth()
{
	return m_bandwidth;
}

CForce* CFemGrid2::getFirstPointLoad()
{
	int i;
	int height = getImage()->getHeight();

	// Find first non-empty row

	for (i=0; (i<height)&&(m_pointForces[i].empty()); i++);

	// If no forces where found return NULL

	if (i==height)
		return NULL;

	// Update search variables

	m_currentPointForceRow = i;
	m_currentPointForceIter = m_pointForces[i].begin();

	// Return found force

	return (*m_currentPointForceIter);
}

CForce* CFemGrid2::getNextPointLoad()
{
	int i;
	int height = getImage()->getHeight();

	// Check if we have more forces on current row

	m_currentPointForceIter++;

	// Return found force 

	if (m_currentPointForceIter!=m_pointForces[m_currentPointForceRow].end())
		return (*m_currentPointForceIter);

	// Go to next row

	m_currentPointForceRow++;

	// Find next non-empty row

	for (i=m_currentPointForceRow; (i<height)&&(m_pointForces[i].empty()); i++);

	// Return NULL if no forces where found

	if (i==height)
		return NULL;

	// Update search variables

	m_currentPointForceRow = i;
	m_currentPointForceIter = m_pointForces[i].begin();

	// Return found force

	return (*m_currentPointForceIter);
}


void CFemGrid2::getNearestDofs(int x, int y, int *dofs)
{
	int rows, cols;
	this->getGridSize(rows, cols);
	//rows++; cols++;

	if (x<0)
		x = 0;

	if (y<0)
		y = 0;

	if (x>=this->getImage()->getWidth())
		x = this->getImage()->getWidth()-1;

	if (y>=this->getImage()->getHeight())
		y = this->getImage()->getHeight()-1;

	int stride = this->getStride();
	int xg = (int) ( 0.49 + ((double)x)/(double)stride );
	int yg = (int) ( 0.49 + ((double)y)/(double)stride );

	//this->addPoint(xg*stride, yg*stride);

	dofs[0] = m_dofs[yg][xg][0];
	dofs[1] = m_dofs[yg][xg][1];
}

void CFemGrid2::addPoint(int x, int y)
{
	m_xpoints.push_back(x);
	m_ypoints.push_back(y);
}

void CFemGrid2::clearPoints()
{
	m_xpoints.clear();
	m_ypoints.clear();
}

void CFemGrid2::erasePointLoad(int x, int y, int brushSize)
{
	unsigned int i;
	double fx, fy;
	int fxs, fys;

	CForceQueIter fi;
	CForceQueIter prevfi;

	for (i=y-brushSize/2; i<(unsigned int)(y+brushSize/2); i++)
	{
		if ((i>=0)&&(i<m_pointForces.size())&&(!m_pointForces[i].empty()))
		{
			for (fi=m_pointForces[i].begin(); fi!=m_pointForces[i].end(); fi++)
			{
				CForce* force = (*fi);
				force->getPosition(fx, fy);

				fxs = (int)fx;
				fys = (int)fy;

				if ((fxs>(x-brushSize/2))&&(fxs<(x+brushSize/2)))
				{
					if (fi!=m_pointForces[i].begin())
						prevfi = fi-1;
					else
					{
						prevfi = m_pointForces[i].begin();
						m_pointForces[i].erase(fi);
						break;
					}
					m_pointForces[i].erase(fi);
					fi = prevfi;
				}
			}
		}
	}
}

CConstraint* CFemGrid2::getFirstPointConstraint()
{
	int i;
	int height = getImage()->getHeight();

	// Make sure m_pointConstraints has been initialiased

	// Find first non-empty row

	for (i=0; (i<height)&&(m_pointConstraints[i].empty()); i++);

	// If no constraints where found return NULL

	if (i==height)
		return NULL;

	// Update search variables

	m_currentPointConstraintRow = i;
	m_currentPointConstraintIter = m_pointConstraints[i].begin();

	// Return found constraint

	return (*m_currentPointConstraintIter);
}

CConstraint* CFemGrid2::getNextPointConstraint()
{
	int i;
	int height = getImage()->getHeight();

	// Check if we have more constraints on current row

	m_currentPointConstraintIter++;

	// Return found constraint 

	if (m_currentPointConstraintIter!=m_pointConstraints[m_currentPointConstraintRow].end())
		return (*m_currentPointConstraintIter);

	// Go to next row

	m_currentPointConstraintRow++;

	// Find next non-empty row

	for (i=m_currentPointConstraintRow; (i<height)&&(m_pointConstraints[i].empty()); i++);

	// Return NULL if no constraints where found

	if (i==height)
		return NULL;

	// Update search variables

	m_currentPointConstraintRow = i;
	m_currentPointConstraintIter = m_pointConstraints[i].begin();

	// Return found constraint

	return (*m_currentPointConstraintIter);
}

void CFemGrid2::erasePointConstraint(int x, int y, int brushSize)
{
	unsigned int i;
	double cx, cy;
	int cxs, cys;

	CConstraintQueIter ci;
	CConstraintQueIter prevci;

	for (i=y-brushSize/2; i<(unsigned int)(y+brushSize/2); i++)
	{
		if ((i>=0)&&(i<m_pointConstraints.size())&&(!m_pointConstraints[i].empty()))
		{
			for (ci=m_pointConstraints[i].begin(); ci!=m_pointConstraints[i].end(); ci++)
			{
				CConstraint* constraint = (*ci);
				constraint->getPosition(cx, cy);

				cxs = (int)cx;
				cys = (int)cy;

				if ((cxs>(x-brushSize/2))&&(cxs<(x+brushSize/2)))
				{
					if (ci!=m_pointConstraints[i].begin())
						prevci = ci-1;
					else
					{
						prevci = m_pointConstraints[i].begin();
						m_pointConstraints[i].erase(ci);
						break;
					}

					m_pointConstraints[i].erase(ci);
					ci = prevci;
				}
			}
		}
	}
}

int CFemGrid2::getPointConstraintsSize()
{
	CConstraintQueIter ci;
	int i;
	int constraintCounter = 0;
	int height = getImage()->getHeight();

	for (i=0; i<height; i++)
	{
		for (ci=m_pointConstraints[i].begin(); (!m_pointConstraints[i].empty())&&(ci!=m_pointConstraints[i].end()); ci++)
			constraintCounter++;
	}

	return constraintCounter;
}

void CFemGrid2::drawGrid()
{
	int i, j, l;
	double dx, dy;
	double k = m_displacementScale/m_maxNodeValue;
	double alpha = 0.7;
	int topo[8];
	double ex[4];
	double ey[4];
	double value;

	if (m_displacements!=NULL)
	{
		for (i=0; i<m_rows; i++)
		{
			for (j=0; j<m_cols; j++)
			{
				this->getElement(i, j, value, ex, ey, topo);

				if (m_grid[i][j]>m_elementTreshold)
				{
					glBegin(GL_QUADS);
					glColor4f(1.0f-m_grid[i][j]*m_maxIntensity, 1.0f-m_grid[i][j]*m_maxIntensity, 1.0f-m_grid[i][j]*m_maxIntensity, alpha);
					for (l=0; l<4; l++)
					{
						dx = k*m_displacements[topo[l*2]];
						dy = k*m_displacements[topo[l*2+1]];
						if (m_drawDisplacements)
							glVertex2d(ex[l]/m_elementScaleFactor + dx, ey[l]/m_elementScaleFactor + dy);
						else
							glVertex2d(ex[l]/m_elementScaleFactor, ey[l]/m_elementScaleFactor);
					}
					glEnd();
				}
			}
		}
	}
}

void CFemGrid2::drawDensity()
{
	int i, j, l;
	//double k = m_displacementScale/m_maxNodeValue;
	double alpha = 0.7;
	int topo[8];
	double ex[4];
	double ey[4];
	double value;

	for (i=0; i<m_rows; i++)
	{
		for (j=0; j<m_cols; j++)
		{
			this->getElement(i, j, value, ex, ey, topo);

			if (m_grid[i][j]>m_elementTreshold)
			{
				glBegin(GL_QUADS);
				glColor4f(1.0f-this->getFieldValue(0,i,j), 1.0f-this->getFieldValue(0,i,j), 1.0f-this->getFieldValue(0,i,j), 1.0);
				for (l=0; l<4; l++)
						glVertex2d(ex[l]/m_elementScaleFactor, ey[l]/m_elementScaleFactor);
				glEnd();
			}
		}
	}
}


void CFemGrid2::drawUndeformedGrid()
{
	int i, j, l;
	double alpha = 0.7;
	int topo[8];
	double ex[4];
	double ey[4];
	double value;

	if (m_displacements!=NULL)
	{
		for (i=0; i<m_rows; i++)
		{
			for (j=0; j<m_cols; j++)
			{
				this->getElement(i, j, value, ex, ey, topo);

				if (m_grid[i][j]>m_elementTreshold)
				{
					glBegin(GL_QUADS);
					glColor4f(1.0f-m_grid[i][j]*m_maxIntensity, 1.0f-m_grid[i][j]*m_maxIntensity, 1.0f-m_grid[i][j]*m_maxIntensity, alpha);
					for (l=0; l<4; l++)
						glVertex2d(ex[l]/m_elementScaleFactor, ey[l]/m_elementScaleFactor);
					glEnd();
				}
			}
		}
	}
}

void CFemGrid2::drawStructure()
{
	int i, j, l;
	double alpha = 1.0;
	int topo[8];
	double ex[4];
	double ey[4];
	double value;

	if (m_displacements!=NULL)
	{
		for (i=0; i<m_rows; i++)
		{
			for (j=0; j<m_cols; j++)
			{
				this->getElement(i, j, value, ex, ey, topo);

				if (m_grid[i][j]>m_elementTreshold)
				{
					glBegin(GL_QUADS);
					glColor4f(1.0f-m_grid[i][j], 1.0f-m_grid[i][j], 1.0f-m_grid[i][j], alpha);
					for (l=0; l<4; l++)
						glVertex2d(ex[l]/m_elementScaleFactor, ey[l]/m_elementScaleFactor);
					glEnd();
				}
			}
		}
	}
}


void CFemGrid2::drawDoubleDofs()
{
}


void CFemGrid2::setDisplacement(int dof, double value)
{
	m_displacements[dof] = value;
}

void CFemGrid2::setDisplacementSize(int size)
{
	if (m_displacements!=NULL)
		delete [] m_displacements;

	m_displacements = new double[size+1];
	m_displacementSize = size+1;

	int i;

	for (i=0; i<m_displacementSize; i++)
		m_displacements[i] = 0.0;
}

void CFemGrid2::clearDisplacements()
{
}

void CFemGrid2::setDisplacementScale(double scaleFactor)
{
	m_displacementScale = scaleFactor;
}

void CFemGrid2::deactivateDofs()
{
	int i, j, k;

	if (m_dofs!=NULL)
	{
		for (i=0; i<m_dofRows; i++)
			for (j=0; j<m_dofCols; j++)
				for (k=0; k<2; k++)
					m_dofs[i][j][k]=-1;
	}
}

void CFemGrid2::initResults()
{
	int i, j, k;

	// Delete old results grid, if any.

	clearResults();

	// Create new results grid (element)

	m_results = new double** [m_rows];

	for (i=0; i<m_rows; i++)
	{
		m_results[i] = new double* [m_cols];
		for (j=0; j<m_cols; j++)
		{
			m_results[i][j] = new double[8]; 
			for (k=0; k<8; k++)
				m_results[i][j][k]=0.0;
		}
	}

	// Create new results grid (node)

	m_nodeResults = new double* [m_rows+1];
	m_nodeUsage = new int* [m_rows+1];

	for (i=0; i<m_rows+1; i++)
	{
		m_nodeResults[i] = new double [m_cols+1];
		m_nodeUsage[i] = new int [m_cols+1];
		for (j=0; j<m_cols+1; j++)
		{
			m_nodeResults[i][j] = 0.0; 
			m_nodeUsage[i][j] = 0;
		}
	}
}

void CFemGrid2::clearResults()
{
	int i, j;

	// Delete old results grid, if any.

	if (m_results!=NULL)
	{
		for (i=0; i<m_rows; i++)
		{
			for (j=0; j<m_cols; j++)
				delete [] m_results[i][j];

			delete [] m_results[i];
		}
		delete [] m_results;
	}

	m_results = NULL;

	// Delete old node results grid, if any.

	if (m_nodeResults!=NULL)
	{
		for (i=0; i<m_rows+1; i++)
		{
			delete [] m_nodeUsage[i];
			delete [] m_nodeResults[i];
		}
		delete [] m_nodeResults;
		delete [] m_nodeUsage;
	}

	m_nodeResults = NULL;
	m_nodeUsage = NULL;
}

void CFemGrid2::averageNodeResults()
{
	if (m_nodeResults!=NULL)
	{
		int i, j;

		double divisor = 4.0;

		for (i=0; i<m_rows+1; i++)
			for (j=0; j<m_cols+1; j++)
			{
				if (m_nodeUsage[i][j]>0)
					m_nodeResults[i][j] = m_nodeResults[i][j] / m_nodeUsage[i][j];
			}
	}
}

void CFemGrid2::zeroNodeResults()
{
	if (m_nodeResults!=NULL)
	{
		int i, j;
		for (i=0; i<m_rows+1; i++)
			for (j=0; j<m_cols+1; j++)
			{
				m_nodeResults[i][j] = 0.0;
				m_nodeUsage[i][j] = 0;
			}
	}
}

void CFemGrid2::setResult(int i, int j, const double *values)
{
	if (m_results!=NULL)
	{
		int k;

		for (k=0; k<3; k++)
			m_results[i][j][k] = values[k];
	}
}

void CFemGrid2::setResult(int i, int j, int index, double value)
{
	if (m_results!=NULL)
		m_results[i][j][index] = value;
}

void CFemGrid2::setNodeResult(int i, int j, const double value)
{
	if (m_nodeResults!=NULL)
	{
		m_nodeResults[i][j] = value;
		m_nodeUsage[i][j] = 1;
	}
}

void CFemGrid2::addNodeResult(int i, int j, const double value)
{
	if (m_nodeResults!=NULL)
	{
		m_nodeResults[i][j] += value;
		m_nodeUsage[i][j] += 1;
	}
}

double CFemGrid2::getResult(int i, int j)
{
	if (m_nodeResults!=NULL)
		return m_nodeResults[i][j];
	else
		return 0.0;
}

void CFemGrid2::drawMisesStress()
{
	int i, j, l;
	double dx, dy;
	double k = m_displacementScale/m_maxNodeValue;
	double sigm;
	float r, g, b;
	int topo[8];
	double ex[4];
	double ey[4];
	double value;


	if ((m_displacements!=NULL)&&(m_results!=NULL))
	{
		for (i=0; i<m_rows; i++)
		{
			for (j=0; j<m_cols; j++)
			{
				if (m_grid[i][j]>m_elementTreshold)
				{
 					sigm = this->m_results[i][j][3];
					this->m_colorMap->getColor(sigm/this->m_maxMisesStressValue/m_upperMisesTreshold, r, g, b);

					this->getElement(i, j, value, ex, ey, topo);

					glBegin(GL_QUADS);
					glColor3f(r, g, b);
					for (l=0; l<4; l++)
					{
						dx = k*m_displacements[topo[l*2]];
						dy = k*m_displacements[topo[l*2+1]];
						if (m_drawDisplacements)
							glVertex2d(ex[l]/m_elementScaleFactor + dx, ey[l]/m_elementScaleFactor + dy);
						else
							glVertex2d(ex[l]/m_elementScaleFactor, ey[l]/m_elementScaleFactor);
					}
					glEnd();
				}
			}
		}
	}
}

void CFemGrid2::drawMisesStressSmooth()
{
	int i, j, l;
	double dx, dy;
	double k = m_displacementScale/m_maxNodeValue;
	double sigm;
	float r, g, b;
	int topo[8];
	double ex[4];
	double ey[4];
	double value;

	if ((m_displacements!=NULL)&&(m_nodeResults!=NULL))
	{
		for (i=0; i<m_rows; i++)
		{
			for (j=0; j<m_cols; j++)
			{
				if (m_grid[i][j]>m_elementTreshold)
				{

					this->getElement(i, j, value, ex, ey, topo);

					glBegin(GL_QUADS);
					for (l=0; l<4; l++)
					{
						switch (l) {
							case 0:
								sigm = this->m_nodeResults[i][j];
								break;
							case 1:
								sigm = this->m_nodeResults[i][j+1];
								break;
							case 2:
								sigm = this->m_nodeResults[i+1][j+1];
								break;
							case 3:
								sigm = this->m_nodeResults[i+1][j];
								break;
						}

						this->m_colorMap->getColor(sigm/this->m_maxMisesStressValue/m_upperMisesTreshold, r, g, b);
						glColor3f(r, g, b);

						dx = k*m_displacements[topo[l*2]];
						dy = k*m_displacements[topo[l*2+1]];
						if (m_drawDisplacements)
							glVertex2d(ex[l]/m_elementScaleFactor + dx, ey[l]/m_elementScaleFactor + dy);
						else
							glVertex2d(ex[l]/m_elementScaleFactor, ey[l]/m_elementScaleFactor);
					}
					glEnd();
				}
			}
		}
	}
}

void CFemGrid2::drawStressArrow(double x, double y, const double *values)
{
	double maxStress;
	double maxPosStress;
	double maxNegStress;
	double sig1;
	double sig2;
	double maxSig;

	maxStress = getMaxStressValue();
	maxPosStress = getMaxPosStressValue();
	maxNegStress = getMaxNegStressValue();

	if (!this->m_lockScaleFactor)
	{
		m_maxStressFactor = 1.0/maxStress;
		m_maxPosStressFactor = 1.0/maxPosStress;
		m_maxNegStressFactor = 1.0/maxNegStress;
	}

	if (m_stressMode==CFemGrid2::SM_ALL)
	{
		sig1 = values[0]*m_maxStressFactor;
		sig2 = values[1]*m_maxStressFactor;
	}

	if ((m_stressMode==CFemGrid2::SM_POSITIVE)||(m_stressMode==CFemGrid2::SM_NEGATIVE))
	{
		if (values[0]>0.0)
			sig1 = values[0]*m_maxPosStressFactor;
		else
			sig1 = values[0]*m_maxNegStressFactor;

		if (values[1]>0.0)
			sig2 = values[1]*m_maxPosStressFactor;
		else
			sig2 = values[1]*m_maxNegStressFactor;
	}

	double alpha = values[2];

	if (fabs(sig1)>fabs(sig2))
		maxSig = fabs(sig1);
	else
		maxSig = fabs(sig2);

	if ( (maxSig>m_lowerStressTreshold)&&(maxSig<m_upperStressTreshold) )
	{

		glLineWidth(m_stressWidth);
		glBegin(GL_LINES);

		if  (((m_stressMode==CFemGrid2::SM_POSITIVE)&&(sig1>0.0))||
			((m_stressMode==CFemGrid2::SM_NEGATIVE)&&(sig1<0.0))||
			(m_stressMode==CFemGrid2::SM_ALL))
		{
			if (sig1>0)
				glColor4f(1.0f, 0.0f, 0.0f, (double)m_stressAlpha);
			else
				glColor4f(0.0f, 0.0f, 1.0f, (double)m_stressAlpha);

			glVertex2d(x - 0.5*sig1*m_stressSize*cos(alpha), y - 0.5*sig1*m_stressSize*sin(alpha));
			glVertex2d(x + 0.5*sig1*m_stressSize*cos(alpha), y + 0.5*sig1*m_stressSize*sin(alpha));
		}

		if  (((m_stressMode==CFemGrid2::SM_POSITIVE)&&(sig2>0.0))||
			((m_stressMode==CFemGrid2::SM_NEGATIVE)&&(sig2<0.0))||
			(m_stressMode==CFemGrid2::SM_ALL))
		{
			if (sig2>0)
				glColor4f(1.0f, 0.0f, 0.0f, (double)m_stressAlpha);
			else
				glColor4f(0.0f, 0.0f, 1.0f, (double)m_stressAlpha);

			glVertex2d(x - 0.5*sig2*m_stressSize*cos(alpha+M_PI/2.0), y - 0.5*sig2*m_stressSize*sin(alpha+M_PI/2.0));
			glVertex2d(x + 0.5*sig2*m_stressSize*cos(alpha+M_PI/2.0), y + 0.5*sig2*m_stressSize*sin(alpha+M_PI/2.0));
		}

		glEnd();
	}
}

void CFemGrid2::drawStress()
{
	int i, j;
	double k = m_displacementScale/m_maxNodeValue;
	double xm;
	double ym;
	double values[3];

	if (m_results!=NULL)
	{
		glPushAttrib(GL_COLOR_BUFFER_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		for (i=0; i<m_rows; i+=m_stressStep)
		{
			for (j=0; j<m_cols; j+=m_stressStep)
			{
				if (m_grid[i][j]>0.001f)
				{
					xm = (j*m_stride+(j+1)*m_stride)*0.5;
					ym = (i*m_stride+(i+1)*m_stride)*0.5;
					values[0] = m_results[i][j][0];
					values[1] = m_results[i][j][1];
					values[2] = m_results[i][j][2];
					drawStressArrow(xm, ym, values);
				}
			}
		}
		glPopAttrib();
	}
}

void CFemGrid2::drawForces()
{
	int i;
	int height = getImage()->getHeight();

	CForceQueIter fi;

	for (i=0; i<height; i++)
	{
		for (fi=m_pointForces[i].begin(); (!m_pointForces[i].empty())&&(fi!=m_pointForces[i].end()); fi++)
			(*fi)->render();
	}
}

void CFemGrid2::drawConstraints()
{
	// Draw constraints

	int i;
	int height = getImage()->getHeight();

	CConstraintQueIter ci;

	for (i=0; i<height; i++)
	{
		for (ci=m_pointConstraints[i].begin(); (!m_pointConstraints[i].empty())&&(ci!=m_pointConstraints[i].end()); ci++)
			(*ci)->render();
	}
}

void CFemGrid2::drawDebugPoints()
{
	unsigned int i;

	// Draw test points

	glBegin(GL_LINES);
	glLineWidth(2.0);
	glColor3f(1.0f, 0.0f, 1.0f);
	for (i=0; i<m_xpoints.size(); i++)
	{
		glVertex2i(m_xpoints[i]-2, m_ypoints[i]);
		glVertex2i(m_xpoints[i]+2, m_ypoints[i]);
		glVertex2i(m_xpoints[i], m_ypoints[i]-2);
		glVertex2i(m_xpoints[i], m_ypoints[i]+2);
	}
	glEnd();
}

void CFemGrid2::drawGridPoints()
{
	int i, j, l;
	double alpha = 0.7;
	double ex[4];
	double ey[4];

	for (i=0; i<m_rows; i++)
	{
		for (j=0; j<m_cols; j++)
		{
			this->getElementCoords(i, j, ex, ey);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glLineWidth(1.0f);
			glBegin(GL_QUADS);
			glColor4f(0.2f, 0.2f, 0.2f, 0.5f);
			for (l=0; l<4; l++)
				glVertex2d(ex[l], ey[l]);
			glEnd();
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
}

void CFemGrid2::setMaxStressValue(double value)
{
	m_maxStressValue = value;
}

double CFemGrid2::getMaxStressValue()
{
	return m_maxStressValue;
}

void CFemGrid2::setMaxNodeValue(double value)
{
	m_maxNodeValue = value;
}

double CFemGrid2::getMaxNodeValue()
{
	return m_maxNodeValue;
}

void CFemGrid2::setStressAlpha(double alpha)
{
	m_stressAlpha = alpha;
}

void CFemGrid2::setStressSize(double size)
{
	m_stressSize = size;
}

void CFemGrid2::setStressWidth(double width)
{
	m_stressWidth = width;
}

void CFemGrid2::setLockScale(bool flag)
{
	m_lockScaleFactor = flag;
}

void CFemGrid2::setStressTreshold(double lower, double upper)
{
	m_lowerStressTreshold = lower;
	m_upperStressTreshold = upper;
}

void CFemGrid2::setMaxIntensity(double intensity)
{
	m_maxIntensity = intensity;
}

int CFemGrid2::getPointLoadSize()
{
	CForceQueIter fi;
	int i;
	int pointForceCounter = 0;
	int height = getImage()->getHeight();

	for (i=0; i<height; i++)
	{
		for (fi=m_pointForces[i].begin(); (!m_pointForces[i].empty())&&(fi!=m_pointForces[i].end()); fi++)
			pointForceCounter++;
	}

	return pointForceCounter;
}

int CFemGrid2::getDisplacementSize()
{
	return m_displacementSize;
}

double CFemGrid2::getDisplacement(int dof)
{
	return m_displacements[dof];
}

void CFemGrid2::getResult(int i, int j, double *values)
{
	if (m_results!=NULL)
	{
		int k;

		for (k=0; k<3; k++)
			values[k] = m_results[i][j][k];
	}
}

void CFemGrid2::resetStressDrawing()
{
	m_stressDrawn = false;
}

void CFemGrid2::setDrawStressOnce(bool flag)
{
	m_drawStressOnce = flag;
}

void CFemGrid2::saveToStream(ostream &out)
{
	CImage* image = this->getImage();
	int i, j;
	int width, height;
	GLubyte uivalue;
	int lastValue;
	int value;
	int valueCount = 0;
	double x, y;

	if (image!=NULL)
	{
		//
		// Write image map RLE (only black & white)
		//

		width = image->getWidth();
		height = image->getHeight();

		out << width << " " << height << endl;

		lastValue = -1;
		bool firstTime = true;

		for (j=0; j<height; j++)
		{
			for (i=0; i<width; i++)
			{
				image->getValue(i, j, 0, uivalue);

				value = (int)uivalue;

				valueCount++;

				if (value!=lastValue)
				{
					if (firstTime)
						firstTime = false;
					else
						out << valueCount << endl;

					out << value << " ";
					valueCount=0;
				}

				lastValue = value;
			}
		}
		out << ++valueCount << endl;

		out << -1 << " " << -1 << endl;

		//
		// Write forces
		//

		unsigned int i;

		out << this->getPointLoadSize() << endl;

		CForceQueIter fi;

		for (i=0; i<m_pointForces.size(); i++)
		{
			for (fi=m_pointForces[i].begin(); fi!=m_pointForces[i].end(); fi++)
			{
				CForce* force = (*fi);
				force->getPosition(x, y);
				out << x << " " << y << endl;
				force->saveToStream(out);
			}
		}

		//
		// Write constraints
		//

		out << this->getPointConstraintsSize() << endl;

		CConstraintQueIter ci;

		for (i=0; i<m_pointConstraints.size(); i++)
		{
			for (ci=m_pointConstraints[i].begin(); ci!=m_pointConstraints[i].end(); ci++)
			{
				CConstraint* constraint = (*ci);
				constraint->getPosition(x, y);
				out << x << " " << y << endl;
				constraint->saveToStream(out);
				out << endl;
			}
		}

	}
}

void CFemGrid2::readFromStream(istream &in)
{
	CImagePtr image = this->getImage();
	int width, height;
	int i;
	int value;
	int valueCount;
	int row = 0;
	int col = 0;
	bool finished = false;
	int nForces;
	int nConstraints;
	double x, y;

	if (image!=NULL)
	{

		//
		// Read image size
		//

		in >> width >> height;

		//
		// Set image size
		//

		//image->setChannels(4);
		image->setSize(width, height);

		// 
		// Read RLE image information 
		//

		while (true) 
		{
			in >> value >> valueCount;

			if (value==-1)
				break;

			for (i=0; i<valueCount; i++)
			{
				image->setValue(col, row, 0, (GLubyte)value);
				image->setValue(col, row, 1, (GLubyte)value);
				image->setValue(col, row, 2, (GLubyte)value);

				col++;

				if (col==width)
				{
					col = 0;
					row++;
				}
			}
		}

		// 
		// To setup forces and constraints we must assign 
		// the image to ourselves
		//

		//CImageGrid2::setImage(image);
		this->setImageSize(image->getWidth(), image->getHeight());

		clearForces();
		clearConstraints();
		clearResults();
		clearDofs();

		//
		// Read forces
		//

		in >> nForces;

		for (i=0; i<nForces; i++)
		{
			CForce* force = new CForce();
			in >> x >> y;
			force->setPosition(x, y);
			force->readFromStream(in);
			this->addForce(force);
		}

		//
		// Read constraints
		//

		in >> nConstraints;

		for (i=0; i<nConstraints; i++)
		{
			CConstraint* constraint = new CConstraint();
			in >> x >> y;
			constraint->setPosition(x, y);
			constraint->readFromStream(in);

			// Convert from old constraint types

			if (constraint->getConstraintType()==CConstraint::CT_X) 
			{
				constraint->setConstraintType(CConstraint::CT_VECTOR);
				constraint->setDirection(1.0, 0.0);
			}

			if (constraint->getConstraintType()==CConstraint::CT_Y) 
			{
				constraint->setConstraintType(CConstraint::CT_VECTOR);
				constraint->setDirection(0.0, 1.0);
			}

			if (constraint->getConstraintType()==CConstraint::CT_XY) 
			{
				constraint->setConstraintType(CConstraint::CT_VECTOR);
				constraint->setDirection(1.0, 0.0);

				CConstraint* extraConstraint = new CConstraint();
				extraConstraint->setPosition(x, y);
				extraConstraint->setDirection(0.0, 1.0);
				extraConstraint->setConstraintType(CConstraint::CT_VECTOR);

				this->addConstraint(extraConstraint);
			}

			this->addConstraint(constraint);
		}
	}
}

void CFemGrid2::setStressMode(TStressMode mode)
{
	m_stressMode = mode;
}

double CFemGrid2::getMaxPosStressValue()
{
	return m_maxPosStressValue;
}

double CFemGrid2::getMaxNegStressValue()
{
	return m_maxNegStressValue;
}

void CFemGrid2::setMaxPosStressValue(double value)
{
	m_maxPosStressValue = value;
}

void CFemGrid2::setMaxNegStressValue(double value)
{
	m_maxNegStressValue = value;
}

void CFemGrid2::calcCenterOfGravity(int &x, int &y)
{
	CImage* image = this->getImage();
	int i, j;

	double sumFx, sumFy, sumF;
	GLubyte red, green, blue;
	double value;
	int width, height;

	sumFx = 0.0;
	sumFy = 0.0;
	sumF = 0.0;

	if (image!=NULL)
	{
		width = image->getWidth();
		height = image->getHeight();

		for (j=0; j<height; j++)
		{
			for (i=0; i<width; i++) 
			{
				image->getValue(i, j, 0, red);
				image->getValue(i, j, 1, green);
				image->getValue(i, j, 2, blue);

				value = (double)(255-red) + (double)(255-green) + (double)(255-blue);

				if (value>0.0)
				{
					sumFx+=255.0*(double)i;
					sumFy+=255.0*(double)j;
					sumF+=255.0;
				}
			}
		}

		x = (int)(sumFx/sumF);
		y = (int)(sumFy/sumF);

		m_pixelArea = sumF/255.0;

	}
	else
	{
		x = -1;
		y = -1;
	}
}

void CFemGrid2::getElementPos(double x, double y, int& r, int& c)
{
	r = (int)((double)y / m_stride);
	c = (int)((double)x / m_stride);	
}

double CFemGrid2::getStiffness(double x, double y)
{
	int r, c;

	this->getElementPos(x, y, r, c);
	return this->getGridValue(r, c);
}

void CFemGrid2::setStiffness(double x, double y, double value)
{
	int r, c;
	this->getElementPos(x, y, r, c);
	this->setGridValue(r, c, value);
}

void CFemGrid2::setStiffness(double x, double y, double value, int offsetElements, bool special)
{
	int r, c;

	int i, j;

	this->getElementPos(x, y, r, c);

	for (i=r-offsetElements; i<=r+offsetElements; i++)
	{
		for (j=c-offsetElements; j<=c+offsetElements; j++)
		{
			this->setGridValue(i, j, value);
			this->setSpecialElement(i, j, special);
		}
	}
}

void CFemGrid2::setStiffnessLine(double x1, double y1, double x2, double y2, double value, double width)
{
	CVec3d v;
	v.setComponents(x2-x1, y2-y1, 0.0);
	double l = v.length();
	v.normalize();

	CVec3d up;
	up.setComponents(0.0, 0.0, 1.0);
	
	CVec3d left = v * up;
	left.normalize();

	double step = 0.5;
	double t = 0.0;
	double s = 0.0;
	
	CVec3d p0;
	CVec3d p1;
	p0.setComponents(x1, y1, 0.0);
	double x, y, z;

	p1 = p0;// - (width/2.0)*left;

//	while (s<=width)
//	{
		p1 = p0;// - (width/2.0)*left + s*left;

		while (t<=l)
		{
			p1.getComponents(x, y, z);
			this->setStiffness(x, y, value);
			t = t + step;
			p1 = p1 + v*step;
		}
//		s = s + step;
		t = 0.0;
//	}
}

CForce* CFemGrid2::getNearestForce(int x, int y)
{
	//  
	//  x-n        x        x+n
	// +----+----+----+----+----+
	// |    |    |    |    |    | y-n
	// |    |    |    |    |    |
	// +----+----+----+----+----+
	// |    |    |    |    |    |
	// |    |    |    |    |    |
	// +----+----+----+----+----+
	// |    |    | mm |    |    | y
	// |    |    | mm |    |    |
	// +----+----+----+----+----+
	// |    |    |    |    |    |
	// |    |    |    |    |    |
	// +----+----+----+----+----+
	// |    |    |    |    |    |
	// |    |    |    |    |    | x+n
	// +----+----+----+----+----+
	//
	// mm - Mouse position
	// n  - Pixel search distance (n=2 above)
	//

	CForceSelectionPtr selection = new CForceSelection();
	this->getForces(x-20, y-20, x+20, y+20, selection);

	if (selection->getSize()==0)
		return NULL;

	int i;
	double dist;
	double minDist = 1e300;
	double xf, yf;
	CForcePtr nearestForce = NULL;

	for (i=0; i<selection->getSize(); i++)
	{
		CForcePtr force = selection->getForce(i);
		force->getPosition(xf, yf);
		dist = sqrt(pow(xf - (double)x, 2) + pow(yf - (double)y, 2));
		if (dist<minDist)
		{
			nearestForce = force;
			minDist = dist;
		}
	}

	return nearestForce;
}

void CFemGrid2::getForces(int x1, int y1, int x2, int y2, CForceSelection *selection)
{
	int i;
	double fx, fy;
	int fxs, fys;

	CForceQueIter fi;
	CForceQueIter prevfi;

	for (i=y1; i<y2; i++)
	{
		for (fi=m_pointForces[i].begin(); (!m_pointForces[i].empty())&&(fi!=m_pointForces[i].end()); fi++)
		{
			CForce* force = (*fi);
			force->getPosition(fx, fy);

			fxs = (int)fx;
			fys = (int)fy;

			if ((fxs>x1)&&(fxs<x2))
				selection->add(force);
		}
	}
}

void CFemGrid2::getConstraints(int x1, int y1, int x2, int y2, CConstraintSelection *selection)
{
	int i;
	double fx, fy;
	int fxs, fys;

	CConstraintQueIter fi;
	CConstraintQueIter prevfi;

	for (i=y1; i<y2; i++)
	{
		for (fi=m_pointConstraints[i].begin(); (!m_pointConstraints[i].empty())&&(fi!=m_pointConstraints[i].end()); fi++)
		{
			CConstraint* constraint = (*fi);
			constraint->getPosition(fx, fy);

			fxs = (int)fx;
			fys = (int)fy;

			if ((fxs>x1)&&(fxs<x2))
				selection->add(constraint);
		}
	}
}

void CFemGrid2::removePointForce(CForce *force)
{
	double fx, fy;
	int fxs, fys;

	CForceQueIter fi;
	CForceQueIter prevfi;

	force->getPosition(fx, fy);

	fxs = (int)fx;
	fys = (int)fy;

	for (fi=m_pointForces[fys].begin(); (!m_pointForces[fys].empty())&&(fi!=m_pointForces[fys].end()); fi++)
	{
		CForce* aForce = (*fi);

		if (force = aForce)
		{
			if (fi!=m_pointForces[fys].begin())
				prevfi = fi-1;
			else
			{
				prevfi = m_pointForces[fys].begin();
				m_pointForces[fys].erase(fi);
				return;
			}
			m_pointForces[fys].erase(fi);
			fi = prevfi;
		}
	}
}

void CFemGrid2::moveForce(CForce* force, int x, int y)
{
	double fx, fy;
	int fxs, fys;

	CForceQueIter fi;
	CForceQueIter eraseForce;

	CForcePtr holdForce = force;

	force->getPosition(fx, fy);

	fxs = (int)fx;
	fys = (int)fy;


	for (fi=m_pointForces[fys].begin(); (!m_pointForces[fys].empty())&&(fi!=m_pointForces[fys].end()); fi++)
	{
		CForcePtr aForce = (*fi);
		if (force == aForce)
			eraseForce = fi;
	}

	m_pointForces[fys].erase(eraseForce);


	if ((int)y>=(int)m_pointForces.size())
	{
		force->setPosition((double)x, (double)m_pointForces.size()-1);
		m_pointForces[m_pointForces.size()-1].push_back(force);
	}
	else
	{
		force->setPosition((double)x, (double)y);
		m_pointForces[(int)y].push_back(force);
	}

}

void CFemGrid2::removePointConstraint(CConstraint *constraint)
{
	double fx, fy;
	int fxs, fys;

	CConstraintQueIter fi;
	CConstraintQueIter prevfi;

	constraint->getPosition(fx, fy);

	fxs = (int)fx;
	fys = (int)fy;

	for (fi=m_pointConstraints[fys].begin(); (!m_pointConstraints[fys].empty())&&(fi!=m_pointConstraints[fys].end()); fi++)
	{
		CConstraint* aConstraint = (*fi);

		if (constraint = aConstraint)
		{
			if (fi!=m_pointConstraints[fys].begin())
				prevfi = fi-1;
			else
			{
				prevfi = m_pointConstraints[fys].begin();
				m_pointConstraints[fys].erase(fi);
				return;
			}
			m_pointConstraints[fys].erase(fi);
			fi = prevfi;
		}
	}
}

void CFemGrid2::calcCenterOfStiffness(int &cgx, int &cgy)
{
	CImage* image = this->getImage();
	int i, j;

	double sumFx, sumFy, sumF;
	GLubyte red, green, blue;
	double value;
	int width, height;

	sumFx = 0.0;
	sumFy = 0.0;
	sumF = 0.0;

	if (image!=NULL)
	{
		width = image->getWidth();
		height = image->getHeight();

		for (j=0; j<height; j++)
		{
			for (i=0; i<width; i++) 
			{
				image->getValue(i, j, 0, red);
				image->getValue(i, j, 1, green);
				image->getValue(i, j, 2, blue);

				value = (double)(255-red) + (double)(255-green) + (double)(255-blue);

				sumFx+=value*(double)i;
				sumFy+=value*(double)j;
				sumF+=value;
			}
		}

		cgx = (int)(sumFx/sumF);
		cgy = (int)(sumFy/sumF);

		m_pixelArea = sumF/(255.0*3.0);

	}
	else
	{
		cgx = -1;
		cgy = -1;
	}
}

double CFemGrid2::getPixelArea()
{
	return m_pixelArea;
}

CConstraintSelectionPtr CFemGrid2::getConstraints()
{
	int i;

	CConstraintSelectionPtr selection = new CConstraintSelection();

	CConstraintQueIter fi;
	CConstraintQueIter prevfi;

	for (i=0; i<this->getImage()->getHeight(); i++)
	{
		for (fi=m_pointConstraints[i].begin(); (!m_pointConstraints[i].empty())&&(fi!=m_pointConstraints[i].end()); fi++)
		{
			CConstraint* constraint = (*fi);
			selection->add(constraint);
		}
	}

	return selection;
}

CForceSelectionPtr CFemGrid2::getForces()
{
	int i;

	CForceSelectionPtr selection = new CForceSelection();

	CForceQueIter fi;
	CForceQueIter prevfi;

	for (i=0; i<this->getImage()->getHeight(); i++)
	{
		for (fi=m_pointForces[i].begin(); (!m_pointForces[i].empty())&&(fi!=m_pointForces[i].end()); fi++)
		{
			CForce* force = (*fi);
			selection->add(force);
		}
	}

	return selection;
}

int CFemGrid2::getConstraintsSize()
{
	int i;

	CConstraintQueIter fi;
	CConstraintQueIter prevfi;

	int constraintCount = 0;

	for (i=0; i<this->getImage()->getHeight(); i++)
	{
		for (fi=m_pointConstraints[i].begin(); (!m_pointConstraints[i].empty())&&(fi!=m_pointConstraints[i].end()); fi++)
		{
			constraintCount++;
		}
	}

	return constraintCount;
}

void CFemGrid2::getElements(int x1, int y1, int x2, int y2, CElementList& list)
{
	double ex = (double)(x2 - x1);
	double ey = (double)(y2 - y1);

	CVec3d v;
	v.setComponents(ex, ey, 0.0);
	double l = v.length();
	v.normalize();

	double t;
	double dt = 0.5;
	double x, y, z;
	int r, c, oldR, oldC;
	CVec3d p;

	p.setComponents((double)x1, (double)y1, 0.0);
	p.getComponents(x, y, z);
	t = 0.0;
	oldR = (int)(x / m_stride);
	oldC = (int)(y / m_stride);

	vector<int> elementPos;
	elementPos.push_back(oldR);
	elementPos.push_back(oldC);
	list.push_back(elementPos);

	while (t<l)
	{
		r = (int)(x / m_stride);
		c = (int)(y / m_stride);
		if ((r!=oldR)||(c!=oldC))
		{
			
		}
	}
}

void CFemGrid2::setShowReactionForces(bool flag)
{
	int i;

	CConstraintSelectionPtr selection = new CConstraintSelection();

	CConstraintQueIter fi;
	CConstraintQueIter prevfi;

	for (i=0; i<this->getImage()->getHeight(); i++)
	{
		for (fi=m_pointConstraints[i].begin(); (!m_pointConstraints[i].empty())&&(fi!=m_pointConstraints[i].end()); fi++)
		{
			CConstraint* constraint = (*fi);
			constraint->setShowReactionForce(flag);
		}
	}
}

void CFemGrid2::setAverageStress(bool flag)
{
	m_averageStress = flag;
}

bool CFemGrid2::getAverageStress()
{
	return m_averageStress;
}

void CFemGrid2::setStressStep(int step)
{
	m_stressStep = step;
}

int CFemGrid2::getStressStep()
{
	return m_stressStep;
}

void CFemGrid2::updatePixelArea()
{
	CImage* image = this->getImage();
	int i, j;

	double sumF;
	GLubyte red, green, blue;
	double value;
	int width, height;

	sumF = 0.0;

	if (image!=NULL)
	{
		width = image->getWidth();
		height = image->getHeight();

		for (j=0; j<height; j++)
		{
			for (i=0; i<width; i++) 
			{
				image->getValue(i, j, 0, red);
				image->getValue(i, j, 1, green);
				image->getValue(i, j, 2, blue);

				value = (double)(255-red) + (double)(255-green) + (double)(255-blue);

				if (value>0.0)
				{
					sumF+=255.0;
				}
			}
		}

		m_pixelArea = sumF/255.0;
	}
}

int CFemGrid2::getElementCount()
{
	int rows, cols;
	int i, j;
	double value;

	int nElements = 0;

	this->getGridSize(rows, cols);

	for (i=0; i<rows; i++)
	{
		for (j=0; j<cols; j++)
		{
			value = this->getGridValue(i, j);	
			if (value>m_elementTreshold)
				nElements++;
		}
	}

	return nElements;
}

void CFemGrid2::setDrawStress(bool flag)
{
	m_drawStress = flag;
}

bool CFemGrid2::getDrawStress()
{
	return m_drawStress;
}

void CFemGrid2::setDrawDisplacements(bool flag)
{
	m_drawDisplacements = flag;
}

bool CFemGrid2::getDrawDisplacements()
{
	return m_drawDisplacements;
}

void CFemGrid2::setColorMap(CColorMap* colorMap)
{
	m_colorMap = colorMap;
}

CColorMap* CFemGrid2::getColorMap()
{
	return m_colorMap;
}

void CFemGrid2::setElementTreshold(double value)
{
	m_elementTreshold = value;
}

double CFemGrid2::getElementTreshold()
{
	return m_elementTreshold;
}

void CFemGrid2::setMaxMisesStressValue(double value)
{
	m_maxMisesStressValue = value;
}

void CFemGrid2::setStressType(TStressType stressType)
{
	m_stressType = stressType;
}

void CFemGrid2::setUpperMisesTreshold(double upper)
{
	m_upperMisesTreshold = upper;
}

void CFemGrid2::setDrawForcesAndConstraints(bool flag)
{
	m_drawForcesAndConstraints = flag;
}

bool CFemGrid2::getDrawForcesAndConstraints()
{
	return m_drawForcesAndConstraints;
}

void CFemGrid2::setDimmedConstraints(bool flag)
{
	m_dimmedConstraints = flag;
}

bool CFemGrid2::getDimmedConstraints()
{
	return m_dimmedConstraints;
}

void CFemGrid2::setUndeformedGrid(bool flag)
{
	m_undeformedGrid = flag;
}

bool CFemGrid2::getUndeformedGrid()
{
	return m_undeformedGrid;
}

void CFemGrid2::setShowDensity(bool flag)
{
	m_drawDensity = flag;
}

bool CFemGrid2::getShowDensity()
{
	return m_drawDensity;
}


void CFemGrid2::assignNonElements(Matrix& M, double value)
{
	int rows, cols, i, j;

	this->getGridSize(rows, cols);

	if ((M.ncols() == cols)&&(M.nrows() == rows))
	{
		for (i=0; i<rows; i++)
			for (j=0; j<cols; j++)
			{
				if (this->getGridValue(i,j)<m_elementTreshold)
					M(i+1,j+1) = value;
			}
	}
}
