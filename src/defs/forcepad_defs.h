#ifndef FORCEPAD_DEFS
#define FORCEPAD_DEFS

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cmath>
#include <string>
#include <vector>
#include <map>

#define USE_NEWMAT

#ifdef USE_NEWMAT
#include <include.h>
#include <newmat.h>
#include <newmatio.h>
#include <newmatap.h>
#endif

#ifdef USE_EIGEN
#include <eigen3/Eigen/Dense>
#ifndef EIGEN_REDEFS
#define EIGEN_REDEFS
typedef Eigen::Matrix<double, 1, Eigen::Dynamic> RowVector;
typedef Eigen::Matrix<double, Eigen::Dynamic, 1> ColumnVector;
typedef Eigen::MatrixXd Matrix;
#endif
#endif

#endif