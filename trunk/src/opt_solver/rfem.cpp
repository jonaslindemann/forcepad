#include "RFemBase.h"
#include "Node3d.h"
#include "Plani4.h"
#include "Plani4Mesh2d.h"

using namespace std;

int main(int argc, char* argv[])
{
	cout << "Creating mesh..." << endl;

	CPlani4Mesh2dPtr mesh = new CPlani4Mesh2d(10, 10, 1.0, 1.0);

	cout << "Setting up boundary conditions." << endl;

	mesh->constrainNode(0, 0, 1, 0.0);
	mesh->constrainNode(0, 0, 2, 0.0);
	mesh->constrainNode(0, mesh->getRows(), 2, 0.0);
	mesh->applyForceNode(mesh->getRows(), mesh->getCols() % 2, 0.0, -1e4);

	cout << "Enumerating dofs... ";

	int dofCount = mesh->enumerateDofs(1);

	cout << "Total number of dofs = " << dofCount << endl << endl;

	cout << "Setting up material properties." << endl;

	mesh->setYoungsModulus(2.1e9);
	mesh->setPoissonsRatio(0.35);
	mesh->setThickness(0.1);

	cout << "Setting up system matrix." << endl;

	mesh->assembleGlobalStiffnessMatrix();

	Matrix K = mesh->getGlobalStiffnessMatrix();

	using namespace std;

	cout << "Writing global stiffness matrix to disk." << endl;
	
	fstream cf;
	cf.open("calfem.m", ios::out);
	calfem::writeMatrix("K", K, cf);
	cf.close();


	return 0;
}

