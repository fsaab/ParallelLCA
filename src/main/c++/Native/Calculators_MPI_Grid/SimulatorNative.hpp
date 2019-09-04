//
//  SimulatorNative.hpp
//  LCA.Kernel
//
//  Created by francois saab on 2/18/18.
//  Copyright © 2018 fsaab. All rights reserved.
//

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
#include "../Calculators/Models/AppSettings.hpp"
#include "../Calculators/Models/CalculatorData.h"
#include "../Utilities/ResultsUtilsSingle.hpp"
#include "../Calculators/Models/TwoDimVectorStore.hpp"
#include "Models/TwoDimStreamedVectorStore.hpp"
#include "../LCAModels/LCAIndexes.hpp"
#include <omp.h>
#include <string>
#include <vector>
#include "../LCAModels/CalcImpactFactorItem.hpp"
#include "../LCAModels/ExchangeItem.hpp"
#include "../libs/libEigen.hpp"
#include "../Uncertainties/EntitySampler.hpp"
#include "../Calculators/Models/IterationsResults.hpp"
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

    static void iterate(int start, int end, int iterationOffset,
                        int threadSize,
                        int slotsize,
                        CalculatorData *calculatorData,
                        AppSettings settings,long dataslotsize_samples,long dataslotsize_output
                        // ParameterTable parametersTable)
    )
    {

        //ParameterUtils<double> paramUtils;

        srand(time(NULL) + iterationOffset);
        unsigned seed = rand();
        std::default_random_engine gen(seed);

        long ElementaryFlowsIndexLength = (*calculatorData).elementaryFlowsLength;
        long ProcessesIndexLength = (*calculatorData).processesLength;
        long ImpactCategoryIndexLength = (*calculatorData).impactCategoriesLength;
        //long Plength = parameters.size();
        int iter = start;

        EntitySampler<double> entitySampler;

        IterationsResults *iterationResults_i = new IterationsResults(settings,
                                                                      (*calculatorData).impactCategoriesLength,
                                                                      (*calculatorData).A_unc_size,
                                                                      (*calculatorData).B_unc_size,
                                                                      (*calculatorData).Q_size,
                                                                      0,
                                                                      slotsize);

        for (auto &&cell : (*calculatorData).A_unc_exchanges)
        {
            cell.reloadDistribution();
        }
        for (auto &&cell : (*calculatorData).B_unc_exchanges)
        {
            cell.reloadDistribution();
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

        i = 0;
        for (auto &&cell : (*calculatorData).A_exchanges)
        {

            ExchangeItem ent = cell;

            if (ent.exch.uncertaintyType == 0)
            {
                rowsA_ptr[i] = ent.i;
                colsA_ptr[i] = ent.j;
                dataA_ptr[i] = entitySampler.getCellValue(ent.exch);
                //,paramUtils.getIterpreter(cell.exch.exchangeId, interpreters, "PROCESS")
                i++;
            }
        }

        libEigen::fillSparseMatrix(&A, Alength - (*calculatorData).A_unc_size, rowsA_ptr, colsA_ptr, dataA_ptr);

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
        for (auto &&cell : (*calculatorData).B_exchanges)
        {
            ExchangeItem ent = cell;

            if (ent.exch.uncertaintyType == 0)
            {
                rowsB_ptr[i] = ent.i;
                colsB_ptr[i] = ent.j;
                dataB_ptr[i] = entitySampler.getCellValue(ent.exch);
                //,paramUtils.getIterpreter(cell.exch.exchangeId, interpreters, "PROCESS")
                i++;
            }
        }

        libEigen::fillSparseMatrix(&B, Blength - (*calculatorData).B_unc_size, rowsB_ptr, colsB_ptr, dataB_ptr);

        // for (int i = 0; i < (*calculatorData).B_unc_size; i++)
        // {
        //     ExchangeItem ent = (*calculatorData).B_unc_exchanges[i];
        //     dataB_unc_ptr[i] = B.coeff(ent.i, ent.j);
        // }

        long Qlength = (*calculatorData).Q_size;
        long *rowsQ_ptr = new long[Qlength];
        long *colsQ_ptr = new long[Qlength];
        double *dataQ_ptr = new double[Qlength];

        solver.setTolerance(0.0000001);

        long slotIndex = 0;

        while (iter <= end)
        {

            //   map<string, expression_t > interpreters;
            //
            //   map<std::pair<string, string>, double > parameters_sim;
            //
            //   interpreters = entitySampler.sampleParameterTable(parametersTable, gen, parameters_sim);

            //    double *dataP_ptr = new double[Plength];
            //    int parami = 0;
            //    for (map< std::pair<string, string>, double >::iterator it = parameters_sim.begin(); it != parameters_sim.end(); ++it) {
            //
            //    dataP_ptr[parami] = it->second;
            //    parami++;
            //    }

            for (i = 0; i < (*calculatorData).A_unc_size; i++)
            {

                ExchangeItem ent = (*calculatorData).A_unc_exchanges[i];
                rowsA_unc_ptr[i] = ent.i;
                colsA_unc_ptr[i] = ent.j;
                dataA_unc_ptr[i] = entitySampler.getNextSimulatedCellValue(&ent, gen);
            }

            SMatrix A_unc(ProcessesIndexLength, ProcessesIndexLength);
            libEigen::fillSparseMatrix(&A_unc, (*calculatorData).A_unc_size, rowsA_unc_ptr, colsA_unc_ptr, dataA_unc_ptr);

            VectorXd scalarsVec(ProcessesIndexLength);

            if (A.cols() != bVec.rows())
            {
                throw LCAException(200, "While solving linear system, matrices dimensions does not match.");
            }

            if (A.cols() != A.rows())
            {
                throw LCAException(200, "While solving linear system, matrix A is not square");
            }

            //auto strt = std::chrono::high_resolution_clock::now();
            solver.compute(A + A_unc);
            scalarsVec = solver.solveWithGuess(bVec, guess);
            // auto finish = std::chrono::high_resolution_clock::now();
            // std::chrono::duration<double> elapsed = finish - strt;
            // std::cout << "solve," << settings.ProjectId << "," << elapsed.count() << std::endl;

            for (i = 0; i < (*calculatorData).B_unc_size; i++)
            {
                ExchangeItem ent = (*calculatorData).B_unc_exchanges[i];
                rowsB_unc_ptr[i] = ent.i;
                colsB_unc_ptr[i] = ent.j;
                dataB_unc_ptr[i] = entitySampler.getNextSimulatedCellValue(&ent, gen);
            }

            SMatrix B_unc(ElementaryFlowsIndexLength, ProcessesIndexLength);
            libEigen::fillSparseMatrix(&B_unc, (*calculatorData).B_unc_size, rowsB_unc_ptr, colsB_unc_ptr, dataB_unc_ptr);

            VectorXd lciX = (B + B_unc) * scalarsVec;

            i = 0;
            for (auto &&cell : (*calculatorData).Q_cells)
            {
                CalcImpactFactorItem ent = cell;
                rowsQ_ptr[i] = ent.i;
                colsQ_ptr[i] = ent.j;
                dataQ_ptr[i] = entitySampler.getImpactCellValue(ent.imf
                                                                //,paramUtils.getIterpreter(cell.imf.id, interpreters, "GLOBAL")
                );

                i++;
            }

            SMatrix Q(ImpactCategoryIndexLength, ElementaryFlowsIndexLength);
            libEigen::fillSparseMatrix(&Q, Qlength, rowsQ_ptr, colsQ_ptr, dataQ_ptr);

            VectorXd lciaX = Q * lciX;

            double *lciaR = new double[ImpactCategoryIndexLength];
            for (int i = 0; i < ImpactCategoryIndexLength; i++)
            {
                lciaR[i] = lciaX(i);
            }

            //cout << " putting LCIA " << iter << endl;

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

            if (iter != start && (iter) % slotsize == 0)
            {
                cout << " Splitting LCIA " << iter << endl;

                FileUtils fileUtils;
                string slotPath = settings.RootPath + "data/calculations/" + settings.CalculationId + "/Iters/LCIA/" + to_string(iterationOffset) + "_" + to_string(slotIndex);
                fileUtils.newfolder(slotPath);

                TwoDimStreamedVectorStore store;

                store.splitIterationsDataInFiles(
                    slotsize,
                    (*iterationResults_i).store_lcia_ptr,
                    ImpactCategoryIndexLength,
                    slotPath,
                    dataslotsize_output);
            }

            if (settings.Sensitivity)
            {
                if (iter != start && (iter) % slotsize == 0)
                {
                    FileUtils fileUtils;
                    string slotPath = settings.RootPath + "data/calculations/" + settings.CalculationId + "/Iters/Samples/" + to_string(iterationOffset) + "_" + to_string(slotIndex);
                    fileUtils.newfolder(slotPath);
                    TwoDimStreamedVectorStore store;

                    store.splitIterationsDataInFiles(
                        slotsize,
                        (*iterationResults_i).store_A_ptr,
                        (*calculatorData).A_unc_size,
                        slotPath,
                        dataslotsize_samples);

                    store.splitIterationsDataInFiles(slotsize,
                                                     (*iterationResults_i).store_B_ptr,
                                                     (*calculatorData).B_unc_size,
                                                     slotPath,
                                                     dataslotsize_samples);

                    store.splitIterationsDataInFiles(slotsize,
                                                     (*iterationResults_i).store_Q_ptr,
                                                     Qlength,
                                                     slotPath,
                                                     dataslotsize_samples);
                }
            }

            if (iter != start && iter % slotsize == 0)
            {
                delete iterationResults_i;

                iterationResults_i = new IterationsResults(settings,
                                                           (*calculatorData).impactCategoriesLength,
                                                           (*calculatorData).A_unc_size,
                                                           (*calculatorData).B_unc_size,
                                                           (*calculatorData).Q_size,
                                                           0,
                                                           slotsize);

                slotIndex++;
            }

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

        // return iterationResults_i;
    }
};

#endif /* SimulatorNative_hpp */
