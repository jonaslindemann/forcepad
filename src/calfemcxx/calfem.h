 //
// CALFEM C++ libray
// Copyright (C) 1999-2009 Division of Structural Mecahnics, Lund University
//
// Written by Jonas Lindemann
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// Comments and suggestions to jonas.lindemann@byggmek.lth.se

#ifndef _calfem_h_
#define _calfem_h_

#include "forcepad_defs.h"

#ifdef CALFEM_DEBUG_OUTPUT
#define PrintMatrix(A) calfem::printMatrix(#A, A);
#else
#define PrintMatrix(A)
#endif

namespace calfem {
    
#ifdef USE_NEWMAT
    
    void hooke(int ptype, double E, double v, Matrix &D);
    
    void plante(
                RowVector &ex,
                RowVector &ey,
                RowVector &ep,
                Matrix &D,
                RowVector &eq,
                Matrix &Ke,
                ColumnVector &fe);
    
    void plants(
                RowVector &ex,
                RowVector &ey,
                RowVector &ep,
                Matrix &D,
                RowVector &ed,
                RowVector &es,
                RowVector &et);
    
    void bar2e(
               RowVector &ex,
               RowVector &ey,
               RowVector &ep,
               Matrix &Ke);
    
    void bar2s(
               RowVector &ex,
               RowVector &ey,
               RowVector &ep,
               RowVector &ed,
               double &es);
    
    
    void beam3e(
                RowVector &ex,
                RowVector &ey,
                RowVector &ez,
                RowVector &eo,
                RowVector &ep,
                RowVector &eq,
                Matrix &Ke,
                ColumnVector &fe);
    
    void beam3s(
                RowVector &ex,
                RowVector &ey,
                RowVector &ez,
                RowVector &eo,
                RowVector &ep,
                RowVector &ed,
                RowVector &eq,
                int n,
                Matrix &es,
                Matrix &edi,
                ColumnVector &eci);
    
    void flw2ts(
                RowVector &ex,
                RowVector &ey,
                Matrix &D,
                RowVector &ed,
                RowVector &es,
                RowVector &et);
    
    void flw2te(
                RowVector &ex,
                RowVector &ey,
                double ep,
                Matrix &D,
                double eq,
                Matrix &Ke,
                ColumnVector &fe);
    
    void plani4e(
                 RowVector &ex,
                 RowVector &ey,
                 RowVector &ep,
                 Matrix &D,
                 RowVector &eq,
                 Matrix &Ke,
                 ColumnVector &fe);
    
    void plani4s(
                 RowVector &ex,
                 RowVector &ey,
                 RowVector &ep,
                 Matrix &D,
                 RowVector &ed,
                 Matrix &es,
                 Matrix &et);
    
    void assem(
               RowVector &Topo,
               SymmetricBandMatrix &K,
               Matrix &Ke,
               ColumnVector &f,
               ColumnVector &fe);
    
    void assemSystem(
                     RowVector &Topo,
                     SimpleIntArray& dofMapping,
                     SymmetricBandMatrix &K,
                     Matrix &Ke,
                     ColumnVector &f,
                     ColumnVector &fe);
    
    void assem(
               RowVector &Topo,
               SymmetricBandMatrix &K,
               Matrix &Ke);
    
    ReturnMatrix matrixMax1(Matrix A, Matrix B);
    ReturnMatrix matrixMax2(double a, Matrix B);
    ReturnMatrix matrixMax3(Matrix A, double b);
    ReturnMatrix matrixMin1(Matrix A, Matrix B);
    ReturnMatrix matrixMin2(double a, Matrix B);
    ReturnMatrix matrixMin3(Matrix A, double b);
    ReturnMatrix matrixSqrt(Matrix A);
    ReturnMatrix elementMultiply(Matrix A, Matrix B);
    void assignWhereLessThan(Matrix& toMatrix, Matrix& condMatrix, Matrix& valueMatrix, double lessThanValue);
    void assignWhereGreaterThan(Matrix& toMatrix, Matrix& condMatrix, Matrix& valueMatrix, double greaterThanValue);
    void assignWhereGreaterThan(Matrix& toMatrix, Matrix& condMatrix, double value, double greaterThanValue);
    
    double maxValue(RowVector& rowVector);
    double minValue(RowVector& rowVector);
    void printMatrix(std::string name, Matrix & A);
    
    void writeRowVector(const std::string& name, RowVector &v, std::ostream &out, bool matlab=true);
    void writeColVector(const std::string& name, ColumnVector &v, std::ostream &out, bool matlab=true);
    void writeMatrix(const std::string& name, Matrix &m, std::ostream &out, bool matlab=true);
    void writeMatrix(const std::string& name, SymmetricBandMatrix &m, std::ostream &out, bool matlab=true);
#endif
    
#ifdef USE_EIGEN
    void hooke(int ptype, double E, double v, Matrix &D);
    
    void plante(
                RowVector &ex,
                RowVector &ey,
                RowVector &ep,
                Matrix &D,
                RowVector &eq,
                Matrix &Ke,
                ColumnVector &fe);
    
    void plants(
                RowVector &ex,
                RowVector &ey,
                RowVector &ep,
                Matrix &D,
                RowVector &ed,
                RowVector &es,
                RowVector &et);
    
    void bar2e(
               RowVector &ex,
               RowVector &ey,
               RowVector &ep,
               Matrix &Ke);
    
    void bar2s(
               RowVector &ex,
               RowVector &ey,
               RowVector &ep,
               RowVector &ed,
               double &es);
    
    
    void beam3e(
                RowVector &ex,
                RowVector &ey,
                RowVector &ez,
                RowVector &eo,
                RowVector &ep,
                RowVector &eq,
                Matrix &Ke,
                ColumnVector &fe);
    
    void beam3s(
                RowVector &ex,
                RowVector &ey,
                RowVector &ez,
                RowVector &eo,
                RowVector &ep,
                RowVector &ed,
                RowVector &eq,
                int n,
                Matrix &es,
                Matrix &edi,
                ColumnVector &eci);
    
    void flw2ts(
                RowVector &ex,
                RowVector &ey,
                Matrix &D,
                RowVector &ed,
                RowVector &es,
                RowVector &et);
    
    void flw2te(
                RowVector &ex,
                RowVector &ey,
                double ep,
                Matrix &D,
                double eq,
                Matrix &Ke,
                ColumnVector &fe);
    
    void plani4e(
                 RowVector &ex,
                 RowVector &ey,
                 RowVector &ep,
                 Matrix &D,
                 RowVector &eq,
                 Matrix &Ke,
                 ColumnVector &fe);
    
    void plani4s(
                 RowVector &ex,
                 RowVector &ey,
                 RowVector &ep,
                 Matrix &D,
                 RowVector &ed,
                 Matrix &es,
                 Matrix &et);
    
    void assem(
               RowVector &Topo,
               SymmetricBandMatrix &K,
               Matrix &Ke,
               ColumnVector &f,
               ColumnVector &fe);
    
    void assemSystem(
                     RowVector &Topo,
                     SimpleIntArray& dofMapping,
                     SymmetricBandMatrix &K,
                     Matrix &Ke,
                     ColumnVector &f,
                     ColumnVector &fe);
    
    void assem(
               RowVector &Topo,
               SymmetricBandMatrix &K,
               Matrix &Ke);
    
    ReturnMatrix matrixMax1(Matrix A, Matrix B);
    ReturnMatrix matrixMax2(double a, Matrix B);
    ReturnMatrix matrixMax3(Matrix A, double b);
    ReturnMatrix matrixMin1(Matrix A, Matrix B);
    ReturnMatrix matrixMin2(double a, Matrix B);
    ReturnMatrix matrixMin3(Matrix A, double b);
    ReturnMatrix matrixSqrt(Matrix A);
    ReturnMatrix elementMultiply(Matrix A, Matrix B);
    void assignWhereLessThan(Matrix& toMatrix, Matrix& condMatrix, Matrix& valueMatrix, double lessThanValue);
    void assignWhereGreaterThan(Matrix& toMatrix, Matrix& condMatrix, Matrix& valueMatrix, double greaterThanValue);
    void assignWhereGreaterThan(Matrix& toMatrix, Matrix& condMatrix, double value, double greaterThanValue);
    
    double maxValue(RowVector& rowVector);
    double minValue(RowVector& rowVector);
    void printMatrix(std::string name, Matrix & A);
    
    void writeRowVector(const std::string& name, RowVector &v, std::ostream &out, bool matlab=true);
    void writeColVector(const std::string& name, ColumnVector &v, std::ostream &out, bool matlab=true);
    void writeMatrix(const std::string& name, Matrix &m, std::ostream &out, bool matlab=true);
    void writeMatrix(const std::string& name, SymmetricBandMatrix &m, std::ostream &out, bool matlab=true);
#endif
    
}
#endif
