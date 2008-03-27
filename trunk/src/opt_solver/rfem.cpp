#include "RFemBase.h"
#include "Node3d.h"
#include "Plani4.h"
#include "Plani4Mesh2d.h"

using namespace std;

int main(int argc, char* argv[])
{
	cout << "Creating mesh..." << endl;

	//   m_rows = 2, m_cols = 2
	//
	//    x,x    11,12  13,14
	//   o------o------o
	//   |      |      |
	//   |5,6   |7,8   |9,10
	//   o------o------o
	//   |      |      |
	//   |x,x   |1,2   |3,4
	//   o------o------o

	CPlani4Mesh2dPtr mesh = new CPlani4Mesh2d(10, 10, 1.0, 1.0);

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
	Matrix topo = mesh->getTopoMatrix();
	Matrix Ke = mesh->getElementMatrix();
	Matrix coord = mesh->getCoordMatrix();
	Matrix dof = mesh->getDofMatrix();

	using namespace std;

	cout << "Writing global stiffness matrix to disk." << endl;
	
	fstream cf;
	cf.open("c:\\users\\jonas\\documents\\development\\forcepad\\src\\opt_solver\\matlab\\calfem.m", ios::out);

	Matrix fullK = K;
	calfem::writeMatrix("K", fullK, cf);
	calfem::writeColVector("f", f, cf);
	calfem::writeMatrix("topo", topo, cf);
	calfem::writeMatrix("coord", coord, cf);
	calfem::writeMatrix("dof", dof, cf);
	calfem::writeMatrix("Ke", Ke, cf);

	cout << "Solve equation system." << endl;

	BandLUMatrix LU = K;
	ColumnVector a = LU.i() * f;

	calfem::writeColVector("a", a, cf);

	cf.close();
	return 0;
}

