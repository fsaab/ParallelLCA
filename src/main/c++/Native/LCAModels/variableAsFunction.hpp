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

#ifndef variableAsFunction_h
#define variableAsFunction_h

#include <stdio.h>
#include <string>
#include <vector>
#include <cmath>
#include <random>
using namespace std;
#include "../includes/exprtk.hpp"
#include <iostream>
//#include "CalcParameter.hpp"



typedef exprtk::expression<double> expression_t;
typedef exprtk::symbol_table<double> symbol_table_t;

template <typename T>

struct variableAsFunction : public exprtk::ifunction<T>
{

    expression_t expressionsTable;
    //CalcParameter *p;
    string formula;

    variableAsFunction() : exprtk::ifunction<T>(0) {}

    variableAsFunction(expression_t expressionsTable_, string formula_ /*CalcParameter *p_*/) : exprtk::ifunction<T>(0)
    {

       
        expressionsTable = expressionsTable_;
       // p = p_;
       formula=formula_;

       // cout << "  .................init.................  " << /*(*p).*/formula << endl;

        // symbol_table_t symt = expressionsTable.get_symbol_table();
        // std::vector<std::pair<std::string, double>> variable_list;
        // symt.get_variable_list(variable_list);

        // cout<<(symt.symbol_exists("y")?"y exists":"")<<endl;

        // for (std::pair<std::string, double> variable : variable_list)
        // {
        //     cout<<"v "<<variable.first<<" "<<variable.second<<endl;
        // }
    }

    double eval(std::string formulae, expression_t &expressiont)
    {
        // symbol_table_t symt = expressiont.get_symbol_table();
        // std::vector<std::pair<std::string, double>> variable_list;
        // symt.get_variable_list(variable_list);

        // for (std::pair<std::string, double> variable : variable_list)
        // {
        //     cout<<"v "<<variable.first<<" "<<variable.second<<endl;
        // }

        typedef exprtk::parser<T> parser_t;
        parser_t parser;
        parser.compile(formulae, expressiont);
        T y = expressiont.value();

        return y;
    }

    inline T operator()()
    {
        cout << "  ............EVALUAING.................  [" 
                << /*(*p).*/formula << "] to "
                << eval(/*(*p).*/formula, expressionsTable) 
                << endl;
        
        double v= eval(/*(*p).*/formula, expressionsTable);

        return v;
    }
};

#endif /* variableAsFunction_hpp */
