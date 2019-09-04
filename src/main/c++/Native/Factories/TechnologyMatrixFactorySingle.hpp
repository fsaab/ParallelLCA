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

#ifndef TechnologyMatrixFactorySingle_hpp
#define TechnologyMatrixFactorySingle_hpp

#include <stdio.h>
#include <vector>
#include "../LCAModels/ExchangeItem.hpp"
#include "../Utilities/ResultsUtilsSingle.hpp"
#include "../Calculators/Models/AppSettings.hpp"

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

#include "../Calculators/Models/AppSettings.hpp"
#include "../Calculators/Models/CalculatorData.h"
#include "../Utilities/ResultsUtilsSingle.hpp"
#include "../LCAModels/LCAIndexes.hpp"
#include "../Utilities/LCAUtils.hpp"

#include "../DAL/LCADB.hpp"

#include "../Graph/LCAEdgeCompact.hpp"
#include <boost/range/algorithm.hpp>

#include "../Uncertainties/EntitySampler.hpp"
#include "../DAL/LCADBUtils.h"

using namespace std;

class TechnologyMatrixFactorySingle
{
  public:
    CalculatorData *calculatorData;
    LCADB *lcadb;
    AppSettings settings;

    TechnologyMatrixFactorySingle()
    {
    }

    TechnologyMatrixFactorySingle(LCADB *lcadb_, CalculatorData *calculatorData_, AppSettings settings_)
    {

        calculatorData = calculatorData_;
        lcadb = lcadb_;
        settings = settings_;
    }

    vector<ExchangeItem> readAExchangeItems(AppSettings settings)
    {
        LCADBUtils dbUtils(lcadb, settings);

        return dbUtils.readExchangeItems(
            settings.RootPath + "data/calculations/" + settings.CalculationId + "/CellsA.txt");
    }

    vector<ExchangeItem> AExchangeItems(GraphData *graph)
    {


        LCAUtils lcaUtils(lcadb, calculatorData);

        (*calculatorData).lcaIndexes.IntermediateFlowsIndex = lcaUtils.getOutputIntermediateFlowIdsArray((*calculatorData).lcaIndexes.ProcessesIndex);
        (*calculatorData).lcaIndexes.loadIndexesForItermediateFlows();

        vector<ExchangeItem> flows;

        //val edges=g.g.edges
        for (auto &&edentry : (*graph).edgesListCompact)
        {

            LCAEdgeCompact ed=edentry.second;
            // g.g.edges.foreach(ed=>{

            Exchange exch = (*lcadb).exchanges[ed.InputExchangeId];

            long uniqueFlowId = ed.ProducerExchangeId;


            ExchangeItem eitem = ExchangeItem{

                (*calculatorData).lcaIndexes.IntermediateFlowsIndexIndices[uniqueFlowId],
                (*calculatorData).lcaIndexes.ProcessesIndexIndices[exch.processId],
               -1,-1,-1,-1,
                exch
                
                };

            flows.push_back(eitem);
        }

        vector<ExchangeItem> outflows = lcaUtils.getOutputIntermediateFlowsArray((*calculatorData).lcaIndexes.ProcessesIndex);
        flows.insert(flows.end(), outflows.begin(), outflows.end());

        return flows;
    }

    vector<ExchangeItem> AExchangeItemsAll()
    {

        LCAUtils lcaUtils(lcadb, calculatorData);

        vector<long> outflowsids = lcaUtils.getOutputIntermediateFlowIdsArray((*calculatorData).lcaIndexes.ProcessesIndex);

        (*calculatorData).lcaIndexes.IntermediateFlowsIndex = outflowsids;

        (*calculatorData).lcaIndexes.loadIndexesForItermediateFlows();

        vector<ExchangeItem> flows = lcaUtils.getIntermediateFlowsArray((*calculatorData).lcaIndexes.ProcessesIndex);

        return flows;
    }

    vector<ExchangeItem> exchanges(AppSettings settings,
                                   long *Ai_ptr, long *Aj_ptr, vector<string> A_ptr)
    {

        LCADBUtils dbUtils(lcadb, settings);

        return dbUtils.loadExchangeItems(Ai_ptr, Aj_ptr, A_ptr);
    }

    vector<ExchangeItem> buildExchanges(AppSettings settings,
                                        long *Ai_ptr, long *Aj_ptr, vector<string> A_ptr)
    {

        LCADBUtils dbUtils(lcadb, settings);

        return dbUtils.loadExchangeItems(Ai_ptr, Aj_ptr, A_ptr);
    }

    //    static vector<ExchangeCell> cells(AppSettings settings){
    //
    //        ResultsUtilsSingle resUtils;
    //
    //        return resUtils.readExchangeCells(settings.RootPath+"data/calculations/"+settings.CalculationId+"/A.txt");
    //
    //    }

    static SMatrix build(
        AppSettings settings,
        CalculatorData *calculatorData)
    {

        long Alength = ((*calculatorData).A_exchanges).size();
        long *rowsA_ptr = new long[Alength];
        long *colsA_ptr = new long[Alength];
        double *dataA_ptr = new double[Alength];
        // cout << "Alength" <<Alength<<endl;

        EntitySampler<double> entitySampler;

        int i = 0;
        while (i < Alength)
        {
            ExchangeItem ent = (*calculatorData).A_exchanges[i];
            rowsA_ptr[i] = ent.i;
            colsA_ptr[i] = ent.j;

            dataA_ptr[i] = entitySampler.getCellValue(ent.exch);

            i++;
        }
        int Arows = (*calculatorData).lcaIndexes.IntermediateFlowsIndexLength();

        SMatrix m(Arows, Arows);

        libEigen::fillSparseMatrix(&m, Alength, rowsA_ptr, colsA_ptr, dataA_ptr);
        return m;
    }

    static SMatrix sample(
        AppSettings settings,
        CalculatorData *calculatorData)
    {
        EntitySampler<double> entitySampler = EntitySampler<double>();

        long Alength = ((*calculatorData).A_exchanges).size();
        long *rowsA_ptr = new long[Alength];
        long *colsA_ptr = new long[Alength];
        double *dataA_ptr = new double[Alength];

        unsigned seed = rand();

        std::default_random_engine gen(seed);

        int i = 0;
        while (i < Alength)
        {
            ExchangeItem ent = (*calculatorData).A_exchanges[i];
            rowsA_ptr[i] = ent.i;
            colsA_ptr[i] = ent.j;
            dataA_ptr[i] = entitySampler.getNextSimulatedCellValue(&ent, gen
                                                                   //, (*lcadb).parametersTable.interpreters["PROCESS_" + ent.exch.exchangeId]
            );
            i++;
        }

        int Arows = (*calculatorData).lcaIndexes.IntermediateFlowsIndexLength();

        SMatrix m(Arows, Arows);

        libEigen::fillSparseMatrix(&m, Alength, rowsA_ptr, colsA_ptr, dataA_ptr);
        return m;
    }
};

#endif /* TechnologyMatrixFactorySingle_hpp */
