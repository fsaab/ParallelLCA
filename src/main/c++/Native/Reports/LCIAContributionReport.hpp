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
#ifndef LCIAContributionReport_hpp
#define LCIAContributionReport_hpp

#include <stdio.h>
#include <vector>
#include "../Utilities/ResultsUtilsSingle.hpp"
#include "../Calculators/Models/AppSettings.hpp"
#include "../Calculators/Models/CalculatorData.h"
#include "../Calculators/Models/TwoDimStore.hpp"
#include "../LCAModels/LCAIndexes.hpp"
#include "../Calculators/SimulatorNative.hpp"
#include <omp.h>
#include "../Factories/TechnologyMatrixFactorySingle.hpp"
#include "../Factories/InterventionMatrixFactorySingle.hpp"
#include "../Factories/CharacterisationMatrixFactory.hpp"
#include "../libs/libDescriptive.hpp"
#include <chrono>
#include "../Utilities/GraphUtilSingle.hpp"
#include "../Calculators/Models/LCIContribution.h"
#include "../Calculators/Models/LCIAContribution.h"
using namespace std;

class LCIAContributionReport {
public:

    AppSettings settings;
    CalculatorData* calculatorData;
    LCADB* lcadb;

    LCIAContributionReport(AppSettings _settings,CalculatorData* _calculatorData, LCADB* _lcadb)
    : calculatorData(_calculatorData), settings(_settings), lcadb(_lcadb)
    {
    }

    vector<LCIAContribution> lciaContribution(vector<LCIContribution> v_lcicontribs,
            double* lciaR, SMatrix Q) {

        map<long, double> impacts_cat;
       
        for (auto && cat : (*calculatorData).lcaIndexes.ImpactCategoryIndex) {

            impacts_cat[cat] = lciaR[(*calculatorData).lcaIndexes.ImpactCategoryIndexIndices[cat]];
        }

        map<std::pair<int, long>, LCIAContribution> impact_contribs;

        for (auto && lcicontrib : v_lcicontribs) {

            for (auto && cat : (*calculatorData).lcaIndexes.ImpactCategoryIndex) {

                double factor = Q.row((*calculatorData).lcaIndexes.ImpactCategoryIndexIndices[cat])
                        .coeff((*calculatorData).lcaIndexes.ElementaryFlowsIndexIndices[lcicontrib.flow]);

                double oldcontribution = impact_contribs[std::pair<int, long>(cat, lcicontrib.proc)].contr;
                double newcontribution = lcicontrib.contr * factor;

                impact_contribs[std::pair<int, long>(cat, lcicontrib.proc)] = LCIAContribution(cat,
                        impacts_cat[cat],
                        lcicontrib.proc,
                        (*lcadb).impatCategoriesUnits[cat],
                        oldcontribution + newcontribution
                );

            }
            
        }

        vector<LCIAContribution> v_lciacontribs;

        for (map<std::pair<int, long>, LCIAContribution>::iterator it = impact_contribs.begin(); it != impact_contribs.end(); ++it) {

            v_lciacontribs.push_back((*it).second);
        }

        return v_lciacontribs;
    }

    void print(vector<LCIAContribution> v_contribs) {

        string lciContributionPath = settings.RootPath + "data/calculations/" + settings.CalculationId + "/lcia" + "_" + settings.CalculationId + ".txt";

        std::stringstream ss;

        for (auto && contrib : v_contribs) {
            if (contrib.contr != 0) {
                ss << contrib.category << ","
                        << contrib.impact << ","
                        << contrib.proc << ","
                        << contrib.contr << ","
                        << contrib.unit
                        << endl;
            }
        }

        FileUtils fileUtils;

        fileUtils.writeTextToFile(ss.str(), lciContributionPath);

    }

};

#endif /* ContributionAnalysisReport_hpp */
