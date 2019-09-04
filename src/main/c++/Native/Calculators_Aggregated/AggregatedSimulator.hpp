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
#ifndef AggregatedSimulator_hpp
#define AggregatedSimulator_hpp

#include <stdio.h>
#include <time.h>
#include <string>
#include <vector>
#include <chrono> // for high_resolution_high_resolution_clock
#include <sstream>
#include <algorithm>
#include <omp.h>
#include <tr1/unordered_map>
#include <boost/range/algorithm.hpp>
#include <boost/algorithm/string/join.hpp>
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

#include "../libs/libEigen.hpp"
#include "../Graph/GraphData.h"
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
#include "../Calculators/Models/CalculatorData.h"
#include "../Graph/GraphData.h"

using namespace std;

class AggregatedSimulator
{

public:
    CalculatorData *calculatorData;
    AppSettings settings;

    AggregatedSimulator(CalculatorData *calculatorData_,
                        AppSettings settings_)
    {

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

    void solveSequentialGraph(GraphData *graph, AppSettings settings)
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

        auto strtS = std::chrono::high_resolution_clock::now();

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

            for (int k = 1; k < stacksize; k++) // skip first layer
            {
                vector<LCAEdgeCompact> stackEdges = (*graph).edgesStack[k - 1];
                long stackEdgesSize = stackEdges.size();

                for (long se = 0; se < stackEdgesSize; se++)
                {

                    (*(stackEdges[se].OutputScalar)).scalar = 0;
                }
            }

            if (rootIsForeground)
            {
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

                        double total_demand = unit_demand * (*(stackEdges[se].InputScalar)).scalar;

                        double scalar = total_demand / unit_supply;

                        (*(stackEdges[se].OutputScalar)).scalar += scalar;

                        // cout << unit_supply << "|"
                        //      << unit_demand << "|"
                        //      << total_demand << "|"
                        //      << (*(stackEdges[se].OutputScalar)).scalar << endl;

                        // procsScalarsMap[currEdge.ProcessDestId]+=scalar;
                    }
                }
            }
        }

        auto finishS = std::chrono::high_resolution_clock::now();
        auto elapsedS = std::chrono::duration_cast<std::chrono::microseconds>(finishS - strtS);
        //std::cout << "solve_sequential," << settings.ProjectId << "," << elapsedS.count() << std::endl;
    }

    IterationsResults *iterate_aggreagtedDS(int start, int end, int iterationOffset,
                                            int slotsP,
                                            int slotsize, GraphData *graph
                                            //,double *scalars_PTR_g
    )
    {

        ParameterUtils<double> paramUtils;

        srand(time(NULL) + iterationOffset);
        unsigned seed = rand();

        std::default_random_engine gen(seed);

        long ImpactCategoryIndexLength = (*calculatorData).impactCategoriesLength;

        long Plength = 3; //ptbl.parameters.size();
        int iter = start;

        EntitySampler<double> entitySampler;

        IterationsResults *iterationResults_i = new IterationsResults(settings,
                                                                      (*calculatorData).impactCategoriesLength,
                                                                      (*calculatorData).A_unc_size,
                                                                      (*calculatorData).B_unc_size,
                                                                      (*calculatorData).Q_size,
                                                                      Plength,
                                                                      slotsize);

        for (auto &&cell : (*calculatorData).A_exchanges)
        {
            paramUtils.reloadExchangesDistribution(cell);
        }

        for (auto &&cell : (*calculatorData).B_exchanges)
        {
            paramUtils.reloadExchangesDistribution(cell);
        }

        /**read file for corresponding iteration offset*/

        FileUtils fileUtils;

        map<long, vector<vector<double>>> scoresvals;

        //cout << "BackgroundLayerBarrier_ProcessesIndex " << (*calculatorData).lcaIndexes.BackgroundLayerBarrier_ProcessesIndex.size();

        for (long bl_barrier_procid : (*calculatorData).lcaIndexes.BackgroundLayerBarrier_ProcessesIndex)
        {

            string scores_path;
            scores_path = "data/Scores/" + to_string(settings.montecarlo_iterations) + "/" + to_string(settings.ImpactMethodId) + "/" + to_string(iterationOffset) + "/" + to_string(bl_barrier_procid) + ".txt";

            //cout << "reading scores samples " << scores_path << endl;

            vector<string> scoreslines = fileUtils.readTextFromFile(scores_path);
            // cout << "scoreslines.size()" << scoreslines.size() << endl;

            scoresvals[bl_barrier_procid].resize(scoreslines.size());

            for (int i_scoreline = 0; i_scoreline < scoreslines.size(); i_scoreline++)
            {
                string scoreline = scoreslines[i_scoreline];
                //cout<<scoreline<<"++++++++++++++"<<endl;
                vector<string> scores = split(scoreline, ",");

                for (int j = 0; j < scores.size(); j++)
                {
                    string score = scores[j];
                    if (score != "")
                    {
                        scoresvals[bl_barrier_procid][i_scoreline].push_back(std::stod(score));
                        //cout<<score <<"++";
                    }
                }
            }

            //cout << "scores parsed for" << iterationOffset << endl;
        }

        //cout << "done reading scores for" << iterationOffset << endl;

        /**END read file for corresponding iteration offset*/

        while (iter <= end)
        {
            //double *dataP_ptr = new double[Plength];

            int length_frontlayer = (*calculatorData).lcaIndexes.FrontLayer_ProcessesIndex.size();
            //cout << "length_frontlayer " << length_frontlayer << endl;

            long fl_barrier_size = (*calculatorData).lcaIndexes.FrontLayerBarrier_ProcessesIndex.size();
            //cout << "fl_barrier_size " << fl_barrier_size << endl;

            bool isRootFrotnlayer = std::find((*calculatorData).lcaIndexes.FrontLayer_ProcessesIndex.begin(),
                                              (*calculatorData).lcaIndexes.FrontLayer_ProcessesIndex.end(),
                                              settings.RootProcessId) != (*calculatorData).lcaIndexes.FrontLayer_ProcessesIndex.end();

            double *lciR;

            libEigen lib;

            double *lciaR = new double[(*calculatorData).impactCategoriesLength];

            //cout << "fl_barrier_size " << fl_barrier_size << endl;
            if (fl_barrier_size == 0) //if analysing background layer root node
            {
                for (int ici = 0; ici < (*calculatorData).impactCategoriesLength; ici++)
                {
                    lciaR[ici] += scoresvals[settings.RootProcessId][(iter - iterationOffset)][ici];
                }
            }
            else
            {

                if (isRootFrotnlayer)
                {
                    solveSequentialGraph(graph, settings);
                }

                for (int i = 0; i < ImpactCategoryIndexLength; i++)
                    lciaR[i] = 0;

                for (long fl_barrieri_proc : (*calculatorData).lcaIndexes.FrontLayerBarrier_ProcessesIndex)
                {

                    //cout<<"fl barr id "<<fl_barrieri_proc<<endl;
                    vector<std::pair<long, Exchange>> fl_proc_demands = (*calculatorData).barrier_demand[fl_barrieri_proc];
                    for (std::pair<long, Exchange> demand : fl_proc_demands)
                    {
                        ExchangeItem *e = new ExchangeItem{-1, -1, -1, -1, -1, -1, demand.second};
                        double unit_demand = abs(entitySampler.getCellValue((*e).exch));
                        double scalar = (*graph).nodesInfoMap[fl_barrieri_proc].scalar;
                        //entitySampler.getNextSimulatedCellValue(new ExchangeItem{-1, -1, -1, -1, -1, -1, demand.second}, gen);
                        double total_demand = abs(scalar) * unit_demand;
                        long bgl_procid = demand.first;

                        for (int ici = 0; ici < (*calculatorData).impactCategoriesLength; ici++)
                        {

                            lciaR[ici] += total_demand * scoresvals[bgl_procid][(iter - iterationOffset)][ici];
                            // cout << "<<>>" << unit_demand << " " << (*graph).nodesInfoMap[fl_barrieri_proc].scalar << " " << total_demand << " " << bgl_procid << " "
                            //      << scoresvals[bgl_procid][(iter - iterationOffset)][ici] << endl;
                        }
                    }
                }
            }

            // for (int i = 0; i < ImpactCategoryIndexLength; i++)
            //     cout << lciaR[i] << " ------- ";
            // cout << endl;

            TwoDimVectorStore::putDataAtIteration((*iterationResults_i).store_lcia_ptr, iter - iterationOffset, lciaR, ImpactCategoryIndexLength);

            //delete[] dataQ_ptr;
            //delete[] dataP_ptr;
            delete[] lciaR;

            iter++;
        }

        return iterationResults_i;
    }
};

#endif /* AggregatedSimulator_hpp */
