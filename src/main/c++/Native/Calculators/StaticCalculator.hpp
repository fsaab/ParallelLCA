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

#ifndef StaticCalculator_hpp
#define StaticCalculator_hpp

#include <stdio.h>
#include <stdio.h>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <tr1/unordered_map>
#include <chrono> // for high_resolution_high_resolution_clock

#include "../Calculators/Models/AppSettings.hpp"
#include "../DAL/LCADBUtils.h"
#include "../LCAModels/LCAIndexes.hpp"
#include "../Graph/LCAEdgeCompact.hpp"
#include "../Factories/TechnologyMatrixFactorySingle.hpp"
#include "../Factories/InterventionMatrixFactorySingle.hpp"
#include "../LCAModels/ExchangeItem.hpp"
#include "../LCAModels/CalcImpactFactorItem.hpp"
#include "../Factories/CharacterisationMatrixFactory.hpp"
#include "../Utilities/FileUtils.hpp"
//#include "../libs/libUmfpack.hpp"
#include "../Utilities/LCAUtils.hpp"
#include "../Utilities/GraphUtilSingle.hpp"
#include "../Factories/DemandVectorFactory.hpp"
#include "../Utilities/ParameterUtils.h"
#include "../Graph/GraphData.h"
#include "../Upstream/UpstreamLCIACalculator.hpp"
#include "../Reports/LCIContributionReport.hpp"
#include "../Reports/LCIAContributionReport.hpp"
#include "../Messaging/LCAException.hpp"

using namespace std;

typedef exprtk::expression<double> expression_t;

class StaticCalculator
{

    /*<scope_owner,expresion>*/
    /*unordered_*/ map<string, expression_t> expressionsTable;

  public:
    CalculatorData *calculatorData;
    bool basic;
    GraphData *graph;
    LCADB *lcadb;

    StaticCalculator(CalculatorData *calculatorData_, bool basic_, GraphData *graph_, LCADB *lcadb_)
    {

        calculatorData = calculatorData_;
        basic = basic_;
        graph = graph_;
        lcadb = lcadb_;

        // ParameterUtils<double> paramUtils;
        // map<string /*scope_owner*/, vector<CalcParameter *>> parametersLeafNodes;
        // paramUtils.buildDependecyGraph((*lcadb).parameters, parametersLeafNodes);

        ParameterUtils<double> parameterUtils;

        parameterUtils.createExpressionTable(
            (*lcadb).parameters,
            expressionsTable,
            (*lcadb).parameterVariablesInfoMap);
    }

    double *solve(AppSettings settings)
    {
        typedef exprtk::expression<double> expression_t;

        auto strtA = std::chrono::high_resolution_clock::now();

        long Alength = (*calculatorData).A_exchanges.size();
        long *rowsA_ptr = new long[Alength];
        long *colsA_ptr = new long[Alength];
        double *dataA_ptr = new double[Alength];

        int i = 0;
        ResultsUtilsSingle res;
        EntitySampler<double> entitySampler;
        ParameterUtils<double> paramUtils;

        for (auto &&cell : (*calculatorData).A_exchanges)
        {
            ExchangeItem ent = cell;
            rowsA_ptr[i] = ent.i;
            colsA_ptr[i] = ent.j;

             if (cell.exch.amountFormula != "")
            {
                expression_t expressionTable_filtered = paramUtils.getExpressionTable(cell.exch.processId, "PROCESS", expressionsTable);
                dataA_ptr[i] = entitySampler.getCellValue(ent.exch, expressionTable_filtered);
            }
            else
            {
                dataA_ptr[i] = entitySampler.getCellValue(ent.exch);
            }


            i++;
        }

        auto finishA = std::chrono::high_resolution_clock::now();
        auto elapsedA = std::chrono::duration_cast<std::chrono::microseconds>(finishA - strtA);
        std::cout << "A/c++," << settings.ProjectId << "," << elapsedA.count() << std::endl;

        auto strtS = std::chrono::high_resolution_clock::now();

        if ((*calculatorData).demand_demandVectorArray.size() != (*calculatorData).processesLength)
        {
            throw LCAException(200, "While solving system in static phase, \n matrices( A, b) sizes does not match ");
        }

        if ((*calculatorData).processesLength != (*calculatorData).intermediateFlowsLength)
        {
            throw LCAException(200, "While solving system in static phase, matrix A is not square.");
        }

        libEigen lib;
        double *scalars_PTR = lib.solve_bicgstab((*calculatorData).demand_demandVectorArray.size(),
                                                 Alength,
                                                 rowsA_ptr,
                                                 colsA_ptr,
                                                 dataA_ptr,
                                                 &((*calculatorData).demand_demandVectorArray)[0]);

        auto finishS = std::chrono::high_resolution_clock::now();
        auto elapsedS = std::chrono::duration_cast<std::chrono::microseconds>(finishS - strtS);
        std::cout << "solve," << settings.ProjectId << "," << elapsedS.count() << std::endl;

        for (int i = 0; i < (*calculatorData).lcaIndexes.ProcessesIndexLength(); i++)
        {
            (*graph).nodesInfoMap[(*calculatorData).lcaIndexes.ProcessesIndex[i]].scalar = scalars_PTR[i];
        }

        delete[] rowsA_ptr;
        delete[] colsA_ptr;
        delete[] dataA_ptr;

        return scalars_PTR;
    }

    double *lci(AppSettings settings, double *scalars_PTR, long *rowsB_ptr, long *colsB_ptr, double *dataB_ptr)
    {
        long Blength = (*calculatorData).B_exchanges.size();

        double *lciR = libEigen::MV_mult((*calculatorData).lcaIndexes.ElementaryFlowsIndexLength(),
                                         (*calculatorData).lcaIndexes.ProcessesIndexLength(),
                                         Blength,
                                         rowsB_ptr,
                                         colsB_ptr,
                                         dataB_ptr,
                                         (*calculatorData).lcaIndexes.ProcessesIndexLength(),
                                         scalars_PTR);
        return lciR;
    }

    void loadB(AppSettings settings, long *rowsB_ptr, long *colsB_ptr, double *dataB_ptr)
    {
        auto strtB = std::chrono::high_resolution_clock::now();

        long Blength = (*calculatorData).B_exchanges.size();
        EntitySampler<double> entitySampler;
        ParameterUtils<double> paramUtils;

        int i = 0;
        for (auto &&cell : (*calculatorData).B_exchanges)
        {
            ExchangeItem ent = cell;
            rowsB_ptr[i] = ent.i;
            colsB_ptr[i] = ent.j;

            if (cell.exch.amountFormula != "")
            {
                expression_t expressionTable_filtered = paramUtils.getExpressionTable(cell.exch.processId, "PROCESS", expressionsTable);
                dataB_ptr[i] = entitySampler.getCellValue(ent.exch, expressionTable_filtered);
            }
            else
            {
                dataB_ptr[i] = entitySampler.getCellValue(ent.exch);
            }
            i++;
        }

        auto finishB = std::chrono::high_resolution_clock::now();
        auto elapsedB = std::chrono::duration_cast<std::chrono::microseconds>(finishB - strtB);
        std::cout << "B/c++," << settings.ProjectId << "," << elapsedB.count() << std::endl;
    }

    bool run(int RootProcessId,
             double OutputQunatity,
             string SolvingMethod,
             string SystemId,
             string CalculationId,
             int ImpactMethodId,
             int lcia,
             int ismontecarlo,
             int montecarlo_iterations,
             int Parallelism,
             string RootPath,
             long ProjectId,
             int Version)
    {

        AppSettings settings{
            RootProcessId,
            OutputQunatity,
            SolvingMethod,
            SystemId,
            CalculationId,
            ImpactMethodId,
            lcia == 1,
            ismontecarlo == 1,
            montecarlo_iterations,
            Parallelism,
            RootPath,
            ProjectId,
            Version, false};

        typedef exprtk::expression<double> expression_t;
        ParameterUtils<double> putils;

        double *scalars_PTR = solve(settings);

        long Blength = (*calculatorData).B_exchanges.size();
        long *rowsB_ptr = new long[Blength];
        long *colsB_ptr = new long[Blength];
        double *dataB_ptr = new double[Blength];

        loadB(settings, rowsB_ptr, colsB_ptr, dataB_ptr);

        double *lciR;
        lciR = lci(settings, scalars_PTR, rowsB_ptr, colsB_ptr, dataB_ptr);

        delete[] rowsB_ptr;
        delete[] colsB_ptr;
        delete[] dataB_ptr;
        delete scalars_PTR;

        auto strtI = std::chrono::high_resolution_clock::now();

        int elemslength = (*calculatorData).lcaIndexes.ElementaryFlowsIndexLength();
        string report_lci_path_out = settings.RootPath + "data/calculations/" + settings.CalculationId + "/" + "lci_array" + "_" + settings.CalculationId + ".txt";
        ofstream file_lci = ofstream(report_lci_path_out.c_str());

        std::stringstream ss_lci;
        for (int elemind = 0; elemind < elemslength; elemind++)
        {
            int elem = (*calculatorData).lcaIndexes.ElementaryFlowsIndex[elemind];
            ss_lci << elem << "," << (lciR[elemind]) << std::endl;
        }

        file_lci << ss_lci.str();
        file_lci.close();

        vector<LCIContribution> lcicontrib;

        if (!basic)
        {
            LCIContributionReport lcireport(settings, calculatorData, lcadb);
            lcicontrib = lcireport.lciContribution(graph, lciR, expressionsTable);
            lcireport.print(lcicontrib);
        }

        auto finishI = std::chrono::high_resolution_clock::now();
        auto elapsedI = std::chrono::duration_cast<std::chrono::microseconds>(finishI - strtI);
        std::cout << "lci/c++," << settings.ProjectId << "," << elapsedI.count() << std::endl;

        auto strtQ = std::chrono::high_resolution_clock::now();
        long Qlength = (*calculatorData).Q_cells.size();
        long *rowsQ_ptr = new long[Qlength];
        long *colsQ_ptr = new long[Qlength];
        double *dataQ_ptr = new double[Qlength];

        int i = 0;
        for (auto &&cell : (*calculatorData).Q_cells)
        {
            CalcImpactFactorItem ent = cell;
            rowsQ_ptr[i] = ent.i;
            colsQ_ptr[i] = ent.j;
            dataQ_ptr[i] = ent.imf.amount;
            i++;
        }

        auto finishQ = std::chrono::high_resolution_clock::now();
        auto elapsedQ = std::chrono::duration_cast<std::chrono::microseconds>(finishQ - strtQ);
        std::cout << "Q/c++," << settings.ProjectId << "," << elapsedQ.count() << std::endl;

        auto strtLCIA = std::chrono::high_resolution_clock::now();

        double *lciaR = libEigen::MV_mult((*calculatorData).lcaIndexes.ImpactCategoryIndexLength(),
                                          (*calculatorData).lcaIndexes.ElementaryFlowsIndexLength(),
                                          Qlength,
                                          rowsQ_ptr,
                                          colsQ_ptr,
                                          dataQ_ptr,
                                          (*calculatorData).lcaIndexes.ElementaryFlowsIndexLength(),
                                          lciR);

        int catlength = (*calculatorData).lcaIndexes.ImpactCategoryIndexLength();

        string report_path_out = settings.RootPath + "data/calculations/" + settings.CalculationId + "/" + "lcia_array" + "_" + settings.CalculationId + ".txt";
        ofstream file = ofstream(report_path_out.c_str());

        std::stringstream ss;

        for (int categoryind = 0; categoryind < catlength; categoryind++)
        {
            int cat = (*calculatorData).lcaIndexes.ImpactCategoryIndex[categoryind];
            ss << cat << "," << (lciaR[categoryind]) << std::endl;
        }

        file << ss.str();
        file.close();

        if (!basic)
        {
            LCIAContributionReport lciareport(settings, calculatorData, lcadb);
            CharacterisationMatrixFactory charactFactory(lcadb, calculatorData, settings);
            SMatrix Q = charactFactory.build(settings, calculatorData);
            lciareport.print(lciareport.lciaContribution(lcicontrib, lciaR, Q));
        }

        auto finishLCIA = std::chrono::high_resolution_clock::now();
        auto elapsedLCIA = std::chrono::duration_cast<std::chrono::microseconds>(finishLCIA - strtLCIA);
        std::cout << "LCIA/c++," << settings.ProjectId << "," << elapsedLCIA.count() << std::endl;

        if (!basic)
        {
            auto strtLCIAtree = std::chrono::high_resolution_clock::now();

            UpstreamLCIACalculator upstreamCalc(settings, calculatorData, lcadb, graph);
            upstreamCalc.run(lciaR);

            auto finishLCIAtree = std::chrono::high_resolution_clock::now();
            auto elapsedLCIAtree = std::chrono::duration_cast<std::chrono::microseconds>(finishLCIAtree - strtLCIAtree);
            std::cout << "lcia_tree/c++," << settings.ProjectId << "," << elapsedLCIAtree.count() << std::endl;
        }

        delete[] rowsQ_ptr;
        delete[] colsQ_ptr;
        delete[] dataQ_ptr;

        return true;
    }
};
#endif /* StaticCalculator_hpp */
