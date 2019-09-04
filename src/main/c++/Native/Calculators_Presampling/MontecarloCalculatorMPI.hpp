//  LCA.Kernel
//
//  Created by francois saab on 2/18/18.
//  saab.francois@gmail.com
//  francois.saab.1@ens.etsmtl.ca. Thesis At ETS : etmtl.ca
//  Copyright © 2018 fsaab. All rights reserved.
//

#ifndef MontecarloCalculatorMPI_hpp
#define MontecarloCalculatorMPI_hpp

#include <stdio.h>
#include <vector>
#include <chrono>
#include <omp.h>
#include <boost/mpi.hpp>
#include <boost/serialization/vector.hpp>



#include "../Calculators/Models/AppSettings.hpp"
#include "../Calculators/Models/CalculatorData.h"
#include "../Utilities/ResultsUtilsSingle.hpp"
#include "../Calculators/Models/TwoDimVectorStore.hpp"
#include "../LCAModels/LCAIndexes.hpp"
#include "../LCAModels/CalcImpactFactorItem.hpp"
#include "../LCAModels/ExchangeItem.hpp"
#include "../libs/libEigen.hpp"
#include "../Uncertainties/EntitySampler.hpp"
#include "../Calculators/Models/IterationsResults.hpp"
#include "../Utilities/ParameterUtils.h"

#include "SimulatorNative.hpp"
#include "SensitivityCalculator.hpp"

using namespace std;

class MontecarloCalculatorMPI
{
  public:
 
    static void run_presampled(AppSettings settings,
                               CalculatorData *calculatorData, 
                               IterationsResults *iterationsResults, 
                               int slotsP, 
                               double *scalars_PTR_g)
    {


        cout<<"...........................presampled............."<<endl;

        boost::mpi::environment env;
        boost::mpi::communicator world;

        int slotsize = settings.montecarlo_iterations / slotsP;  // 10,000/20=500
        int slotnum = settings.montecarlo_iterations / slotsize; //10,000/500=20
        auto strt = std::chrono::high_resolution_clock::now();

        long impcatsLength = (*calculatorData).impactCategoriesLength;
        long Qlength = (*calculatorData).Q_size;
        //long Plength = (*lcadb).ptbl.parameters.size();
        long B_unc_size = (*calculatorData).B_unc_size;
        long A_unc_size = (*calculatorData).A_unc_size;
        long rows = settings.montecarlo_iterations;

        cout<<"world.rank() "<<world.rank()<<endl;

        if (world.rank() == 0)
        {
            for (int i = 0; i < slotnum; i++)
            {

                int iterationOffset = (i * slotsize) + 1;
                IterationsResults iterationResults_i;

                world.recv(i + 1, 1, iterationResults_i);

                int iter = iterationOffset;
                while (iter <= iterationOffset + slotsize - 1)
                {

                    TwoDimVectorStore::putDataAtIteration((*iterationsResults).store_lcia_ptr,
                                                          iter - 1,
                                                          &(((iterationResults_i).store_lcia_ptr)[iter - iterationOffset])[0],
                                                          impcatsLength);

                    // TwoDimVectorStore::putDataAtIteration((*iterationsResults).store_A_ptr, iter - 1,
                    //         &(((iterationResults_i).store_A_ptr)[iter - iterationOffset])[0]
                    //         , A_unc_size);

                    // TwoDimVectorStore::putDataAtIteration((*iterationsResults).store_B_ptr, iter - 1,
                    //         &(((iterationResults_i).store_B_ptr)[iter - iterationOffset])[0]
                    //         , B_unc_size);

                    // TwoDimVectorStore::putDataAtIteration((*iterationsResults).store_Q_ptr, iter - 1,
                    //         &(((iterationResults_i).store_Q_ptr)[iter - iterationOffset])[0]
                    //         , Qlength);

                    // TwoDimVectorStore::putDataAtIteration((*iterationsResults).store_P_ptr, iter - 1,
                    //         &(((iterationResults_i).store_P_ptr)[iter - iterationOffset])[0]
                    //         , Plength);

                    iter++;
                }
            }
        }
        else
        {
            int i = world.rank() - 1;
            int iterationOffset = (i * slotsize) + 1;
            cout<<"int iterationOffset= "<<iterationOffset<<endl;
            IterationsResults *iterationResults_i = SimulatorNative::iterate_presampling(iterationOffset,
                                                                                         iterationOffset + slotsize - 1,
                                                                                         iterationOffset,
                                                                                         slotsP,
                                                                                         slotsize,
                                                                                         calculatorData,
                                                                                         settings, scalars_PTR_g);
            world.send(0, 1, *iterationResults_i);
            delete iterationResults_i;
        }

        auto finish = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> elapsed = finish - strt;
        std::cout << "Montecarlo_presampling," << settings.ProjectId << "," << elapsed.count() << std::endl;
    }

    static void uncertainty(bool lcia,
                            AppSettings settings,
                            CalculatorData *calculatorData,
                            IterationsResults *iterationsResults)
    {

        TwoDimVectorStore store;
        int impcatsLength = (*calculatorData).impactCategoriesLength;

        string report_path_out = settings.RootPath + "data/calculations/" + settings.CalculationId + "/" + "uncertainty-lcia" + "_" + settings.CalculationId + ".txt";

        ofstream *file = new ofstream(report_path_out.c_str());

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

            //cout << "calculating for impact cat " << cat << endl;

            ss << cat << "," << (mean) << "," << //mean
                (std) << "," <<                  //std
                //(min) << "," << // min
                //(max) << "," << //max
                (median) << "," << //median
                (q1) << "," << (q2) << "," << (q3) << "," << (q4) << "," << (q5) << "," << (q6) << std::endl;
        }

        *file << ss.str();
        file->close();
        delete file;
    }
};

#endif /* MontecarloCalculatorMPI_hpp */
