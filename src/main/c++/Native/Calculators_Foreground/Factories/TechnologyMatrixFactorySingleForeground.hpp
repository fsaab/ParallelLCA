//
//  TechnologyMatrixFactorySingle.hpp
//  LCA.Kernel
//
//  Created by francois saab on 2/17/18.
//  Copyright © 2018 fsaab. All rights reserved.
//

#ifndef TechnologyMatrixFactorySingleForeground_hpp
#define TechnologyMatrixFactorySingleForeground_hpp

#include <stdio.h>
#include <vector>
#include <Eigen/Dense>
#include <Eigen/SparseCore>
#include <Eigen/SparseLU>
#include <Eigen/IterativeLinearSolvers>
#include <boost/range/algorithm.hpp>

//#include <Eigen/UmfPackSupport>
using Eigen::BiCGSTAB;
using Eigen::SparseMatrix;
using Eigen::VectorXd;
typedef Eigen::Triplet<double> Triplet;
typedef Eigen::SparseMatrix<double> SMatrix;

#include "../../libs/libEigen.hpp"
#include "../../Calculators/Models/AppSettings.hpp"
#include "../../Calculators/Models/CalculatorData.h"
#include "../../Utilities/ResultsUtilsSingle.hpp"
#include "../../LCAModels/LCAIndexes.hpp"
#include "../Utilities/LCAUtilsForeground.hpp"
#include "../../DAL/LCADB.hpp"
#include "../../Graph/LCAEdgeCompact.hpp"
#include "../../Uncertainties/EntitySampler.hpp"
#include "../../DAL/LCADBUtils.h"
#include "../../LCAModels/ExchangeItem.hpp"
#include "../../Utilities/ResultsUtilsSingle.hpp"
#include "../../Calculators/Models/AppSettings.hpp"

using namespace std;

class TechnologyMatrixFactorySingleForeground
{
  public:
    CalculatorData *calculatorData;
    LCADB *lcadb;
    AppSettings settings;

    TechnologyMatrixFactorySingleForeground()
    {
    }

    TechnologyMatrixFactorySingleForeground(LCADB *lcadb_, CalculatorData *calculatorData_, AppSettings settings_)
    {

        calculatorData = calculatorData_;
        lcadb = lcadb_;
        settings = settings_;
    }

    static double getCellValue(ExchangeItem c)
    {
        return c.exch.amount * c.exch.conversionFactor * (c.exch.input ? -1 : 1);
    }

    vector<ExchangeItem> readAExchangeItems(AppSettings settings)
    {
        LCADBUtils dbUtils(lcadb, settings);

        return dbUtils.readExchangeItems(
            settings.RootPath + "data/calculations/" + settings.CalculationId + "/CellsA.txt");
    }

    vector<ExchangeItem> AExchangeItems(GraphData *graph)
    {

        LCAUtilsForeground lcaUtils(lcadb, calculatorData);

        (*calculatorData).lcaIndexes.IntermediateFlowsIndex = lcaUtils.getOutputIntermediateFlowIdsArray((*calculatorData).lcaIndexes.ProcessesIndex);
        (*calculatorData).lcaIndexes.loadIndexesForItermediateFlows();

        (*calculatorData).lcaIndexes.FrontLayer_IntermediateFlowsIndex = lcaUtils.getOutputIntermediateFlowIdsArray((*calculatorData).lcaIndexes.FrontLayer_ProcessesIndex);
        ;
        (*calculatorData).lcaIndexes.loadIndexesForFrontLayerItermediateFlows();

        (*calculatorData).lcaIndexes.BackgroundLayer_IntermediateFlowsIndex = lcaUtils.getOutputIntermediateFlowIdsArray((*calculatorData).lcaIndexes.BackgroundLayer_ProcessesIndex);
        (*calculatorData).lcaIndexes.loadIndexesForBackgroundLayerItermediateFlows();

        vector<ExchangeItem> flows;

        //val edges=g.g.edges
        for (auto &&edentry :  (*graph).edgesListCompact)
        {

            LCAEdgeCompact ed=edentry.second;
            // g.g.edges.foreach(ed=>{

            Exchange exch = (*lcadb).exchanges[ed.InputExchangeId];
            long uniqueFlowId = ed.ProducerExchangeId;

            bool includeInFlowFrontlayer = (*calculatorData).lcaIndexes.FrontLayer_IntermediateFlowsIndexIndices.find(uniqueFlowId) !=
                                           (*calculatorData).lcaIndexes.FrontLayer_IntermediateFlowsIndexIndices.end();

            bool includeInFlowBagroundlayer = (*calculatorData).lcaIndexes.BackgroundLayer_IntermediateFlowsIndexIndices.find(uniqueFlowId) !=
                                              (*calculatorData).lcaIndexes.BackgroundLayer_IntermediateFlowsIndexIndices.end();

            ExchangeItem eitem = ExchangeItem{
                (*calculatorData).lcaIndexes.IntermediateFlowsIndexIndices[uniqueFlowId],
                (*calculatorData).lcaIndexes.ProcessesIndexIndices[exch.processId],
                (includeInFlowFrontlayer ? (*calculatorData).lcaIndexes.FrontLayer_IntermediateFlowsIndexIndices[uniqueFlowId] : -1),
                (includeInFlowFrontlayer ? (*calculatorData).lcaIndexes.FrontLayer_ProcessesIndexIndices[exch.processId] : -1),
                (includeInFlowBagroundlayer ? (*calculatorData).lcaIndexes.BackgroundLayer_IntermediateFlowsIndexIndices[uniqueFlowId] : -1),
                (includeInFlowBagroundlayer ? (*calculatorData).lcaIndexes.BackgroundLayer_ProcessesIndexIndices[exch.processId] : -1),
                exch};

            flows.push_back(eitem);
        }

        vector<ExchangeItem> outflows = lcaUtils.getOutputIntermediateFlowsArray((*calculatorData).lcaIndexes.ProcessesIndex);
        flows.insert(flows.end(), outflows.begin(), outflows.end());

        return flows;
    }

    vector<ExchangeItem> AExchangeItemsAll()
    {

        LCAUtilsForeground lcaUtils(lcadb, calculatorData);

        vector<long> outflowsids = lcaUtils.getOutputIntermediateFlowIdsArray((*calculatorData).lcaIndexes.ProcessesIndex);

        (*calculatorData).lcaIndexes.IntermediateFlowsIndex = outflowsids;

        (*calculatorData).lcaIndexes.loadIndexesForItermediateFlows();

        vector<ExchangeItem> flows = lcaUtils.getIntermediateFlowsArray((*calculatorData).lcaIndexes.ProcessesIndex);

        return flows;
    }

    vector<ExchangeItem> exchanges(AppSettings settings, long *Ai_ptr, long *Aj_ptr, vector<string> A_ptr)
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

        int i = 0;
        while (i < Alength)
        {
            ExchangeItem ent = (*calculatorData).A_exchanges[i];
            rowsA_ptr[i] = ent.i;
            colsA_ptr[i] = ent.j;

            dataA_ptr[i] = getCellValue(ent);

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

#endif /* TechnologyMatrixFactorySingleForeground_hpp */
