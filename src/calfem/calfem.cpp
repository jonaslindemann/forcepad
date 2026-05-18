//
// CALFEM C++ libray
// Copyright (C) 1999-2023 Division of Structural Mecahnics, Lund University
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

#include "calfem.h"

namespace calfem {

void bar3e(const RowVec &ex, const RowVec &ey, const RowVec &ez, const RowVec &ep, double eq, Matrix &Ke, ColVec &fe)
{
    double E = ep(0);
    double A = ep(1);

    ColVec b(3);
    RowVec n(3);
    Matrix G(2, 6);
    Matrix Kle(2, 2);
    ColVec fle(2);

    b << ex(1) - ex(0), ey(1) - ey(0), ez(1) - ez(0);

    double L = sqrt((b.transpose() * b));

    n = b.transpose() / L;

    G << n(0), n(1), n(2), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, n(0), n(1), n(2);

    double a = E * A / L;

    Kle << a, -a, -a, a;

    a = eq * L / 2.0;

    fle << a, a;

    Ke = G.transpose() * Kle * G;
    fe = G.transpose() * fle;
}

void bar3s(const RowVec &ex, const RowVec &ey, const RowVec &ez, const RowVec &ep, const RowVec &ed, double eq, int n,
           ColVec &es, ColVec &edi, ColVec &eci)
{
    double E = ep(0);
    double A = ep(1);

    ColVec b(3);
    RowVec nn(3);
    Matrix G(2, 6);
    Matrix Kle(2, 2);
    ColVec fle(2);

    b << ex(1) - ex(0), ey(1) - ey(0), ez(1) - ez(0);

    double L = sqrt((b.transpose() * b));

    nn = b.transpose() / L;

    G << nn(0), nn(1), nn(2), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, nn(0), nn(1), nn(2);

    double c1 = E * A / L;

    Kle << c1, -c1, -c1, c1;

    c1 = eq * L / 2.0;

    fle << c1, c1;

    ColVec ae = ed.transpose();
    ColVec ale = G * ae;

    RowVec B(2);

    B << -1.0 / L, 1.0 / L;

    for (int i = 1; i <= n; i++)
    {
        eci(i - 1) = (i - 1.0) * L / (n - 1.0);
        double x = eci(i - 1);
        double up = -eq * (0.5 * pow(x, 2) - 0.5 * L * x) / E / A;
        double Np = -eq * (x - 0.5 * L);

        RowVec N(2);
        N << 1.0 - x / L, x / L;

        edi(i - 1) = N * ale + up; // [1 x 2] * [2 x 1]
        es(i - 1) = E * A * B * ale + Np;
        eci(i - 1) = x;
    }
}

void beam3e(const RowVec &ex, const RowVec &ey, const RowVec &ez, const RowVec &eo, const RowVec &ep, const RowVec &eq,
            Matrix &Ke, ColVec &fe)
{
    ColVec bb(3);
    bb << ex(1) - ex(0), ey(1) - ey(0), ez(1) - ez(0);

    double L = sqrt((bb.transpose() * bb));

    RowVec n1 = (bb / L).transpose();

    double lc = sqrt((eo * eo.transpose()));
    RowVec n3 = eo / lc;

    double qx = eq(0);
    double qy = eq(1);
    double qz = eq(2);
    double qw = eq(3);

    double E = ep(0);
    double Gs = ep(1);
    double A = ep(2);
    double Iy = ep(3);
    double Iz = ep(4);
    double Kv = ep(5);

    double a = E * A / L;
    double b = 12.0 * E * Iz / pow(L, 3);
    double c = 6.0 * E * Iz / pow(L, 2);
    double d = 12.0 * E * Iy / pow(L, 3);
    double e = 6.0 * E * Iy / pow(L, 2);
    double f = Gs * Kv / L;
    double g = 2.0 * E * Iy / L;
    double h = 2 * E * Iz / L;

    Matrix Kle(12, 12);

    Kle << a, 0, 0, 0, 0, 0, -a, 0, 0, 0, 0, 0, 0, b, 0, 0, 0, c, 0, -b, 0, 0, 0, c, 0, 0, d, 0, -e, 0, 0, 0, -d, 0, -e,
        0, 0, 0, 0, f, 0, 0, 0, 0, 0, -f, 0, 0, 0, 0, -e, 0, 2 * g, 0, 0, 0, e, 0, g, 0, 0, c, 0, 0, 0, 2 * h, 0, -c, 0,
        0, 0, h, -a, 0, 0, 0, 0, 0, a, 0, 0, 0, 0, 0, 0, -b, 0, 0, 0, -c, 0, b, 0, 0, 0, -c, 0, 0, -d, 0, e, 0, 0, 0, d,
        0, e, 0, 0, 0, 0, -f, 0, 0, 0, 0, 0, f, 0, 0, 0, 0, -e, 0, g, 0, 0, 0, e, 0, 2 * g, 0, 0, c, 0, 0, 0, h, 0, -c,
        0, 0, 0, 2 * h;

    ColVec fle(12);

    fle << qx, qy, qz, qw, -qz * L / 6.0, qy * L / 6.0, qx, qy, qz, qw, qz * L / 6.0, -qy * L / 6.0;

    fle = fle * L / 2.0;

    RowVec n2(3);

    n2 << n3(1) * n1(2) - n3(2) * n1(1), -n1(2) * n3(0) + n1(0) * n3(2), n3(0) * n1(1) - n1(0) * n3(1);

    // Use fixed-size matrix for stack allocation (eliminates heap overhead)
    Matrix3x3 An;

    An.row(0) = n1;
    An.row(1) = n2;
    An.row(2) = n3;

    // Use fixed-size matrix for stack allocation
    Matrix12x12 G = Matrix12x12::Zero();

    G.block(0, 0, 3, 3) = An;
    G.block(3, 3, 3, 3) = An;
    G.block(6, 6, 3, 3) = An;
    G.block(9, 9, 3, 3) = An;

    Ke = G.transpose() * Kle * G;
    fe = G.transpose() * fle;
}

void beam3s(const RowVec &ex, const RowVec &ey, const RowVec &ez, const RowVec &eo, const RowVec &ep, const RowVec &ed,
            const RowVec &eq, const int n, Matrix &es, Matrix &edi, ColVec &eci)
{
    ColVec bb(3);
    bb << ex(1) - ex(0), ey(1) - ey(0), ez(1) - ez(0);

    double L = sqrt((bb.transpose() * bb));

    RowVec n1 = (bb / L).transpose();

    double lc = sqrt((eo * eo.transpose()));
    RowVec n3 = eo / lc;

    double qx = eq(0);
    double qy = eq(1);
    double qz = eq(2);
    double qw = eq(3);

    double E = ep(0);
    double Gs = ep(1);
    double A = ep(2);
    double Iy = ep(3);
    double Iz = ep(4);
    double Kv = ep(5);

    RowVec n2(3);

    n2 << n3(1) * n1(2) - n3(2) * n1(1), -n1(2) * n3(0) + n1(0) * n3(2), n3(0) * n1(1) - n1(0) * n3(1);

    // Use fixed-size matrix for stack allocation (eliminates heap overhead)
    Matrix3x3 An;

    An.row(0) = n1;
    An.row(1) = n2;
    An.row(2) = n3;

    // Use fixed-size matrix for stack allocation
    Matrix12x12 G = Matrix12x12::Zero();

    G.block(0, 0, 3, 3) = An;
    G.block(3, 3, 3, 3) = An;
    G.block(6, 6, 3, 3) = An;
    G.block(9, 9, 3, 3) = An;

    // Use fixed-size vectors for stack allocation
    Vector12 u;
    Vector12 diffSol;

    diffSol << 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -qx * pow(L, 2) / 2.0 / E / A, qy * pow(L, 4) / 24.0 / E / Iz,
        qz * pow(L, 4) / 24.0 / E / Iy, -qw * pow(L, 2) / 2.0 / Gs / Kv, -qz * pow(L, 3) / 6.0 / E / Iy,
        qy * pow(L, 3) / 6.0 / E / Iz;

    // u = G*ed.AsColumn() - diffSol;
    u = G * ed.transpose() - diffSol;

    // Use fixed-size matrix for stack allocation
    Matrix12x12 C;

    C << 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, L, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, pow(L, 3),
        pow(L, 2), L, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, pow(L, 3), pow(L, 2), L, 1.0,
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, L, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        -3.0 * pow(L, 2), -2.0 * L, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3.0 * pow(L, 2), 2.0 * L, 1, 0.0, 0.0, 0.0, 0.0, 0.0,
        0, 0.0;

    Vector12 m;

    m = C.inverse() * u;

    es.resize(n, 6);
    es.setZero();

    edi.resize(n, 4);
    edi.setZero();

    eci.resize(n);
    eci.setZero();

    for (auto i = 0; i < n; i++)
    {
        eci(i) = double(i) * L / double(n - 1);
        double x = eci(i);

        // Use fixed-size matrices for stack allocation (critical for performance)
        Matrix6x12 T1;

        T1 << E * A, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -6.0 * E * Iz, 0.0, 0.0, 0.0, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -6.0 * E * Iy, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Gs * Kv, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -6.0 * E * Iy * x,
            -2.0 * E * Iy, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 6.0 * E * Iz * x, 2.0 * E * Iz, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
            0.0, 0.0;

        Vector6 T2;

        T2 << -qx * x, -qy * x, -qz * x, -qw * x, -qz * pow(x, 2) / 2.0, qy * pow(x, 2) / 2.0;

        es.row(i) = (T1 * m + T2).transpose();

        Matrix4x12 T3;

        T3 << x, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, pow(x, 3), pow(x, 2), x, 1.0, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, pow(x, 3), pow(x, 2), x, 1.0, 0.0, 0.0, 0.0, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, x, 1.0;

        Vector4 T4;

        T4 << -qx * pow(x, 2) / 2.0 / E / A, qy * pow(x, 4) / 24.0 / E / Iz, qz * pow(x, 4) / 24.0 / E / Iy,
            -qw * pow(x, 2) / 2.0 / Gs / Kv;

        edi.row(i) = (T3 * m + T4).transpose();
    }
}

void spy(const SpMatrix &matrix)
{
    auto rows = matrix.rows();
    auto cols = matrix.cols();

    for (auto i = 0; i < rows; i++)
    {
        for (auto j = 0; j < cols; j++)
        {
            if (abs(matrix.coeff(i, j)) > 0.0)
                std::cout << "o ";
            else
                std::cout << ". ";
        }
        std::cout << "\n";
    }
}

std::vector<Eigen::Index> toZeroIndex(const std::vector<Eigen::Index> &vec)
{
    std::vector<Eigen::Index> idx;

    for (auto &dof : vec)
        idx.push_back(dof - 1);

    return idx;
}

void extractAllDofs(Eigen::Index nDofs, const std::vector<Eigen::Index> &bcDofs, std::vector<Eigen::Index> &freeDofs,
                    std::vector<Eigen::Index> &allDofs)
{
    allDofs.clear();
    freeDofs.clear();

    for (auto i = 1; i <= nDofs; i++)
        allDofs.push_back(i);

    std::set_difference(allDofs.begin(), allDofs.end(), bcDofs.begin(), bcDofs.end(),
                        std::inserter(freeDofs, freeDofs.begin()));
}

void extractElDisp(const IntMatrix &edof, const ColVec &a, Matrix &ed)
{
    auto nElements = edof.rows();
    auto nDofs = edof.cols();

    ed.resize(nElements, nDofs);

    for (auto i = 0; i < nElements; i++)
    {
        IntColVec ind = edof.row(i).array() - 1;
        ed.row(i) = a(ind);
    }
}

void extractEdof(const IntMatrix &dofs, const Matrix &coords, const IntMatrix &topo, IntMatrix &edof, Matrix &ex,
                 Matrix &ey, Matrix &ez)
{
    auto dofsPerNode = dofs.cols();
    auto nElements = topo.rows();
    auto nodesPerElement = topo.cols();
    auto nNodes = coords.rows();

    edof.resize(nElements, nodesPerElement * dofsPerNode);
    edof.setZero();

    ex.resize(nElements, nodesPerElement);
    ex.setZero();

    ey.resize(nElements, nodesPerElement);
    ey.setZero();

    ez.resize(nElements, nodesPerElement);
    ez.setZero();

    for (auto i = 0; i < nElements; i++)
    {
        for (auto j = 0; j < nodesPerElement; j++)
        {
            auto k = topo(i, j);
            auto x = coords(k - 1, 0);
            auto y = coords(k - 1, 1);
            auto z = coords(k - 1, 2);
            ex(i, j) = x;
            ey(i, j) = y;
            ez(i, j) = z;
            edof.block(i, j * dofsPerNode, 1, dofsPerNode) = dofs.row(k - 1);
        }
    }
}

void assem(IntRowVec &Topo, Matrix &K, Matrix &Ke, ColVec &f, ColVec &fe)
{
    int i, j, r, c;

    for (i = 0; i < Ke.rows(); i++)
        for (j = i; j < Ke.cols(); j++)
        {
            r = Topo(i) - 1;
            c = Topo(j) - 1;

            K(r, c) = K(r, c) + Ke(i, j);
        }

    for (i = 0; i < fe.rows(); i++)
    {
        r = Topo(i) - 1;
        f(r) = f(r) + fe(i);
    }
}

bool solveq(const Matrix &K, const ColVec &f, const IntColVec &bcDofs, const ColVec &bcVals, ColVec &a, ColVec &Q)
{
    auto nDofs = K.rows();

    std::vector<Eigen::Index> allDofsVec;
    std::vector<Eigen::Index> freeDofsVec;
    std::vector<Eigen::Index> bcDofsVec;

    a.setZero();

    calfem::extractAllDofs(nDofs, bcDofsVec, freeDofsVec, allDofsVec);

    auto nFreeDofs = freeDofsVec.size();

    Matrix Ksys(nFreeDofs, nFreeDofs);
    Ksys.setZero();

    ColVec fsys(nFreeDofs);

    auto ind = calfem::toZeroIndex(freeDofsVec);
    auto pind = calfem::toZeroIndex(bcDofsVec);

    Ksys = K(ind, ind);
    fsys = f(ind) - K(ind, pind) * bcVals; // [ ind x 1 ] + [ ind x pind ] x [ pind ]

    ColVec asys = Ksys.llt().solve(fsys);

    a(ind) = asys;
    a(pind) = bcVals;

    Q = K * a - f;

    return true;
}

void spassem(const IntRowVec &Topo, TripletList &Ktriplets, const Matrix &Ke, ColVec &f, const ColVec &fe)
{
    int i, j, r, c;

    for (i = 0; i < Ke.rows(); i++)
    {
        r = Topo(i) - 1;
        if (r < 0) continue;
        for (j = 0; j < Ke.cols(); j++)
        {
            c = Topo(j) - 1;
            if (c < 0) continue;
            Ktriplets.push_back(Triplet(r, c, Ke(i, j)));
        }
    }

    for (i = 0; i < fe.rows(); i++)
    {
        r = Topo(i) - 1;
        if (r >= 0)
            f(r) = f(r) + fe(i);
    }
}

bool spsolveq(const SpMatrix &K, const ColVec &f, const IntColVec &bcDofs, const ColVec &bcVals, ColVec &a, ColVec &Q)
{
    auto nDofs = K.rows();

    std::vector<Eigen::Index> allDofsVec;
    std::vector<Eigen::Index> freeDofsVec;
    std::vector<Eigen::Index> bcDofsVec;

    a.setZero();

    for (auto i = 0; i < bcDofs.rows(); i++)
    {
        bcDofsVec.push_back(bcDofs(i));
        a(bcDofs(i) - 1) = bcVals(i);
    }

    calfem::extractAllDofs(nDofs, bcDofsVec, freeDofsVec, allDofsVec);

    auto nFreeDofs = freeDofsVec.size();
    auto nBcDofs = bcDofsVec.size();

    SpMatrix Ksys(nFreeDofs, nFreeDofs);
    SpMatrix Ksysf(nFreeDofs, nBcDofs);

    TripletList Ksyslist;
    TripletList Ksysflist;

    std::cout << "K non zeros = " << K.nonZeros() << "\n";
    std::cout << "K size = " << K.rows() << "\n";

    Ksyslist.reserve(K.nonZeros());
    Ksysflist.reserve(K.nonZeros());

    auto ind = calfem::toZeroIndex(freeDofsVec);
    auto pind = calfem::toZeroIndex(bcDofsVec);

    std::set<Eigen::Index> indSet(ind.begin(), ind.end());
    std::set<Eigen::Index> pindSet(pind.begin(), pind.end());

    std::map<Eigen::Index, Eigen::Index> indMap;
    std::map<Eigen::Index, Eigen::Index> pindMap;

    Eigen::Index i = 0;

    for (auto &idx : ind)
        indMap[idx] = i++;

    i = 0;

    for (auto &idx : pind)
        pindMap[idx] = i++;

    for (Eigen::Index k = 0; k < K.outerSize(); ++k)
        for (SpMatrix::InnerIterator it(K, k); it; ++it)
        {
            auto v = it.value();
            auto r = it.row();
            auto c = it.col();

            if ((indSet.find(r) != indSet.end()) && (indSet.find(c) != indSet.end()))
            {
                Ksyslist.push_back(Triplet(indMap[r], indMap[c], v));
            }

            if ((indSet.find(r) != indSet.end()) && (pindSet.find(c) != pindSet.end()))
            {
                Ksysflist.push_back(Triplet(indMap[r], pindMap[c], v));
            }
        }

    Ksys.setFromTriplets(Ksyslist.begin(), Ksyslist.end());
    Ksysf.setFromTriplets(Ksysflist.begin(), Ksysflist.end());

    std::cout << "Ksys non zeros = " << Ksys.nonZeros() << "\n";
    std::cout << "Ksys size = " << Ksys.rows() << "\n";

    ColVec fsys(nFreeDofs);

    fsys = f(ind) - Ksysf * bcVals;

    std::cout << fsys.maxCoeff() << "\n";
    std::cout << f.maxCoeff() << "\n";

    // Eigen::SparseLU<SpMatrix> solver;
    Eigen::SimplicialLLT<SpMatrix> solver;

    solver.compute(Ksys);

    if (solver.info() != Eigen::Success)
    {
        std::cout << "Solver failed.\n";
        return false;
    }
    Eigen::VectorXd asys = solver.solve(fsys);

    a(ind) = asys;
    a(pind) = bcVals;

    Q = K * a - f;

    return true;
}

SparseSolver::SparseSolver()
{}

std::shared_ptr<SparseSolver> SparseSolver::create()
{
    return std::make_shared<SparseSolver>();
}

bool SparseSolver::setup(const SpMatrix &K, const IntColVec &bcDofs, const ColVec &bcVals)
{
    m_K = &K;
    m_bcDofs = &bcDofs;
    m_bcVals = &bcVals;

    m_nDofs = K.rows();

    m_allDofsVec.clear();
    m_freeDofsVec.clear();
    m_bcDofsVec.clear();

    m_a.resize(m_nDofs);
    m_a.setZero();

    for (auto i = 0; i < bcDofs.rows(); i++)
    {
        m_bcDofsVec.push_back(bcDofs(i));
        m_a(bcDofs(i) - 1) = bcVals(i);
    }

    calfem::extractAllDofs(m_nDofs, m_bcDofsVec, m_freeDofsVec, m_allDofsVec);

    m_nFreeDofs = m_freeDofsVec.size();
    auto nBcDofs = m_bcDofsVec.size();

    m_Ksys.resize(m_nFreeDofs, m_nFreeDofs);
    m_Ksysf.resize(m_nFreeDofs, nBcDofs);

    m_Ksyslist.clear();
    m_Ksysflist.clear();

    std::cout << "K non zeros = " << K.nonZeros() << "\n";
    std::cout << "K size = " << K.rows() << "\n";

    m_Ksyslist.reserve(K.nonZeros());
    m_Ksysflist.reserve(K.nonZeros());

    m_ind = calfem::toZeroIndex(m_freeDofsVec);
    m_pind = calfem::toZeroIndex(m_bcDofsVec);

    std::set<Eigen::Index> indSet(m_ind.begin(), m_ind.end());
    std::set<Eigen::Index> pindSet(m_pind.begin(), m_pind.end());

    std::map<Eigen::Index, Eigen::Index> indMap;
    std::map<Eigen::Index, Eigen::Index> pindMap;

    Eigen::Index i = 0;

    for (auto &idx : m_ind)
        indMap[idx] = i++;

    i = 0;

    for (auto &idx : m_pind)
        pindMap[idx] = i++;

    for (int k = 0; k < (*m_K).outerSize(); ++k)
        for (SpMatrix::InnerIterator it((*m_K), k); it; ++it)
        {
            auto v = it.value();
            auto r = it.row();
            auto c = it.col();

            if ((indSet.find(r) != indSet.end()) && (indSet.find(c) != indSet.end()))
            {
                m_Ksyslist.push_back(Triplet(indMap[r], indMap[c], v));
            }

            if ((indSet.find(r) != indSet.end()) && (pindSet.find(c) != pindSet.end()))
            {
                m_Ksysflist.push_back(Triplet(indMap[r], pindMap[c], v));
            }
        }

    m_Ksys.setFromTriplets(m_Ksyslist.begin(), m_Ksyslist.end());
    m_Ksysf.setFromTriplets(m_Ksysflist.begin(), m_Ksysflist.end());

    std::cout << "Ksys non zeros = " << m_Ksys.nonZeros() << "\n";
    std::cout << "Ksys size = " << m_Ksys.rows() << "\n";

    return true;
}

bool SparseSolver::solve(const ColVec &f, ColVec &a, ColVec &Q)
{
    a.resize(m_nDofs);
    a.setZero();

    for (auto i = 0; i < (*m_bcDofs).rows(); i++)
    {
        m_bcDofsVec.push_back((*m_bcDofs)(i));
        a((*m_bcDofs)(i)-1) = (*m_bcVals)(i);
    }

    m_fsys.resize(m_nFreeDofs);

    m_fsys = f(m_ind) - m_Ksysf * (*m_bcVals);

    std::cout << m_fsys.maxCoeff() << "\n";
    std::cout << f.maxCoeff() << "\n";

    m_solver.compute(m_Ksys);
    if (m_solver.info() != Eigen::Success)
    {
        std::cout << "Solver failed.\n";
        return false;
    }
    m_asys = m_solver.solve(m_fsys);

    a(m_ind) = m_asys;
    a(m_pind) = (*m_bcVals);

    Q = (*m_K) * a - f;

    return true;
}

bool SparseSolver::recompute(const ColVec &f, ColVec &a, ColVec &Q)
{
    a.resize(m_nDofs);
    a.setZero();

    for (auto i = 0; i < (*m_bcDofs).rows(); i++)
    {
        m_bcDofsVec.push_back((*m_bcDofs)(i));
        a((*m_bcDofs)(i)-1) = (*m_bcVals)(i);
    }

    m_fsys.resize(m_nFreeDofs);

    m_fsys = f(m_ind) - m_Ksysf * (*m_bcVals);

    std::cout << m_fsys.maxCoeff() << "\n";
    //std::cout << (*m_f).maxCoeff() << "\n";

    if (m_solver.info() != Eigen::Success)
    {
        std::cout << "Solver failed.\n";
        return false;
    }

    m_asys = m_solver.solve(m_fsys);

    a(m_ind) = m_asys;
    a(m_pind) = (*m_bcVals);

    Q = (*m_K) * a - f;

    return true;
}

void hooke(int ptype, double E, double v, Matrix &D)
{
    double a;
    switch (ptype) {
    case 1:
        a = E / (1.0 - pow(v, 2));
        D.resize(3, 3);
        D << 1.0*a, v*a,   0.0,
             v*a,   1.0*a, 0.0,
             0.0,   0.0,   a*(1.0-v)/2.0;
        break;
    case 2:
        a = E / ((1.0+v) * (1.0-2.0*v));
        D.resize(4, 4);
        D << a*(1-v),   a*v,       a*v,       0.0,
             a*v,       (1.0-v)*a, a*v,       0.0,
             a*v,       a*v,       (1.0-v)*a, 0.0,
             0.0,       0.0,       0.0,       a*(1.0-2.0*v)/2.0;
        break;
    case 3:
        a = E / ((1.0+v) * (1.0-2.0*v));
        D.resize(4, 4);
        D << a*(1-v),   a*v,       a*v,       0.0,
             a*v,       (1.0-v)*a, a*v,       0.0,
             a*v,       a*v,       (1.0-v)*a, 0.0,
             0.0,       0.0,       0.0,       a*(1.0-2.0*v)/2.0;
        break;
    case 4:
        a = E / ((1.0+v) * (1.0-2.0*v));
        D.resize(6, 6);
        D << a*(1.0-v), a*v,       a*v,       0.0,                   0.0,                   0.0,
             a*v,       a*(1.0-v), a*v,       0.0,                   0.0,                   0.0,
             a*v,       a*v,       a*(1.0-v), 0.0,                   0.0,                   0.0,
             0.0,       0.0,       0.0,       a*0.5*(1.0-2.0*v),     0.0,                   0.0,
             0.0,       0.0,       0.0,       0.0,                   a*0.5*(1.0-2.0*v),     0.0,
             0.0,       0.0,       0.0,       0.0,                   0.0,                   a*0.5*(1.0-2.0*v);
        break;
    default:
        break;
    }
}

void bar2e(const RowVec &ex, const RowVec &ey, const RowVec &ep, Matrix &Ke)
{
    double E = ep(0);
    double A = ep(1);

    ColVec b(2);
    b << ex(1)-ex(0), ey(1)-ey(0);
    double L = b.norm();

    Matrix Kle(2, 2);
    Kle << 1.0, -1.0, -1.0, 1.0;
    Kle *= E * A / L;

    RowVec n(2);
    n = b.transpose() / L;

    Matrix G(2, 4);
    G << n(0), n(1), 0.0,  0.0,
         0.0,  0.0,  n(0), n(1);

    Ke = G.transpose() * Kle * G;
}

void bar2s(const RowVec &ex, const RowVec &ey, const RowVec &ep, const RowVec &ed, double &es)
{
    double E = ep(0);
    double A = ep(1);

    ColVec b(2);
    b << ex(1)-ex(0), ey(1)-ey(0);
    double L = b.norm();

    RowVec n(2);
    n = b.transpose() / L;

    Matrix G(2, 4);
    G << n(0), n(1), 0.0,  0.0,
         0.0,  0.0,  n(0), n(1);

    RowVec v(2);
    v << -1.0, 1.0;

    ColVec u = ed.transpose();
    es = E * A * (v * G * u).value() / L;
}

static void buildGaussPoints(int ir, Matrix &gp, ColVec &wp)
{
    double g1, g2, w1, w2;

    if (ir == 1) {
        g1 = 0.0; w1 = 2.0;
        gp.resize(1, 2);
        gp << g1, g1;
        wp.resize(1);
        wp << w1 * w1;
    } else if (ir == 2) {
        g1 = 0.577350269189626; w1 = 1.0;
        gp.resize(4, 2);
        gp << -g1, -g1,
               g1, -g1,
              -g1,  g1,
               g1,  g1;
        wp.resize(4);
        wp << w1*w1, w1*w1, w1*w1, w1*w1;
    } else {
        g1 = 0.774596669241483; g2 = 0.0;
        w1 = 0.555555555555555; w2 = 0.888888888888888;
        gp.resize(9, 2);
        gp << -g1, -g1,  -g2, -g1,  g1, -g1,
              -g1,  g2,   g2,  g2,  g1,  g2,
              -g1,  g1,   g2,  g1,  g1,  g1;
        double ww[9] = {w1*w1, w2*w1, w1*w1,
                        w1*w2, w2*w2, w1*w2,
                        w1*w1, w2*w1, w1*w1};
        wp = Eigen::Map<ColVec>(ww, 9);
    }
}

void plani4e(const RowVec &ex, const RowVec &ey, const RowVec &ep, const Matrix &D,
             const RowVec &eq, Matrix &Ke, ColVec &fe)
{
    int ptype = (int)ep(0);
    double t  = ep(1);
    int ir    = (int)ep(2);
    int ngp   = ir * ir;

    Matrix gp;
    ColVec wp;
    buildGaussPoints(ir, gp, wp);

    ColVec xsi = gp.col(0);
    ColVec eta = gp.col(1);

    Matrix N(ngp, 4);
    N.col(0) = ((1.0 - xsi.array()) * (1.0 - eta.array())).matrix() / 4.0;
    N.col(1) = ((1.0 + xsi.array()) * (1.0 - eta.array())).matrix() / 4.0;
    N.col(2) = ((1.0 + xsi.array()) * (1.0 + eta.array())).matrix() / 4.0;
    N.col(3) = ((1.0 - xsi.array()) * (1.0 + eta.array())).matrix() / 4.0;

    int r2 = ngp * 2;
    Matrix dNr(r2, 4);
    dNr.setZero();

    for (int ip = 0; ip < ngp; ip++) {
        dNr(2*ip,   0) = -(1.0 - eta(ip)) / 4.0;
        dNr(2*ip,   1) =  (1.0 - eta(ip)) / 4.0;
        dNr(2*ip,   2) =  (1.0 + eta(ip)) / 4.0;
        dNr(2*ip,   3) = -(1.0 + eta(ip)) / 4.0;
        dNr(2*ip+1, 0) = -(1.0 - xsi(ip)) / 4.0;
        dNr(2*ip+1, 1) = -(1.0 + xsi(ip)) / 4.0;
        dNr(2*ip+1, 2) =  (1.0 + xsi(ip)) / 4.0;
        dNr(2*ip+1, 3) =  (1.0 - xsi(ip)) / 4.0;
    }

    Matrix exey(4, 2);
    exey.col(0) = ex.transpose();
    exey.col(1) = ey.transpose();
    Matrix JT = dNr * exey;

    Matrix Dm;
    if (ptype == 1) {
        if (D.cols() > 3) {
            Matrix Cm = D.inverse();
            Dm.resize(3, 3);
            Dm << Cm(0,0), Cm(0,1), Cm(0,3),
                  Cm(1,0), Cm(1,1), Cm(1,3),
                  Cm(3,0), Cm(3,1), Cm(3,3);
            Dm = Dm.inverse();
        } else {
            Dm = D;
        }
    } else {
        Dm = D;
    }

    Ke.resize(8, 8);
    Ke.setZero();
    fe.resize(8);
    fe.setZero();

    RowVec b = eq;

    for (int ip = 0; ip < ngp; ip++) {
        Matrix J(2, 2);
        J.row(0) = JT.row(2*ip);
        J.row(1) = JT.row(2*ip+1);

        double detJ = J.determinant();
        Matrix Jinv = J.inverse();

        Matrix dNx_loc(2, 4);
        dNx_loc.row(0) = dNr.row(2*ip);
        dNx_loc.row(1) = dNr.row(2*ip+1);
        Matrix dNx = Jinv * dNx_loc;

        Matrix B(3, 8);
        B << dNx(0,0), 0.0,       dNx(0,1), 0.0,       dNx(0,2), 0.0,       dNx(0,3), 0.0,
             0.0,       dNx(1,0), 0.0,       dNx(1,1), 0.0,       dNx(1,2), 0.0,       dNx(1,3),
             dNx(1,0), dNx(0,0), dNx(1,1), dNx(0,1), dNx(1,2), dNx(0,2), dNx(1,3), dNx(0,3);

        Matrix N2(2, 8);
        N2 << N(ip,0), 0.0,     N(ip,1), 0.0,     N(ip,2), 0.0,     N(ip,3), 0.0,
              0.0,     N(ip,0), 0.0,     N(ip,1), 0.0,     N(ip,2), 0.0,     N(ip,3);

        Ke += B.transpose() * Dm * B * detJ * wp(ip) * t;
        fe += N2.transpose() * b.transpose() * detJ * wp(ip) * t;
    }
}

void plani4s(const RowVec &ex, const RowVec &ey, const RowVec &ep, const Matrix &D,
             const RowVec &ed, Matrix &es, Matrix &et)
{
    int ptype = (int)ep(0);
    int ir    = (int)ep(2);
    int ngp   = ir * ir;

    Matrix gp;
    ColVec wp;
    buildGaussPoints(ir, gp, wp);

    ColVec xsi = gp.col(0);
    ColVec eta = gp.col(1);

    int r2 = ngp * 2;
    Matrix dNr(r2, 4);
    dNr.setZero();

    for (int ip = 0; ip < ngp; ip++) {
        dNr(2*ip,   0) = -(1.0 - eta(ip)) / 4.0;
        dNr(2*ip,   1) =  (1.0 - eta(ip)) / 4.0;
        dNr(2*ip,   2) =  (1.0 + eta(ip)) / 4.0;
        dNr(2*ip,   3) = -(1.0 + eta(ip)) / 4.0;
        dNr(2*ip+1, 0) = -(1.0 - xsi(ip)) / 4.0;
        dNr(2*ip+1, 1) = -(1.0 + xsi(ip)) / 4.0;
        dNr(2*ip+1, 2) =  (1.0 + xsi(ip)) / 4.0;
        dNr(2*ip+1, 3) =  (1.0 - xsi(ip)) / 4.0;
    }

    Matrix exey(4, 2);
    exey.col(0) = ex.transpose();
    exey.col(1) = ey.transpose();
    Matrix JT = dNr * exey;

    Matrix Dm;
    if (ptype == 1) {
        if (D.cols() > 3) {
            Matrix Cm = D.inverse();
            Dm.resize(3, 3);
            Dm << Cm(0,0), Cm(0,1), Cm(0,3),
                  Cm(1,0), Cm(1,1), Cm(1,3),
                  Cm(3,0), Cm(3,1), Cm(3,3);
            Dm = Dm.inverse();
        } else {
            Dm = D;
        }
    } else {
        Dm = D;
    }

    es.resize(ngp, Dm.rows());
    et.resize(ngp, Dm.rows());

    for (int ip = 0; ip < ngp; ip++) {
        Matrix J(2, 2);
        J.row(0) = JT.row(2*ip);
        J.row(1) = JT.row(2*ip+1);

        Matrix Jinv = J.inverse();

        Matrix dNx_loc(2, 4);
        dNx_loc.row(0) = dNr.row(2*ip);
        dNx_loc.row(1) = dNr.row(2*ip+1);
        Matrix dNx = Jinv * dNx_loc;

        Matrix B(3, 8);
        B << dNx(0,0), 0.0,       dNx(0,1), 0.0,       dNx(0,2), 0.0,       dNx(0,3), 0.0,
             0.0,       dNx(1,0), 0.0,       dNx(1,1), 0.0,       dNx(1,2), 0.0,       dNx(1,3),
             dNx(1,0), dNx(0,0), dNx(1,1), dNx(0,1), dNx(1,2), dNx(0,2), dNx(1,3), dNx(0,3);

        et.row(ip) = (B * ed.transpose()).transpose();
        es.row(ip) = (Dm * et.row(ip).transpose()).transpose();
    }
}

void writeMatrix(const std::string &name, Matrix &m, std::ostream &out)
{
    out << name << " = [";
    for (int i = 0; i < m.rows(); i++) {
        for (int j = 0; j < m.cols(); j++) {
            out << m(i, j);
            if (j < m.cols()-1) out << " ";
        }
        if (i < m.rows()-1) out << "; ";
    }
    out << "];\n";
}

} // namespace calfem
