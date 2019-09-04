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
#ifndef libUmfpack_hpp
#define libUmfpack_hpp

#include <stdio.h>


#include <iostream>

# include <cstdlib>

# include <iomanip>
# include <ctime>

using namespace std;

//#include "/usr/include/suitesparse/umfpack.h"
#include <suitesparse/umfpack.h>
#include <Eigen/Dense>
#include <Eigen/SparseCore>
#include <Eigen/SparseLU>
#include <Eigen/IterativeLinearSolvers>

//#include <Eigen/UmfPackSupport>
using Eigen::VectorXd;
using Eigen::BiCGSTAB;
using Eigen::SparseMatrix;

typedef Eigen::Triplet<double> Triplet;
typedef Eigen::SparseMatrix<double> SMatrix;

class libUmfpack
{
    
    
public:
    static void fillSparseMatrix(SMatrix *m, int n, int *rows, int *cols,
                                 double *data) {
        
        
        
        std::vector<Triplet> triplets;
        triplets.reserve(n);
        for (int i = 0; i < n; i++) {
            triplets.push_back(Triplet(rows[i], cols[i], data[i]));
        }
        m->setFromTriplets(triplets.begin(), triplets.end(),[] (const double &a,const double &b) { return a+b; });
        m->makeCompressed();
    }
//    
//    
//    
//    static double * MV_mult(
//                     int dimrows,int dimcols, int n, int *rowsA, int *colsA, double *dataA,//construct A
//                     int dimvect,double *x//construct b
//    ){
//        
//        SMatrix m(dimrows, dimcols);
//                
//        fillSparseMatrix(&m, n, rowsA, colsA, dataA);
//                
//        
//        VectorXd xVec(dimvect);
//                
//        for (int i = 0; i < dimvect; i++) {
//            xVec(i) = x[i];
//        }
//                
//        VectorXd result=m*xVec;
//        
//        
//        double *y=new double[dimrows];
//        
//        
//        // copy results to x
//        for (int i = 0; i < dimrows; i++) {
//            y[i] = result(i);
//        }
//        
//        
//        return y;
//        
//    }
//
//
//        static void umfpack(int dim, int n, int *rowsA, int *colsA, double *dataA,
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
//    
//    
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




// int test(){

// double dataA[4] = {2,2,3,1};

// int rowsA[4] = {0,0,1,1};

// int colsA[4] = {0,1,0,1};

// double b[2] = {4.0,4.0};


// double* sol= solve(2,4,rowsA,colsA,dataA,b);

//  std::cout << "solution"<< sol[0]<<" "<<sol[1] ;

// return 1;

// }
    
    
    
    
    
static double* solve_umfpack(int dim, int Alength, int *rowsA, int *colsA, double *dataA,
                         double *b)


{

    
      
        SMatrix m(dim, dim);
        fillSparseMatrix(&m, Alength, rowsA, colsA, dataA);
        
        long nz=m.nonZeros();
    
        int* Ap=new int[dim+1];
        int* Ai=new int[nz];
        double* Ax=new double[nz];
        
        int k = 0;
        int aPi=0;
        
        int prevcol=-1;
        
         for (int j = 0; j < dim; j++)
            {
             for (int i = 0; i < dim;i++){

                double v=m.coeff(i,j);
                    
                if(v!=0){
                    
                    if(j!=prevcol){
                        
                         prevcol=j;
                         Ap[aPi] = k;
                         
                         aPi++;
                   
                    }
                    Ai[k] = i;
                    Ax[k] = v;
                    k++;
                    
                     }
                    
            }
            
        }
        
        Ap[aPi]=nz;
        

  int i;
 
  double *null = ( double * ) NULL;
  void *Numeric;
  int status;
  void *Symbolic;
   double* x= new double[dim];

 
  cout << "\n";
  cout << "UMFPACK_SIMPLE:\n";
  cout << "  C++ version\n";
  cout << "  Use UMFPACK to solve the sparse linear system A*x=b.\n";
//
//  Carry out the symbolic factorization.
//
  

  status = umfpack_di_symbolic ( dim, dim, Ap, Ai, Ax, &Symbolic, null, null );
  
  cout << "  1\n";
//
//  Use the symbolic factorization to carry out the numeric factorization.
//
  status = umfpack_di_numeric (Ap, Ai, Ax, Symbolic, &Numeric, null, null );
   cout << "  2\n";
//
//  Free the memory associated with the symbolic factorization.
//
  umfpack_di_free_symbolic ( &Symbolic );
   cout << "  3\n";
//
//  Solve the linear system.
//
  status = umfpack_di_solve ( UMFPACK_A, Ap, Ai, Ax, x, b, Numeric, null, null );
   cout << "  4\n";
//
//  Free the memory associated with the numeric factorization.
//
  umfpack_di_free_numeric ( &Numeric );
   cout << "  5\n";
//
//  Print the solution.
//
  cout << "\n";
  cout << "  Computed solution:\n";
  cout << "\n";
  for ( i = 0; i < dim; i++ ) 
  {
    cout << x[i] << " , ";
  }
//
//  Terminate.
//
  cout << "\n";
  cout << "UMFPACK_SIMPLE:\n";
  cout << "  Normal end of execution.\n";
  cout << "\n";
  

  return x;

}
//****************************************************************************80




/*
 int main()
 {
 
 test();
 }*/
};

#endif /* libEigen_hpp */
