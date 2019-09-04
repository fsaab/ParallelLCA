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
#include <sstream>
#include <algorithm>
#include <boost/algorithm/string/join.hpp>
#include <tr1/unordered_map>
#include <Eigen/Dense>
#include <Eigen/SparseCore>
#include <Eigen/SparseLU>
#include <Eigen/IterativeLinearSolvers>
#include <omp.h>
#include <string>
#include <vector>

#include "../Calculators/Models/AppSettings.hpp"
#include "../Calculators/Models/CalculatorData.h"
#include "../Utilities/ResultsUtilsSingle.hpp"
#include "../Calculators/Models/TwoDimStore.hpp"
#include "../LCAModels/LCAIndexes.hpp"
#include "../LCAModels/CalcImpactFactorItem.hpp"
#include "../LCAModels/ExchangeItem.hpp"
#include "../libs/libEigen.hpp"
#include "../Uncertainties/EntitySampler.hpp"
#include "../Calculators/Models/IterationsResults.hpp"
#include "../Utilities/ParameterUtils.h"
#include "../Graph/GraphData.h"
#include "../Calculators/Models/CalculatorData.h"

using Eigen::BiCGSTAB;
using Eigen::ConjugateGradient;
using Eigen::SparseMatrix;
using Eigen::VectorXd;

typedef Eigen::Triplet<double> Triplet;
typedef Eigen::SparseMatrix<double> SMatrix;

class SimulatorNative
{

public:
    GraphData *graph;
    CalculatorData *calculatorData;
    AppSettings settings;
    /*<scope_owner,expresion>*/
    /*unordered_*/ map<string, expression_t> expressionsTable;

    SimulatorNative(GraphData *graph_, CalculatorData *calculatorData_,
                    AppSettings settings_)
    {
        graph = graph_;
        calculatorData = calculatorData_;
        settings = settings_;
    }

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

    double *loadDemandBacgroundLayer(long ProcessesIndexLength_bl)
    {
        double *demand = new double[ProcessesIndexLength_bl + 1];

        //create the new demand vector
        for (int k = 0; k < ProcessesIndexLength_bl + 1; k++)
        {
            if (k == ProcessesIndexLength_bl)
            {
                demand[k] = 1;
            }
            else
            {
                demand[k] = 0;
            }
        }
        return demand;
    }

    void loadAOnePlusBackground(AppSettings settings,
                                long *rowsA_ptr_bl,
                                long *colsA_ptr_bl,
                                double *dataA_ptr_bl, std::default_random_engine gen
                                //map<long, double> procsScalarsMap
    )
    {

        auto strtA = std::chrono::high_resolution_clock::now();

        EntitySampler<double> entitySampler = EntitySampler<double>();

        /*bglayer variables*/
        long ProcessesIndexLength = (*calculatorData).processesLength;

        long ProcessesIndexLength_bl = ProcessesIndexLength - (*calculatorData).processesLength_frontLayer;

        //cout << "load background layer raw data into arrays" << endl;
        /*load background layer raw data into arrays*/
        int i = 0, j = 0;

        for (auto &&cell : (*calculatorData).A_background_exchanges)
        {
            ExchangeItem ent = cell;
            EntitySampler<double> entitySampler = EntitySampler<double>();

            //entitySampler.get/*NextSimulated*/CellValue(ent.exch/* ,gen*/);
            double v = entitySampler.getCellValue(ent.exch);

            rowsA_ptr_bl[j] = ent.i_bl;
            colsA_ptr_bl[j] = ent.j_bl;
            dataA_ptr_bl[j] = v;
            j++;
        }

        //cout << "adding front layer node" << endl;

        rowsA_ptr_bl[j] = ProcessesIndexLength_bl;
        colsA_ptr_bl[j] = ProcessesIndexLength_bl;
        dataA_ptr_bl[j] = 1; // always +1
        j++;
        //cout << "Load the new matrix: foreground as one node + bacground layer" << endl;

        // Load the new matrix: foreground as one node + bacground layer
        for (long fl_barrieri_proc : (*calculatorData).lcaIndexes.FrontLayerBarrier_ProcessesIndex)
        {

            vector<std::pair<long, Exchange>> fl_proc_demands = (*calculatorData).barrier_demand[fl_barrieri_proc];

            for (std::pair<long, Exchange> demand : fl_proc_demands)
            {
                double unit_demand = entitySampler.getCellValue(demand.second);
                double total_demand = (*graph).nodesInfoMap[fl_barrieri_proc].scalar * unit_demand;

                rowsA_ptr_bl[j] = (*calculatorData).lcaIndexes.BackgroundLayer_ProcessesIndexIndices[demand.first]; // Index of the producer
                colsA_ptr_bl[j] = ProcessesIndexLength_bl;
                dataA_ptr_bl[j] = total_demand;

                //cout << "demand " << unit_demand << " " << procsScalarsMap[fl_barrieri_proc] << endl;

                j++;
            }
        }

        auto finishA = std::chrono::high_resolution_clock::now();
        auto elapsedA = std::chrono::duration_cast<std::chrono::microseconds>(finishA - strtA);
        //std::cout << "A/c++," << settings.ProjectId << "," << elapsedA.count() << std::endl;
    }

    void loadB_bgl(AppSettings settings,
                   long *rowsB_ptr, long *colsB_ptr, double *dataB_ptr,
                   long Blength, std::default_random_engine gen)
    {

        EntitySampler<double> sampler;

        auto strtB = std::chrono::high_resolution_clock::now();

        int i = 0;
        for (auto &&ent : (*calculatorData).B_exchanges)
        {

            if (ent.exch.isBackground)
            {

                rowsB_ptr[i] = ent.i_bl;
                colsB_ptr[i] = ent.j_bl;
                dataB_ptr[i] = sampler.getNextSimulatedCellValue(&ent, gen);

                i++;
            }
        }

        auto finishB = std::chrono::high_resolution_clock::now();
        auto elapsedB = std::chrono::duration_cast<std::chrono::microseconds>(finishB - strtB);
        //std::cout << "B/c++," << settings.ProjectId << "," << elapsedB.count() << std::endl;
    }

    double *lci_bgl(AppSettings settings, double *scalars_PTR, long ProcessesIndexLength,

                    long *rowsB_ptr,
                    long *colsB_ptr,
                    double *dataB_ptr,
                    long Blength,
                    long ElementaryFlowsIndexLength)
    {

       auto strtlci = std::chrono::high_resolution_clock::now();
              

        long ProcessesIndexLength_bl = ProcessesIndexLength - (*calculatorData).processesLength_frontLayer;
        libEigen lib;

        double *lciR = lib.MV_mult(ElementaryFlowsIndexLength,
                                   ProcessesIndexLength_bl,
                                   Blength,
                                   rowsB_ptr,
                                   colsB_ptr,
                                   dataB_ptr,
                                   ProcessesIndexLength_bl,
                                   scalars_PTR);
                auto finishlci = std::chrono::high_resolution_clock::now();
                auto elapsedlci = std::chrono::duration_cast<std::chrono::microseconds>(finishlci - strtlci);
                //std::cout << "lci/c++," << settings.ProjectId << "," << elapsedlci.count() << std::endl;

        return lciR;
    }

    void solveSequentialGraph(AppSettings settings)
    {
        EntitySampler<double> entitySampler;

        long ElementaryFlowsIndexLength = (*calculatorData).elementaryFlowsLength;
        long ProcessesIndexLength = (*calculatorData).processesLength;
        long ImpactCategoryIndexLength = (*calculatorData).impactCategoriesLength;

        /*foregroundlayer variables*/
        long ProcessesIndexLength_fl = (*calculatorData).processesLength_frontLayer;
        int length_frontlayer = (*calculatorData).lcaIndexes.FrontLayer_ProcessesIndex.size();
        //cout << "length_frontlayer " << length_frontlayer << endl;
        long fl_barrier_size = (*calculatorData).lcaIndexes.FrontLayerBarrier_ProcessesIndex.size();
        //cout << "fl_barrier_size " << fl_barrier_size << endl;

        //SMatrix m_A(length_frontlayer, length_frontlayer);

        /*Collapse/solve the foreground layer*/
        if (fl_barrier_size > 0)
        {
            // cout << "settings.RootProcessId "
            //      << (*calculatorData).lcaIndexes.FrontLayer_ProcessesIndexIndices[settings.RootProcessId]
            //      << endl;

            long rootId = (*calculatorData).stack[0][0];

            bool rootIsForeground = (*calculatorData).lcaIndexes.FrontLayer_ProcessesIndexIndices.find(rootId) !=
                                    (*calculatorData).lcaIndexes.FrontLayer_ProcessesIndexIndices.end();

            EntitySampler<double> sampler;
            long stacksize = (*calculatorData).stack.size();

            if (rootIsForeground)
            {

                for (int k = 1; k < stacksize; k++) // skip first layer
                {
                    vector<LCAEdgeCompact> stackEdges = (*graph).edgesStack[k - 1];
                    long stackEdgesSize = stackEdges.size();

                    for (long se = 0; se < stackEdgesSize; se++)
                    {

                        (*(stackEdges[se].OutputScalar)).scalar = 0;
                    }
                }

                for (int k = 1; k < stacksize; k++) // skip first layer
                {
                    vector<LCAEdgeCompact> stackEdges = (*graph).edgesStack[k - 1];
                    long stackEdgesSize = stackEdges.size();

                    for (long se = 0; se < stackEdgesSize; se++)
                    {
                        LCAEdgeCompact currEdge = stackEdges[se];
                        //loop over the processes in each layer of the stack

                        double unit_supply = abs(currEdge.OutputCalculatedValue);

                        double unit_demand = abs(currEdge.InputCalculatedValue);

                        double total_demand = unit_demand * (*(stackEdges[se].InputScalar)).scalar; //procsScalarsMap[currEdge.ProcessSrcId];

                        double scalar = total_demand / unit_supply;

                        (*(stackEdges[se].OutputScalar)).scalar += scalar;

                        // procsScalarsMap[currEdge.ProcessDestId]+=scalar;
                    }
                }
            }
        }
    }

    IterationsResults *iterate_sequential(int start, int end, int iterationOffset,
                                          int slotsP,
                                          int slotsize
                                          /*,
                                          double *scalars_PTR_g*/
                                          // ParameterTable ptbl
    )
    {

        long ElementaryFlowsIndexLength = (*calculatorData).elementaryFlowsLength;
        long ProcessesIndexLength = (*calculatorData).processesLength;
        long ImpactCategoryIndexLength = (*calculatorData).impactCategoriesLength;

        ParameterUtils<double> paramUtils;

        srand(time(NULL) + iterationOffset);
        unsigned seed = rand();
        std::default_random_engine gen(seed);

        //long Plength = ptbl.parameters.size();
        int iter = start;

        EntitySampler<double> entitySampler;

        IterationsResults *iterationResults_i = new IterationsResults(settings,
                                                                      (*calculatorData).impactCategoriesLength,
                                                                      (*calculatorData).A_unc_size,
                                                                      (*calculatorData).B_unc_size,
                                                                      (*calculatorData).Q_size,
                                                                      0,
                                                                      slotsize);

        for (auto &&cell : (*calculatorData).A_exchanges)
        {
            paramUtils.reloadExchangesDistribution(cell);
        }
        for (auto &&cell : (*calculatorData).B_exchanges)
        {
            paramUtils.reloadExchangesDistribution(cell);
        }

        /**END   read file fo corresponding iteration offset*/

        while (iter <= end)
        {

            libEigen lib;

            bool isRootFrotnlayer = std::find((*calculatorData).lcaIndexes.FrontLayer_ProcessesIndex.begin(),
                                              (*calculatorData).lcaIndexes.FrontLayer_ProcessesIndex.end(),
                                              settings.RootProcessId) != (*calculatorData).lcaIndexes.FrontLayer_ProcessesIndex.end();

            double *lciR;

            if (isRootFrotnlayer)
            {

                auto strtS = std::chrono::high_resolution_clock::now();

                long ElementaryFlowsIndexLength = (*calculatorData).elementaryFlowsLength;
                long ProcessesIndexLength = (*calculatorData).processesLength;
                long ImpactCategoryIndexLength = (*calculatorData).impactCategoriesLength;

                /*foregroundlayer variables*/
                long ProcessesIndexLength_fl = (*calculatorData).processesLength_frontLayer;
                int length_frontlayer = (*calculatorData).lcaIndexes.FrontLayer_ProcessesIndex.size();
                //cout << "length_frontlayer " << length_frontlayer << endl;
                long fl_barrier_size = (*calculatorData).lcaIndexes.FrontLayerBarrier_ProcessesIndex.size();
                //cout << "fl_barrier_size " << fl_barrier_size << endl;

                /*bglayer variables*/
                long ProcessesIndexLength_bl = ProcessesIndexLength - (*calculatorData).processesLength_frontLayer;

                //cout << "ProcessesIndexLength_bl: " << ProcessesIndexLength_bl << endl;
                double *demand = loadDemandBacgroundLayer(ProcessesIndexLength_bl);

                long lengthTmp = 0;

                for (long fl_barrieri_proc : (*calculatorData).lcaIndexes.FrontLayerBarrier_ProcessesIndex)
                    lengthTmp += (*calculatorData).barrier_demand[fl_barrieri_proc].size();

                long Alength_bl = ((*calculatorData).A_size - (*calculatorData).A_size_fl) +
                                  (lengthTmp) + // in_exchnages
                                  +1;           // +1 is to account for the collapsed front layer

                solveSequentialGraph(settings);

                auto finishS = std::chrono::high_resolution_clock::now();
                auto elapsedS = std::chrono::duration_cast<std::chrono::microseconds>(finishS - strtS);
                //std::cout << "solve_sequential," << settings.ProjectId << "," << elapsedS.count() << std::endl;

                long *rowsA_ptr_bl = new long[Alength_bl];
                long *colsA_ptr_bl = new long[Alength_bl];
                double *dataA_ptr_bl = new double[Alength_bl];

                loadAOnePlusBackground(settings,
                                       rowsA_ptr_bl,
                                       colsA_ptr_bl,
                                       dataA_ptr_bl, gen);

                auto strtsolve = std::chrono::high_resolution_clock::now();

                int frontLayer = 1; // "1" is for the collapssed front layer

                double *scalars_PTR = lib.solve_bicgstab(ProcessesIndexLength_bl + frontLayer,
                                                         Alength_bl,
                                                         rowsA_ptr_bl,
                                                         colsA_ptr_bl,
                                                         dataA_ptr_bl,
                                                         demand);

                delete[] rowsA_ptr_bl;
                delete[] colsA_ptr_bl;
                delete[] dataA_ptr_bl;

                delete[] demand;

                for (int i = 0; i < ProcessesIndexLength_bl; i++)
                {
                    long processid = (*calculatorData).lcaIndexes.BackgroundLayer_ProcessesIndex[i];
                    (*graph).nodesInfoMap[processid].scalar = scalars_PTR[i];
                }

                auto finishsolve = std::chrono::high_resolution_clock::now();
                auto elapsedsolve = std::chrono::duration_cast<std::chrono::microseconds>(finishsolve - strtsolve);
                //std::cout << "solve_bgl_/c++," << settings.ProjectId << "," << elapsedsolve.count() << std::endl;


                 long Blength = (*calculatorData).B_size;
                long *rowsB_ptr = new long[Blength];
                long *colsB_ptr = new long[Blength];
                double *dataB_ptr = new double[Blength];

                loadB_bgl(settings, rowsB_ptr, colsB_ptr, dataB_ptr, Blength, gen);


              

                
                lciR = lci_bgl(settings, scalars_PTR, ProcessesIndexLength,
                               rowsB_ptr, colsB_ptr, dataB_ptr, Blength, ElementaryFlowsIndexLength);
 
                delete[] rowsB_ptr;
                delete[] colsB_ptr;
                delete[] dataB_ptr;

                delete[] scalars_PTR;

               
            }
            auto strtQ = std::chrono::high_resolution_clock::now();

            long Qlength = (*calculatorData).Q_size;
            long *rowsQ_ptr = new long[Qlength];
            long *colsQ_ptr = new long[Qlength];
            double *dataQ_ptr = new double[Qlength];

            int i = 0;
            for (auto &&cell : (*calculatorData).Q_cells)
            {

                CalcImpactFactorItem ent = cell;
                rowsQ_ptr[i] = ent.i;
                colsQ_ptr[i] = ent.j;
                dataQ_ptr[i] = entitySampler.getImpactCellValue(ent.imf);

                i++;
            }

            auto finishQ = std::chrono::high_resolution_clock::now();
            auto elapsedQ = std::chrono::duration_cast<std::chrono::microseconds>(finishQ - strtQ);
            //std::cout << "Q/c++," << settings.ProjectId << "," << elapsedQ.count() << std::endl;
            auto strtlcia = std::chrono::high_resolution_clock::now();

            double *lciaR = lib.MV_mult(ImpactCategoryIndexLength,
                                        ElementaryFlowsIndexLength,
                                        Qlength,
                                        rowsQ_ptr,
                                        colsQ_ptr,
                                        dataQ_ptr,
                                        ElementaryFlowsIndexLength,
                                        lciR);
            auto finishlcia = std::chrono::high_resolution_clock::now();
            auto elapsedlcia = std::chrono::duration_cast<std::chrono::microseconds>(finishlcia - strtlcia);
            //std::cout << "lcia/c++," << settings.ProjectId << "," << elapsedlcia.count() << std::endl;

            auto strtlcias = std::chrono::high_resolution_clock::now();
            TwoDimVectorStore::putDataAtIteration((*iterationResults_i).store_lcia_ptr, iter - iterationOffset, lciaR, ImpactCategoryIndexLength);
            auto finishlcias = std::chrono::high_resolution_clock::now();
            auto elapsedlcias = std::chrono::duration_cast<std::chrono::microseconds>(finishlcias - strtlcias);
            //std::cout << "lcias/c++," << settings.ProjectId << "," << elapsedlcias.count() << std::endl;
            // if (settings.Sensitivity)
            // {
            //     if (dataA_ptr_unc.size() > 0)
            //         TwoDimVectorStore::putDataAtIteration((*iterationResults_i).store_A_ptr, iter - iterationOffset, &dataA_ptr_unc[0], (*calculatorData).A_unc_size);

            //     if (dataB_ptr_unc.size() > 0)
            //         TwoDimVectorStore::putDataAtIteration((*iterationResults_i).store_B_ptr, iter - iterationOffset, &dataB_ptr_unc[0], (*calculatorData).B_unc_size);

            //     TwoDimVectorStore::putDataAtIteration((*iterationResults_i).store_Q_ptr, iter - iterationOffset, dataQ_ptr, Qlength);

            //     // TwoDimVectorStore::putDataAtIteration((*iterationResults_i).store_P_ptr, iter - iterationOffset, dataP_ptr, Plength);
            // }

            delete[] rowsQ_ptr;
            delete[] colsQ_ptr;
            delete[] dataQ_ptr;
            // delete[] dataP_ptr;

            delete[] lciaR;
            delete[] lciR;

            iter++;
        }

        return iterationResults_i;
    }
};

#endif /* SimulatorNative_hpp */
