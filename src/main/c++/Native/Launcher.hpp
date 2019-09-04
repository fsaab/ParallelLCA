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
#ifndef Launcher_hpp
#define Launcher_hpp

#include <stdio.h>
#include <vector>
#include "Utilities/FileUtils.hpp"
//#include "Exchange.hpp"
//#include "ExchangeObj.hpp"
//
//#include "ExchangeItem.hpp"
//#include "ExchangeCell.hpp"
#include <chrono>
#include <sstream>
#include <time.h>
#include <iostream>

#include "Calculators/Models/AppSettings.hpp"
#include <vector>
//#include "ExchangeLink.hpp"
//#include "Exchange.hpp"
//
//#include "LCAEdgeCompact.hpp"
#include <omp.h>
#include <tr1/unordered_map>

#include "DAL/LCADB.hpp"

//#include "LCADBUtils.h"
#include "Utilities/GraphUtilSingle.hpp"
#include <vector>
#include "libs/StaticCalculator.hpp"
//#include "ResultsUtilsSingle.hpp"
//#include "ParameterUtils.h"
//#include "libStochastic.hpp"
//#include "libQBAInv.hpp"
#include "Factories/CalculatorDataFactory.hpp"
#include "Factories/GraphFactory.hpp"
#include "DAL/LCADBFactory.hpp"

class Launcher {
public:

    void launch(int RootProcessId,
            double OutputQunatity,
            string SolvingMethod,
            string SystemId,
            string CalculationId,
            int ImpactMethodId,
            int lcia,
            int ismontecarlo,
            int montecarlo_iterations,
            int Parallelism,
            string RootPath,
            long ProjectId,
            int Version) 
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
            Parallelism,
            RootPath,
            ProjectId,
            Version, false};

        LCADBFactory dbFactory(settings);
        LCADB* lcadb = dbFactory.create();

        dbFactory.update(lcadb, settings);
        
        AppSettings set_v2 = settings;
        set_v2.Version = 2;
        dbFactory.update(lcadb, set_v2);
        
         AppSettings set_v3 = settings;
        set_v3.Version = 3;
        dbFactory.update(lcadb, set_v3);

        vector<long> v;
        v.push_back(settings.RootProcessId);

        GraphFactory graphFactory(settings);
        GraphData* graph = graphFactory.create(lcadb, v);

        CalculatorDataFactory simFactory(settings);
        CalculatorData* calculatorData = simFactory.create(lcadb, graph);

        //    libQBAInv libqbainv(calculatorData);
        //    long size=0;
        //    double * arr=libqbainv.run(size,
        //    settings.RootProcessId,
        //            settings.OutputQunatity,
        //            settings.SolvingMethod,
        //            settings.SystemId,
        //            settings.CalculationId,
        //            settings.ImpactMethodId,
        //            settings.lcia,
        //            (settings.montecarlo ? 1 : 0),
        //            settings.montecarlo_iterations,
        //            settings.Parallelism,
        //            settings.RootPath,
        //            settings.ProjectId,
        //            settings.Version,
        //            &lcadb);


        //        long calculatorDatal = (long) calculatorData;
        //        long lcadbl = (long) lcadb;
        //
        //        //cout << "loading pointers:-" << endl;
        //
        //        CalculatorData * calculatorData2 = (CalculatorData*) calculatorDatal;
        //
        //        LCADB * lcadb2 = (LCADB*) lcadbl;

        StaticCalculator lib(calculatorData, false);
        double* scalars_PTR = new double [(* calculatorData).processesLength];

        lib.run(settings.RootProcessId,
                settings.OutputQunatity,
                settings.SolvingMethod,
                settings.SystemId,
                settings.CalculationId,
                settings.ImpactMethodId,
                settings.lcia,
                (settings.montecarlo ? 1 : 0),
                settings.montecarlo_iterations,
                settings.Parallelism,
                settings.RootPath,
                settings.ProjectId,
                settings.Version,
                lcadb, graph, scalars_PTR
                );
        //
        //        libStochastic libstoch;

        //if (settings.montecarlo) {

        //        libstoch.run(settings.RootProcessId,
        //                settings.OutputQunatity,
        //                settings.SolvingMethod,
        //                settings.SystemId,
        //                settings.CalculationId,
        //                settings.ImpactMethodId,
        //                settings.lcia,
        //                (settings.montecarlo ? 1 : 0),
        //                settings.montecarlo_iterations,
        //                settings.Parallelism,
        //                settings.RootPath,
        //                settings.ProjectId,
        //                settings.Version,
        //                lcadb, false, calculatorData, scalars_PTR);

        //}
        //return arr;
    }



};

#endif /* Launcher_hpp */
