//
//  libQBAInv.hpp
//  LCA.Kernel
//
//  Created by francois saab on 2/26/18.
//  Copyright © 2018 fsaab. All rights reserved.
//

#ifndef libQBAInvEigen_hpp
#define libQBAInvEigen_hpp

#include <stdio.h>
#include "../Calculators/Models/AppSettings.hpp"
#include <stdio.h>
#include "../LCAModels/LCAIndexes.hpp"
#include <iostream>
#include "../Factories/TechnologyMatrixFactorySingle.hpp"
#include "../Factories/InterventionMatrixFactorySingle.hpp"
#include "../LCAModels/ExchangeItem.hpp"
#include "../LCAModels/CalcImpactFactorItem.hpp"
#include "../Factories/CharacterisationMatrixFactory.hpp"
using namespace std;
#include "../Calculators/MontecarloCalculator.hpp"
//#include <chrono>  // for high_resolution_clock
#include "../Calculators/Models/IterationsResults.hpp"
//#include "SensitivityCalculator.hpp"
//#include "ScalarsFactoryMUMPS.hpp"
#include "../Utilities/FileUtils.hpp"
#include "../DAL/LCADB.hpp"
#include "../Graph/GraphData.h"
class libQBAInvEigen
{
  public:
    static Eigen::MatrixXd calculateByTransposeSystemBICGSTAB(
        CalculatorData *calculatorData, AppSettings settings)
    {

        SMatrix m = TechnologyMatrixFactorySingle::build(settings, calculatorData);

        long Acols = m.cols();

        int nnz = m.nonZeros();

        SMatrix m_B = InterventionMatrixFactorySingle::build(settings, calculatorData);
        long Brows = m_B.rows();

        SMatrix Q = CharacterisationMatrixFactory::build(settings, calculatorData);
        int Qrows = Q.rows();

        SMatrix QB = Q * m_B;

        SMatrix m__rhs_T = QB.transpose();

        BiCGSTAB<SparseMatrix<double> /*, Eigen::IncompleteLUT< double, int >*/> solver;

        SMatrix mt = m.transpose();
        solver.compute(mt);
        SMatrix QBAinv_t(Acols, Qrows);

        std::vector<Triplet> triplets;
        triplets.reserve(Acols * Qrows);

        if (mt.rows() != m__rhs_T.rows())
        {

            throw LCAException(200, "Sizes does not match while solving linear system in the LCIA contribution analysis report.");
        }

        #pragma omp parallel for
        for (int col = 0; col < m__rhs_T.cols(); col++)
        {

            VectorXd xVec(Acols), bVec(Acols);

            xVec = solver.solve(m__rhs_T.col(col));

            #pragma omp critical
            {
                for (int i = 0; i < Acols; i++)
                {

                    triplets.push_back(Triplet(i, col, xVec(i)));
                }
            }
        }

        QBAinv_t.setFromTriplets(triplets.begin(), triplets.end());
        QBAinv_t.makeCompressed();

        return (QBAinv_t.transpose());
    }

    static Eigen::MatrixXd calculateByTransposeSystemBICGSTAB_Simple(
        long Alength, long *rowsA_ptr, long *colsA_ptr, double *dataA_ptr,
        long Blength, long *rowsB_ptr, long *colsB_ptr, double *dataB_ptr,
        long Qlength, long *rowsQ_ptr, long *colsQ_ptr, double *dataQ_ptr,
        AppSettings settings,
        CalculatorData *calculatorData
        )
    {

        int Arows = (*calculatorData).lcaIndexes.IntermediateFlowsIndexLength();
        SMatrix m(Arows, Arows);
        libEigen::fillSparseMatrix(&m, Alength, rowsA_ptr, colsA_ptr, dataA_ptr);

        long Acols = m.cols();
        int nnz = m.nonZeros();
        int Brows = (*calculatorData).lcaIndexes.ElementaryFlowsIndexLength();
        int Bcols = (*calculatorData).lcaIndexes.ProcessesIndexLength();

        SMatrix m_B(Brows, Bcols);
        libEigen::fillSparseMatrix(&m_B, Blength, rowsB_ptr, colsB_ptr, dataB_ptr);

        int Qcols = (*calculatorData).lcaIndexes.ElementaryFlowsIndexLength();
        int Qrows = (*calculatorData).lcaIndexes.ImpactCategoryIndexLength();

        SMatrix Q(Qrows, Qcols);
        libEigen::fillSparseMatrix(&Q, Qlength, rowsQ_ptr, colsQ_ptr, dataQ_ptr);
        SMatrix QB = Q * m_B;
        SMatrix m__rhs_T;
        m__rhs_T = QB.transpose();

        BiCGSTAB<SparseMatrix<double> /*, Eigen::IncompleteLUT< double, int >*/> solver;
        SMatrix mt = m.transpose();
        solver.compute(mt);
        SMatrix QBAinv_t(Acols, Qrows);

        std::vector<Triplet> triplets;
        triplets.reserve(Acols * Qrows);

        if (mt.rows() != m__rhs_T.rows())
        {
            throw LCAException(200, "Sizes does not match while solving linear system in the LCIA contribution analysis report.");
        }

        #pragma omp parallel for
        for (int col = 0; col < m__rhs_T.cols(); col++)
        {
            VectorXd xVec(Acols), bVec(Acols);
            xVec = solver.solve(m__rhs_T.col(col));

            #pragma omp critical
            {
                for (int i = 0; i < Acols; i++)
                {
                    triplets.push_back(Triplet(i, col, xVec(i)));
                }
            }
        }

        QBAinv_t.setFromTriplets(triplets.begin(), triplets.end());
        QBAinv_t.makeCompressed();

        return (QBAinv_t.transpose());
    }

};

#endif /* libQBAInv_hpp */
