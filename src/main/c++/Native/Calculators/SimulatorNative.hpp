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

#ifndef SimulatorNative_hpp
#define SimulatorNative_hpp

using namespace std;

#include <stdio.h>
#include <time.h>
#include <string>
#include <vector>
#include <chrono>
#include <boost/algorithm/string/join.hpp>
#include <sstream>
#include <algorithm>
#include "Models/AppSettings.hpp"
#include "Models/CalculatorData.h"
#include "../Utilities/ResultsUtilsSingle.hpp"
#include "Models/TwoDimVectorStore.hpp"
#include "../LCAModels/LCAIndexes.hpp"
#include <omp.h>
#include <string>
#include <vector>
#include "../LCAModels/CalcImpactFactorItem.hpp"
#include "../LCAModels/ExchangeItem.hpp"
#include "../libs/libEigen.hpp"
#include "../Uncertainties/EntitySampler.hpp"
#include "Models/IterationsResults.hpp"
#include "../Utilities/ParameterUtils.h"
#include <tr1/unordered_map>
#include <Eigen/Dense>
#include <Eigen/SparseCore>
#include <Eigen/SparseLU>
#include <Eigen/IterativeLinearSolvers>

using Eigen::BiCGSTAB;
using Eigen::ConjugateGradient;
using Eigen::SparseMatrix;
using Eigen::VectorXd;

typedef Eigen::Triplet<double> Triplet;
typedef Eigen::SparseMatrix<double> SMatrix;

typedef exprtk::expression<double> expression_t;

class SimulatorNative
{
public:
    static std::vector<std::string> split(std::string input, std::string sep)
    {

        vector<string> strs;
        boost::split(strs, input, boost::is_any_of(sep));
        return strs;
    }

    static vector<vector<double>> parseSamples(string path)
    {

        FileUtils fileUtils;

        vector<string> samplelines = fileUtils.readTextFromFile(path);
        long samplelines_size = samplelines.size();
        vector<vector<double>> samplesvals;
        samplesvals.resize(samplelines_size);

        for (int i_sampleline = 0; i_sampleline < samplelines_size; i_sampleline++)
        {
            string sampleline = samplelines[i_sampleline];
            vector<string> samples = split(sampleline, ",");
            for (auto &&sample : samples)
            {
                //if (sample != "")
                // {
                samplesvals[i_sampleline].push_back(std::stod(sample));
                // }
            }
        }
        return samplesvals;
    }

    static IterationsResults *iterate(int start, int end, int iterationOffset,
                                      int threadSize,
                                      int slotsize,
                                      CalculatorData *calculatorData,
                                      AppSettings settings)
    {

        srand(time(NULL) + iterationOffset);
        unsigned seed = rand();
        std::default_random_engine gen(seed);

        long ElementaryFlowsIndexLength = (*calculatorData).elementaryFlowsLength;
        long ProcessesIndexLength = (*calculatorData).processesLength;
        long ImpactCategoryIndexLength = (*calculatorData).impactCategoriesLength;
        int iter = start;

        EntitySampler<double> entitySampler;

        IterationsResults *iterationResults_i = new IterationsResults(settings,
                                                                      (*calculatorData).impactCategoriesLength,
                                                                      (*calculatorData).A_unc_size,
                                                                      (*calculatorData).B_unc_size,
                                                                      (*calculatorData).Q_size,
                                                                      0,
                                                                      slotsize);

        ParameterUtils<double> paramUtils;

        /*<scope_owner,expresion>*/
        /*unordered_*/ map<string, expression_t> expressionsTable;

        paramUtils.createExpressionTable((*calculatorData).parameters,
                                         expressionsTable,
                                         (*calculatorData).parameterVariableInfoMap);

        for (auto &&cell : (*calculatorData).A_unc_exchanges)
        {
            paramUtils.reloadExchangesDistribution(cell, expressionsTable);
        }
        for (auto &&cell : (*calculatorData).B_unc_exchanges)
        {
            paramUtils.reloadExchangesDistribution(cell, expressionsTable);
        }

        libEigen lib;

        SMatrix A(ProcessesIndexLength, ProcessesIndexLength);

        long Alength = (*calculatorData).A_size;
        long *rowsA_ptr = new long[Alength - (*calculatorData).A_unc_size];
        long *colsA_ptr = new long[Alength - (*calculatorData).A_unc_size];
        double *dataA_ptr = new double[Alength - (*calculatorData).A_unc_size];
        long *rowsA_unc_ptr = new long[(*calculatorData).A_unc_size];
        long *colsA_unc_ptr = new long[(*calculatorData).A_unc_size];
        double *dataA_unc_ptr = new double[(*calculatorData).A_unc_size];

        int i;
        auto strt_A = std::chrono::high_resolution_clock::now();

        i = 0;
        for (auto &&cell : (*calculatorData).A_exchanges)
        {
            ExchangeItem ent = cell;

            if (ent.exch.uncertaintyType == 0)
            {
                rowsA_ptr[i] = ent.i;
                colsA_ptr[i] = ent.j;
                dataA_ptr[i] = entitySampler.getCellValue(ent.exch);

                i++;
            }
        }

        libEigen::fillSparseMatrix(&A, Alength - (*calculatorData).A_unc_size, rowsA_ptr, colsA_ptr, dataA_ptr);

        auto finish_A = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_A = finish_A - strt_A;
        //std::cout << "A," << settings.ProjectId << "," << elapsed_A.count() << std::endl;

        for (i = 0; i < (*calculatorData).A_unc_size; i++)
        {
            ExchangeItem ent = (*calculatorData).A_unc_exchanges[i];
            rowsA_unc_ptr[i] = ent.i;
            colsA_unc_ptr[i] = ent.j;
            dataA_unc_ptr[i] = entitySampler.getCellValue(ent.exch);
        }

        SMatrix A_unc(ProcessesIndexLength, ProcessesIndexLength);
        libEigen::fillSparseMatrix(&A_unc, (*calculatorData).A_unc_size, rowsA_unc_ptr, colsA_unc_ptr, dataA_unc_ptr);

        double *demand = &((*calculatorData).demand_demandVectorArray)[0];

        VectorXd bVec(ProcessesIndexLength);

        for (int i = 0; i < ProcessesIndexLength; i++)
        {
            bVec(i) = demand[i];
        }

        Eigen::setNbThreads(0);

        BiCGSTAB<SparseMatrix<double> /*, Eigen::IncompleteLUT< double, int >*/> solver;
        solver.compute(A + A_unc);
        VectorXd guess(ProcessesIndexLength);
        guess = solver.solve(bVec);

        long Blength = (*calculatorData).B_size;
        long *rowsB_ptr = new long[Blength - (*calculatorData).B_unc_size];
        long *colsB_ptr = new long[Blength - (*calculatorData).B_unc_size];
        double *dataB_ptr = new double[Blength - (*calculatorData).B_unc_size];
        long *rowsB_unc_ptr = new long[(*calculatorData).B_unc_size];
        long *colsB_unc_ptr = new long[(*calculatorData).B_unc_size];
        double *dataB_unc_ptr = new double[(*calculatorData).B_unc_size];

        SMatrix B(ElementaryFlowsIndexLength, ProcessesIndexLength);

        i = 0;

        auto strt_B = std::chrono::high_resolution_clock::now();

        for (auto &&cell : (*calculatorData).B_exchanges)
        {
            ExchangeItem ent = cell;

            if (ent.exch.uncertaintyType == 0)
            {
                rowsB_ptr[i] = ent.i;
                colsB_ptr[i] = ent.j;
                dataB_ptr[i] = entitySampler.getCellValue(ent.exch);

                i++;
            }
        }

        libEigen::fillSparseMatrix(&B, Blength - (*calculatorData).B_unc_size, rowsB_ptr, colsB_ptr, dataB_ptr);

        auto finish_B = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_B = finish_B - strt_B;
        //std::cout << "B," << settings.ProjectId << "," << elapsed_B.count() << std::endl;

        long Qlength = (*calculatorData).Q_size;
        long *rowsQ_ptr = new long[Qlength];
        long *colsQ_ptr = new long[Qlength];
        double *dataQ_ptr = new double[Qlength];

        solver.setTolerance(0.0001);

        long slotIndex = 0;

        while (iter <= end)
        {

            //cout<<"Iter#"<<iter<<endl;

            auto strt_As = std::chrono::high_resolution_clock::now();

            for (i = 0; i < (*calculatorData).A_unc_size; i++)
            {
                ExchangeItem ent = (*calculatorData).A_unc_exchanges[i];
                rowsA_unc_ptr[i] = ent.i;
                colsA_unc_ptr[i] = ent.j;
                dataA_unc_ptr[i] = entitySampler.getNextSimulatedCellValue(&ent, gen);
            }

            SMatrix A_unc(ProcessesIndexLength, ProcessesIndexLength);
            libEigen::fillSparseMatrix(&A_unc, (*calculatorData).A_unc_size, rowsA_unc_ptr, colsA_unc_ptr, dataA_unc_ptr);

            auto finish_As = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed_As = finish_As - strt_As;
            //std::cout << "As," << settings.ProjectId << "," << elapsed_As.count() << std::endl;

            VectorXd scalarsVec(ProcessesIndexLength);

            if (A.cols() != bVec.rows())
            {
                throw LCAException(200, "While solving linear system, matrices dimensions does not match.");
            }

            if (A.cols() != A.rows())
            {
                throw LCAException(200, "While solving linear system, matrix A is not square");
            }

            auto strt = std::chrono::high_resolution_clock::now();
            solver.compute(A + A_unc);
            scalarsVec = solver.solveWithGuess(bVec, guess);
            auto finish = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = finish - strt;
            //std::cout << "solve," << settings.ProjectId << "," << elapsed.count() << std::endl;

            auto strt_Bs = std::chrono::high_resolution_clock::now();

            for (i = 0; i < (*calculatorData).B_unc_size; i++)
            {
                ExchangeItem ent = (*calculatorData).B_unc_exchanges[i];
                rowsB_unc_ptr[i] = ent.i;
                colsB_unc_ptr[i] = ent.j;
                dataB_unc_ptr[i] = entitySampler.getNextSimulatedCellValue(&ent, gen);
            }

            SMatrix B_unc(ElementaryFlowsIndexLength, ProcessesIndexLength);
            libEigen::fillSparseMatrix(&B_unc, (*calculatorData).B_unc_size, rowsB_unc_ptr, colsB_unc_ptr, dataB_unc_ptr);

            auto finish_Bs = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed_Bs = finish_Bs - strt_Bs;
            //std::cout << "Bs," << settings.ProjectId << "," << elapsed_Bs.count() << std::endl;

            auto strtlci = std::chrono::high_resolution_clock::now();

            VectorXd lciX = (B + B_unc) * scalarsVec;
            auto finishlci = std::chrono::high_resolution_clock::now();
            auto elapsedlci = std::chrono::duration_cast<std::chrono::microseconds>(finishlci - strtlci);
            //std::cout << "lci/c++," << settings.ProjectId << "," << elapsedlci.count() << std::endl;
            auto strtQ = std::chrono::high_resolution_clock::now();

            i = 0;
            for (auto &&cell : (*calculatorData).Q_cells)
            {
                CalcImpactFactorItem ent = cell;
                rowsQ_ptr[i] = ent.i;
                colsQ_ptr[i] = ent.j;
                dataQ_ptr[i] = entitySampler.getImpactCellValue(ent.imf);

                i++;
            }

            SMatrix Q(ImpactCategoryIndexLength, ElementaryFlowsIndexLength);
            libEigen::fillSparseMatrix(&Q, Qlength, rowsQ_ptr, colsQ_ptr, dataQ_ptr);

            auto finishQ = std::chrono::high_resolution_clock::now();
            auto elapsedQ = std::chrono::duration_cast<std::chrono::microseconds>(finishQ - strtQ);
            //std::cout << "Q/c++," << settings.ProjectId << "," << elapsedQ.count() << std::endl;
           
           
            auto strtlcia = std::chrono::high_resolution_clock::now();
            VectorXd lciaX = Q * lciX;

            double *lciaR = new double[ImpactCategoryIndexLength];
            for (int i = 0; i < ImpactCategoryIndexLength; i++)
            {
                lciaR[i] = lciaX(i);
            }
            auto finishlcia = std::chrono::high_resolution_clock::now();
            auto elapsedlcia = std::chrono::duration_cast<std::chrono::microseconds>(finishlcia - strtlcia);
            //std::cout << "lcia/c++," << settings.ProjectId << "," << elapsedlcia.count() << std::endl;

            TwoDimVectorStore::putDataAtIteration((*iterationResults_i).store_lcia_ptr, iter - iterationOffset - (slotIndex * slotsize), lciaR, ImpactCategoryIndexLength);

            if (settings.Sensitivity)
            {
                // if (dataA_v_unc.size() > 0)
                TwoDimVectorStore::putDataAtIteration((*iterationResults_i).store_A_ptr, iter - iterationOffset - (slotIndex * slotsize), dataA_unc_ptr, (*calculatorData).A_unc_size);

                // if (dataB_v_unc.size() > 0)
                TwoDimVectorStore::putDataAtIteration((*iterationResults_i).store_B_ptr, iter - iterationOffset - (slotIndex * slotsize), dataB_unc_ptr, (*calculatorData).B_unc_size);

                TwoDimVectorStore::putDataAtIteration((*iterationResults_i).store_Q_ptr, iter - iterationOffset - (slotIndex * slotsize), dataQ_ptr, Qlength);
            }

            // TwoDimVectorStore::putDataAtIteration((*iterationResults_i).store_P_ptr, iter - iterationOffset, dataP_ptr, Plength);

            // delete[] dataP_ptr;
            // dataP_ptr = nullptr;

            delete[] lciaR;
            lciaR = nullptr;

            iter++;
        }

        delete[] rowsA_ptr;
        delete[] colsA_ptr;
        delete[] dataA_ptr;

        delete[] rowsB_ptr;
        delete[] colsB_ptr;
        delete[] dataB_ptr;

        delete[] rowsQ_ptr;
        delete[] colsQ_ptr;
        delete[] dataQ_ptr;

        return iterationResults_i;
    }
};

#endif /* SimulatorNative_hpp */
