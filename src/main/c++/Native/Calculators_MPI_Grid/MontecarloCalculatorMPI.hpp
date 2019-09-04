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
#include "../Utilities/ResultsUtilsSingle.hpp"
#include "../Calculators/Models/AppSettings.hpp"
#include "../Calculators/Models/CalculatorData.h"
#include "../Utilities/ResultsUtilsSingle.hpp"
#include "../Calculators/Models/TwoDimVectorStore.hpp"
#include "../LCAModels/LCAIndexes.hpp"
#include "./SimulatorNative.hpp"
#include "../Factories/TechnologyMatrixFactorySingle.hpp"
#include "../Factories/InterventionMatrixFactorySingle.hpp"
#include "../Factories/CharacterisationMatrixFactory.hpp"
#include "../libs/libDescriptive.hpp"
#include "../Calculators/SensitivityCalculator.hpp"
using namespace std;
#include "../Utilities/FileUtils.hpp"

class MontecarloCalculatorMPI
{
  public:
    static void run(AppSettings settings,
                    CalculatorData *calculatorData, int slotsP, long slotsize, long dataslotsize_samples, long dataslotsize_output)
    {

        boost::mpi::environment env;
        boost::mpi::communicator world;

        int parallelism = slotsP;                                       //settings.Parallelism;//5
        long threadSize = settings.montecarlo_iterations / parallelism; //1000/5=200
        int slotnum = settings.montecarlo_iterations / slotsize;        //1000/100=10

        cout << slotsP << " " << threadSize << " " << settings.montecarlo_iterations << endl;

        /*caching some variables*/
        long impcatsLength = (*calculatorData).impactCategoriesLength;
        long Qlength = (*calculatorData).Q_size;
        //long Plength = (*lcadb).ptbl.parameters.size();
        long B_unc_size = (*calculatorData).B_unc_size;
        long A_unc_size = (*calculatorData).A_unc_size;
        long rows = settings.montecarlo_iterations;

        auto strt = std::chrono::high_resolution_clock::now();

        int i = world.rank();
        cout << "rank " << i << endl;
        int iterationOffset = (i * threadSize) + 1;

        cout << "iterating " << iterationOffset << "   " << iterationOffset + threadSize - 1;

        SimulatorNative::iterate(iterationOffset,
                                 iterationOffset + threadSize - 1,
                                 iterationOffset,
                                 threadSize,
                                 slotsize,
                                 calculatorData,
                                 settings, dataslotsize_samples, dataslotsize_output);

        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - strt;

        int res = 1;

        if (world.rank() > 0)
            world.send(0, 1, res);

        std::cout << "Montecarlo_slot," << settings.ProjectId << "," << elapsed.count() << std::endl;
    }

    static std::vector<std::string> split(std::string input, std::string sep)
    {

        vector<string> strs;
        boost::split(strs, input, boost::is_any_of(sep));
        return strs;
    }

    static ImpactStat *getImpactStat(double *ptr_lcia, AppSettings settings)
    {

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

        ImpactStat *stat = new ImpactStat(mean,   //mean
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
        return stat;
    }

    static void uncertainty(bool lcia,
                            AppSettings settings,
                            CalculatorData *calculatorData, long threadSize, long threads,
                            long slotNum, long dataslotsNum, long dataslotsize, long datasize)
    {

        FileUtils fileUtils;
        std::stringstream ss;

        cout << threadSize << " " << threads << " " << slotNum << " " << dataslotsNum << endl;

        cout << "traverse vertical data slots first" << endl;

        //traverse vertical data slots first
        for (int dataslot = 0; dataslot < dataslotsNum; dataslot++)
        {
            long local_iter = 0;

            vector<vector<double>> store_lcia;
            TwoDimVectorStore::initStore(store_lcia, settings.montecarlo_iterations, dataslotsize);

            cout << "traverse parallel compute nodes" << endl;

            //traverse parallel compute nodes
            for (int thread = 0; thread < threads; thread++)
            {
                int iterationOffset = (thread * threadSize) + 1;

                cout << "traverse sequential slots in each compute node" << endl;

                //traverse sequential slots in each compute node
                for (int slotIndex = 0; slotIndex < slotNum; slotIndex++)
                {
                    string slotPath = settings.RootPath + "data/calculations/" + settings.CalculationId + "/Iters/LCIA/" + to_string(iterationOffset) + "_" + to_string(slotIndex) + "/" + to_string(dataslot + 1);

                    cout << slotPath << endl;
                    vector<string> slot_lines = fileUtils.readTextFromFile(slotPath);

                    for (auto &&line : slot_lines)
                    {
                        vector<string> v_values = split(line, ",");

                        for (auto &&str : v_values)
                        {
                            if (str != "")
                                store_lcia[local_iter].push_back(stod(str));
                        }

                        local_iter++;
                    }
                }
            }

            TwoDimVectorStore store;
            cout << "traverse the filled partial store_lcia, calculate stats metrics on it" << endl;

            //traverse the filled partial store_lcia, calculate stats metrics on it
            for (int categoryind = dataslot * dataslotsize; categoryind < (dataslot + 1) * dataslotsize; categoryind++)
            {
                if (categoryind == datasize)
                    break;

                double *ptr_lcia = &(store.getDataForAllIterations(store_lcia, settings.montecarlo_iterations, categoryind-dataslot * dataslotsize))[0];

                // for (int i = 0; i < settings.montecarlo_iterations; i++)
                // {
                //     cout << ptr_lcia[i] << ",";
                // }

                // cout << endl;

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

                int cat = (*calculatorData).lcaIndexes.ImpactCategoryIndex[(dataslot * dataslotsize) + categoryind];

                ImpactStat stat(cat, mean, //mean
                                std,       //std
                                min,       // min
                                max,       //max
                                median,    //median
                                q1,
                                q2,
                                q3,
                                q4,
                                q5,
                                q6);

                (*calculatorData).impactStats.push_back(stat);

                ss << cat << "," << (mean) << "," << //mean
                    (std) << "," <<                  //std
                    //(min) << "," << // min
                    //(max) << "," << //max
                    (median) << "," << //median
                    (q1) << "," << (q2) << "," << (q3) << "," << (q4) << "," << (q5) << "," << (q6) << std::endl;

                delete ptr_lcia;
            }
        }

        string report_path_out = settings.RootPath + "data/calculations/" + settings.CalculationId + "/" + "uncertainty-lcia" + "_" + settings.CalculationId + ".txt";
        ofstream file = ofstream(report_path_out.c_str());
        file << ss.str();
        file.close();
    }
};

#endif /* MontecarloCalculatorMPI_hpp */
