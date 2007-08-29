#ifndef _MatlabGen_h_
#define _MatlabGen_h_

#include "calfem.h"
#include "matlabgen.h"

#include <iostream>

void writeRowVector(const char* name, RowVector &v, std::ostream &out);
void writeColVector(const char* name, ColumnVector &v, std::ostream &out);
void writeMatrix(const char* name, Matrix &m, std::ostream &out);

#endif