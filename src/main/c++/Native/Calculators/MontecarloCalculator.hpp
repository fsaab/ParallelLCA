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
#ifndef MontecarloCalculator_hpp
#define MontecarloCalculator_hpp

#include <stdio.h>
#include <vector>
#include <chrono>
#include <omp.h>

#include "../Utilities/ResultsUtilsSingle.hpp"
#include "Models/AppSettings.hpp"
#include "Models/CalculatorData.h"
#include "../Utilities/ResultsUtilsSingle.hpp"
#include "Models/TwoDimVectorStore.hpp"
#include "../LCAModels/LCAIndexes.hpp"
#include "SimulatorNative.hpp"
#include "../Factories/TechnologyMatrixFactorySingle.hpp"
#include "../Factories/InterventionMatrixFactorySingle.hpp"
#include "../Factories/CharacterisationMatrixFactory.hpp"
#include "../libs/libDescriptive.hpp"


using namespace std;

class MontecarloCalculator
{
  public:
    static void run(AppSettings settings,
                    CalculatorData *calculatorData,
                    LCADB *lcadb, IterationsResults *iterationsResults
                     //,double *scalars_PTR_g
                   )
    {

        int parallelism = settings.Parallelism;//5
        long threadSize = settings.montecarlo_iterations / parallelism; //100/5=20
        int slotsize = threadSize;                                         //10
        int slotnum = settings.montecarlo_iterations / slotsize;//100/10=10

        auto strt = std::chrono::high_resolution_clock::now();

        long impcatsLength = (*calculatorData).impactCategoriesLength;
        long Qlength = (*calculatorData).Q_size;
        long Plength = (*lcadb).parameters.size();
        long B_unc_size = (*calculatorData).B_unc_size;
        long A_unc_size = (*calculatorData).A_unc_size;
        long rows = settings.montecarlo_iterations;

        FileUtils fileUtils;
        string slotPath = settings.RootPath + "data/calculations/" + settings.CalculationId + "/Iters/";
        fileUtils.newfolder(slotPath);
        slotPath = settings.RootPath + "data/calculations/" + settings.CalculationId + "/Iters/LCIA/";
        fileUtils.newfolder(slotPath);
       
        slotPath = settings.RootPath + "data/calculations/" + settings.CalculationId + "/Iters/Samples/";
        fileUtils.newfolder(slotPath);

       #pragma omp parallel for shared(threadSize) num_threads(parallelism)
        for (int i = 0; i < parallelism; i++)
        {


            int iterationOffset = (i * threadSize) + 1;



           IterationsResults *iterationResults_i = SimulatorNative::iterate(iterationOffset,
                                                                             iterationOffset + threadSize - 1,
                                                                             iterationOffset,
                                                                             threadSize,
                                                                             slotsize,
                                                                             calculatorData,
                                                                             settings
            );

           #pragma omp critical
            {

                // cout << "slot*****************: " << i << endl;
                // cout << "iterationOffset" << iterationOffset << endl;

                int iter = iterationOffset;
                while (iter <= iterationOffset + threadSize - 1)
                {

                    TwoDimVectorStore::putDataAtIteration((*iterationsResults).store_lcia_ptr,
                                                          iter - 1,
                                                          &(((*iterationResults_i).store_lcia_ptr)[iter - iterationOffset])[0],
                                                          impcatsLength);

                    if (settings.Sensitivity)
                    {

                        TwoDimVectorStore::putDataAtIteration((*iterationsResults).store_A_ptr, iter - 1,
                                                              &(((*iterationResults_i).store_A_ptr)[iter - iterationOffset])[0], A_unc_size);

                        TwoDimVectorStore::putDataAtIteration((*iterationsResults).store_B_ptr, iter - 1,
                                                              &(((*iterationResults_i).store_B_ptr)[iter - iterationOffset])[0], B_unc_size);

                        TwoDimVectorStore::putDataAtIteration((*iterationsResults).store_Q_ptr, iter - 1,
                                                              &(((*iterationResults_i).store_Q_ptr)[iter - iterationOffset])[0], Qlength);
                    }
                    
                    // TwoDimVectorStore::putDataAtIteration((*iterationsResults).store_P_ptr, iter - 1,
                    // &(((*iterationResults_i).store_P_ptr)[iter - iterationOffset])[0]
                    // , Plength);

                    iter++;
                }

                delete iterationResults_i;
                iterationResults_i = nullptr;
           }
        }

        //cout << "Montecarlo done!" << endl;

        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - strt;
        std::cout << "Montecarlo," << settings.ProjectId << "," << elapsed.count() << std::endl;
    }

    void uncertainty(bool lcia,
                     AppSettings settings,
                     CalculatorData *calculatorData,
                     /*vector< vector<double > >* store_lcia_ptr*/ IterationsResults *iterationsResults,
                     LCADB *lcadb)
    {

        TwoDimVectorStore store;
        int impcatsLength = (*calculatorData).impactCategoriesLength;

        string report_path_out = settings.RootPath + "data/calculations/" + settings.CalculationId + "/" + "uncertainty-lcia" + "_" + settings.CalculationId + ".txt";
        ofstream file = ofstream(report_path_out.c_str());
        std::stringstream ss;

        for (int categoryid = 0; categoryid < impcatsLength; categoryid++)
        {

            double *ptr_lcia = &(store.getDataForAllIterations((*iterationsResults).store_lcia_ptr, settings.montecarlo_iterations, categoryid))[0];
            vector<double> all = Descriptive::all(ptr_lcia, settings.montecarlo_iterations);

            double mean = all[0];   //mean
            double std = all[1];    //std
            double min = all[2];    // min
            double max = all[3];    //max
            double median = all[4]; //median
            double q1 = all[5];
            double q2 = all[6];
            double q3 = all[7];
            double q4 = all[8];
            double q5 = all[9];
            double q6 = all[10];

            ImpactStat stat(mean,   //mean
                            std,    //std
                            min,    // min
                            max,    //max
                            median, //median
                            q1,
                            q2,
                            q3,
                            q4,
                            q5,
                            q6);

            (*calculatorData).impactStats.push_back(stat);
            int cat = (*calculatorData).lcaIndexes.ImpactCategoryIndex[categoryid];

            ss << cat << "," << (mean) << "," << //mean
                (std) << "," <<                  //std
                //(min) << "," << // min
                //(max) << "," << //max
                (median) << "," << //median
                (q1) << "," << (q2) << "," << (q3) << "," << (q4) << "," << (q5) << "," << (q6) << std::endl;
        }

        file << ss.str();
        file.close();
    }
};

#endif /* MontecarloCalculator_hpp */
