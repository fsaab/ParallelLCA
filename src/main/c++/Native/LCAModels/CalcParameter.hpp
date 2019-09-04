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

#ifndef CalcParameter_h
#define CalcParameter_h

#include <stdio.h>
#include <string>
#include <vector>
#include <cmath>
#include <random>
#include "../includes/exprtk.hpp"
#include <iostream>

typedef exprtk::expression<double> expression_t;
typedef exprtk::symbol_table<double> symbol_table_t;
typedef double T;

using namespace std;

class CalcParameter
{

  public:
    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar &param_id;
        ar &ref_id;
        ar &name;
        ar &f_owner;
        ar &scope;
        ar &conversionFactor;
        ar &input;
        ar &value;
        ar &formula;
        ar &uncertaintyType;
        ar &parameter1;
        ar &parameter2;
        ar &parameter3;
        ar &parameter1Formula;
        ar &parameter2Formula;
        ar &parameter3Formula;
    }

    int param_id = 0;
    string ref_id = "";
    string name = "";
    long f_owner = 0;
    string scope = "";
    double conversionFactor = 1;
    bool input = false;
    double value = .0;
    string formula;
    int uncertaintyType = 0;
    double parameter1 = 0.0;
    double parameter2 = 0.0;
    double parameter3 = 0.0;
    string parameter1Formula;
    string parameter2Formula;
    string parameter3Formula;
    std::normal_distribution<float> distribution;

    CalcParameter() {}

    CalcParameter(int param_id_,
                  string ref_id_,
                  string name_,
                  long f_owner_,
                  string scope_,
                  bool input_,
                  double conversionFactor_,
                  double value_,
                  string formula_,
                  int uncertaintyType_,
                  double parameter1_,
                  double parameter2_,
                  double parameter3_,
                  string parameter1Formula_,
                  string parameter2Formula_,
                  string parameter3Formula_
                  // std::normal_distribution<float> distribution_
    )
    {
        param_id = param_id_;
        ref_id = ref_id_;
        name = name_;
        f_owner = f_owner_;
        scope = scope_;
        conversionFactor = conversionFactor_;
        input = input_;
        value = value_;
        formula = formula_;
        uncertaintyType = uncertaintyType_;
        parameter1 = parameter1_;
        parameter2 = parameter2_;
        parameter3 = parameter3_;
        parameter1Formula = parameter1Formula_;
        parameter2Formula = parameter2Formula_;
        parameter3Formula = parameter3Formula_;
    }
};

#endif /* CalcParameter_h */
