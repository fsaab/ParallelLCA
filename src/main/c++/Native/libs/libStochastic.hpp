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
#ifndef libStochastic_hpp
#define libStochastic_hpp

#include <stdio.h>
#include <iostream>

#include "../Calculators/Models/AppSettings.hpp"
#include "../LCAModels/LCAIndexes.hpp"
#include "../LCAModels/ExchangeItem.hpp"
#include "../LCAModels/CalcImpactFactorItem.hpp"
#include "../Calculators/Models/IterationsResults.hpp"
#include "../Calculators/SensitivityCalculator.hpp"
#include "../Utilities/FileUtils.hpp"
#include "../Graph/GraphData.h"

#include "../Calculators/MontecarloCalculator.hpp"


using namespace std;

class libStochastic
{

public:
  bool run(int RootProcessId,
           double OutputQunatity,
           string SolvingMethod,
           string SystemId,
           string CalculationId,
           int ImpactMethodId,
           int lcia,
           int ismontecarlo,
           int montecarlo_iterations,
           int Paralleism,
           string RootPath,
           long ProjectId,
           int Version,
           LCADB *lcadb,
           bool sensitivity,
           CalculatorData *calculatorData, GraphData *graph
           //, double* scalars_PTR_g
  )
  {
    AppSettings settings{
        RootProcessId,
        OutputQunatity,
        SolvingMethod,
        SystemId,
        CalculationId,
        ImpactMethodId,
        lcia == 1,
        ismontecarlo == 1,
        montecarlo_iterations,
        Paralleism,
        RootPath,
        ProjectId,
        Version, sensitivity};

    vector<double> demand_demandVectorArray = (*calculatorData).demand_demandVectorArray;
    //long Plength = (*lcadb).parameters.size();

    int rows = settings.montecarlo_iterations;
    int impactCategoriesLength = (*calculatorData).impactCategoriesLength;

    IterationsResults *iterationResults = new IterationsResults(settings,
                                                                (*calculatorData).impactCategoriesLength,
                                                                (*calculatorData).A_unc_size,
                                                                (*calculatorData).B_unc_size,
                                                                (*calculatorData).Q_size,
                                                                0, //Plength,
                                                                settings.montecarlo_iterations);

    MontecarloCalculator montecarlo;

    montecarlo.run(settings, calculatorData, lcadb, iterationResults /*,scalars_PTR_g*/, graph);
    montecarlo.uncertainty(true, settings, calculatorData, iterationResults, lcadb);

    if (sensitivity)
    {
      SensitivityCalculator sensitivityAnalyser = SensitivityCalculator(settings, calculatorData, iterationResults);
      sensitivityAnalyser.run();
    }
    return true;
  }
};
#endif /* libStochastic_hpp */
