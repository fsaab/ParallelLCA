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
#ifndef GraphData_h
#define GraphData_h
#include <stdio.h>
#include <string>
#include <vector>
#include "../LCAModels/CalcImpactFactorItem.hpp"
#include "../LCAModels/ExchangeItem.hpp"
using namespace std;
#include "../Calculators/Models/AppSettings.hpp"
#include "../LCAModels/LCAIndexes.hpp"
#include "../Calculators/Models/ImpactStat.hpp"
#include "LCAEdgeCompact.hpp"
#include <tr1/unordered_map>
#include "NodeInfo.hpp"
struct GraphData {

    vector<LCAEdgeCompact> edgesListCompact;

    /** Need refactoring*/
    std::tr1::unordered_map<long, bool> nodesListCompact;
    std::tr1::unordered_map<long, double> procs_scalar;

   /*Stack based*/
    unordered_map<int, vector<long>> stack;

    //LCAIndexes lcaIndexes;

    GraphData(){}
};


#endif /* CalculatorData_h */
