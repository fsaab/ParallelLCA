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

#ifndef ParameterVariableInfo_h
#define ParameterVariableInfo_h

#include <stdio.h>
#include <string>
#include <vector>
#include <cmath>
#include <random>
#include "../includes/exprtk.hpp"
#include <iostream>

#include "variableAsFunction.hpp"
#include "variableAsStochasticFunction.hpp"

typedef exprtk::expression<double> expression_t;
typedef exprtk::symbol_table<double> symbol_table_t;
typedef double T;

using namespace std;

class ParameterVariableInfo
{

  public:
    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
       
    }

    variableAsStochasticFunction<double> stochasticFunction;//=exprtk::ifunction<T>(0);
    variableAsFunction<double> staticFunction;

    // vector<CalcParameter *> functions;
    // vector<CalcParameter *> variables;

    ParameterVariableInfo() {}

    ParameterVariableInfo(variableAsStochasticFunction<double> stochasticFunction_,
    variableAsFunction<double> staticFunction_
    )
    {
       stochasticFunction=stochasticFunction_;
       staticFunction=staticFunction_;
    }
};

#endif /* CalcParameter_h */
