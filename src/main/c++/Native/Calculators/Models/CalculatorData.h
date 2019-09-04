//
//  CalculatorData.h
//  LCA.Kernel
//
//  Created by francois saab on 2/18/18.
//  Copyright © 2018 fsaab. All rights reserved.
//

#ifndef CalculatorData_h
#define CalculatorData_h

#include <stdio.h>
#include <string>
#include <vector>
#include <boost/mpi.hpp>
#include <unordered_map>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/string.hpp>

#include "../../LCAModels/CalcImpactFactorItem.hpp"
#include "../../LCAModels/ExchangeItem.hpp"
#include "../../Calculators/Models/AppSettings.hpp"
#include "../../LCAModels/LCAIndexes.hpp"
#include "../../Calculators/Models/ImpactStat.hpp"

#include "../../LCAModels/CalcParameter.hpp"
#include "../../includes/exprtk.hpp"
#include "../../Utilities/ParameterUtils.h"
#include "../../Uncertainties/EntitySampler.hpp"
#include "../../LCAModels/ParameterVariableInfo.hpp"
#include "../../Utilities/FileUtils.hpp"

typedef exprtk::expression<double> expression_t;
typedef exprtk::symbol_table<double> symbol_table_t;
#include <regex>
using namespace std;

class CalculatorData
{
  private:
  public:
    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {

        //ar &settings;
        ar &A_exchanges;
        ar &B_exchanges;
        ar &Q_cells;

        ar &A_unc_exchanges;
        ar &B_unc_exchanges;
        ar &Q_unc_cells;

        ar &A_static_exchanges;
        ar &B_static_exchanges;

        // ar & ASamplesIds_indices;
        // ar & BSamplesIds_indices;

        ar &lcaIndexes;

        ar &stack;
        ar &barrier_demand;

        ar &demand_demandVectorArray;
        ar &impactStats;

        ar &parameters;
        ar &parameterVariableInfoMap;

        ar &impactCategoriesLength;
        ar &elementaryFlowsLength;
        ar &intermediateFlowsLength;
        ar &processesLength;
        ar &A_size;
        ar &B_size;
        ar &Q_size;
        //ar & P_size;
        ar &A_unc_size;
        ar &B_unc_size;
        ar &A_size_fl;
        ar &B_size_fl;
        //ar & A_unc_frontlayer_size;
        //ar & B_unc_frontlayer_size;
        ar &processesLength_frontLayer;
    }

    AppSettings settings;
    vector<ExchangeItem> A_exchanges;
    vector<ExchangeItem> A_foreground_exchanges;
    vector<ExchangeItem> A_background_exchanges;

    vector<ExchangeItem> B_exchanges;
    vector<CalcImpactFactorItem> Q_cells;

    vector<ExchangeItem> A_unc_exchanges;
    vector<ExchangeItem> B_unc_exchanges;
    vector<CalcImpactFactorItem> Q_unc_cells;

    vector<ExchangeItem> A_static_exchanges;
    vector<ExchangeItem> B_static_exchanges;

    unordered_map<long, long> ASamplesIds_indices;
    /*unordered_*/ map<long, long> BSamplesIds_indices;

    LCAIndexes lcaIndexes;

    unordered_map<int, vector<long>> stack;
    unordered_map<long, vector<std::pair<long, Exchange>>> barrier_demand;

    vector<double> demand_demandVectorArray;
    vector<ImpactStat> impactStats;

    long impactCategoriesLength = 0;
    long elementaryFlowsLength = 0;
    long intermediateFlowsLength = 0;
    long processesLength = 0;

    long A_size = 0;
    long B_size = 0;
    long Q_size = 0;
    //long P_size;

    long A_unc_size = 0;
    long B_unc_size = 0;

    /*Parameters*/
    map<std::pair<string, string>, CalcParameter> parameters;
    map<std::pair<string, string>, ParameterVariableInfo> parameterVariableInfoMap;

    long A_size_fl = 0;
    long B_size_fl = 0;
    long processesLength_frontLayer = 0;

    long A_unc_frontlayer_size = 0;
    long B_unc_frontlayer_size = 0;

    static std::vector<std::string> split(std::string input, std::string sep)
    {
        vector<string> strs;
        boost::split(strs, input, boost::is_any_of(sep));
        return strs;
    }

    CalculatorData()
    {
    }

    CalculatorData(AppSettings settings_,
                   vector<ExchangeItem> A_exchanges_,
                   vector<ExchangeItem> B_exchanges_,
                   vector<CalcImpactFactorItem> Q_Cells_,
                   LCAIndexes lcaIndexes_,
                   vector<double> demand_demandVectorArray_,
                   vector<ImpactStat> impactStats_)
    {
        settings = settings_;
        A_exchanges = A_exchanges_;
        B_exchanges = B_exchanges_;
        Q_cells = Q_Cells_;
        demand_demandVectorArray = demand_demandVectorArray_;
        lcaIndexes = lcaIndexes_;
        impactStats = impactStats_;
    }

    void loadSamplesIndices()
    {

        FileUtils fileUtils;
        std::stringstream ss_A_samples_indices_path;
        ss_A_samples_indices_path << "data/all_exchanges_A_indices.txt";

        if (fileUtils.exists(ss_A_samples_indices_path.str()))
        {

            vector<string> all_exchanges_A_strs = split((fileUtils.readTextFromFile(
                                                            ss_A_samples_indices_path.str())[0]),
                                                        ",");

            

            long Asamplessize = all_exchanges_A_strs.size();

          //cout<<"all_exchanges_A_strs first last "<<all_exchanges_A_strs[0]<<" "<<all_exchanges_A_strs[Asamplessize-2]<<endl;

            //cout<<"all_exchanges_A_strs.size() "<<all_exchanges_A_strs.size()<<endl;
            for (long ind_sample = 0; ind_sample < Asamplessize; ind_sample++)
            {
                string exchid_str = all_exchanges_A_strs[ind_sample];
                if (exchid_str != "")
                {
                    ASamplesIds_indices[stol(exchid_str)] = ind_sample;
                }
            }
        }

        

        std::stringstream ss_B_samples_indices_path;
        ss_B_samples_indices_path << "data/all_exchanges_B_indices.txt";

        if (fileUtils.exists(ss_B_samples_indices_path.str()))
        {

            vector<string> all_exchanges_B_strs = split((fileUtils.readTextFromFile(
                                                            ss_B_samples_indices_path.str())[0]),
                                                        ",");

            long Bsamplessize = all_exchanges_B_strs.size();

            for (long ind_sampleB = 0; ind_sampleB < Bsamplessize; ind_sampleB++)
            {
                string exchid_str = all_exchanges_B_strs[ind_sampleB];
                if (exchid_str != "")
                {

                    BSamplesIds_indices[stol(exchid_str)] = ind_sampleB;
                }
            }
        }
    }

    void loadLengths()
    {

        impactCategoriesLength = lcaIndexes.ImpactCategoryIndexLength();
        elementaryFlowsLength = lcaIndexes.ElementaryFlowsIndexLength();
        intermediateFlowsLength = lcaIndexes.IntermediateFlowsIndexLength();
        processesLength = lcaIndexes.ProcessesIndexLength();
        processesLength_frontLayer = lcaIndexes.FrontLayer_ProcessesIndexLength();

        A_size = A_exchanges.size();
        B_size = B_exchanges.size();
        Q_size = Q_cells.size();
        //P_size = A_exchanges.size();
    }

    void LoadUncertainCells()
    {

        for (int i = 0; i < A_exchanges.size(); i++)
        {

            ExchangeItem cell = A_exchanges[i];

            if (cell.exch.uncertaintyType != 0)
            {
                A_unc_exchanges.push_back(cell);
                A_unc_size++;
            }
            else
            {

                A_static_exchanges.push_back(cell);
            }

             if (!cell.exch.isBackground)
            {

                A_foreground_exchanges.push_back(cell);

                A_size_fl++;
            }else{
                A_background_exchanges.push_back(cell);
            }
        
        }

       for (int i = 0; i < B_exchanges.size(); i++)
        {

            ExchangeItem cell = B_exchanges[i];

            if (cell.exch.uncertaintyType != 0)
            {
                B_unc_exchanges.push_back(cell);
                B_unc_size;
            }
            else
            {

                B_static_exchanges.push_back(cell);
            }

             if (!cell.exch.isBackground)
            {

                B_size_fl++;
            }
        
        }

        for (auto &&cell : Q_cells)
        {
            if (cell.imf.uncertaintyType != 0)
            {
                Q_unc_cells.push_back(cell);
            }
        }
    }

    void LoadAndWriteUncertainCells()
    {
        FileUtils fileUtils;

        ParameterUtils<double> paramUtils;
        // map<string /*scope_owner*/, vector<CalcParameter *>> parametersLeafNodes;
        // paramUtils.buildDependecyGraph(parameters, parametersLeafNodes);

        EntitySampler<double> sampler;

        /*<scope_owner,expresion>*/
        /*unordered_*/ map<string, expression_t> expressionsTable;
        paramUtils.createExpressionTable(parameters, expressionsTable, parameterVariableInfoMap);

        string ss_A_samples_ids_path="./data/all_exchanges_A_indices.txt";
        vector<string> ss_A_samples_ids;
       for (int i = 0; i < A_exchanges.size(); i++)
        {

            ExchangeItem cell = A_exchanges[i];
            if (cell.exch.uncertaintyType != 0)
            {
                A_unc_exchanges.push_back(cell);
                A_unc_size++;
                ss_A_samples_ids.push_back(to_string(cell.exch.exchangeId));
                // if (!cell.exch.isBackground)
                // {
                //     A_unc_frontlayer_size++;
                // }
            }
            else
            {
                // if (cell.exch.amountFormula != "")
                // {

                //     expression_t expressionsTable_tmp = paramUtils.getExpressionTable(cell.exch.processId, "PROCESS", expressionsTable);

                //     symbol_table_t symt = expressionsTable_tmp.get_symbol_table();
                //     std::vector<std::pair<std::string, double>> variable_list;
                //     symt.get_variable_list(variable_list);
                //     bool found = false;

                //     for (std::pair<std::string, double> variable : variable_list)
                //     {
                //         //cout << variable.first << " " << variable.second << endl;
                //         CalcParameter p = parameters[std::make_pair("PROCESS_" + to_string(cell.exch.processId), variable.first)];
                //         if (p.uncertaintyType != 0)
                //         {

                //             if (cell.exch.amountFormula.find(p.name) != std::string::npos)
                //             {
                //                 A_unc_exchanges.push_back(cell);
                //                 found = true;
                //                 break;
                //             }
                //         }
                //     }

                //     if (!found)
                //         A_static_exchanges.push_back(cell);
                // }
                // else
                // {

                A_static_exchanges.push_back(cell);

                //}
            }

            if (!cell.exch.isBackground)
            {

                A_foreground_exchanges.push_back(cell);

                A_size_fl++;
            }else{
                A_background_exchanges.push_back(cell);
            }
        }

        

        fileUtils.writeTextToFile(boost::algorithm::join(ss_A_samples_ids, ","), ss_A_samples_ids_path);

        string ss_B_samples_ids_path="./data/all_exchanges_B_indices.txt";
        vector<string> ss_B_samples_ids;

        //cout<<"A_unc_exchanges.size "<<A_unc_exchanges.size()<<endl;

        for (int i = 0; i < B_exchanges.size(); i++)
        {

            ExchangeItem cell = B_exchanges[i];
            if (cell.exch.uncertaintyType != 0)
            {
                B_unc_exchanges.push_back(cell);
                B_unc_size++;
                ss_B_samples_ids.push_back(to_string(cell.exch.exchangeId));
                // if (!cell.exch.isBackground)
                //     {
                //         B_unc_frontlayer_size++;
                //     }
            }
            else
            {

                // expression_t expressionsTable_tmp = paramUtils.getExpressionTable(cell.exch.processId, "PROCESS", expressionsTable);

                // symbol_table_t symt = expressionsTable_tmp.get_symbol_table();

                // std::vector<std::pair<std::string, double>> variable_list;
                // symt.get_variable_list(variable_list);

                // bool found = false;

                // for (std::pair<std::string, double> variable : variable_list)
                // {

                //     CalcParameter p = parameters[std::make_pair("PROCESS_" + cell.exch.processId, variable.first)];

                //     if (p.uncertaintyType != 0)
                //     {
                //         if (cell.exch.amountFormula.find(p.name) != std::string::npos)
                //         {
                //             B_unc_exchanges.push_back(cell);
                //             found = true;
                //             break;
                //         }
                //     }
                // }

                // if (!found)

                B_static_exchanges.push_back(cell);
            }

            if (!cell.exch.isBackground)
            {

                B_size_fl++;
            }
        }

        //cout<<"B_unc_exchanges.size "<<B_unc_exchanges.size()<<endl;

        fileUtils.writeTextToFile(boost::algorithm::join(ss_B_samples_ids, ","), ss_B_samples_ids_path);
        
        for (auto &&cell : Q_cells)
        {
            if (cell.imf.uncertaintyType != 0)
            {
                Q_unc_cells.push_back(cell);
            }
        }
    }
};

#endif /* CalculatorData_h */
