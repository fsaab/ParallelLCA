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
#ifndef LCIContributionReport_hpp
#define LCIContributionReport_hpp

#include <stdio.h>
#include <vector>
#include "../Utilities/ResultsUtilsSingle.hpp"
#include "../Calculators/Models/AppSettings.hpp"
#include "../Calculators/Models/CalculatorData.h"
#include "../Calculators/Models/TwoDimStore.hpp"
#include "../LCAModels/LCAIndexes.hpp"
#include <omp.h>
#include "../Factories/TechnologyMatrixFactorySingle.hpp"
#include "../Factories/InterventionMatrixFactorySingle.hpp"
#include "../Factories/CharacterisationMatrixFactory.hpp"
#include "../libs/libDescriptive.hpp"
#include <chrono>
#include "../Utilities/GraphUtilSingle.hpp"
#include "../Calculators/Models/LCIContribution.h"
#include "../Calculators/Models/LCIAContribution.h"

#include <stdio.h>
#include <vector>
#include <omp.h>
#include <chrono>
#include "../Calculators/Models/LCIContribution.h"
using namespace std;

class LCIContributionReport
{
  public:
    AppSettings settings;
    CalculatorData *calculatorData;
    LCADB *lcadb;

    LCIContributionReport(AppSettings _settings,
                          CalculatorData *_calculatorData, LCADB *_lcadb

                          )
        : calculatorData(_calculatorData), settings(_settings), lcadb(_lcadb)
    {
    }

    vector<LCIContribution> lciContribution(GraphData *graph, double *lciR,
                                            map<string, expression_t> &expressionsTable)
    {

        map<long, double> flowSolutionDS;

        for (auto &&flowid : (*calculatorData).lcaIndexes.ElementaryFlowsIndex)
        {
            flowSolutionDS[flowid] = lciR[(*calculatorData).lcaIndexes.ElementaryFlowsIndexIndices[flowid]];
        }

        LCAUtils lcaUtils(lcadb, calculatorData);
        vector<LCIContribution> v_lcicontribs;

        for (auto &&p : (*calculatorData).lcaIndexes.ProcessesIndex)
        {
            vector<long> elem_exch_p = (*lcadb).elementaryExchangesByProcess[p];

            if (elem_exch_p.size() > 0)
            {
                map<long, double> elem_exch_p_merged = lcaUtils.mergeExchanges(p, elem_exch_p, expressionsTable);
                int unitid = (*lcadb).exchanges[elem_exch_p[0]].unitid;

                for (std::map<long, double>::iterator it = elem_exch_p_merged.begin(); it != elem_exch_p_merged.end(); ++it)
                {
                    long f = (*it).first;
                    double merged = (*it).second;
                    double scalar = (*graph).nodesInfoMap[p].scalar;
                    v_lcicontribs.push_back(LCIContribution(f, flowSolutionDS[f], p, unitid, merged * scalar, scalar));
                }
            }
        }

        return v_lcicontribs;
    }

    void print(vector<LCIContribution> v_contribs)
    {

        string lciContributionPath = settings.RootPath + "data/calculations/" + settings.CalculationId + "/lci" + "_" + settings.CalculationId + ".txt";

        std::stringstream ss;

        for (auto &&contrib : v_contribs)
        {

            ss << contrib.flow << ","
               << contrib.q << ","
               << contrib.unit << ","
               << contrib.proc << ","
               << contrib.contr << ","
               << contrib.scalar << endl;
        }

        FileUtils fileUtils;

        fileUtils.writeTextToFile(ss.str(), lciContributionPath);
    }
};

#endif /* ContributionAnalysisReport_hpp */
