/**---------------------------   
 * PROJECT: ParallelLCA
 * Auth:
 *   Francois Saab
 * Mail: saab.francois@gmail.com, francois.saab.1@ens.etsmtl.ca
 * Date: 1/1/2017
 *
 * Copyright © 2017 Francois Saab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. 
 *
 *--------------------------*/
#ifndef libEigen_hpp
#define libEigen_hpp

#include <stdio.h>

#include <iostream>

#include "../Messaging/LCAException.hpp"
#include <Eigen/Dense>
#include <Eigen/SparseCore>
#include <Eigen/SparseLU>
#include <Eigen/IterativeLinearSolvers>

//#include <Eigen/UmfPackSupport>
using Eigen::BiCGSTAB;
using Eigen::SparseMatrix;
using Eigen::VectorXd;

typedef Eigen::Triplet<double> Triplet;
typedef Eigen::SparseMatrix<double> SMatrix;

class libEigen
{
  public:
    static void fillSparseMatrix(SMatrix *m, int n, long *rows, long *cols,
                                 double *data)
    {

        std::vector<Triplet> triplets;
        triplets.reserve(n);
        for (int i = 0; i < n; i++)
        {

            triplets.push_back(Triplet(rows[i], cols[i], data[i]));
        }
        m->setFromTriplets(triplets.begin(), triplets.end(), [](const double &a, const double &b) { return a + b; });
        m->makeCompressed();
    }

    static double *MV_mult(
        int dimrows, int dimcols, int n, long *rowsA, long *colsA, double *dataA, //construct A
        int dimvect, double *x                                                    //construct b
    )
    {

        SMatrix m(dimrows, dimcols);

        fillSparseMatrix(&m, n, rowsA, colsA, dataA);

        VectorXd xVec(dimvect);

        for (int i = 0; i < dimvect; i++)
        {
            xVec(i) = x[i];
        }

        if (m.cols() != xVec.rows())
        {
            throw LCAException(300, "error while multiplying matrices. matrices dimensions does not match. \n This happens at the LCI or LCIA level ");
        }

        VectorXd result = m * xVec;

        double *y = new double[dimrows];

        // copy results to x
        for (int i = 0; i < dimrows; i++)
        {
            y[i] = result(i);
        }

        return y;
    }

    void bicgstab(int dim, int n, long *rowsA, long *colsA, double *dataA,
                  double *b, double *x)
    {

        SMatrix m(dim, dim);

        fillSparseMatrix(&m, n, rowsA, colsA, dataA);
        // initialize the vectors
        VectorXd xVec(dim), bVec(dim);
        for (int i = 0; i < dim; i++)
        {

            bVec(i) = b[i];
        }
        if (m.cols() != bVec.rows())
        {
            throw LCAException(200, "While solving linear system, matrices dimensions does not match.");
        }

        if (m.cols() != m.rows())
        {
            throw LCAException(200, "While solving linear system, matrix A is not square");
        }

        BiCGSTAB<SparseMatrix<double> /*, Eigen::IncompleteLUT< double, int >*/> solver;
        solver.compute(m);
        solver.setTolerance(0.0001);
        xVec = solver.solve(bVec);
        // copy results to x
        for (int i = 0; i < dim; i++)
        {
            x[i] = xVec(i);
        }
    }

    static void cg(int dim, int n, long *rowsA, long *colsA, double *dataA,
                   double *b, double *x)
    {

        SMatrix m(dim, dim);
        fillSparseMatrix(&m, n, rowsA, colsA, dataA);

        // initialize the vectors
        VectorXd xVec(dim), bVec(dim);
        for (int i = 0; i < dim; i++)
        {
            bVec(i) = b[i];
        }

        Eigen::ConjugateGradient<SparseMatrix<double>
                                 /*, Eigen::Lower|Eigen::Upper, Eigen::IdentityPreconditioner*/>
            solver;
        solver.compute(m);
        xVec = solver.solve(bVec);

        // copy results to x
        for (int i = 0; i < dim; i++)
        {
            x[i] = xVec(i);
        }
    }

    //    static void umfpack(int dim, int n, int *rowsA, int *colsA, double *dataA,
    //                         double *b, double *x) {
    //
    //
    //        SMatrix m(dim, dim);
    //        fillSparseMatrix(&m, n, rowsA, colsA, dataA);
    //
    //        // initialize the vectors
    //        VectorXd xVec(dim), bVec(dim);
    //        for (int i = 0; i < dim; i++) {
    //            bVec(i) = b[i];
    //        }
    //
    //        Eigen::UmfPackLU<SMatrix> solver;
    //
    //        solver.compute(m);
    //
    //         xVec =solver.solve(bVec);
    //
    //        // copy results to x
    //        for (int i = 0; i < dim; i++) {
    //            x[i] = xVec(i);
    //        }
    //    }

    double *solve_bicgstab(int dim, int n, long *rowsA, long *colsA, double *dataA,
                           double *b)
    {

        double *sol = new double[dim];

        bicgstab(dim, n, rowsA, colsA, dataA, b, sol);

        return sol;
    }

    static double *solve_cg(int dim, int n, long *rowsA, long *colsA, double *dataA,
                            double *b)
    {

        double *sol = new double[dim];

        cg(dim, n, rowsA, colsA, dataA, b, sol);

        return sol;
    }

    //    static double* solve_umfpack(int dim, int n, int *rowsA, int *colsA, double *dataA,
    //                         double *b)
    //    {
    //
    //        double* sol= new double[dim];
    //
    //        umfpack(dim, n, rowsA, colsA, dataA, b, sol);
    //
    //
    //        return sol;
    //    }
    //

    // int test(){

    // double dataA[4] = {2,2,3,1};

    // int rowsA[4] = {0,0,1,1};

    // int colsA[4] = {0,1,0,1};

    // double b[2] = {4.0,4.0};

    // double* sol= solve(2,4,rowsA,colsA,dataA,b);

    //  std::cout << "solution"<< sol[0]<<" "<<sol[1] ;

    // return 1;

    // }

    /*
     int main()
     {
 
     test();
     }*/
};

#endif /* libEigen_hpp */
