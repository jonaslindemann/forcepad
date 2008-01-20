#include "RFemBase.h"
#include "Node3d.h"
#include "Plani4.h"
#include "Plani4Mesh2d.h"

using namespace std;

int main(int argc, char* argv[])
{
	cout << "Creating mesh..." << endl;

	CPlani4Mesh2dPtr mesh = new CPlani4Mesh2d(2, 2, 1.0, 1.0);

	cout << "Setting up boundary conditions." << endl;

	mesh->constrainNode(0, 0, 1, 0.0);
	mesh->constrainNode(0, 0, 2, 0.0);
	mesh->constrainNode(mesh->getRows(), 0, 1, 0.0);
	mesh->constrainNode(mesh->getRows(), 0, 2, 0.0);
	mesh->applyForceNode(0, mesh->getCols(), 0.0, -1e4);

	cout << "Enumerating dofs... ";

	mesh->enumerateDofs(1);
	
	cout << "Total number of dofs = " << mesh->getTotalDofs() << endl;

	cout << "Setting up material properties." << endl;

	mesh->setYoungsModulus(2.1e9);
	mesh->setPoissonsRatio(0.35);
	mesh->setThickness(0.1);

	cout << "Setting up system matrix." << endl;

	mesh->assembleGlobalStiffnessMatrix();
	mesh->assembleForceVector();

	SymmetricBandMatrix K = mesh->getGlobalStiffnessMatrix();
	ColumnVector f = mesh->getForceVector();

	using namespace std;

	cout << "Writing global stiffness matrix to disk." << endl;
	
	fstream cf;
	cf.open("calfem.m", ios::out);

	Matrix fullK = K;
	calfem::writeMatrix("K", fullK, cf);
	calfem::writeColVector("f", f, cf);

	cout << "Solve equation system." << endl;

	//BandLUMatrix LU;
	LinearEquationSolver LU = fullK;
	ColumnVector a;

	a = LU.i() * f;

	cout << fullK.determinant() << endl;

	calfem::writeColVector("a", a, cf);

	cf.close();
	return 0;
}

