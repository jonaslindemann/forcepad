#ifndef _CPlani4Mesh2d_h_
#define _CPlani4Mesh2d_h_

#include "Mesh2d.h"
#include "Plani4.h"

SmartPointer(CPlani4Mesh2d);

class CPlani4Mesh2d : public CMesh2d {
private:
	Matrix m_D;
	double m_E;
	double m_v;
	double m_t;
	Matrix m_Ke;
	ColumnVector m_fe;
	CPlani4Ptr m_plani4;
	CPlani4::TElementType m_elementType;
	CPlani4::TIntegrationScheme m_integrationScheme;
	void updateDMatrix(double relativeStiffness=1.0);
	void updateElement();
public:
	CPlani4Mesh2d(unsigned int rows, unsigned int cols, double width, double height);
	virtual ~CPlani4Mesh2d();

	void setYoungsModulus(double E);
	void setPoissonsRatio(double v);
	void setThickness(double t);

	virtual ReturnMatrix onCreateElementMatrix(TNodeList& nodes, double relativeStiffness);
	virtual int onGetElementMatrixSize();

};

#endif