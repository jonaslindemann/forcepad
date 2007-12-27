#ifndef _CPlani4_H_
#define _CPlani4_H_

#include "Element.h"
#include "calfem.h"

SmartPointer(CPlani4);

class CPlani4 : public CElement {
public:
	enum TElementType { ET_PLANE_STRESS, ET_PLANE_STRAIN };
	enum TIntegrationScheme { IS_GAUSS_1, IS_GAUSS_4, IS_GAUSS_9 }; 
private:
	TElementType m_elementType;
	double m_thickness;
	TIntegrationScheme m_integrationScheme;
	Matrix m_D;
public:
	CPlani4 ();
	virtual ~CPlani4 ();

	ClassInfo("CPlani4",CElement);

	void setElementType(TElementType elementType);
	void setThickness(double thickness);
	void setIntegrationScheme(TIntegrationScheme scheme);
	void setConstitutiveMatrix(Matrix& D);

	virtual ReturnMatrix getStiffnessMatrix();
};

#endif
