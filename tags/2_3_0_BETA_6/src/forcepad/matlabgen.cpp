#include "matlabgen.h"

void writeRowVector(const char* name, RowVector &v, std::ostream &out)
{
	out << name << " = [" << std::endl;

	int i;

	for (i=1; i<=v.Ncols(); i++)
		out << v(i) << std::endl;

	out << "]';" << endl;
}

void writeColVector(const char* name, ColumnVector &v, std::ostream &out)
{
	out << name << " = [" << std::endl;

	int i;

	for (i=1; i<=v.Ncols(); i++)
		out << v(i) << std::endl;

	out << "];" << endl;
}

void writeMatrix(const char* name, Matrix &m, std::ostream &out)
{
	out << name << " = [" << std::endl;

	int i, j;

	for (i=1; i<=m.Nrows(); i++)
	{
		for (j=1; j<=m.Ncols(); j++)
			out << m(i,j) << " ";

		out << ";" << std::endl;
	}

	out << "];" << endl;
}


