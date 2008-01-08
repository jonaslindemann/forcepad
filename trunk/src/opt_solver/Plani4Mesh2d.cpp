#include "Plani4Mesh2d.h"

CPlani4Mesh2d::CPlani4Mesh2d(unsigned int rows, unsigned int cols, double width, double height)
:CMesh2d(rows, cols, width, height, 2)
{
	m_E = 2.1e6;
	m_v = 0.35;
	m_t = 1.0;

	m_elementType = CPlani4::ET_PLANE_STRESS;
	m_integrationScheme = CPlani4::IS_GAUSS_1;

	m_plani4 = new CPlani4();

	m_Ke.resize(8,8); m_Ke = 0.0;
	m_fe.resize(8);   m_fe = 0.0;
	
	updateDMatrix();
	updateElement();
}

CPlani4Mesh2d::~CPlani4Mesh2d()
{

}

void CPlani4Mesh2d::updateElement()
{
	m_plani4->getNode(1)->setComponents(0.0, 0.0);
	m_plani4->getNode(2)->setComponents(this->getElementWidth(), 0.0);
	m_plani4->getNode(3)->setComponents(this->getElementWidth(), this->getElementHeight());
	m_plani4->getNode(4)->setComponents(0.0, this->getElementHeight());
	m_plani4->setThickness(m_t);
	m_plani4->setElementType(m_elementType);
	m_plani4->setIntegrationScheme(m_integrationScheme);
	m_plani4->setConstitutiveMatrix(m_D);
}

void CPlani4Mesh2d::updateDMatrix(double relativeStiffness)
{
	int ptype;

	if (m_elementType == CPlani4::ET_PLANE_STRESS)
	{
		ptype = 1;
		m_D.resize(3,3); m_D = 0.0;
	}
	else
	{
		ptype = 2;
		m_D.resize(4,4); m_D = 0.0;
	}

	calfem::hooke(ptype, m_E*relativeStiffness, m_v, m_D);
}

void CPlani4Mesh2d::setYoungsModulus(double E)
{
	m_E = E;
	this->updateDMatrix();
}

void CPlani4Mesh2d::setPoissonsRatio(double v)
{
	m_v = v;
}

void CPlani4Mesh2d::setThickness(double t)
{
	m_t = t;
}

ReturnMatrix CPlani4Mesh2d::onCreateElementMatrix(TNodeList& nodes, double relativeStiffness)
{
	m_plani4->setNode(1, nodes[0]);
	m_plani4->setNode(2, nodes[1]);
	m_plani4->setNode(3, nodes[2]);
	m_plani4->setNode(4, nodes[3]);
	this->updateDMatrix(relativeStiffness);
	m_Ke = m_plani4->getStiffnessMatrix();
	m_Ke.release(); return m_Ke;
}

int CPlani4Mesh2d::onGetElementMatrixSize()
{
	return 8;
}


