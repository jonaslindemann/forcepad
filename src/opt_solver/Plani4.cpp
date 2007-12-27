#include "Plani4.h"

#include "calfem.h"

// ------------------------------------------------------------
CPlani4::CPlani4 ()
		:CElement()
{
	this->setNodeCount(4);
	this->setNodeDofSize(2);
}

// ------------------------------------------------------------
CPlani4::~CPlani4 ()
{

}

// ------------------------------------------------------------
void CPlani4::setElementType(TElementType elementType)
{
	m_elementType = elementType;
}

// ------------------------------------------------------------
void CPlani4::setThickness(double thickness)
{
	m_thickness = thickness;
}

// ------------------------------------------------------------
void CPlani4::setIntegrationScheme(TIntegrationScheme scheme)
{
	m_integrationScheme = scheme;
}

// ------------------------------------------------------------
void CPlani4::setConstitutiveMatrix(Matrix& D)
{
	m_D = D;
}

// ------------------------------------------------------------
ReturnMatrix CPlani4::getStiffnessMatrix()
{
//void plani4e(
//			RowVector &ex, 
//			RowVector &ey,
//			RowVector &ep,
//			Matrix &D,
//			RowVector &eq,
//			Matrix &Ke,
//			ColumnVector &fe)
	RowVector ex(4);
	RowVector ey(4);
	RowVector ep(3);

	//ptype t ir

	double ptype, ir;

	if (m_integrationScheme == IS_GAUSS_1)
		ir = 1.0;
	else if (m_integrationScheme == IS_GAUSS_4)
		ir = 2.0;
	else
		ir = 3.0;

	if (m_elementType == ET_PLANE_STRESS)
		ptype = 1.0;
	else
		ptype = 2.0;

	ep << ptype << m_thickness << ir;

	ex << this->getNode(1)->getX() << this->getNode(2)->getX() << this->getNode(3)->getX() << this->getNode(4)->getX();
	ey << this->getNode(1)->getY() << this->getNode(2)->getY() << this->getNode(3)->getY() << this->getNode(4)->getY();

	RowVector eq(2);
	eq = 0.0;

	Matrix Ke(8,8);
	ColumnVector fe(8);

	calfem::plani4e(ex, ey, ep, m_D, eq, Ke, fe);

	Ke.release(); return Ke;
}

