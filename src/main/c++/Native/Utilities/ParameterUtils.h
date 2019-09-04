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
#ifndef ParameterUtils_H
#define ParameterUtils_H

#include <stdio.h>
#include <vector>
#include "FileUtils.hpp"
#include "../LCAModels/Exchange.hpp"
#include "../DAL/ExchangeObj.hpp"
#include "../LCAModels/CalcImpactFactor.hpp"
#include "../DAL/CalcImpactFactorObj.hpp"
#include "../LCAModels/ExchangeItem.hpp"
#include "../LCAModels/ExchangeCell.hpp"
#include "../LCAModels/CalcImpactFactorItem.hpp"
#include <regex>
#include <sstream>
#include <time.h>
#include <iostream>

#include "../DAL/LCADB.hpp"
#include "../includes/exprtk.hpp"
#include "../LCAModels/CalcParameter.hpp"

#include "../LCAModels/variableAsFunction.hpp"
#include "../LCAModels/variableAsStochasticFunction.hpp"

#include "../LCAModels/ParameterVariableInfo.hpp"

typedef exprtk::expression<double> expression_t;
typedef exprtk::symbol_table<double> symbol_table_t;

template <typename T>
class ParameterUtils
{

  private:
    /*
    name:getVariables
    Description:
        takes a formula as string and return all the dependant variables inside that formula
        Uses regular expressions to parse the math formula
    */
    vector<string> getVariables(string formula)
    {
        std::regex e("\\b(?!exp|ln|log|sqrt)[A-Za-z]\\w*\\b");
        std::smatch m;
        vector<string> v;

        string s = formula;

        while (std::regex_search(s, m, e))
        {
            for (auto x : m)
                v.push_back(x);

            s = m.suffix().str();
        }
    }

  public:

    expression_t getExpressionTable(long owner, string scope, map<string, expression_t> &expressionsTable)
    {
        expression_t expressionsTable_filtered;

        string scope_id = scope + "_" + to_string(owner);

        if (expressionsTable.find(scope_id) == expressionsTable.end())
        {
            expressionsTable_filtered = expressionsTable["GLOBAL_0"];
        }
        else
        {
            expressionsTable_filtered = expressionsTable[scope_id];
        }

        return expressionsTable_filtered;
    }

    // void createExpressionTable(  map<std::pair<string, string>, CalcParameter> & parameters,expression_t & expressionTable)
    // {

    //     typedef exprtk::symbol_table<T> symbol_table_t;

    //     /*<scope,symbol_table>*/
    //     symbol_table_t sym_table;

    //     for (auto &&p : parameters)
    //     {
    //         sym_table.add_variable(p.first.second, p.second.value);
    //     }

    //     expressionTable.register_symbol_table(sym_table);

    // }

    double eval(std::string formulae, expression_t &expression)
    {
        typedef exprtk::parser<T> parser_t;
        parser_t parser;
        parser.compile(formulae, expression);
        T y = expression.value();

        return y;
    }

    double getExpressionValue(string expr, double value, expression_t expressionsTable)
    {
        double res = (expr == "" ? value : eval(expr, expressionsTable));
        return res;
    }

    /*
        name:buildDependecyGraph
        Description:
        pseudocode: 
            create parameters dependency graph:

            foreach parameter p in parameters
                1) if leafNode then push in parametersLeafNodes
                2) foreach parameter p, variables=getVariables(p):
                    p.variables=variables
                    foreach variable in variables : variable.functions.push_back(p)
        */

    // void buildDependecyGraph(map<std::pair<string, string>, CalcParameter> &parameters,
    //                          map<string /*scope_owner*/, vector<CalcParameter *>> &parametersLeafNodes)
    // {
    //     for (auto &&p : parameters)
    //     {
    //         string owner_str = (p.second.scope == "GLOBAL" ? "0" : to_string(p.second.f_owner));
    //         CalcParameter parentParameter=p.second;

    //         if (p.second.formula == "")
    //         {
    //             parametersLeafNodes[p.second.scope + "_" + owner_str].push_back(&(p.second));
    //         }
    //         else
    //         {
    //             vector<string> v_variables = getVariables(p.second.formula);

    //             for (string v_str : v_variables)
    //             {
    //                 string scope_owner_tmp = "";
    //                 scope_owner_tmp = parentParameter.scope + "_" + to_string(parentParameter.f_owner);
    //                 bool found_in_process = (parentParameter.scope != "GLOBAL" && parameters.find(make_pair(scope_owner_tmp, v_str)) != parameters.end());
    //                 scope_owner_tmp = (found_in_process ? scope_owner_tmp : "GLOBAL_0");

    //                 bool found_in_global = (parentParameter.scope == "GLOBAL" && parameters.find(make_pair("GLOBAL_0", v_str)) != parameters.end());
    //                 if (found_in_process || found_in_global) // it needs to be found somewhere
    //                 {
    //                     p.second.variables.push_back(&parameters[make_pair(scope_owner_tmp, v_str)]);
    //                 }
    //                 else //otherwise raise an exception
    //                 {
    //                     //...............EXCEPTION................
    //                     //..................................
    //                     //...........................................
    //                     //..................................................
    //                 }
    //             }

    //             for (auto &&v : p.second.variables)
    //             {
    //                 parameters[make_pair((*v).scope + "_" + to_string((*v).f_owner), (*v).name)]
    //                     .functions
    //                     .push_back(&(p.second));
    //             }
    //         }
    //     }
    // }

    // inline T eval(string expr, expression_t expressionsTable)
    // {
    //     ParameterUtils<T> paramUtils;

    //     return paramUtils.eval(expr, expressionsTable);
    // }

    void evaluateAddSymbol(CalcParameter &p,
                           map<string, expression_t> &expressionsTableMap,
                           std::tr1::unordered_map<string, symbol_table_t> &sym_tables, bool stochastic,
                           ParameterVariableInfo& p_info)
    {
        ParameterUtils<double> paramUtils;
        string scope_owner = p.scope + "_" + to_string(p.f_owner);

        if (p.formula == "")
        {
            if (stochastic)
                sym_tables[scope_owner /*scope_owner*/].add_function(p.name /*param_name*/, p_info.stochasticFunction);
            else
                sym_tables[scope_owner /*scope_owner*/].add_variable(p.name /*param_name*/, p.value);
        }
        else
        {
            if (stochastic)
                sym_tables[scope_owner /*scope_owner*/].add_function(p.name /*param_name*/, p_info.stochasticFunction);
            else
            {
                sym_tables[scope_owner /*scope_owner*/].add_function(p.name /*param_name*/, p_info.staticFunction);
            }
        }

        sym_tables[scope_owner /*scope_owner*/].add_constants();
    }

    // void createExpressionTable(map<std::pair<string, string>, CalcParameter> &parameters,
    //                            map<string /*scope_owner*/, vector<CalcParameter *>> &parametersLeafNodes,
    //                            map<string, expression_t> &expressionsTableMap)
    // {
    //     /*<<scope_owner>, symbol_table>*/
    //     std::tr1::unordered_map<string, symbol_table_t> sym_tables;
    //     map<std::pair<string, string>, CalcParameter>::iterator it;
    //     ParameterUtils<double> paramUtils;

    //     //1) evaluate leafnodes of GLOBAL_0
    //     vector<CalcParameter *> scope_leafnodes_GLOBAL_ptr = parametersLeafNodes["GLOBAL_0"];
    //     for (auto &&p : scope_leafnodes_GLOBAL_ptr)
    //     {
    //         evaluateAddSymbol((*p), expressionsTableMap, sym_tables);
    //     }

    //     //2) evaluate leafnodes other than "GLOBAL_0"
    //     for (auto &&leafRow : parametersLeafNodes) //foreach scope_owner
    //     {
    //         string scope_owner_str = leafRow.first;
    //         vector<CalcParameter *> scope_leafnodes_ptr = leafRow.second;

    //         if (leafRow.first != "GLOBAL_0")
    //         {
    //             for (auto &&p : scope_leafnodes_ptr) //evaluate for scope_owner leaf nodes
    //             {
    //                 evaluateAddSymbol(*p, expressionsTableMap, sym_tables);
    //             }
    //         }

    //         //END:evaluate for scope_owner leaf nodes

    //         //2.2) evaluate other parameters levels, starting from leafnodes+1 level
    //         vector<CalcParameter *> scope_nodes_ptr = scope_leafnodes_ptr;
    //         while (scope_nodes_ptr.size() > 0) //as long as we have functions
    //         {
    //             vector<CalcParameter *> scope_nodes_ptr_tmp;

    //             for (auto &&p : scope_nodes_ptr) //evaluate for scope_owner leaf nodes
    //             {
    //                 vector<CalcParameter *> functions = (*p).functions; //parents parameters

    //                 //insert all the functions of all the parameters p[]
    //                 scope_nodes_ptr_tmp.insert(scope_nodes_ptr_tmp.end(), functions.begin(), functions.end());

    //                 for (auto &&func : functions)
    //                 {
    //                     /*evaluate node*/
    //                     string func_scope_owner_str = (*func).scope + "_" + to_string((*func).f_owner);
    //                     evaluateAddSymbol(*func, expressionsTableMap, sym_tables);
    //                 }
    //             }

    //             //boost::copy(boost::unique(boost::sort(scope_nodes_ptr_tmp)), std::back_inserter(scope_nodes_ptr));

    //             //should be unique
    //             scope_nodes_ptr = scope_nodes_ptr_tmp;
    //         }
    //     }

    //     //3) append the evaluated parameters to the symbol table
    //     for (std::pair<string, symbol_table_t> symt : sym_tables)
    //     {
    //         expression_t expressiont;
    //         expressiont.register_symbol_table(symt.second);
    //         expressiont.register_symbol_table(sym_tables["GLOBAL_0"]);
    //         expressionsTableMap[symt.first /*scope_owner*/] = expressiont;
    //     }
    // }

    void createExpressionTable(map<std::pair<string, string>, CalcParameter> &parameters,
                               map<string, expression_t> &expressionsTableMap,
                               map<std::pair<string, string>, ParameterVariableInfo> &parameterVariablesInfoMap)
    {
        /*<<scope_owner>, symbol_table>*/
        std::tr1::unordered_map<string, symbol_table_t> sym_tables;
        map<std::pair<string, string>, CalcParameter>::iterator it;
        ParameterUtils<double> paramUtils;

        //1) evaluate symbol
        for (auto &&p : parameters)
        {
            evaluateAddSymbol(p.second,
                              expressionsTableMap,
                              sym_tables,
                              false,
                              parameterVariablesInfoMap[p.first]);
        }

        //2) append the evaluated parameters to the symbol table
        for (std::pair<string, symbol_table_t> symt : sym_tables)
        {
            expression_t expressiont;
            expressiont.register_symbol_table(symt.second);
            expressiont.register_symbol_table(sym_tables["GLOBAL_0"]);
            expressionsTableMap[symt.first /*scope_owner*/] = expressiont;
        }

        //3) re-evaluate "p.second.eval_instance"
        for (auto &&p : parameters)
        {
            string scope_owner = p.second.scope + "_" + to_string(p.second.f_owner);
            expression_t expressionsTable_tmp = paramUtils.getExpressionTable(p.second.f_owner, p.second.scope, expressionsTableMap);
            //p.value = getParameterCellValue(p, expressionsTable_tmp);
            parameterVariablesInfoMap[p.first].staticFunction = variableAsFunction<double>(expressionsTable_tmp, p.second.formula);
        }
    }

    void createExpressionTableStochastic(map<std::pair<string, string>, CalcParameter> &parameters,
                                         map<string, expression_t> &expressionsTableMap,
                                         std::default_random_engine gen,
                                         map<std::pair<string, string>, ParameterVariableInfo> &parameterVariablesInfoMap
                                         )
    {
        /*<<scope_owner>, symbol_table>*/
        std::tr1::unordered_map<string, symbol_table_t> sym_tables;
        map<std::pair<string, string>, CalcParameter>::iterator it;
        ParameterUtils<double> paramUtils;

        //1) evaluate symbol
        for (auto &&p : parameters)
        {
            evaluateAddSymbol(p.second, expressionsTableMap, sym_tables, true,
                              parameterVariablesInfoMap[p.first]);
        }

        //2) append the evaluated parameters to the symbol table
        for (std::pair<string, symbol_table_t> symt : sym_tables)
        {
            expression_t expressiont;
            expressiont.register_symbol_table(symt.second);
            expressiont.register_symbol_table(sym_tables["GLOBAL_0"]);
            expressionsTableMap[symt.first /*scope_owner*/] = expressiont;
        }

        //3) re-evaluate "p.second.eval_instance"
        for (auto &&p : parameters)
        {
            string scope_owner = p.second.scope + "_" + to_string(p.second.f_owner);
            expression_t expressionsTable_tmp = paramUtils.getExpressionTable(p.second.f_owner, p.second.scope, expressionsTableMap);
            //p.value = getParameterCellValue(p, expressionsTable_tmp);
            parameterVariablesInfoMap[p.first].stochasticFunction = variableAsStochasticFunction<double>(expressionsTable_tmp, &(p.second), gen);
        }
    }


    // void sampleParameterTable(map<std::pair<string, string>, CalcParameter> &parameters,
    //                           map<string, expression_t> &expressionsTableMap,
    //                           std::default_random_engine gen)
    // {

    //     ParameterUtils<double> paramUtils;

    //     //WARNING: instead put dependency tree of parameters, and back propagate
    //     for (std::pair<std::pair<string, string>, CalcParameter> p_row : parameters)
    //     {
    //         expression_t expressionsTable_tmp = paramUtils.getExpressionTable(p_row.second.f_owner, "PROCESS", expressionsTableMap);

    //         if (p_row.second.uncertaintyType > 0)
    //         {
    //             p_row.second.value = EntitySampler::getNextSimulatedParameterCellValue(p_row.second, gen, expressionsTable_tmp);
    //         }
    //         else if (p_row.second.uncertaintyType == 0)
    //         {

    //             p_row.second.value = getParameterCellValue(p_row.second /*, expressionsTable_tmp*/);
    //         }
    //     }
    // }

    void reloadExchangesDistribution(ExchangeItem &exchItem,
                                     map<string, expression_t> &expressionsTableMap)
    {

        expression_t expressionsTable_tmp = getExpressionTable(exchItem.exch.processId, "PROCESS", expressionsTableMap);

        double mean = 0.0;
        double stddev = 0.0;

        switch (exchItem.exch.uncertaintyType)
        {

        case 1:

            mean = log(fabs(getExpressionValue(exchItem.exch.parameter1Formula, exchItem.exch.parameter1, expressionsTable_tmp)));
            stddev = log(fabs(getExpressionValue(exchItem.exch.parameter2Formula, exchItem.exch.parameter2, expressionsTable_tmp)));
            exchItem.distribution = std::normal_distribution<float>(mean, stddev);

            break;

        case 2:

            mean = getExpressionValue(exchItem.exch.parameter1Formula, exchItem.exch.parameter1, expressionsTable_tmp);
            stddev = getExpressionValue(exchItem.exch.parameter2Formula, exchItem.exch.parameter2, expressionsTable_tmp);
            exchItem.distribution = std::normal_distribution<float>(mean, stddev);

            break;
        }
    }

    void reloadExchangesDistribution(ExchangeItem &exchItem)
    {
        double mean = 0.0;
        double stddev = 0.0;

        switch (exchItem.exch.uncertaintyType)
        {

        case 1:

            mean = log(fabs(exchItem.exch.parameter1));
            stddev = log(fabs(exchItem.exch.parameter2));
            exchItem.distribution = std::normal_distribution<float>(mean, stddev);

            break;

        case 2:

            mean = exchItem.exch.parameter1;
            stddev = exchItem.exch.parameter2;
            exchItem.distribution = std::normal_distribution<float>(mean, stddev);

            break;
        }
    }

    void reloadParametersDistribution(CalcParameter &p, map<string, expression_t> &expressionsTableMap)
    {
        expression_t expressionsTable_tmp = getExpressionTable(p.f_owner, "PROCESS", expressionsTableMap);

        double mean = 0;
        double stddev = 0;

        switch (p.uncertaintyType)
        {

        case 1:

            mean = log(fabs(getExpressionValue(p.parameter1Formula, p.parameter1, expressionsTable_tmp)));
            stddev = log(fabs(getExpressionValue(p.parameter2Formula, p.parameter2, expressionsTable_tmp)));
            p.distribution = std::normal_distribution<float>(mean, stddev);

            break;

        case 2:

            mean = getExpressionValue(p.parameter1Formula, p.parameter1, expressionsTable_tmp);
            stddev = getExpressionValue(p.parameter2Formula, p.parameter2, expressionsTable_tmp);
            p.distribution = std::normal_distribution<float>(mean, stddev);

            break;
        }
    }
};

#endif /* ParameterUtils_H */
