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

#ifndef LCADB_hpp
#define LCADB_hpp

#include <stdio.h>
#include "../Calculators/Models/AppSettings.hpp"
#include "../Utilities/ResultsUtilsSingle.hpp"
#include <tr1/unordered_map>
#include "../LCAModels/LCAIndexes.hpp"
#include "../LCAModels/ImpactCategory.hpp"
#include "../LCAModels/UnitOfMeasurement.hpp"
#include "../LCAModels/Flow.hpp"
#include "../LCAModels/Process.hpp"
#include "../LCAModels/ImpactCategory.hpp"
#include "../LCAModels/CalcImpactFactor.hpp"
#include "../LCAModels/Exchange.hpp"
#include "../LCAModels/UnitOfMeasurement.hpp"
#include "../LCAModels/CalcParameter.hpp"
#include "../LCAModels/ParameterVariableInfo.hpp"
class LCADB
{

public:

  /*Technosphere*/
  std::tr1::unordered_map<long, Exchange> exchanges;
  std::tr1::unordered_map<long, Process> processes;
  std::tr1::unordered_map<long, Flow> flows;

    /*Indexes*/
    std::tr1::unordered_map<long, vector<long>> inputExchangesByProcess;
    std::tr1::unordered_map<long, vector<long>> outputExchangesByProcess;
    std::tr1::unordered_map<long, vector<long>> producersExchangesByFlow;
    std::tr1::unordered_map<long, vector<long>> elementaryExchangesByProcess;

    /*Parameters*/
    map<std::pair<string, string>, CalcParameter> parameters;
    //map<string/*scope_owner*/,vector<CalcParameter*> > parametersLeafNodes;
    map<std::pair<string, string>, ParameterVariableInfo> parameterVariablesInfoMap;


  /*biosphere*/
  std::tr1::unordered_map<long, ImpactCategory> impactCategories;
  std::tr1::unordered_map<long, UnitOfMeasurement> unitOfMeasurements;
  std::tr1::unordered_map<long, string> processesLocationCodes;
  /*std::tr1::unordered_*/map<std::pair<long, long>, CalcImpactFactor> impactFactors;
  std::tr1::unordered_map<long, long> impatCategoriesUnits;
  std::tr1::unordered_map<long, vector<long>> impactMethodCategories;

   
};

#endif /* LCADB_hpp */
