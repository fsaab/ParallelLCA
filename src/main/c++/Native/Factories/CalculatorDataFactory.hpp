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

#ifndef CalculatorDataFactory_hpp
#define CalculatorDataFactory_hpp

#include <stdio.h>
#include <vector>
#include <chrono>
#include <sstream>
#include <time.h>
#include <iostream>
#include <omp.h>
#include <tr1/unordered_map>
#include <cmath>
#include <queue>
#include <chrono> // for high_resolution_clock
#include <vector>

#include "../Calculators/Models/AppSettings.hpp"
#include "../LCAModels/ExchangeLink.hpp"
#include "../LCAModels/Exchange.hpp"
#include "../Graph/LCAEdgeCompact.hpp"
#include "../Utilities/FileUtils.hpp"
#include "../LCAModels/Exchange.hpp"
#include "../DAL/ExchangeObj.hpp"
#include "../LCAModels/ExchangeItem.hpp"
#include "../LCAModels/ExchangeCell.hpp"
#include "../LCAModels/EdgeShare.hpp"
#include "../DAL/LCADB.hpp"
#include "../Utilities/LCAUtils.hpp"
#include "../LCAModels/ContributionTReeNode.hpp"
#include "../Graph/GraphData.h"
#include "../Calculators/Models/CalculatorData.h"
#include "TechnologyMatrixFactorySingle.hpp"
#include "InterventionMatrixFactorySingle.hpp"
#include "CharacterisationMatrixFactory.hpp"

class CalculatorDataFactory
{

  public:
    AppSettings settings;

    CalculatorDataFactory(AppSettings settings_) : settings(settings_)
    {
    }

    CalculatorData *create(LCADB *lcadb, GraphData *graph)
    {

        CalculatorData *calculatorData = new CalculatorData();

        for (std::tr1::unordered_map<long, NodeInfo>::iterator it = (*graph).nodesInfoMap.begin();
             it != (*graph).nodesInfoMap.end();
             ++it)
        {
            (*calculatorData).lcaIndexes.ProcessesIndex.push_back((*it).first);
        }


        (*calculatorData).lcaIndexes.loadIndexesForProcesses();

        TechnologyMatrixFactorySingle techFactory(lcadb, calculatorData, settings);
        InterventionMatrixFactorySingle interFactory(lcadb, calculatorData);
        CharacterisationMatrixFactory charactFactory(lcadb, calculatorData, settings);

        (*calculatorData).A_exchanges = techFactory.AExchangeItems(graph);
        (*calculatorData).B_exchanges = interFactory.BExchangeItems();
        (*calculatorData).Q_cells = charactFactory.impactFactors();

        (*calculatorData).loadLengths();

        (*calculatorData).demand_demandVectorArray = vector<double>((*calculatorData).processesLength);
        (*calculatorData).demand_demandVectorArray[(*calculatorData).lcaIndexes.ProcessesIndexIndices[settings.RootProcessId]] = settings.OutputQunatity;

        (*calculatorData).parameters = (*lcadb).parameters;

        (*calculatorData).LoadUncertainCells();

        return calculatorData;
    }
};

#endif /* CalculatorDataFactory_hpp */
