//
//  StaticCalculator.hpp
//  LCA.Kernel
//
//  Created by francois saab on 2/26/18.
//  Copyright © 2018 fsaab. All rights reserved.
//

#ifndef StaticCalculator_hpp
#define StaticCalculator_hpp

#include <stdio.h>
#include <stdio.h>
#include <iostream>
#include <chrono> // for high_resolution_high_resolution_clock
#include <tr1/unordered_map>
#include <algorithm>
#include <ctime>

#include "../Calculators/Models/AppSettings.hpp"
#include "../DAL/LCADBUtils.h"
#include "../LCAModels/LCAIndexes.hpp"
#include "../Graph/LCAEdgeCompact.hpp"
#include "../Factories/InterventionMatrixFactorySingle.hpp"
#include "../LCAModels/ExchangeItem.hpp"
#include "../LCAModels/CalcImpactFactorItem.hpp"
#include "../Factories/CharacterisationMatrixFactory.hpp"
#include "../Utilities/FileUtils.hpp"
#include "../Factories/DemandVectorFactory.hpp"
#include "../Utilities/ParameterUtils.h"
#include "../Upstream/UpstreamLCIACalculator.hpp"
#include "../Reports/LCIContributionReport.hpp"
#include "../Reports/LCIAContributionReport.hpp"
#include "../Messaging/LCAException.hpp"
#include "../Calculators/Models/CalculatorData.h"
#include "../Graph/GraphData.h"

using namespace std;

class StaticCalculator
{
  public:
    CalculatorData *calculatorData;
    bool basic;
    GraphData *graph;
    LCADB *lcadb;

    /*<scope_owner,expresion>*/
    /*unordered_*/ map<string, expression_t> expressionsTable;

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

    double *lci_bgl(AppSettings settings, double *scalars_PTR, long ProcessesIndexLength,

                    long *rowsB_ptr,
                    long *colsB_ptr,
                    double *dataB_ptr,
                    long Blength,
                    long ElementaryFlowsIndexLength)
    {

        //cout << "ElementaryFlowsIndexLength " << ElementaryFlowsIndexLength << endl;
        //cout << "Blength " << Blength << endl;

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

        return lciR;
    }

    void solveSequentialGraph(AppSettings settings)
    {
        auto strtS = std::chrono::high_resolution_clock::now();

        EntitySampler<double> entitySampler;

        long ElementaryFlowsIndexLength = (*calculatorData).elementaryFlowsLength;
        long ProcessesIndexLength = (*calculatorData).processesLength;
        long ImpactCategoryIndexLength = (*calculatorData).impactCategoriesLength;

        /*foregroundlayer variables*/
        long ProcessesIndexLength_fl = (*calculatorData).processesLength_frontLayer;
        int length_frontlayer = (*calculatorData).lcaIndexes.FrontLayer_ProcessesIndex.size();
        long fl_barrier_size = (*calculatorData).lcaIndexes.FrontLayerBarrier_ProcessesIndex.size();

        /*Collapse/solve the foreground layer*/
        if (fl_barrier_size > 0)
        {
            long rootId = (*calculatorData).stack[0][0];

            bool rootIsForeground = (*calculatorData).lcaIndexes.FrontLayer_ProcessesIndexIndices.find(rootId) !=
                                    (*calculatorData).lcaIndexes.FrontLayer_ProcessesIndexIndices.end();

            EntitySampler<double> sampler;
            long stacksize = (*calculatorData).stack.size();

            if (rootIsForeground)
            {

                for (int k = 1; k < stacksize; k++) // reset scalars
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

                        double unit_supply = abs(currEdge.OutputCalculatedValue);

                        double unit_demand = abs(currEdge.InputCalculatedValue);

                        double total_demand = unit_demand * (*(stackEdges[se].InputScalar)).scalar; //procsScalarsMap[currEdge.ProcessSrcId];

                        double scalar = total_demand / unit_supply;

                        (*(stackEdges[se].OutputScalar)).scalar += scalar;
                    }
                }

                /*copy foreground layer  scalars*/
                for (auto idfl : (*calculatorData).lcaIndexes.FrontLayerBarrier_ProcessesIndex)
                {
                    (*graph).foregroundNodesScalars[idfl] = (*graph).nodesInfoMap[idfl].scalar;
                }
            }
        }

        auto finishS = std::chrono::high_resolution_clock::now();
        auto elapsedS = std::chrono::duration_cast<std::chrono::microseconds>(finishS - strtS);
        std::cout << "solve_sequential," << settings.ProjectId << "," << elapsedS.count() << std::endl;
    }

    double *solve(AppSettings settings, long *rowsA_ptr,
                  long *colsA_ptr,
                  double *dataA_ptr,
                  long Alength,
                  double *demand,
                  long demandLength,
                  long processesLength)
    {

        auto strtS = std::chrono::high_resolution_clock::now();

        if (demandLength != processesLength)
        {
            throw LCAException(200, "While solving system in static phase, matrices( A, b) sizes does not match.");
        }

        libEigen lib;
        double *scalars_PTR = lib.solve_bicgstab(demandLength,
                                                 Alength,
                                                 rowsA_ptr,
                                                 colsA_ptr,
                                                 dataA_ptr,
                                                 demand);

        auto finishS = std::chrono::high_resolution_clock::now();
        auto elapsedS = std::chrono::duration_cast<std::chrono::microseconds>(finishS - strtS);
        std::cout << "solve," << settings.ProjectId << "," << elapsedS.count() << std::endl;

        return scalars_PTR;
    }

    double *lci(AppSettings settings, double *scalars_PTR,
                long *rowsB_ptr, long *colsB_ptr, double *dataB_ptr, long Blength)
    {

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

    void loadAOnePlusBackground(AppSettings settings,
                                long *rowsA_ptr_bl,
                                long *colsA_ptr_bl,
                                double *dataA_ptr_bl
                                //map<long, double> procsScalarsMap
    )
    {

        auto strtA = std::chrono::high_resolution_clock::now();

        EntitySampler<double> entitySampler = EntitySampler<double>();

        /*bglayer variables*/
        long ProcessesIndexLength = (*calculatorData).processesLength;

        long ProcessesIndexLength_bl = ProcessesIndexLength - (*calculatorData).processesLength_frontLayer;

        /*load background layer raw data into arrays*/
        int i = 0, j = 0;
        for (auto &&cell : (*calculatorData).A_background_exchanges)
        {


                ExchangeItem ent = cell;
                EntitySampler<double> entitySampler = EntitySampler<double>();

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
                double total_demand = (*graph).foregroundNodesScalars[fl_barrieri_proc] * unit_demand;
                rowsA_ptr_bl[j] = (*calculatorData).lcaIndexes.BackgroundLayer_ProcessesIndexIndices[demand.first];
                colsA_ptr_bl[j] = ProcessesIndexLength_bl;
                dataA_ptr_bl[j] = total_demand;

                j++;
            }
        }

        auto finishA = std::chrono::high_resolution_clock::now();
        auto elapsedA = std::chrono::duration_cast<std::chrono::microseconds>(finishA - strtA);
        std::cout << "A/c++," << settings.ProjectId << "," << elapsedA.count() << std::endl;
    }

    void loadB(AppSettings settings,
               long *rowsB_ptr, long *colsB_ptr, double *dataB_ptr,
               long Blength)
    {

        EntitySampler<double> sampler;

        auto strtB = std::chrono::high_resolution_clock::now();

        int i = 0;
        for (auto &&ent : (*calculatorData).B_exchanges)
        {

            rowsB_ptr[i] = ent.i;
            colsB_ptr[i] = ent.j;
            dataB_ptr[i] = sampler.getCellValue(ent.exch);

            i++;
        }

        auto finishB = std::chrono::high_resolution_clock::now();
        auto elapsedB = std::chrono::duration_cast<std::chrono::microseconds>(finishB - strtB);
        std::cout << "B/c++," << settings.ProjectId << "," << elapsedB.count() << std::endl;
    }
    void loadB_bgl(AppSettings settings,
                   long *rowsB_ptr, long *colsB_ptr, double *dataB_ptr,
                   long Blength)
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
                dataB_ptr[i] = sampler.getCellValue(ent.exch);

                i++;
            }
        }

        auto finishB = std::chrono::high_resolution_clock::now();
        auto elapsedB = std::chrono::duration_cast<std::chrono::microseconds>(finishB - strtB);
        std::cout << "B/c++," << settings.ProjectId << "," << elapsedB.count() << std::endl;
    }
    void loadAForeground(AppSettings settings, long *rowsA_ptr_fl,
                         long *colsA_ptr_fl,
                         double *dataA_ptr_fl, long &A_fl_length)
    {
        EntitySampler<double> entitySampler = EntitySampler<double>();

        /*load frontlayer and  bacground layer raw data into arrays*/
        int i = 0;
        for (auto &&cell : (*calculatorData).A_exchanges)
        {
            ExchangeItem ent = cell;

            double v = entitySampler.getCellValue(ent.exch);

            if (!ent.exch.isBackground)
            {
                if (ent.i_fl != -1)
                {

                    rowsA_ptr_fl[i] = ent.i_fl;
                    colsA_ptr_fl[i] = ent.j_fl;
                    dataA_ptr_fl[i] = v;
                    i++;
                }
            }
        }
        A_fl_length = i;
    }

    void writeLCI(double *lciR, AppSettings settings)
    {
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
    }

    void writeLCIA(double *lciaR, AppSettings settings)
    {
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
    }

    void loadA(AppSettings settings, long *rowsA_ptr,
               long *colsA_ptr,
               double *dataA_ptr)
    {
        auto strtA = std::chrono::high_resolution_clock::now();

        typedef exprtk::expression<double> expression_t;
        ParameterUtils<double> putils;
        EntitySampler<double> entitySampler = EntitySampler<double>();

        int i = 0;

        EntitySampler<double> sampler;

        for (auto &&cell : (*calculatorData).A_exchanges)
        {
            ExchangeItem ent = cell;
            rowsA_ptr[i] = ent.i;
            colsA_ptr[i] = ent.j;
            dataA_ptr[i] = sampler.getCellValue(ent.exch);

            i++;
        }

        auto finishA = std::chrono::high_resolution_clock::now();
        auto elapsedA = std::chrono::duration_cast<std::chrono::microseconds>(finishA - strtA);
        std::cout << "A/c++," << settings.ProjectId << "," << elapsedA.count() << std::endl;
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
        EntitySampler<double> entitySampler = EntitySampler<double>();
        double *scalars_PTR;
        double *lciR;

        bool isRootFrotnlayer = std::find((*calculatorData).lcaIndexes.FrontLayer_ProcessesIndex.begin(),
                                          (*calculatorData).lcaIndexes.FrontLayer_ProcessesIndex.end(),
                                          settings.RootProcessId) != (*calculatorData).lcaIndexes.FrontLayer_ProcessesIndex.end();

        if (isRootFrotnlayer)

        {
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

            //cout << "Alength_bl Alength_fl" << Alength_bl << " " << (*calculatorData).A_size_fl << endl;

            solveSequentialGraph(settings);

            long *rowsA_ptr_bl = new long[Alength_bl];
            long *colsA_ptr_bl = new long[Alength_bl];
            double *dataA_ptr_bl = new double[Alength_bl];

            loadAOnePlusBackground(settings,
                                   rowsA_ptr_bl,
                                   colsA_ptr_bl,
                                   dataA_ptr_bl);

            auto strtsolve = std::chrono::high_resolution_clock::now();

            int frontLayer = 1; // "1" is for the collapssed front layer

            libEigen lib;

            double *scalars_PTR = lib.solve_bicgstab(ProcessesIndexLength_bl + frontLayer,
                                                     Alength_bl,
                                                     rowsA_ptr_bl,
                                                     colsA_ptr_bl,
                                                     dataA_ptr_bl,
                                                     demand);

            auto finishsolve = std::chrono::high_resolution_clock::now();
            auto elapsedsolve = std::chrono::duration_cast<std::chrono::microseconds>(finishsolve - strtsolve);
            std::cout << "solve_bgl/c++," << settings.ProjectId << "," << elapsedsolve.count() << std::endl;

            delete[] rowsA_ptr_bl;
            delete[] colsA_ptr_bl;
            delete[] dataA_ptr_bl;

            delete[] demand;

            for (int i = 0; i < ProcessesIndexLength_bl; i++)
            {
                long processid = (*calculatorData).lcaIndexes.BackgroundLayer_ProcessesIndex[i];
                (*graph).nodesInfoMap[processid].scalar = scalars_PTR[i];
            }

            long Blength = (*calculatorData).B_size;
            long *rowsB_ptr = new long[Blength];
            long *colsB_ptr = new long[Blength];
            double *dataB_ptr = new double[Blength];

            loadB_bgl(settings, rowsB_ptr, colsB_ptr, dataB_ptr, Blength);

            lciR = lci_bgl(settings, scalars_PTR, ProcessesIndexLength,
                           rowsB_ptr, colsB_ptr, dataB_ptr, Blength, ElementaryFlowsIndexLength);

            delete[] rowsB_ptr;
            delete[] colsB_ptr;
            delete[] dataB_ptr;

            delete[] scalars_PTR;
        }
        else
        {
            long Alength = (*calculatorData).A_exchanges.size();
            long *rowsA_ptr = new long[Alength];
            long *colsA_ptr = new long[Alength];
            double *dataA_ptr = new double[Alength];

            loadA(settings, rowsA_ptr, colsA_ptr, dataA_ptr);

            scalars_PTR = solve(settings,
                                rowsA_ptr, colsA_ptr, dataA_ptr,
                                Alength,
                                &((*calculatorData).demand_demandVectorArray)[0],
                                (*calculatorData).demand_demandVectorArray.size(),
                                (*calculatorData).processesLength);

            delete[] rowsA_ptr;
            delete[] colsA_ptr;
            delete[] dataA_ptr;

            for (int i = 0; i < (*calculatorData).lcaIndexes.ProcessesIndexLength(); i++)
            {
                long processid = (*calculatorData).lcaIndexes.ProcessesIndex[i];
                (*graph).nodesInfoMap[processid].scalar = scalars_PTR[i];
            }

            long Blength = (*calculatorData).B_exchanges.size();
            long *rowsB_ptr = new long[Blength];
            long *colsB_ptr = new long[Blength];
            double *dataB_ptr = new double[Blength];

            loadB(settings, rowsB_ptr, colsB_ptr, dataB_ptr, Blength);
            lciR = lci(settings, scalars_PTR, rowsB_ptr, colsB_ptr, dataB_ptr, Blength);

            delete[] rowsB_ptr;
            delete[] colsB_ptr;
            delete[] dataB_ptr;
        }

        auto strtI = std::chrono::high_resolution_clock::now();

        writeLCI(lciR, settings);

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

        delete[] rowsQ_ptr;
        delete[] colsQ_ptr;
        delete[] dataQ_ptr;

        writeLCIA(lciaR, settings);

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

        return true;
    }
};
#endif /* StaticCalculator_hpp */
