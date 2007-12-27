#include "RFemBase.h"
#include "Node3d.h"
#include "Plani4.h"
#include "Plani4Mesh2d.h"

using namespace std;

int main(int argc, char* argv[])
{
	cout << "Creating mesh..." << endl;

	CPlani4Mesh2dPtr mesh = new CPlani4Mesh2d(200, 200, 1.0, 1.0);

	cout << "Done." << endl;
	cout << "Enumerating dofs..." << endl;

	int dofCount = mesh->enumerateDofs(1);

	cout << "Done." << endl;
	cout << "Total number of dofs = " << dofCount << endl << endl;

	mesh->constrainNode(0, 0, 1, 0.0);
	mesh->constrainNode(0, 0, 2, 0.0);
	mesh->constrainNode(0, mesh->getRows(), 2, 0.0);

	mesh->applyForceNode(mesh->getRows(), mesh->getCols() % 2, 0.0, -1e4);

	TConstraintMap& constraints = mesh->getConstraints();
	TConstraintMapIterator ci;

	cout << "Constrained dofs." << endl;

	for (ci=constraints.begin(); ci!=constraints.end(); ci++)
		cout << ci->first << " = " << ci->second << std::endl;

	TForceMap& forces = mesh->getForces();
	TForceMapIterator fi;

	cout << "Force dofs." << endl;

	for (fi=forces.begin(); fi!=forces.end(); fi++)
		cout << fi->first << " = " << fi->second << std::endl;

	Matrix D;
	calfem::hooke(1, 2.1e9, 0.3, D);

	return 0;
}

