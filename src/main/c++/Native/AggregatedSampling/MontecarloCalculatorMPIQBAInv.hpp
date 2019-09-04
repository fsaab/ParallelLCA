//
//  MontecarloCalculatorMPI.hpp
//  LCA.Kernel
//
//  Created by francois saab on 2/18/18.
//  Copyright © 2018 fsaab. All rights reserved.
//

#ifndef MontecarloCalculatorMPIAggregatedDS_hpp
#define MontecarloCalculatorMPIAggregatedDS_hpp

#include <stdio.h>
#include <vector>
#include <chrono>
#include <omp.h>
#include <boost/mpi.hpp>
#include <boost/serialization/vector.hpp>

#include "../Utilities/ResultsUtilsSingle.hpp"
#include "../Calculators/Models/AppSettings.hpp"
#include "../Calculators/Models/CalculatorData.h"
#include "../Utilities/ResultsUtilsSingle.hpp"
#include "../Calculators/Models/TwoDimVectorStore.hpp"
#include "../LCAModels/LCAIndexes.hpp"
#include "../Factories/TechnologyMatrixFactorySingle.hpp"
#include "../Factories/InterventionMatrixFactorySingle.hpp"
#include "../Factories/CharacterisationMatrixFactory.hpp"
#include "../libs/libDescriptive.hpp"
#include "AggregatedSampler.hpp"

using namespace std;

class MontecarloCalculatorMPIQBAInv
{
  public:

    static void run_presample_QBAinv(AppSettings settings,
                                      CalculatorData *calculatorData, int slotsP, double *scalars_PTR_g)
    {

        boost::mpi::environment env; //set number of processes
        boost::mpi::communicator world;

        //cout << "world - size " << world.size() << endl;

        int slotsize = settings.montecarlo_iterations / slotsP; // 10,000/20=500

        int slotnum = settings.montecarlo_iterations / slotsize; //10,000/500=20

        if (world.rank() == 0)
        {
            FileUtils fileUtils;
            fileUtils.newfolder(settings.RootPath + "data/calculations/" + settings.CalculationId + "/iters/");
        }

        auto strt = std::chrono::high_resolution_clock::now();

        long impcatsLength = (*calculatorData).impactCategoriesLength;
        long Qlength = (*calculatorData).Q_size;
        long Plength = 3; //(*lcadb).ptbl.parameters.size();
        long B_unc_size = (*calculatorData).B_unc_size;
        long A_unc_size = (*calculatorData).A_unc_size;
        long rows = settings.montecarlo_iterations;

        //cout << "new  iterationsResults done....!!!!" << endl;

        if (world.rank() == 0)
        {
            for (int i = 0; i < slotnum; i++)
            {
                int iterationOffset = (i * slotsize) + 1;
                int res;
                world.recv(i + 1, 1, res);
            }
        }
        else
        {

            int i = world.rank() - 1;
            //cout << "i " << i << endl;
            int iterationOffset = (i * slotsize) + 1;
            auto strt_1 = std::chrono::high_resolution_clock::now();

            AggregatedSampler::presample_QBAInv(iterationOffset,
                                                  iterationOffset + slotsize - 1,
                                                  iterationOffset,
                                                  slotsP,
                                                  slotsize,
                                                  calculatorData,
                                                  settings, scalars_PTR_g);

            auto finish_1 = std::chrono::high_resolution_clock::now();

            std::chrono::duration<double> elapsed_1 = finish_1 - strt_1;
            std::cout << "Itertion_QBAinv," << settings.ProjectId << "," << elapsed_1.count() << std::endl;

            //cout << "iterations done for " << iterationOffset << endl;
            world.send(0, 1, 1);
            //cout << "sent results for " << iterationOffset << endl;
        }

        auto finish = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> elapsed = finish - strt;
        std::cout << "Montecarlo," << settings.ProjectId << "," << elapsed.count() << std::endl;
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

#endif /* MontecarloCalculatorMPIAggregatedDS_hpp */
