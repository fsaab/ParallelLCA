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

#ifndef GraphFactory_hpp
#define GraphFactory_hpp

#include <stdio.h>
#include <vector>
#include "../Utilities/FileUtils.hpp"
#include "../LCAModels/Exchange.hpp"
#include "../DAL/ExchangeObj.hpp"

#include "../LCAModels/ExchangeItem.hpp"
#include "../LCAModels/ExchangeCell.hpp"
#include <chrono>
#include <sstream>
#include <time.h>
#include <iostream>

#include "../Calculators/Models/AppSettings.hpp"
#include <vector>
#include "../LCAModels/ExchangeLink.hpp"
#include "../LCAModels/Exchange.hpp"

#include "../Graph/LCAEdgeCompact.hpp"
#include <omp.h>
#include <tr1/unordered_map>
#include "../LCAModels/EdgeShare.hpp"
#include "../DAL/LCADB.hpp"
#include "../Utilities/LCAUtils.hpp"
#include <cmath>
#include <queue>
#include "../LCAModels/ContributionTReeNode.hpp"
#include "../Graph/GraphData.h"
#include "../Calculators/Models/CalculatorData.h"
#include <chrono> // for high_resolution_clock
#include "../Utilities/GraphUtilSingle.hpp"
class GraphFactory
{
  private:
  public:
    AppSettings settings;

    GraphFactory(AppSettings settings_) : settings(settings_) {}

    GraphData *create(LCADB *lcadb, vector<long> procs)
    {
        auto strt_gr = std::chrono::high_resolution_clock::now();

        GraphData *graph = new GraphData();
        GraphUtilsSingle graphutil(lcadb, graph, settings);
        graphutil.buildDirectedEdgesHorizontalSingleInPlace(procs, 0);

        auto finish_gr = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_gr = finish_gr - strt_gr;
        std::cout << "g/c++," << settings.ProjectId << "," << elapsed_gr.count() << std::endl;

        return graph;
    }
};

#endif /* GraphFactory_hpp */
